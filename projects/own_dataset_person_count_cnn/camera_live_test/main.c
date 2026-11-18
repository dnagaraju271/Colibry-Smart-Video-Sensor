/*
 * One-frame PAG7920LT -> Colibry ISP scaler -> NNPA test.
 *
 * Camera input: 160x120 monochrome RAW8.
 * ISP output:   40x30 monochrome (4x downscale in X and Y).
 * NNPA input:   [1,30,40,1] uint8, 1200 bytes.
 *
 * The verified NNPA bitstream is embedded in this SRAM firmware.
 * No external flash APIs are called.
 */

#include <stdint.h>
#include <stdio.h>

#include "chip.h"
#include "colibry_drivers.h"
#include "colibry_board.hpp"
#include "isp_driver.hpp"
#include "build/own_count_nnpa_data.h"

#define CHIP_VDD_VOLTAGE        0.9f
#define HFCLK_FREQUENCY_MHZ     40u
#define APB_CLK_DIV             2u

#define CAMERA_INPUT_WIDTH      160u
#define CAMERA_INPUT_HEIGHT     120u
#define MODEL_INPUT_WIDTH       40u
#define MODEL_INPUT_HEIGHT      30u
#define MODEL_INPUT_BYTES       (MODEL_INPUT_WIDTH * MODEL_INPUT_HEIGHT)

#define CAMERA_ONCHIP_MCLK      1u
#define CAMERA_FPS              1u
#define FRAME_TIMEOUT_MS        5000u
#define FRAME_START_TIMEOUT_MS   5000u
#define INFERENCE_TIMEOUT_MS    3000u
#define CLASS_COUNT             3u

static ColibryBoard colibryBoard;

static const char *class_name(uint32_t id)
{
    switch (id) {
    case 0u: return "1_person";
    case 1u: return "2_person";
    case 2u: return "3_person";
    default: return "unknown";
    }
}

static void copy_model_to_nnpa_memory(void)
{
    volatile uint32_t *destination =
        (volatile uint32_t *)NNPAMEM_BASE_ADDR;

    NNPA_reg->nnpa_control.fields.cpu_mem_access = 1u;
    for (uint32_t i = 0; i < OWN_COUNT_NNPA_WORD_COUNT; ++i) {
        destination[i] = own_count_nnpa_words[i];
    }
    NNPA_reg->nnpa_control.fields.cpu_mem_access = 0u;
}

static void configure_isp_for_pag7920_mono(void)
{
    /*

    /*
     * PAG7920LT is configured by the SDK in QQVGA mode (160x120).
     * The sensor delivers a single RAW grayscale channel, so Bayer2RGB
     * and RGB2GRAY stay bypassed. Only the scaler is enabled.
     */
    colibryISP->regmap->isp_ctrl.word =
        ISP_CROPPER_BYPASS_MASK |
        ISP_BAYER2RGB_BYPASS_MASK |
        ISP_RGB2GRAY_BYPASS_MASK;

    /* 4.8 fixed-point ratio: 4.0 == 0x400. */
    colibryISP->isp_scaler_scaling_x_set(0x400u);
    colibryISP->isp_scaler_scaling_y_set(0x400u);
    colibryISP->isp_scaler_size_out_x_set(MODEL_INPUT_WIDTH);
    colibryISP->isp_scaler_size_out_y_set(MODEL_INPUT_HEIGHT);

    /* Clear stale status flags before first frame. */
    colibryISP->regmap->isp_int_clear.word = 0x3Fu;
}

/*
 * Read back the PAG7920LT configuration written by the SDK.
 * Only the register-bank selector is written; sensor image settings are
 * read without modification. Expected values account for EVBTINY's
 * horizontal-flip configuration in camera_init().
 */
