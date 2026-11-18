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
#define INFERENCE_TIMEOUT_MS    3000u
#define CLASS_COUNT             3u

static ColibryBoard colibryBoard;
static IspDriver isp;

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
     * PAG7920LT is configured by the SDK in QQVGA mode (160x120).
     * The sensor delivers a single RAW grayscale channel, so Bayer2RGB
     * and RGB2GRAY stay bypassed. Only the scaler is enabled.
     */
    isp.regmap->isp_ctrl.word =
        ISP_CROPPER_BYPASS_MASK |
        ISP_BAYER2RGB_BYPASS_MASK |
        ISP_RGB2GRAY_BYPASS_MASK;

    /* 4.8 fixed-point ratio: 4.0 == 0x400. */
    isp.isp_scaler_scaling_x_set(0x400u);
    isp.isp_scaler_scaling_y_set(0x400u);
    isp.isp_scaler_size_out_x_set(MODEL_INPUT_WIDTH);
    isp.isp_scaler_size_out_y_set(MODEL_INPUT_HEIGHT);

    /* Clear stale status flags before first frame. */
    isp.regmap->isp_int_clear.word = 0x3Fu;
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

    NNPA_reg->nnpa_control.fields.cpu_mem_access = 0u;

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
               (unsigned long)isp.regmap->isp_int_status.word);
        printf("CAMIF_INT_STATUS=0x%08lx\n",
               (unsigned long)CAMIF_reg->camif_int_status.word);
        return 3;
    }

    scaler_info = isp.isp_scaler_psb_info_get();

    printf("CAMERA_FRAME_CAPTURE: SUCCESS\n");
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
           (unsigned long)isp.regmap->isp_int_status.word);
    printf("CAMIF_INT_STATUS: 0x%08lx\n",
           (unsigned long)CAMIF_reg->camif_int_status.word);
    printf("NNPA_INPUT_LOADER_LEVEL: %lu\n",
           (unsigned long)NNPA_reg->input_loader_level.word);

    dump_frame_rows();
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
