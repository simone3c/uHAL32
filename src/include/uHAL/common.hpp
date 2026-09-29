#ifndef _uHAL_utils_
#define _uHAL_utils_

#include <cstdint>

#define my_assert(x) ((x) ? (void)0 : uHAL::assert_failed(__FILE__, __LINE__))

namespace uHAL{

    enum class bit_state_t{
        LOW = 0,
        HIGH = 1
    };

    /**
     * @brief Returns an integer with the n-th bit set to 1
     * 
     * @param n bit to be set to one. 
     * n must be a within [0, 32)
     * 
     * @return uint32_t the bitmask 1 << n
     */
    constexpr uint32_t BIT(unsigned b){
        return (b < 32) ? static_cast<uint32_t>(1) << b : 0;
    }

    /**
     * @brief Abstraction for a generic physical register located at "addr"
     * 
     * @tparam addr address of the physiscal register
     */
    template<uint32_t addr>
    struct LL_register{
        /**
         * @brief Sets the register to val
         * 
         * @param val value to set the register to
         */
        void set(uint32_t val) const{
            *reinterpret_cast<volatile uint32_t*>(addr) = val;
        }

        /**
         * @brief Sets the register to val bit-wise and'ed with bitmask 
         * 
         * @param bitmask Specifies the bits to be changed
         * @param val Specifies the value to set the register to
         */
        void set(uint32_t val, uint32_t bitmask) const{
            *reinterpret_cast<volatile uint32_t*>(addr) &= ~bitmask;
            *reinterpret_cast<volatile uint32_t*>(addr) |= val & bitmask;
        }

        /**
         * @brief Sets the register to zero
         * 
         */
        void clear() const{
            set(0);
        }
        /**
         * @brief Sets to zero only the bits inside the bitmask
         * 
         * @param bitmask 
         */
        void clear(uint32_t bitmask) const{
            *reinterpret_cast<volatile uint32_t*>(addr) &= ~bitmask;
        }

        /**
         * @brief Returns the value of the register
         * 
         * @return uint32_t 
         */
        uint32_t read() const{
            return *reinterpret_cast<volatile uint32_t*>(addr);
        }
        /**
         * @brief Returns the value of the portion of the register specifies by
         * the bitmask
         * 
         * @param bitmask Represents the bits to be read in the register
         * @return uint32_t 
         */
        uint32_t read(uint32_t bitmask) const{
            return *reinterpret_cast<volatile uint32_t*>(addr) & bitmask;
        }
    };

    template<typename Pred>
    inline void busy_wait(Pred&& p){
        while(!p()){
            asm volatile ("nop");
        }
    }

    void assert_failed(const char* file, int line);
}

#endif