static void dump_pag7920_register_bank(
    uint8_t bank,
    const uint8_t *addresses,
    const uint8_t *expected,
    uint32_t count)
{
    colibryBoard.cameraPAG7920LT.writeRegister(0xEFu, bank);
    colibrySCU->delay_ms(1u);

    for (uint32_t i = 0u; i < count; ++i) {
        const uint8_t actual =
            colibryBoard.cameraPAG7920LT.readRegister(addresses[i]);

        printf("PAG7920LT_REG,bank=0x%02x,addr=0x%02x,expected=0x%02x,actual=0x%02x,match=%u\n",
               (unsigned int)bank,
               (unsigned int)addresses[i],
               (unsigned int)expected[i],
               (unsigned int)actual,
               (unsigned int)(actual == expected[i]));
    }
}

static void diagnose_pag7920_registers(void)
{
    static const uint8_t bank0_addresses[] = {
        0x00u, 0x01u, 0x09u, 0x0Eu, 0x15u,
        0x16u, 0x1Cu, 0x3Eu, 0xAFu
    };
    static const uint8_t bank0_expected[] = {
        0x20u, 0x79u, 0x00u, 0x80u, 0x2Cu,
        0x6Du, 0x02u, 0x04u, 0x31u
    };

    static const uint8_t bank1_addresses[] = {
        0x4Bu, 0x70u, 0xC2u, 0xC3u, 0xC8u,
        0xC9u, 0xCBu, 0xCCu, 0xCEu
    };
    /*
     * camera_init() applies horizontal flip first, then the QQVGA addon.
     * The later QQVGA addon overwrites bank 1 register C2 with 0x02.
     */
    static const uint8_t bank1_expected[] = {
        0x11u, 0x10u, 0x02u, 0x00u, 0x78u,
        0x00u, 0x04u, 0x00u, 0x00u
    };

    static const uint8_t bank2_addresses[] = {
        0x19u, 0x1Au, 0x23u, 0x24u, 0x27u,
        0x28u, 0x2Du, 0x2Eu, 0xD9u
    };
    /*
     * The QQVGA addon runs after setImageFlip() and leaves bank 2 D9 at 0x01.
     */
    static const uint8_t bank2_expected[] = {
        0x81u, 0x00u, 0x78u, 0x00u, 0x09u,
        0x00u, 0x78u, 0x00u, 0x01u
    };

    static const uint8_t bank4_addresses[] = {0x3Au, 0x3Bu};
    static const uint8_t bank4_expected[] = {0x28u, 0x1Eu};

    printf("\n===== PAG7920LT SENSOR REGISTER READBACK =====\n");
    printf("PAG7920LT_REGISTER_CHECK: values compared with SDK init and EVBTINY flip settings\n");

    dump_pag7920_register_bank(
        0u, bank0_addresses, bank0_expected,
        sizeof(bank0_addresses) / sizeof(bank0_addresses[0]));
    dump_pag7920_register_bank(
        1u, bank1_addresses, bank1_expected,
        sizeof(bank1_addresses) / sizeof(bank1_addresses[0]));
    dump_pag7920_register_bank(
        2u, bank2_addresses, bank2_expected,
        sizeof(bank2_addresses) / sizeof(bank2_addresses[0]));
    dump_pag7920_register_bank(
        4u, bank4_addresses, bank4_expected,
        sizeof(bank4_addresses) / sizeof(bank4_addresses[0]));

    /* Leave the sensor on the default register bank, as SDK init does. */
    colibryBoard.cameraPAG7920LT.writeRegister(0xEFu, 0u);
    colibrySCU->delay_ms(1u);
    printf("PAG7920LT_REGISTER_BANK_RESTORED: 0\n");
    printf("===== END PAG7920LT SENSOR REGISTER READBACK =====\n");
}

static uint32_t psb_width(uint32_t info)
{
    return (info & 0x7FFu) + 1u;
}

static uint32_t psb_height(uint32_t info)
{
    return ((info >> 11) & 0x7FFu) + 1u;
}

static uint32_t psb_channels(uint32_t info)
{
    return ((info >> 22) & 0x3u) + 1u;
}

