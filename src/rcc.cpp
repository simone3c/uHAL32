#include <utility>

#include "uHAL/rcc.hpp"
#include "uHAL/gpio.hpp"

#include "uHAL/common.hpp"
#include "cortex_m4.h"

using uHAL::LL_register;
using uHAL::BIT;

enum class io_port_state_t{
    DISABLED = 0,
    ENABLED = 1, 
};

class cr_reg{
    static constexpr LL_register<RCC_BASE> reg{};
    
public:
    enum class clk_state_t{
        OFF = 0, 
        ON = 1
    };
    enum class clk_ready_t{
        NOT_READY = 0,
        READY = 1, 
    };

    static constexpr void clk_set_state(uHAL::rcc::sysclk_src_t clk, clk_state_t status){
        using enum uHAL::rcc::sysclk_src_t;
        uint8_t shift;
        switch(clk){
            case HSI: 
                shift = 0;
                break;
            case HSE: 
                shift = 16;
                break;
            case PLL: 
                shift = 24;
                break;
            default: std::unreachable();
        }

        reg.set(std::to_underlying(status) << shift, uHAL::BIT(shift));
    }

    static constexpr clk_ready_t get_clk_ready_flag(uHAL::rcc::sysclk_src_t clk){
        using enum uHAL::rcc::sysclk_src_t;
        uint32_t shift;
        switch(clk){
            case HSI: 
                shift = 1;
                break;
            case HSE:
                shift = 17;
                break;
            case PLL:
                shift = 25;
                break;
            default: std::unreachable();
        }

        return reg.read(uHAL::BIT(shift)) == 0 ? 
            clk_ready_t::NOT_READY : 
            clk_ready_t::READY;
    }
};

class pllcfgr_reg{
    static constexpr LL_register<RCC_BASE + 0x4> reg{};

public:
    enum class hse_state_t{OFF, ON};

    static constexpr void pll_set_src(uHAL::rcc::pll_src_t v){
        // todo "This bit can be written only when PLL and PLLI2S are disabled."

        if (v == uHAL::rcc::pll_src_t::HSI){
            reg.clear(uHAL::BIT(22));
        }
        else{
            reg.set(1 << 22, uHAL::BIT(22));
        }
    }

    static constexpr void pllq_set(uint32_t v){
        // todo "These bits should be written only if PLL is disabled."
        using uHAL::BIT;
        my_assert(v >= 2 && v <= 16);

        reg.set(v << 24, BIT(27) | BIT(26) | BIT(25) | BIT(24));
    }

    static constexpr void pllm_set(uint32_t v){
        using uHAL::BIT;
        my_assert(v >= 2 && v <= 63);

        reg.set(v, BIT(5) | BIT(4) | BIT(3) | BIT(2) | BIT(1) | BIT(0));
    }

    static constexpr void plln_set(uint32_t v){
        // todo " These bits can be written only if PLL is disabled."
        using uHAL::BIT;
        my_assert(v >= 192 && v <= 432);

        reg.set(v << 6, BIT(14) | BIT(13) | BIT(12) | BIT(11) | BIT(10) | BIT(9) | 
            BIT(8) | BIT(7) | BIT(6));
    }

    static constexpr void pllp_set(uint16_t v){
        using uHAL::BIT;
        my_assert(v == 2 || v == 4 || v == 6 || v == 8);

        reg.set(((v >> 1) - 1) << 16, BIT(17) | BIT(16));
    }

};

class cfgr_reg{
    
    static constexpr LL_register<RCC_BASE + 0x08> reg{};
public:
    enum class sysclock_ready_t{READY, NOT_READY};

    static constexpr void sysclock_switch_set(uHAL::rcc::sysclk_src_t src){
        reg.set(std::to_underlying(src), uHAL::BIT(0) | uHAL::BIT(1));
    }

    static constexpr uHAL::rcc::sysclk_src_t get_sysclock_switch_status(){
        volatile uHAL::rcc::sysclk_src_t tmp = static_cast<uHAL::rcc::sysclk_src_t>(reg.read(BIT(3) | BIT(2)) >> 2);
        
        return tmp;
    }

    static constexpr void hpre_set(uint16_t v){
        using uHAL::BIT;
        my_assert(v <= 15);

        reg.set(v << 4, BIT(7) | BIT(6) | BIT(5) | BIT(4));
    }

    static constexpr void ppre1_set(uint16_t v){
        using uHAL::BIT;
        my_assert(v <= 7);

        reg.set(v << 10, BIT(12) | BIT(11) | BIT(10));
    }
};

class ahb1enr_reg{
    
    static constexpr LL_register<RCC_BASE + 0x30> reg{};
    
public:

// todo refactor by using a bit_state_t
    static constexpr void set_io_port(uHAL::gpio::port_t port, io_port_state_t s){
        using enum uHAL::gpio::port_t;
        uint32_t pos = std::to_underlying(port);
        
        if(s == io_port_state_t::ENABLED){
            reg.set(1 << pos, uHAL::BIT(pos));
        } 
        else{
            reg.clear(uHAL::BIT(pos));
        }            
    }
};

class apb1enr_reg{
    static constexpr LL_register<RCC_BASE + 0x40> reg{};
public:
    static constexpr void set_usart2en(uHAL::bit_state_t v){
        reg.set(std::to_underlying(v) << 17, uHAL::BIT(17));
    }
};

// uHAL functions

