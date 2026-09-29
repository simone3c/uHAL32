#include <utility>

#include "uHAL/gpio.hpp"
#include "uHAL/rcc.hpp"

using uHAL::gpio;
using uHAL::LL_register;

// ---- auxiliary functions ----

template<typename T>
concept gpio_property = 
    std::same_as<T, gpio::mode_t> ||
    std::same_as<T, gpio::otype_t> ||
    std::same_as<T, gpio::osped_t> ||
    std::same_as<T, gpio::pupd_t>;

template<gpio_property PROP>
consteval uint32_t get_offset(){
    if(std::is_same_v<PROP, gpio::mode_t>) return 0x00;
    if(std::is_same_v<PROP, gpio::otype_t>) return 0x04;
    if(std::is_same_v<PROP, gpio::osped_t>) return 0x08;
    return 0x0C;
}

// Auxiliry function that calculates the correct bitmask and value to set a "gpio_property"
// based on T and on the interested pins
template<gpio_property T>
static constexpr auto calc_gpio_property(gpio::pin_bitmask_t pins, T prop) {
    uint8_t mask_len = 2;

    if(std::is_same_v<T, gpio::otype_t>) {
        mask_len = 1;
    }

    uint32_t ret_data = 0;
    uint32_t ret_bitmask = 0;
    for (unsigned i = 0; i < sizeof(pins) * 8; ++i) {
        if (pins & uHAL::BIT(i)) {
            ret_data |= std::to_underlying(prop) << (i * mask_len);
            ret_bitmask |= (mask_len == 1 ? 0b01 : 0b11) << (i * mask_len);
        }
    }
    return std::pair<uint32_t, uint32_t>{ret_data, ret_bitmask};
};

// ---- specific registers abstraction ----

// Models 4 different registers within a port (MODER, OTYPER, OSPEEDR, PUPDR)
// which are accessed in the same way.
// 7 different physical registers are available, depending on the port
template<gpio_property PROP>
class property_reg{
    static constexpr uint32_t OFFSET = get_offset<PROP>();
    static constexpr LL_register<GPIOA_BASE + OFFSET> regA{};
    static constexpr LL_register<GPIOB_BASE + OFFSET> regB{};
    static constexpr LL_register<GPIOC_BASE + OFFSET> regC{};
    static constexpr LL_register<GPIOD_BASE + OFFSET> regD{};
    static constexpr LL_register<GPIOE_BASE + OFFSET> regE{};
    static constexpr LL_register<GPIOH_BASE + OFFSET> regH{};

    static constexpr void write(gpio::port_t port, uint32_t bm, uint32_t data){
        using enum gpio::port_t;
        switch (port)
        {
            case A:
            regA.set(data, bm);
            break;
            case B:
            regB.set(data, bm);
            break;
            case C:
            regC.set(data, bm);
            break;
            case D:
            regD.set(data, bm);
            break;
            case E:
            regE.set(data, bm);
            break;
            case H:
            regH.set(data, bm);
            break;
            default:
            std::unreachable();
        }
    };
public:
    static void set(gpio::port_t port, gpio::pin_bitmask_t bitmask, PROP m) {

        const auto [data, bm] = calc_gpio_property(bitmask, m);
        write(port, bm, data);
    }
};

using mode_reg = property_reg<gpio::mode_t>;
using otype_reg = property_reg<gpio::otype_t>;
using ospeed_reg = property_reg<gpio::osped_t>;
using pupd_reg = property_reg<gpio::pupd_t>;

// Models the ODR registers
// 7 different physical registers are available, depending on the port
class odr_reg{
    static constexpr uint32_t OFFSET = 0x14;
    static constexpr LL_register<GPIOA_BASE + OFFSET> regA{};
    static constexpr LL_register<GPIOB_BASE + OFFSET> regB{};
    static constexpr LL_register<GPIOC_BASE + OFFSET> regC{};
    static constexpr LL_register<GPIOD_BASE + OFFSET> regD{};
    static constexpr LL_register<GPIOE_BASE + OFFSET> regE{};
    static constexpr LL_register<GPIOH_BASE + OFFSET> regH{};
/*
    static void write(gpio::port_t port, gpio::pin_bitmask_t bm, uint32_t data){
        using enum gpio::port_t;
        switch (port)
        {
        case A:
            regA.set(data, bm);
            break;
        case B:
            regB.set(data, bm);
            break;
        case C:
            regC.set(data, bm);
            break;
        case D:
            regD.set(data, bm);
            break;
        case E:
            regE.set(data, bm);
            break;
        case H:
            regH.set(data, bm);
            break;
        default:
            std::unreachable();
        }
    };
*/
   
public:
    static uint32_t read(gpio::port_t port, gpio::pin_bitmask_t bm){
        using enum gpio::port_t;
        switch (port)
        {
        case A: return regA.read(bm);
        case B: return regB.read(bm);
        case C: return regC.read(bm);
        case D: return regD.read(bm);
        case E: return regE.read(bm);
        case H: return regH.read(bm);
        default: std::unreachable();
        }
    };
};

// models the BSRR registers
// 7 different physical registers are available, depending on the port
class bsrr_reg{
    static constexpr uint32_t OFFSET = 0x18;
    static constexpr LL_register<GPIOA_BASE + OFFSET> regA{};
    static constexpr LL_register<GPIOB_BASE + OFFSET> regB{};
    static constexpr LL_register<GPIOC_BASE + OFFSET> regC{};
    static constexpr LL_register<GPIOD_BASE + OFFSET> regD{};
    static constexpr LL_register<GPIOE_BASE + OFFSET> regE{};
    static constexpr LL_register<GPIOH_BASE + OFFSET> regH{};