static void print_pipeline_registers(const char *label)
{
    printf("\n===== PIPELINE REGISTERS: %s =====\n", label);
    printf("CAMIF_PROTOCOL_CFG: 0x%08lx\n",
           (unsigned long)CAMIF_reg->camif_protocol_cfg.word);
    printf("CAMIF_IMG_SIZE_CFG: 0x%08lx\n",
           (unsigned long)CAMIF_reg->camif_img_size_cfg.word);
    printf("CAMIF_IMG_CURR_PTR: 0x%08lx\n",
           (unsigned long)CAMIF_reg->camif_img_curr_ptr.word);
    printf("CAMIF_IMG_COUNT: 0x%08lx\n",
           (unsigned long)CAMIF_reg->camif_img_count.word);
    printf("CAMIF_DATA_FIFO_STATUS: 0x%08lx\n",
           (unsigned long)CAMIF_reg->camif_data_fifo_status.word);
    printf("CAMIF_DATA_FIFO_THR: 0x%08lx\n",
           (unsigned long)CAMIF_reg->camif_data_fifo_thr.word);
    printf("CAMIF_INT_STATUS: 0x%08lx\n",
           (unsigned long)CAMIF_reg->camif_int_status.word);
    printf("ISP_CTRL: 0x%08lx\n",
           (unsigned long)colibryISP->regmap->isp_ctrl.word);
    printf("ISP_STATUS: 0x%08lx\n",
           (unsigned long)colibryISP->regmap->isp_status.word);
    printf("ISP_CROPPER_PSB_INFO: 0x%08lx\n",
           (unsigned long)colibryISP->regmap->isp_cropper_psb_info.word);
    printf("ISP_B2RGB_PSB_INFO: 0x%08lx\n",
           (unsigned long)colibryISP->regmap->isp_b2rgb_psb_info.word);
    printf("ISP_RGB2GRAY_PSB_INFO: 0x%08lx\n",
           (unsigned long)colibryISP->regmap->isp_rgb2gray_psb_info.word);
    printf("ISP_SCALER_PSB_INFO: 0x%08lx\n",
           (unsigned long)colibryISP->regmap->isp_scaler_psb_info.word);
    printf("ISP_SCALER_SCALING_CFG: 0x%08lx\n",
           (unsigned long)colibryISP->regmap->isp_scaler_scaling_cfg.word);
    printf("ISP_SCALER_SIZE_OUT: 0x%08lx\n",
           (unsigned long)colibryISP->regmap->isp_scaler_size_out.word);
    printf("ISP_INT_STATUS: 0x%08lx\n",
           (unsigned long)colibryISP->regmap->isp_int_status.word);
    printf("===== END PIPELINE REGISTERS =====\n");
}

static void print_pipeline_interrupt_flags(void)
{
    const uint32_t isp_status =
        colibryISP->regmap->isp_int_status.word;
    const uint32_t camif_status =
        CAMIF_reg->camif_int_status.word;

    printf("\n===== DECODED PIPELINE INTERRUPT FLAGS =====\n");
    printf("ISP_CROPPER_PSB_ERR: %lu\n",
           (unsigned long)((isp_status >> 0u) & 1u));
    printf("ISP_B2RGB_PSB_ERR: %lu\n",
           (unsigned long)((isp_status >> 1u) & 1u));
    printf("ISP_RGB2GRAY_PSB_ERR: %lu\n",
           (unsigned long)((isp_status >> 2u) & 1u));
    printf("ISP_SCALER_PSB_ERR: %lu\n",
           (unsigned long)((isp_status >> 3u) & 1u));
    printf("ISP_SCALER_PARAM_ERR: %lu\n",
           (unsigned long)((isp_status >> 4u) & 1u));
    printf("ISP_FRAME_START: %lu\n",
           (unsigned long)((isp_status >> 5u) & 1u));

    printf("CAMIF_IMG_OVERFLOW: %lu\n",
           (unsigned long)((camif_status >> 0u) & 1u));
    printf("CAMIF_IMG_UNDERFLOW: %lu\n",
           (unsigned long)((camif_status >> 1u) & 1u));
    printf("CAMIF_IMG_START: %lu\n",
           (unsigned long)((camif_status >> 2u) & 1u));
    printf("CAMIF_IMG_END: %lu\n",
           (unsigned long)((camif_status >> 3u) & 1u));
    printf("CAMIF_DATA_FIFO_READY: %lu\n",
           (unsigned long)((camif_status >> 4u) & 1u));
    printf("CAMIF_DATA_FIFO_OVERFLOW: %lu\n",
           (unsigned long)((camif_status >> 5u) & 1u));
    printf("CAMIF_CDC_FIFO_BYPASS: %lu\n",
           (unsigned long)CAMIF_reg->camif_cdc_fifo_bypass.fields.cdc_fifo_bypass);
    printf("CAMIF_DATA_FIFO_WORDS: %lu\n",
           (unsigned long)CAMIF_reg->camif_data_fifo_status.fields.data_fifo_word_nb);
    printf("CAMIF_DATA_FIFO_THRESHOLD: %lu\n",
           (unsigned long)CAMIF_reg->camif_data_fifo_thr.fields.data_fifo_thr);
    printf("===== END DECODED PIPELINE FLAGS =====\n");
}