void uHAL::rcc::hse_enable(){
    using enum uHAL::rcc::sysclk_src_t;
    cr_reg::clk_set_state(
        HSE, 
        cr_reg::clk_state_t::ON
    );

    busy_wait([&]{
        return cr_reg::get_clk_ready_flag(HSE) == cr_reg::clk_ready_t::READY;
    });
}

void uHAL::rcc::hse_disable(){
    // todo his bit cannot be cleared if the HSE is used directly or indirectly as the system clock
    using enum uHAL::rcc::sysclk_src_t;

    cr_reg::clk_set_state(HSE, cr_reg::clk_state_t::OFF);
    // todo wait ?
}

void uHAL::rcc::hsi_enable(){
    using enum uHAL::rcc::sysclk_src_t;

    cr_reg::clk_set_state(HSI, cr_reg::clk_state_t::ON);

    busy_wait([&]{
        return cr_reg::get_clk_ready_flag(HSI) == cr_reg::clk_ready_t::READY;
    });
}

void uHAL::rcc::hsi_disable(){
    // todo his bit cannot be cleared if the HSI is used directly or indirectly as the system clock
    using enum uHAL::rcc::sysclk_src_t;
    cr_reg::clk_set_state(HSI, cr_reg::clk_state_t::OFF);
    // todo wait ?
}

// ! assuming freq_hz = 48 MHz for now, then it could be useful to provide some predefined frequency
void uHAL::rcc::pll_set_freq(uint32_t freq_hz){
    //todo these bits can be written only when PLL is disabled
/*
    VCO input frequency = PLL input clock frequency (16 MHz assumed) / PLLM with 2 ≤ PLLM ≤ 63
    VCO output frequency = VCO input frequency × PLLN with 192 ≤ PLLN ≤ 432
    PLL output clock frequency (system clock) = VCO output frequency / PLLP with PLLP = 2, 4, 6, or 8
    USB OTG FS clock frequency = VCO output frequency / PLLQ with 2 ≤  PLLQ ≤  15
    Core frequency = PLL output clock frequency / 8
    AHB clock = SYSCLK / AHB prescaler
    APBx clock = AHB clock / APBx prescaler
    
    PAG. 94 datasheet

    NOTE:
    USB requires 48 MHz
    PLL OUTPUT freq (system clock) <= 84 MHz
    192 MHz <= VCO OUT freq <= 432 MHz
    
    (PLLM)  The software has to set these bits correctly to ensure that the VCO input frequency 
    ranges from 1 to 2 MHz. It is recommended to select a frequency of 2 MHz to limit 
    PLL jitter.

*/
    // my_assert(freq_hz <= 84'000'000);
    my_assert(freq_hz == 48); // useless, just for remembering that clk is 48MHz until I add support for other freqs

    /*
        with the values of the enum:
        VCO input frequency = PLL input clock frequency (16 MHz assumed from HSI) / 8 = 2 MHz
        VCO output frequency = VCO input frequency × 192 = 384 MHz
        PLL output clock frequency (system clock) = VCO output frequency / 8 = 48 Mhz
        USB OTG FS clock frequency = VCO output frequency / 8 = 48 Mhz
        AHB clock = SYSCLK / AHB = 48 / 1
        APB1 clock = AHB clock / APB1 prescaler = 48 / 2 = 24 MHz (max speed here is 24?)
    */

    pllcfgr_reg::pllm_set(PLL_M);
    pllcfgr_reg::plln_set(PLL_N);
    pllcfgr_reg::pllq_set(PLL_Q);
    pllcfgr_reg::pllp_set(PLL_P);
    // todo Caution: The clocks are divided with the new prescaler factor from 1 to 16 AHB cycles after HPRE write
    cfgr_reg::hpre_set(HPRE);
    cfgr_reg::ppre1_set(PPRE1);
    

}

void uHAL::rcc::pll_set_source(pll_src_t src){
    pllcfgr_reg::pll_set_src(src);
}

void uHAL::rcc::pll_enable(){
    using enum uHAL::rcc::sysclk_src_t;
    cr_reg::clk_set_state(PLL, cr_reg::clk_state_t::ON);
    busy_wait([&]{
        return cr_reg::get_clk_ready_flag(PLL) == cr_reg::clk_ready_t::READY;
    });
}

void uHAL::rcc::pll_disable(){
    // todo his bit cannot be cleared if the HSI is used directly or indirectly as the system clock

    using enum uHAL::rcc::sysclk_src_t;
    cr_reg::clk_set_state(PLL, cr_reg::clk_state_t::OFF);
    // todo wait ? 
}

void uHAL::rcc::sysclk_set_source(sysclk_src_t src){
    cfgr_reg::sysclock_switch_set(src);
    busy_wait([&]{
        return cfgr_reg::get_sysclock_switch_status() == src;
    });
}

void uHAL::rcc::enable_io_port(gpio::port_t port){
    ahb1enr_reg::set_io_port(port, io_port_state_t::ENABLED);
}
void uHAL::rcc::disable_io_port(gpio::port_t port){
    ahb1enr_reg::set_io_port(port, io_port_state_t::DISABLED);

}

void uHAL::rcc::enable_usart2(){
    apb1enr_reg::set_usart2en(uHAL::bit_state_t::HIGH);
}
void uHAL::rcc::disable_usart2(){
    apb1enr_reg::set_usart2en(uHAL::bit_state_t::LOW);
}