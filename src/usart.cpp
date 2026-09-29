#include "uHAL/usart.hpp"
#include "uHAL/common.hpp"
#include "stm32f401xe.h"
#include "uHAL/gpio.hpp"
#include "uHAL/rcc.hpp"

#include <utility>


using uHAL::LL_register;
using uHAL::bit_state_t;
using uHAL::usart;
using enum uHAL::bit_state_t;

template<uHAL::usart::channel_t CHAN>
class usart_internal{

    static consteval uint32_t get_base(){
        using enum uHAL::usart::channel_t;
        switch(CHAN){
            case USART_1: return USART1_BASE;
            case USART_2: return USART2_BASE;
            case USART_6: return USART6_BASE;
            default: std::unreachable();
        }
    }

    static constexpr uint32_t BASE = get_base();
    static constexpr LL_register<BASE + 0x00> sr_reg{};
    static constexpr LL_register<BASE + 0x04> dr_reg{};
    static constexpr LL_register<BASE + 0x08> brr_reg{};
    static constexpr LL_register<BASE + 0x0C> cr1_reg{};
    static constexpr LL_register<BASE + 0x10> cr2_reg{};
    static constexpr LL_register<BASE + 0x14> cr3_reg{};
    static constexpr LL_register<BASE + 0x18> ctpr_reg{};

    template<typename REG> 
    requires requires (REG r, uint32_t tmp) {
        {r.set(tmp, tmp)} -> std::same_as<void>;
        {r.clear(tmp)} -> std::same_as<void>;
    }
    static constexpr void set_bit(const REG& reg, uint8_t pos, uHAL::bit_state_t v){
        if(v == bit_state_t::HIGH){
            reg.set(1 << pos, uHAL::BIT(pos));
        }
        else{
            reg.clear(uHAL::BIT(pos));
        }
    }

    template<typename REG> 
    requires requires (REG r, uint32_t tmp) {
        {r.read(tmp)} -> std::same_as<uint32_t>;
    }
    static constexpr uHAL::bit_state_t get_bit(const REG& reg, uint8_t mask){
        return reg.read(mask) == 0 ? 
            bit_state_t::LOW : bit_state_t::HIGH;
    }

public:

    static constexpr bit_state_t get_txe_flag(){
        return get_bit(sr_reg, uHAL::BIT(7));
    }
    static constexpr bit_state_t get_tc_flag(){
        return get_bit(sr_reg, uHAL::BIT(6));
    }
    static constexpr bit_state_t get_rxne_flag(){
        return get_bit(sr_reg, uHAL::BIT(5));
    }

    static constexpr uint8_t recv_byte(){
        return dr_reg.read(0x000000FF);
    }
    static constexpr void send_byte(uint8_t b){
        return dr_reg.set(b);
    }

    static constexpr void set_div_mantissa(uint16_t v){
        my_assert(v < (1 << 12));

        return brr_reg.set(v << 4, 0x0000FFF0);
    }
    static constexpr void set_div_fraction(uint8_t v){
        my_assert(v < (1 << 4));

        return brr_reg.set(v, 0x0000000F);
    }

    static constexpr void set_ue(uHAL::bit_state_t v){
        set_bit(cr1_reg, 13, v);
    }
    static constexpr void set_m(uHAL::bit_state_t v){
        set_bit(cr1_reg, 12, v);
    }
    static constexpr void set_te(uHAL::bit_state_t v){
        set_bit(cr1_reg, 3, v);
    }
    static constexpr void set_re(uHAL::bit_state_t v){
        set_bit(cr1_reg, 2, v);
    }

    static constexpr void reset_cr1_reg(){
        cr1_reg.clear();
    }

};

using usart1_internal = usart_internal<uHAL::usart::channel_t::USART_1>;
using usart2_internal = usart_internal<uHAL::usart::channel_t::USART_2>;
using usart6_internal = usart_internal<uHAL::usart::channel_t::USART_6>;

// ---------- uHAL methods implementation -------------

usart usart::init(channel_t chan, uint32_t baud){
    return uHAL::usart{
        .chan = chan,
        .baud = baud,
    };
}

void usart::apply() const {

    constexpr int apb1_clk = 24'000'000;
    
    switch(chan){
    case usart::channel_t::USART_1:


        break;

    case usart::channel_t::USART_2:
        rcc::enable_usart2();
        usart2_internal::reset_cr1_reg();

        // check also thar RCC PORT_A clock for gpios is enabled (?)
        rcc::enable_io_port(gpio::port_t::A);
        gpio::set_alt_function(gpio::port_t::A, gpio::PIN<2> | gpio::PIN<3>, gpio::altf_t::af7);

        uint32_t div = (apb1_clk + baud / 2) / baud;
        usart2_internal::set_div_fraction(div & 0xF);
        usart2_internal::set_div_mantissa((div >> 4) & 0xFFFF);

        //usart2_internal::set_re(bit_state_t::HIGH);
        usart2_internal::set_te(bit_state_t::HIGH);
        usart2_internal::set_ue(bit_state_t::HIGH);

        break;
    }
}

void uHAL::usart::write_buf(std::span<char> buf) const {
    write_buf(chan, buf);
}


void uHAL::usart::write_char(char c) const {
    write_char(chan, c);
}


void uHAL::usart::write_char(uHAL::usart::channel_t chan, char c){
    busy_wait([&](){
        return tx_ready(chan) == bit_state_t::HIGH;
    });

    // todo switch(chan){..}
    usart2_internal::send_byte(c);

    busy_wait([&](){
        return tx_done(chan) == bit_state_t::HIGH;
    });
}

void uHAL::usart::write_buf(uHAL::usart::channel_t chan, std::span<char> buf){
    for(char c : buf){
        write_char(chan, c);
    }
}

bit_state_t uHAL::usart::tx_ready() const {
    return tx_ready(chan);
}

bit_state_t uHAL::usart::tx_ready(uHAL::usart::channel_t chan){
    // todo switch(chan){...}
    return usart2_internal::get_txe_flag();
}

bit_state_t uHAL::usart::tx_done() const {
    return tx_done(chan);
}

bit_state_t uHAL::usart::tx_done(uHAL::usart::channel_t chan){
    // todo switch(chan){...}
    return usart2_internal::get_tc_flag();
}


bit_state_t uHAL::usart::read_ready() const {
    return read_ready(chan);
}

bit_state_t uHAL::usart::read_ready(uHAL::usart::channel_t chan){
    // todo switch(chan){...}
    return usart2_internal::get_rxne_flag();
}