/*
 * The model input feature map is unflattened 40x30x1.
 * This follows the NNPA product specification's unflattened IFMAP
 * memory mapping (depth=1, packing_level=3).
 */
static uint8_t read_model_input_pixel(uint32_t x, uint32_t y)
{
    const uint32_t row_incr = (MODEL_INPUT_WIDTH + 1u) / 2u;
    uint32_t mem_row = (y * row_incr) + (x / 2u);
    uint32_t mem_col = ((x + y) % 2u) * 8u;
    mem_col += (mem_row & 7u);
    mem_row >>= 3u;
    mem_row += NNPA_reg->input_start_maddr.word;

    const volatile uint8_t *word =
        (const volatile uint8_t *)(
            NNPAMEM_BASE_ADDR + (mem_row * NNPA_NBYTE_PER_MEM_WORD)
        );
    return word[mem_col];
}

/*
 * Read a bounded sample from the dedicated CPU-readable CAMIF Data FIFO.
 * This FIFO is separate from the CAMIF-to-ISP stream and NNPA input memory.
 * The diagnostic read does not modify the captured NNPA tensor.
 */
static void dump_camif_fifo_samples(void)
{
    const uint32_t available =
        CAMIF_reg->camif_data_fifo_status.fields.data_fifo_word_nb;
    const uint32_t sample_count = (available > 16u) ? 16u : available;
    uint32_t min_value = 0xFFFFFFFFu;
    uint32_t max_value = 0u;

    printf("\n===== CAMIF RAW DATA FIFO SAMPLE =====\n");
    printf("CAMIF_RAW_FIFO_WORDS_BEFORE_READ: %lu\n",
           (unsigned long)available);

    for (uint32_t i = 0u; i < sample_count; ++i) {
        const uint32_t value =
            CAMIF_reg->camif_cam_data.fields.cam_data;
        if (value < min_value) min_value = value;
        if (value > max_value) max_value = value;
        printf("CAMIF_RAW_SAMPLE,%02lu,%03lx\n",
               (unsigned long)i, (unsigned long)value);
    }

    if (sample_count != 0u) {
        printf("CAMIF_RAW_SAMPLE_STATS,min=%lu,max=%lu\n",
               (unsigned long)min_value,
               (unsigned long)max_value);
    } else {
        printf("CAMIF_RAW_SAMPLE_STATS:no_words_available\n");
    }

    printf("CAMIF_RAW_FIFO_WORDS_AFTER_READ: %lu\n",
           (unsigned long)CAMIF_reg->camif_data_fifo_status.fields.data_fifo_word_nb);
    printf("===== END CAMIF RAW DATA FIFO SAMPLE =====\n");
}

