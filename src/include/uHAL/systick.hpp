#ifndef _uHAL_SYSTICK_
#define _uHAL_SYSTICK_

#include <cstdint>

namespace uHAL{
    
/**
 * @brief Contains all the required methods to configure and enable the SysTick
 * timer to generate periodic interrupt requests 
 */
struct systick{
    using tick_t = uint32_t; // max 24 bits: ticks <= 0x00FFFFFF

    /**
     * @brief Represents the type ofthe Systick callback that is going to be 
     * called when the timer triggers an interrupt 
     */
    using callback_t = void(*)(void);

    /**
     * @brief Represents the Systick's status
     */
    enum class status_t{disabled, enabled};

    /**
     * @brief Stores the number of ticks that the systick timer will count before 
     * triggering an interrupt request 
     */
    tick_t reload;

    /**
     * @brief Stores the clock's source for the systick timer.
     * Either the CPU clock / 8 or the full-speed CPU clock
     */
    enum class clksource_t{ahb_8, ahb} clock_source;

    /**
     * @brief Stores wether the systick should raise an interrupt request or not
     * when the counter reaches 0 
     */
    enum class tickint_t{irq_disabled, irq_enabled} irq;

    /**
     * @brief Stores the callback that is executed after an interrupt request
     * 
     */
    callback_t callback{nullptr};

    /**
     * @brief Applies the object's configuration to the physical peripheral
     * 
     */
    void apply() const;

    // ---- static methods ----
    
    /**
     * @brief Factory method that configures the Systick to trigger a periodic
     * interrupt after a fixed amount of milliseconds 
     * 
     * @param us The period for the Systick to expire, expressed in microseconds
     * @param cb The callback function to be executed when the Systick expires
     * @return systick The struct that contains the desired configuration
     */
    static systick period_us(uint32_t us, callback_t cb);

    /**
     * @brief Enables and disables the physical peripheral
     * 
     * @param status Desired Systick's status (DISABLED or ENABLED)
     */
    static void turn_on_off(status_t status);

    /**
     * @brief Sets the reload value directly inside the physical peripheral
     * 
     * @param ticks Counter of ticks for the Systick expiral
     */
    static void set_reload_value(tick_t ticks);

    /**
     * @brief Sets the callback that is going to be called after an interrupt 
     * request is raised
     * 
     * @param cb Function to be executed
     */
    static void set_callback(callback_t cb);

    /**
     * @brief Enables or disables the interrupt request diretly on the physical
     * peripheral
     * 
     * @param tickint New status for the interrupt request configuration
     */
    static void set_irq_on_off(tickint_t tickint);
};

}

#endif