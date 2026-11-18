/*
 * Own-dataset compact CNN inference test for Colibry EvalboardTiny.
 *
 * The NNPA bitstream and three 40x30 uint8 input images are embedded in the
 * SRAM firmware image. The bitstream is copied directly into NNPA SRAM.
 * This application does not read or write external flash.
 */

#include <stdint.h>
#include <stdio.h>

#include "chip.h"
#include "colibry_drivers.h"
#include "colibry_board.hpp"
#include "build/own_count_board_data.h"

#define CHIP_VDD_VOLTAGE       0.9f
#define HFCLK_FREQUENCY_MHZ    40
#define APB_CLK_DIV            2

#define OWN_COUNT_CLASS_COUNT  3u
#define OWN_COUNT_TIMEOUT      20000000u

static ColibryBoard colibryBoard;

static void copy_model_to_nnpa_memory(void)
{
    volatile uint32_t *destination =
        (volatile uint32_t *)NNPAMEM_BASE_ADDR;

    NNPA_reg->nnpa_control.fields.cpu_mem_access = 1;

    for (uint32_t i = 0; i < OWN_COUNT_NNPA_WORD_COUNT; ++i) {
        destination[i] = own_count_nnpa_words[i];
    }

    NNPA_reg->nnpa_control.fields.cpu_mem_access = 0;
}

static uint32_t make_input_word(const uint8_t *input, uint32_t word_index)
{
    const uint32_t offset = word_index * 4u;

    return ((uint32_t)input[offset]) |
           ((uint32_t)input[offset + 1u] << 8) |
           ((uint32_t)input[offset + 2u] << 16) |
           ((uint32_t)input[offset + 3u] << 24);
}

static int run_one_sample(uint32_t sample_index)
{
    const uint8_t *input = own_count_test_inputs[sample_index];
    uint32_t input_size_bytes;
    uint32_t output_start;
    uint32_t output_size;
    uint32_t output_width;
    uint32_t output_height;
    uint32_t output_depth;
    uint32_t output_elements;
    uint32_t timeout;
    uint8_t *output;
    uint8_t logits[OWN_COUNT_CLASS_COUNT];
    uint32_t predicted_class = 0;

    printf("\n===== SAMPLE %lu =====\n", (unsigned long)sample_index);
    printf("File: %s\n", own_count_sample_names[sample_index]);
    printf("Expected class: %s\n", own_count_expected_classes[sample_index]);

    input_size_bytes =
        NNPA_reg->rd_input_size.word * NNPA_NBYTE_PER_MEM_WORD;

    if (input_size_bytes != OWN_COUNT_INPUT_BYTES) {
        printf("ERROR: NNPA requires %lu input bytes; test image has %u.\n",
               (unsigned long)input_size_bytes,
               (unsigned int)OWN_COUNT_INPUT_BYTES);
        return 1;
    }

    NNPA_reg->nnpa_control.fields.cpu_mem_access = 1;
    NNPA_reg->input_loader_cfg.word =
        (3u << LOADER_NBYTE_VALID_OFFSET) |
        (0u << LOADER_NBIT_MODE_OFFSET) |
        (0u << LOADER_SIGNED_DATA_OFFSET);

    NNPA_reg->nnpa_trig_control.word = INPUT_LOADER_RST_MASK;

    for (uint32_t i = 0; i < OWN_COUNT_INPUT_BYTES / 4u; ++i) {
        NNPA_reg->input_loader_data.word = make_input_word(input, i);
    }

    NNPA_reg->nnpa_int_clear.word = EVT_EXEC_END_MASK;
    NNPA_reg->nnpa_control.fields.cpu_mem_access = 0;
    NNPA_reg->nnpa_trig_control.word = RUN_START_MASK;

    timeout = OWN_COUNT_TIMEOUT;
    while (NNPA_reg->nnpa_int_status.fields.evt_exec_end == 0u &&
           timeout != 0u) {
        --timeout;
        NOP();
    }

    if (NNPA_reg->nnpa_int_status.fields.evt_exec_end == 0u) {
        NNPA_reg->nnpa_control.fields.cpu_mem_access = 1;
        printf("ERROR: NNPA inference timeout.\n");
        printf("NNPA status: 0x%08lx\n",
               (unsigned long)NNPA_reg->nnpa_status.word);
        return 2;
    }

    NNPA_reg->nnpa_int_clear.word = EVT_EXEC_END_MASK;
    NNPA_reg->nnpa_control.fields.cpu_mem_access = 1;

    output_start = NNPA_reg->ofmap_start_maddr.word;
    if (output_start == 0u) {
        printf("ERROR: NNPA output start address is zero.\n");
        return 3;
    }
    output_size = NNPA_reg->rd_ofmap_size.word;
    output_width = NNPA_reg->rd_ofmap_size.fields.ofmap_width + 1u;
    output_height = NNPA_reg->rd_ofmap_size.fields.ofmap_height + 1u;
    output_depth = NNPA_reg->rd_ofmap_size.fields.ofmap_depth + 1u;
    output_elements = output_width * output_height * output_depth;

    output = (uint8_t *)(
        NNPAMEM_BASE_ADDR +
        NNPA_NBYTE_PER_MEM_WORD * output_start
    );

    printf("Output start word: %lu\n", (unsigned long)output_start);
    printf("Output size register: 0x%08lx\n", (unsigned long)output_size);
    printf("Output shape: [%lu, %lu, %lu]\n",
           (unsigned long)output_height,
           (unsigned long)output_width,
           (unsigned long)output_depth);

    printf("First 16 output bytes:");
    for (uint32_t i = 0; i < 16u; ++i) {
        printf(" %02X", output[i]);
    }
    printf("\n");

    if (output_elements != OWN_COUNT_CLASS_COUNT) {
        printf("ERROR: Expected 3 output values, found %lu.\n",
               (unsigned long)output_elements);
        return 3;
    }

    for (uint32_t i = 0; i < OWN_COUNT_CLASS_COUNT; ++i) {
        logits[i] = output[i];
        if (logits[i] > logits[predicted_class]) {
            predicted_class = i;
        }
    }

    printf("Raw uint8 logits: [%u, %u, %u]\n",
           (unsigned int)logits[0],
           (unsigned int)logits[1],
           (unsigned int)logits[2]);

    printf("Predicted class: %s\n",
           predicted_class == 0u ? "1_person" :
           predicted_class == 1u ? "2_person" : "3_person");

    if (predicted_class == sample_index) {
        printf("HOST LABEL COMPARISON: MATCH\n");
    } else {
        printf("HOST LABEL COMPARISON: MISMATCH\n");
    }

    return 0;
}

