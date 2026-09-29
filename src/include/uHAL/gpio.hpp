#ifndef _uHAL_GPIO_
#define _uHAL_GPIO_

#include <cstdint>

#include "common.hpp"
#include "stm32f401xe.h"

namespace uHAL{

    /**
     * @brief Contains all the required methods to configure and use the General Purpose Input/Output
     * pins available in the system 
     */
    struct gpio{

        // todo separate enum class deinition from fields
        
        using pin_bitmask_t = uint16_t;

        /**
         * @brief Indicates the output level of a pin
         * 
         */
        enum class pin_level_t{LOW, HIGH};

        /**
         * @brief Number of pins available for each port
         * 
         */
        static constexpr short PORT_SZ = 16;
        
        /**
         * @brief Useful constant for selecting specific a specific pin within a port
         * 
         * @tparam n Specifies the pin index within a port. Ranged within [0, PORT_SZ)
         */
        template <unsigned n>
        requires (n < PORT_SZ)
        static constexpr pin_bitmask_t PIN = BIT(n);
        
        /**
         * @brief Contains the bitmask to select the specific pins within the selected port
         * 
         */
        pin_bitmask_t pins{0};

        /**
         * @brief Specifies the port (A, B, ..., E, H) to act on
         * 
         */
        enum class port_t{
            A = 0, 
            B = 1, 
            C = 2, 
            D = 3, 
            E = 4, 
            H = 7
        } port{port_t::A};

        /**
         * @brief Specifies the operative mode of the selected pins
         * 
         */
        enum class mode_t{input, output, alternate, analog} mode{mode_t::input};

        /**
         * @brief Specifies, in case of output mode, the output type
         * 
         */
        enum class otype_t{push_pull, open_drain} otype{otype_t::push_pull};

        /**
         * @brief Specifies, in case of output mode, the output speed
         * 
         */
        enum class osped_t{low, medium, high, very_high} ospeed{osped_t::low};

        /**
         * @brief Specifies, in case of output input, the pin pull-up/pull-down resistors configuration
         * 
         */
        enum class pupd_t{none, pull_up, pull_down} pupd{pupd_t::none};
        
        // TODO alternative func e lock e irq
        enum class altf_t{
            af0 = 0, 
            af7 = 7
            // todo ...
        };

        altf_t altf{altf_t::af0};

        /**
         * @brief Applies the object's configuration to the physical peripheral
         * 
         */
        void apply() const;

        /**
         * @brief Sets the pin's level on the physical port
         * 
         * @param lv Desired level to set up. Either LOW or HIGH
         */
        void set_level(pin_level_t lv) const;

        /**
         * @brief Toggles the pin's level on the physical port
         * 
         */
        void toggle_level() const;

        // ---- static methods ----

        /**
         * @brief Factory method to create an output pin
         * 
         * @param port Port to be used (A, B, ..., E, H)
         * @param bitmask A bitmask where a "1" bits select which pins to configure within the port. Many pins can be bit-wise or'ed
         * @param otype Specifies the output mode (Defults to PUSH_PULL)
         * @param ospeed Specifies the output speed (Defaults to LOW)
         * @return GPIO A struct that contains the configuration parameters
         */
        static gpio create_output(
            port_t port,
            pin_bitmask_t bitmask, 
            otype_t otype = otype_t::push_pull, 
            osped_t ospeed = osped_t::low
        );

        /**
         * @brief Sets the pin output level on the specified (port, pin) pair
         * 
         * @param port Port to be used (A, B, ..., E, H)
         * @param bitmask A bitmask where a "1" bits select which pins to configure within the port. Many pins can be bit-wise or'ed
         * @param lv Specifies the desired output level
         */
        static void set_level(port_t port, pin_bitmask_t bitmask, pin_level_t lv);
        
        /**
         * @brief Toggles the output level of the specified (port, pin) pair
         * 
         * @param port Port to be used (A, B, ..., E, H)
         * @param bitmask A bitmask where a "1" bits select which pins to configure within the port. Many pins can be bit-wise or'ed
         */
        static void toggle_level(port_t port, pin_bitmask_t bitmask);

        static void set_alt_function(port_t port, pin_bitmask_t bitmask, altf_t a);


    };
}

#endif