#ifndef _uHAL_FLASH_
#define _uHAL_FLASH_

#include <cstdint>

namespace uHAL{

struct flash{
    static void set_latency(uint8_t l);

    static void enable_data_cache();
    static void disable_data_cache();

    static void enable_instruction_cache();
    static void disable_instruction_cache();
};

}
#endif