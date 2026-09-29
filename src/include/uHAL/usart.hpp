#ifndef _uHAL_USART_
#define _uHAL_USART_

#include <cstdint>
#include <cstddef>
#include <span>

#include "uHAL/common.hpp"

namespace uHAL{

struct usart{

    using enum uHAL::bit_state_t;

    enum class channel_t {
        USART_1,
        USART_2,
        USART_6
    };

    // todo RX, irq, calcolo del clock tramite enum?, parity, 9bit word

    channel_t chan;
    uint32_t baud;

    void apply() const;
    bit_state_t read_ready() const;
    bit_state_t tx_done() const;
    bit_state_t tx_ready() const;
    void write_char(char c) const;
    void write_buf(std::span<char> buf) const;

    static usart init(channel_t chan, uint32_t baud);
    static bit_state_t read_ready(channel_t chan);
    static bit_state_t tx_ready(channel_t chan);
    static bit_state_t tx_done(channel_t chan);
    static void write_char(channel_t chan, char c);
    static void write_buf(channel_t chan, std::span<char> buf);

};

}

#endif