static void dump_raw_input_memory(void)
{
    const uint32_t start_word = NNPA_reg->input_start_maddr.word;
    const volatile uint8_t *raw = (const volatile uint8_t *)(
        NNPAMEM_BASE_ADDR + (start_word * NNPA_NBYTE_PER_MEM_WORD)
    );
    uint32_t min_value = 255u;
    uint32_t max_value = 0u;
    uint32_t sum = 0u;

    printf("\n===== RAW NNPA INPUT MEMORY (SEQUENTIAL BYTES) =====\n");
    printf("RAW_INPUT_START_WORD: %lu\n", (unsigned long)start_word);
    printf("RAW_INPUT_BYTES: %u\n", (unsigned int)MODEL_INPUT_BYTES);

    for (uint32_t i = 0u; i < MODEL_INPUT_BYTES; ++i) {
        const uint32_t value = raw[i];
        if (value < min_value) min_value = value;
        if (value > max_value) max_value = value;
        sum += value;

        if ((i % MODEL_INPUT_WIDTH) == 0u) {
            printf("RAW_INPUT_ROW,%02lu,", (unsigned long)(i / MODEL_INPUT_WIDTH));
        }
        printf("%02X", (unsigned int)value);
        if ((i % MODEL_INPUT_WIDTH) == (MODEL_INPUT_WIDTH - 1u)) {
            printf("\n");
        }
    }

    printf("RAW_INPUT_STATS,min=%lu,max=%lu,mean_x100=%lu\n",
           (unsigned long)min_value,
           (unsigned long)max_value,
           (unsigned long)((sum * 100u) / MODEL_INPUT_BYTES));
    printf("===== END RAW NNPA INPUT MEMORY =====\n");
}

static void dump_frame_rows(void)
{
    uint32_t min_value = 255u;
    uint32_t max_value = 0u;
    uint32_t sum = 0u;

    printf("\n===== CAPTURED ISP OUTPUT FRAME =====\n");
    printf("FRAME_FORMAT: 40x30 GRAYSCALE UINT8\n");

    for (uint32_t y = 0; y < MODEL_INPUT_HEIGHT; ++y) {
        printf("FRAME_ROW,%02lu,", (unsigned long)y);
        for (uint32_t x = 0; x < MODEL_INPUT_WIDTH; ++x) {
            uint32_t value = read_model_input_pixel(x, y);
            if (value < min_value) min_value = value;
            if (value > max_value) max_value = value;
            sum += value;
            printf("%02X", (unsigned int)value);
        }
        printf("\n");
    }

    printf("FRAME_STATS,min=%lu,max=%lu,mean_x100=%lu\n",
           (unsigned long)min_value,
           (unsigned long)max_value,
           (unsigned long)((sum * 100u) / MODEL_INPUT_BYTES));
    printf("===== END CAPTURED FRAME =====\n");
}