int main(void)
{
    board_init_cfg_t board_init_cfg;

    COLIBRY_DRIVERS_INIT();
    colibrySCU->regmap->scu_periph_clk_en.word = 0xFFFFFFFFu;

    board_init_cfg.vdd_setting = CHIP_VDD_VOLTAGE;
    board_init_cfg.hfclk_freq_mhz = HFCLK_FREQUENCY_MHZ;
    board_init_cfg.apb_clk_fdiv = APB_CLK_DIV;
    board_init_cfg.nnpa_clk_fdiv = 1;
    board_init_cfg.spis_enable = 0;
    board_init_cfg.spim_rf_enable = 0;
    board_init_cfg.spim_acc_enable = 0;
    board_init_cfg.spis_dma_channel = 3;
    board_init_cfg.spim0_csn_margin = 100;
    board_init_cfg.no_printf = 0;

    colibryBoard.global_init(board_init_cfg);

    printf("\n============================================\n");
    printf("OWN DATASET - TWO TRAINABLE LAYER CNN TEST\n");
    printf("============================================\n");
    printf("NNPA bytes embedded in SRAM firmware: %u\n",
           (unsigned int)OWN_COUNT_NNPA_SIZE);
    printf("Input tensor: [1, 30, 40, 1] uint8\n");
    printf("Input bytes: %u\n", (unsigned int)OWN_COUNT_INPUT_BYTES);
    printf("Trainable layers: Conv2D + Dense\n");

    printf("\nCopying NNPA model into NNPA SRAM...\n");
    copy_model_to_nnpa_memory();

    if (colibryBoard.check_nnpa_bitstream() != 0u) {
        printf("ERROR: NNPA bitstream configuration check failed.\n");
        return 1;
    }

    colibryBoard.nnpa_allocate_buffers();

    printf("NNPA bitstream loaded and configuration check passed.\n");
    printf("Model ID: 0x%08lx\n",
           (unsigned long)NNPA_reg->rd_model_id.word);
    printf("Model size (16-byte words): %lu\n",
           (unsigned long)NNPA_reg->rd_model_size.word);
    printf("Input size (16-byte words): %lu\n",
           (unsigned long)NNPA_reg->rd_input_size.word);
    printf("Data size (16-byte words): %lu\n",
           (unsigned long)NNPA_reg->rd_data_size.word);
    printf("Input scaling register: 0x%08lx\n",
           (unsigned long)NNPA_reg->rd_input_scaling.word);
    printf("Input offset register: 0x%08lx\n",
           (unsigned long)NNPA_reg->rd_input_offset.word);
    printf("Output scaling register: 0x%08lx\n",
           (unsigned long)NNPA_reg->rd_output_scaling.word);
    printf("Output offset register: 0x%08lx\n",
           (unsigned long)NNPA_reg->rd_output_offset.word);

    USER_LED_ON();
    colibrySCU->delay_ms(150);
    USER_LED_OFF();

    for (uint32_t sample = 0; sample < OWN_COUNT_SAMPLE_COUNT; ++sample) {
        const int result = run_one_sample(sample);
        if (result != 0) {
            printf("CNN TEST FAILED at sample %lu (code %d).\n",
                   (unsigned long)sample, result);
            printf_flush();
            return result;
        }
        printf_flush();
    }

    printf("\n============================================\n");
    printf("ALL THREE NNPA INFERENCE CALLS COMPLETED\n");
    printf("This is a hardware execution test, not an accuracy claim.\n");
    printf("The current dataset validation has weak 2/3-person recall.\n");
    printf("External flash was not accessed or modified by this app.\n");
    printf("============================================\n");
    printf_flush();

    while (1) {
        NOP();
    }

    return 0;
}
