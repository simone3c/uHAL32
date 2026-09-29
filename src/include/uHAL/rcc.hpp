#ifndef _UHAL_RCC_
#define _UHAL_RCC_

#include "common.hpp"
#include "gpio.hpp"
#include "stm32f401xe.h"

namespace uHAL::rcc{
    enum class pll_src_t{
        HSI,
        HSE
    };

    enum class sysclk_src_t{
        HSI = 0b00,
        HSE = 0b01,
        PLL = 0b10,
    };

    enum PRESC_VALUE {
        // unused APB1 = 4, // [1, 2, 4, 8, 16]
        // unused APB2 = 2, // [1, 2, 4, 8, 16]
        PLL_N = 192, // [192, 432]
        PLL_M = 8, // [2, 63]
        PLL_P = 8, // (2, 4, 6, 8)
        PLL_Q = 8, // [2, 15]
        HPRE = 0, // [0, 15]
        PPRE1 = 4, // [0, 7]
    };

    void hse_enable();
    void hse_disable();
    
    void hsi_enable();
    void hsi_disable();

    void pll_set_freq(uint32_t freq_hz);
    void pll_set_source(pll_src_t src);
    void pll_enable();
    void pll_disable();

    void sysclk_set_source(sysclk_src_t src);

    void enable_io_port(gpio::port_t port);
    void disable_io_port(gpio::port_t port);

    void enable_usart2();
    void disable_usart2();

}

#endif