static int capture_one_frame(void)
{
    uint32_t elapsed;
    uint32_t status;
    uint32_t scaler_info;

    if (NNPA_reg->nnpa_status.fields.input_loading_rdy == 0u) {
        printf("ERROR: NNPA input loader is not ready.\n");
        return 1;
    }

    NNPA_reg->nnpa_int_clear.word =
        EVT_DATA_LOADED_MASK |
        EVT_INPUT_LOADER_OVF_MASK |
        EVT_ISP_SIZE_ERR_MASK;

    /* Clear stale peripheral flags so post-capture status is frame-specific. */
    CAMIF_reg->camif_int_clear.word = 0x3Fu;
    colibryISP->regmap->isp_int_clear.word = 0x3Fu;

    NNPA_reg->nnpa_control.fields.cpu_mem_access = 0u;
    print_pipeline_registers("BEFORE_TRIGGER");

    printf("\nTriggering one PAG7920LT frame...\n");
    colibryBoard.cameraPAG7920LT.trigSingleFrame();

    for (elapsed = 0u; elapsed < FRAME_TIMEOUT_MS; ++elapsed) {
        status = NNPA_reg->nnpa_int_status.word;

        if ((status & EVT_DATA_LOADED_MASK) != 0u) {
            break;
        }

        if ((status & (EVT_INPUT_LOADER_OVF_MASK | EVT_ISP_SIZE_ERR_MASK)) != 0u) {
            NNPA_reg->nnpa_control.fields.cpu_mem_access = 1u;
            printf("ERROR: NNPA input stream error, INT_STATUS=0x%08lx\n",
                   (unsigned long)status);
            print_pipeline_registers("NNPA_STREAM_ERROR");
            print_pipeline_interrupt_flags();
            printf("FRAME_DUMP_SKIPPED: NNPA did not report DATA_LOADED\n");
            return 2;
        }

        colibrySCU->delay_ms(1u);
    }

    NNPA_reg->nnpa_control.fields.cpu_mem_access = 1u;

    if ((NNPA_reg->nnpa_int_status.word & EVT_DATA_LOADED_MASK) == 0u) {
        printf("ERROR: camera frame timeout after %lu ms\n",
               (unsigned long)FRAME_TIMEOUT_MS);
        printf("NNPA_INT_STATUS=0x%08lx\n",
               (unsigned long)NNPA_reg->nnpa_int_status.word);
        printf("ISP_INT_STATUS=0x%08lx\n",
               (unsigned long)colibryISP->regmap->isp_int_status.word);
        printf("CAMIF_INT_STATUS=0x%08lx\n",
               (unsigned long)CAMIF_reg->camif_int_status.word);
        print_pipeline_registers("FRAME_TIMEOUT");
        print_pipeline_interrupt_flags();
        printf("FRAME_DUMP_SKIPPED: NNPA did not report DATA_LOADED\n");
        return 3;
    }

    print_pipeline_registers("AFTER_FRAME");
    scaler_info = colibryISP->isp_scaler_psb_info_get();

    printf("NNPA_DATA_LOADED: %lu bytes (%lu 16-byte words)\n",
           (unsigned long)(NNPA_reg->rd_input_size.word * 16u),
           (unsigned long)NNPA_reg->rd_input_size.word);
    printf("ISP_SCALER_INPUT: %lu x %lu, %lu channel(s)\n",
           (unsigned long)psb_width(scaler_info),
           (unsigned long)psb_height(scaler_info),
           (unsigned long)psb_channels(scaler_info));
    printf("ISP_SCALER_OUTPUT: %u x %u\n",
           (unsigned int)MODEL_INPUT_WIDTH,
           (unsigned int)MODEL_INPUT_HEIGHT);
    printf("ISP_INT_STATUS: 0x%08lx\n",
           (unsigned long)colibryISP->regmap->isp_int_status.word);
    printf("CAMIF_INT_STATUS: 0x%08lx\n",
           (unsigned long)CAMIF_reg->camif_int_status.word);
    printf("NNPA_INPUT_LOADER_LEVEL: %lu\n",
           (unsigned long)NNPA_reg->input_loader_level.word);

    /*
     * DATA_LOADED is set and CPU access is enabled here, so the full
     * 40x30 NNPA input buffer can be dumped for diagnosis. This dump
     * does not assert that the scaler output is valid and is not used
     * to authorize inference.
     */
    print_pipeline_interrupt_flags();
    dump_camif_fifo_samples();
    printf("FRAME_DUMP_NOTE: diagnostic input bytes; validity not yet established\n");
    dump_frame_rows();
    dump_raw_input_memory();

    if (psb_width(scaler_info) != CAMERA_INPUT_WIDTH ||
        psb_height(scaler_info) != CAMERA_INPUT_HEIGHT ||
        psb_channels(scaler_info) != 1u) {
        printf("ERROR: ISP scaler did not receive a 160x120 single-channel frame.\n");
        return 4;
    }

    if ((colibryISP->regmap->isp_int_status.word & 0x1Fu) != 0u) {
        printf("ERROR: ISP reported a processing/PSB error.\n");
        return 5;
    }

    printf("CAMERA_FRAME_CAPTURE: SUCCESS\n");
    NNPA_reg->nnpa_int_clear.word = EVT_DATA_LOADED_MASK;
    return 0;
}

