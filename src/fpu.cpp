#include <utility>

#include "uHAL/fpu.hpp"
#include "uHAL/common.hpp"
#include "cortex_m4.h"

using uHAL::fpu;
using uHAL::LL_register;
using uHAL::BIT;

class cpacr_reg{

    static constexpr LL_register<FPU> reg{};
public:
    static constexpr void set_cp(fpu::coproc_n n, fpu::coproc_access_mode m){

        uint8_t mod = 0;
        switch(m){
            using enum fpu::coproc_access_mode;
            case denied:
                mod = 0b00;
            break;
            case privileged:
                mod = 0b01;
            break;
            case reserved:
                mod = 0b10;
            break;
            case full:
                mod = 0b11;
            break;
            default: std::unreachable();
        }

        uint32_t mask = BIT(21) | BIT(20);
        uint32_t shift = 20;
        if(n == fpu::coproc_n::cp11){
            mask = BIT(23) | BIT(22);
            shift = 22;
        }

        reg.set(mod << shift, mask); 
    }
};


void fpu::set_coproc_access(coproc_n cp, coproc_access_mode md){
    cpacr_reg::set_cp(cp, md);
}