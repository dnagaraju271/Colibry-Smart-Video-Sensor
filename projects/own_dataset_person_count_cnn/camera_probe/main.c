/*
 * SRAM-only camera sensor detection probe for Colibry EvalboardTiny.
 *
 * Powers the camera sensor rail, initializes SDK camera auto-detection and
 * reports the identified sensor. It does not capture frames, run NNPA,
 * or access external flash.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "chip.h"
#include "colibry_drivers.h"
#include "colibry_board.hpp"

#define CHIP_VDD_VOLTAGE       0.9f
#define HFCLK_FREQUENCY_MHZ    40u
#define APB_CLK_DIV            2u

#define CAMERA_OUTPUT_WIDTH    160u
#define CAMERA_OUTPUT_HEIGHT   120u
#define CAMERA_TEST_FPS        1u
#define CAMERA_ONCHIP_MCLK     1u

static ColibryBoard colibryBoard;

static const char *camera_name(camera_model_t model)
{
    switch (model) {
    case EIS316:
        return "EIS316";
    case HM0360:
        return "HM0360";
    case PAG7920LT:
        return "PAG7920LT";
    default:
        return "UNKNOWN";
    }
}

int main(void)
{
    board_init_cfg_t board_init_cfg = {};

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
    printf("COLIBRY CAMERA SENSOR DETECTION PROBE\n");
    printf("========================================\n");
    printf("Mode: SRAM-only camera initialization\n");
    printf("NNPA inference: disabled\n");
    printf("External flash: not accessed\n");

    printf("\nEnabling camera sensor power rail...\n");
    colibryBoard.sensors_power_enable();

    printf("Starting SDK camera auto-detection...\n");
    printf("Requested CAMIF output: %u x %u at %u FPS\n",
           (unsigned int)CAMERA_OUTPUT_WIDTH,
           (unsigned int)CAMERA_OUTPUT_HEIGHT,
           (unsigned int)CAMERA_TEST_FPS);

    /*
     * SDK camera_init checks EIS316, HM0360 and PAG7920LT IDs,
     * configures camera clock/startup and sets QQVGA for 160x120.
     * If no supported module responds, the SDK prints an error and exits.
     */
    colibryBoard.camera_init(CAMERA_OUTPUT_WIDTH,
                             CAMERA_OUTPUT_HEIGHT,
                             CAMERA_ONCHIP_MCLK,
                             CAMERA_TEST_FPS);

    printf("\nCAMERA_DETECTION_RESULT: %s\n",
           camera_name(colibryBoard.camera_model));
    printf("CAMERA_INITIALIZATION: SUCCESS\n");
    printf("FRAME_CAPTURE: NOT STARTED\n");
    printf("NNPA_INFERENCE: NOT STARTED\n");
    printf("EXTERNAL_FLASH: UNTOUCHED\n");
    printf("\nCamera sensor detection probe is complete.\n");

    while (1) {
        NOP();
    }

    return 0;
}
