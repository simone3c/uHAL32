#include "uHAL/gpio.hpp"
#include "uHAL/systick.hpp"
#include "uHAL/rcc.hpp"
#include "uHAL/flash.hpp"
#include "uHAL/fpu.hpp"
#include "uHAL/usart.hpp"

#include <stdio.h>
#include <utility>

static inline void spin(uint32_t count) {
    while (count--)  __asm__ volatile ("nop");
}

void systick_cb(void){
    uHAL::gpio::toggle_level(uHAL::gpio::port_t::A, uHAL::gpio::PIN<5>);
}

static void system_clock_init(void){
    uHAL::fpu::set_coproc_access(uHAL::fpu::coproc_n::cp10, uHAL::fpu::coproc_access_mode::full);
    uHAL::fpu::set_coproc_access(uHAL::fpu::coproc_n::cp11, uHAL::fpu::coproc_access_mode::full);

    uHAL::flash::enable_data_cache();
    uHAL::flash::enable_instruction_cache();
    uHAL::flash::set_latency(5);

    // uHAL::rcc::hse_enable(); // ! no HSE soldered on the board, it uses 8MHz signal from ST-LINK
    uHAL::rcc::hsi_enable();
    uHAL::rcc::pll_set_source(uHAL::rcc::pll_src_t::HSI);
    uHAL::rcc::pll_set_freq(48); // todo argument is currently useless
    uHAL::rcc::pll_enable();
    uHAL::rcc::sysclk_set_source(uHAL::rcc::sysclk_src_t::PLL);
}       

int main(void) {
    system_clock_init();
    
    uHAL::gpio led = uHAL::gpio::create_output(
        uHAL::gpio::port_t::A, 
        uHAL::gpio::PIN<5> // todo test | PIN<6>
    );
    led.apply();

    // auto sys = uHAL::systick::period_us(200000, systick_cb);
    // sys.apply();

    auto uart = uHAL::usart::init(
        uHAL::usart::channel_t::USART_2,
        115200
    );
    uart.apply();

    while(4){
        led.toggle_level();

        printf("prova\n");
        
        spin(500000);
    }
}
