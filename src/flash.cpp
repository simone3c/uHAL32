#include "uHAL/flash.hpp"
#include "uHAL/common.hpp"
#include "stm32f401xe.h"

using uHAL::LL_register;
using uHAL::BIT;
using uHAL::flash;

class flash_acr_reg{
    static constexpr LL_register<FLASH_ACR_BYTE0_ADDRESS> reg{};

public:
    enum class status_t{enable, disable};

    static constexpr void set_dcen(status_t s){
        reg.set((s == status_t::enable ? 1 : 0) << 10, BIT(10));
    }

    static constexpr void set_icen(status_t s){
        reg.set((s == status_t::enable ? 1 : 0) << 9, BIT(9));
    }

    static constexpr void set_latency(uint8_t l){
        my_assert(l <= 15);
        reg.set(l, BIT(0) | BIT(1) | BIT(2) | BIT(3));
    }

};
struct flash_keyr_reg{};
struct flash_optkeyr_reg{};
struct flash_sr_reg{};
struct flash_cr_reg{};
struct flash_optcr_reg{};



void flash::set_latency(uint8_t l){
    flash_acr_reg::set_latency(l);
}
void flash::enable_data_cache(){
    flash_acr_reg::set_dcen(flash_acr_reg::status_t::enable);
}
void flash::disable_data_cache(){
    flash_acr_reg::set_dcen(flash_acr_reg::status_t::disable);
}
void flash::enable_instruction_cache(){
    flash_acr_reg::set_icen(flash_acr_reg::status_t::enable);
}
void flash::disable_instruction_cache(){
    flash_acr_reg::set_icen(flash_acr_reg::status_t::disable);
}