static int run_one_inference(void)
{
    uint32_t elapsed;
    uint32_t output_start;
    uint32_t output_elements;
    volatile uint8_t *output;
    uint8_t logits[CLASS_COUNT];
    uint32_t predicted = 0u;

    NNPA_reg->nnpa_int_clear.word = EVT_EXEC_END_MASK;
    NNPA_reg->nnpa_control.fields.cpu_mem_access = 0u;
    NNPA_reg->nnpa_trig_control.word = RUN_START_MASK;

    for (elapsed = 0u; elapsed < INFERENCE_TIMEOUT_MS; ++elapsed) {
        if (NNPA_reg->nnpa_int_status.fields.evt_exec_end != 0u) {
            break;
        }
        colibrySCU->delay_ms(1u);
    }

    NNPA_reg->nnpa_control.fields.cpu_mem_access = 1u;

    if (NNPA_reg->nnpa_int_status.fields.evt_exec_end == 0u) {
        printf("ERROR: inference timeout after %lu ms\n",
               (unsigned long)INFERENCE_TIMEOUT_MS);
        printf("NNPA_STATUS=0x%08lx\n",
               (unsigned long)NNPA_reg->nnpa_status.word);
        return 1;
    }

    NNPA_reg->nnpa_int_clear.word = EVT_EXEC_END_MASK;

    output_start = NNPA_reg->ofmap_start_maddr.word;
    output_elements =
        (NNPA_reg->rd_ofmap_size.fields.ofmap_width + 1u) *
        (NNPA_reg->rd_ofmap_size.fields.ofmap_height + 1u) *
        (NNPA_reg->rd_ofmap_size.fields.ofmap_depth + 1u);

    if (output_start == 0u || output_elements != CLASS_COUNT) {
        printf("ERROR: unexpected NNPA output layout, start=%lu elements=%lu\n",
               (unsigned long)output_start,
               (unsigned long)output_elements);
        return 2;
    }

    output = (volatile uint8_t *)(
        NNPAMEM_BASE_ADDR + output_start * NNPA_NBYTE_PER_MEM_WORD
    );

    for (uint32_t i = 0u; i < CLASS_COUNT; ++i) {
        logits[i] = output[i];
        if (logits[i] > logits[predicted]) {
            predicted = i;
        }
    }

    printf("\n===== LIVE CAMERA INFERENCE =====\n");
    printf("RAW_UINT8_LOGITS: [%u,%u,%u]\n",
           (unsigned int)logits[0],
           (unsigned int)logits[1],
           (unsigned int)logits[2]);
    printf("PREDICTED_CLASS_ID: %lu\n", (unsigned long)predicted);
    printf("PREDICTED_PEOPLE_COUNT_CLASS: %s\n", class_name(predicted));
    printf("LIVE_FRAME_INFERENCE: SUCCESS\n");
    printf("=================================\n");
    return 0;
}

