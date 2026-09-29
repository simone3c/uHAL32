#include "uHAL/common.hpp"

void uHAL::assert_failed(const char* file, int line){
    while(4){
        asm volatile("nop");
    }
}