    static constexpr void write(gpio::port_t port, uint32_t bm, uint32_t data){
        using enum gpio::port_t;
        switch (port)
        {
        case A:
            regA.set(data, bm);
            break;
        case B:
            regB.set(data, bm);
            break;
        case C:
            regC.set(data, bm);
            break;
        case D:
            regD.set(data, bm);
            break;
        case E:
            regE.set(data, bm);
            break;
        case H:
            regH.set(data, bm);
            break;
        default:
            std::unreachable();
        }
    };

public:
    static void set(gpio::port_t p, gpio::pin_bitmask_t pins, gpio::pin_level_t lv){
        const uint32_t shift = (lv == gpio::pin_level_t::HIGH ? 0 : 16);
        const uint32_t data = static_cast<uint32_t>(pins) << shift;
        write(p, data, data);
    }

    static void toggle(gpio::port_t port, gpio::pin_bitmask_t pins){
        const uint32_t odr = odr_reg::read(port, pins);
        const uint32_t data = ((odr & pins) << 16) | (~odr & pins);
        write(port, data, data);
    }
};


// alternate funcution register for pins [0, 7]
class afrl{
    static constexpr uint32_t OFFSET = 0x20;
    static constexpr LL_register<GPIOA_BASE + OFFSET> regA{};
    static constexpr LL_register<GPIOB_BASE + OFFSET> regB{};
    static constexpr LL_register<GPIOC_BASE + OFFSET> regC{};
    static constexpr LL_register<GPIOD_BASE + OFFSET> regD{};
    static constexpr LL_register<GPIOE_BASE + OFFSET> regE{};
    static constexpr LL_register<GPIOH_BASE + OFFSET> regH{};

public:
    static void set_altf(gpio::port_t port, gpio::pin_bitmask_t pins, gpio::altf_t altf){
        using enum gpio::port_t;
        
        pins &= 0x00FF; // this register handles pins [0, 7]

        if(pins == 0){
            return;
        }

        constexpr int len = 8;
        uint32_t mask = 0;

        uint32_t data = 0;
        for(int i = 0; i < len; ++i){
            if(pins & (1 << i)){
                data |= std::to_underlying(altf) << (i * 4);
                mask |= 0xF << (i * 4);
            }
        }

        switch (port)
        {
            case A: return regA.set(data, mask);
            case B: return regB.set(data, mask);
            case C: return regC.set(data, mask);
            case D: return regD.set(data, mask);
            case E: return regE.set(data, mask);
            case H: return regH.set(data, mask);
            default: std::unreachable();
        };
    }
};

// alternate funcution register for pins [8, 15]
class afrh{
    static constexpr uint32_t OFFSET = 0x24;
    static constexpr LL_register<GPIOA_BASE + OFFSET> regA{};
    static constexpr LL_register<GPIOB_BASE + OFFSET> regB{};
    static constexpr LL_register<GPIOC_BASE + OFFSET> regC{};
    static constexpr LL_register<GPIOD_BASE + OFFSET> regD{};
    static constexpr LL_register<GPIOE_BASE + OFFSET> regE{};
    static constexpr LL_register<GPIOH_BASE + OFFSET> regH{};

public:
    static void set_altf(gpio::port_t port, gpio::pin_bitmask_t pins, gpio::altf_t altf){
        using enum gpio::port_t;

        constexpr gpio::pin_bitmask_t mask = 0xFF00;
        pins &= mask; // this register handles pins [8, 15]

        if(pins == 0){
            return;
        }

        constexpr int len = 16;

        uint32_t data = 0;
        for(int i = 8; i < len; ++i){
            if(pins & (1 << i)){
                data |= std::to_underlying(altf) << ((i - 8) * 4);
            }
        }

        switch (port)
        {
            case A: return regA.set(data, pins);
            case B: return regB.set(data, pins);
            case C: return regC.set(data, pins);
            case D: return regD.set(data, pins);
            case E: return regE.set(data, pins);
            case H: return regH.set(data, pins);
            default: std::unreachable();
        };
    }
};

// ---- uHAL methods implementation ----

void gpio::apply() const {

    uHAL::rcc::enable_io_port(port);

    mode_reg::set(port, pins, mode);
    otype_reg::set(port, pins, otype);
    ospeed_reg::set(port, pins, ospeed);
    pupd_reg::set(port, pins, pupd);
    set_level(port, pins, pin_level_t::LOW);
}

void gpio::set_level(pin_level_t lv) const {
    set_level(port, pins, lv);
}

void gpio::toggle_level() const {
    toggle_level(port, pins);
}

// --- static methods ---

gpio gpio::create_output(
    port_t port, 
    pin_bitmask_t bitmask,
    otype_t otype, 
    osped_t ospeed
){
    return gpio{
        .pins = bitmask, 
        .port = port, 
        .mode = mode_t::output, 
        .otype = otype, 
        .ospeed = ospeed, 
        .pupd = pupd_t::none
    };
}

void gpio::set_level(port_t port, pin_bitmask_t bitmask, pin_level_t l){
    bsrr_reg::set(port, bitmask, l);
}

void gpio::toggle_level(port_t port, pin_bitmask_t bitmask){
    bsrr_reg::toggle(port, bitmask);
}

void gpio::set_alt_function(port_t port, pin_bitmask_t pins, altf_t altf){
    mode_reg::set(port, pins, gpio::mode_t::alternate);
    afrl::set_altf(port, pins & 0xFF, altf);
    afrl::set_altf(port, pins & 0xFF00, altf);
}