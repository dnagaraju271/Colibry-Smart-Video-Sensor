/*
 * Minimal Colibry EvalboardTiny LED blinker.
 *
 * Uses the board initialization and USER_LED_ON/OFF macros provided by
 * the ASYGN Colibry SDK. The board LED is active-low.
 */

#include "chip.h"
#include "colibry_drivers.h"
#include "colibry_board.hpp"

#define CHIP_VDD_VOLTAGE       0.9
#define HFCLK_FREQUENCY_MHZ    40
#define APB_CLK_DIV            2

ColibryBoard colibryBoard;

int main(void)
{
    COLIBRY_DRIVERS_INIT();

    board_init_cfg_t board_init_cfg;
    board_init_cfg.vdd_setting = CHIP_VDD_VOLTAGE;
    board_init_cfg.hfclk_freq_mhz = HFCLK_FREQUENCY_MHZ;
    board_init_cfg.apb_clk_fdiv = APB_CLK_DIV;
    board_init_cfg.nnpa_clk_fdiv = 1;
    board_init_cfg.spis_enable = 0;
    board_init_cfg.spim_rf_enable = 0;
    board_init_cfg.spim_acc_enable = 0;
    board_init_cfg.spis_dma_channel = 0;
    board_init_cfg.spim0_csn_margin = 100;

    colibryBoard.global_init(board_init_cfg);

    while (1) {
        USER_LED_ON();
        colibrySCU->delay_ms(300);

        USER_LED_OFF();
        colibrySCU->delay_ms(300);
    }

    return 0;
}