int main(void)
{
    board_init_cfg_t board_init_cfg = {};
    uint32_t input_bytes;
    uint32_t ifmap_width;
    uint32_t ifmap_height;
    uint32_t ifmap_depth;
    uint32_t ifmap_flattened;
    uint32_t ifmap_pack_level;
    int result;

    COLIBRY_DRIVERS_INIT();

    board_init_cfg.vdd_setting = CHIP_VDD_VOLTAGE;
    board_init_cfg.hfclk_freq_mhz = HFCLK_FREQUENCY_MHZ;
    board_init_cfg.apb_clk_fdiv = APB_CLK_DIV;
    board_init_cfg.nnpa_clk_fdiv = 1u;
    board_init_cfg.spis_enable = 0u;
    board_init_cfg.spim_rf_enable = 0u;
    board_init_cfg.spim_acc_enable = 0u;
    board_init_cfg.no_printf = 0u;

    colibryBoard.global_init(board_init_cfg);

    printf("\n========================================\n");
    printf("OWN COUNT CNN - ONE LIVE CAMERA FRAME\n");
    printf("========================================\n");
    printf("NNPA model bytes: %u\n", (unsigned int)OWN_COUNT_NNPA_SIZE);
    printf("NNPA model SHA-256: %s\n", OWN_COUNT_NNPA_SHA256);
    printf("Expected input: [1,30,40,1] uint8 (1200 bytes)\n");
    printf("External flash: NOT ACCESSED\n");

    printf("\nLoading embedded NNPA model into NNPA SRAM...\n");
    copy_model_to_nnpa_memory();
    if (colibryBoard.check_nnpa_bitstream() != 0u) {
        printf("ERROR: NNPA model configuration check failed.\n");
        return 1;
    }
    colibryBoard.nnpa_allocate_buffers();

    input_bytes = NNPA_reg->rd_input_size.word * NNPA_NBYTE_PER_MEM_WORD;
    ifmap_width = NNPA_reg->rd_ifmap_size.fields.ifmap_width + 1u;
    ifmap_height = NNPA_reg->rd_ifmap_size.fields.ifmap_height + 1u;
    ifmap_depth = NNPA_reg->rd_ifmap_size.fields.ifmap_depth + 1u;
    ifmap_flattened = NNPA_reg->rd_ifmap_info.fields.ifmap_flattened;
    ifmap_pack_level = NNPA_reg->rd_ifmap_info.fields.ifmap_pack_level;

    printf("NNPA model ID: 0x%08lx\n",
           (unsigned long)NNPA_reg->rd_model_id.word);
    printf("NNPA input bytes: %lu\n", (unsigned long)input_bytes);
    printf("NNPA IFMAP: %lu x %lu x %lu, flattened=%lu, pack_level=%lu\n",
           (unsigned long)ifmap_width,
           (unsigned long)ifmap_height,
           (unsigned long)ifmap_depth,
           (unsigned long)ifmap_flattened,
           (unsigned long)ifmap_pack_level);

    if (input_bytes != MODEL_INPUT_BYTES ||
        ifmap_width != MODEL_INPUT_WIDTH ||
        ifmap_height != MODEL_INPUT_HEIGHT ||
        ifmap_depth != 1u ||
        ifmap_flattened != 0u ||
        ifmap_pack_level != 3u) {
        printf("ERROR: NNPA input metadata does not match 40x30x1 unpacked uint8 mapping.\n");
        return 2;
    }

    printf("\nEnabling camera sensor power...\n");
    colibryBoard.sensors_power_enable();

    printf("Initializing PAG7920LT at 160x120...\n");
    colibryBoard.camera_init(
        CAMERA_INPUT_WIDTH,
        CAMERA_INPUT_HEIGHT,
        CAMERA_ONCHIP_MCLK,
        CAMERA_FPS
    );

    if (colibryBoard.camera_model != PAG7920LT) {
        printf("ERROR: Expected PAG7920LT; detected model ID %u.\n",
               (unsigned int)colibryBoard.camera_model);
        return 3;
    }

    diagnose_pag7920_registers();

    /* SDK camera_init configures RAW mode; sensor output is 8-bit. */
    CAMIF_reg->camif_protocol_cfg.fields.pixel_in_width = 8u;

    printf("\nConfiguring ISP for monochrome downscale...\n");
    configure_isp_for_pag7920_mono();

    printf("CAMIF input: 160x120 RAW8\n");
    printf("ISP cropper: bypass\n");
    printf("ISP Bayer2RGB: bypass\n");
    printf("ISP RGB2GRAY: bypass (mono sensor)\n");
    printf("ISP scaler: 4x, output 40x30\n");

    result = capture_one_frame();
    if (result != 0) {
        printf("CAMERA_CAPTURE_RESULT: FAIL (%d)\n", result);
        return 4;
    }

    result = run_one_inference();
    if (result != 0) {
        printf("CAMERA_INFERENCE_RESULT: FAIL (%d)\n", result);
        return 5;
    }

    printf("\nONE LIVE CAMERA FRAME TEST COMPLETED\n");
    printf("External flash was not accessed or modified.\n");

    while (1) {
        NOP();
    }
    return 0;
}
