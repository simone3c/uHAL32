#ifndef _CORTEX_M4_
#define _CORTEX_M4_


#include <cstdint>

// --- MEMORY MAPPED PERIPHERALS STRUCT ----

typedef struct {

    volatile uint32_t CTRL;
    volatile uint32_t LOAD;
    volatile uint32_t VAL;
    volatile uint32_t CALIB;

} systick_typedef;
constexpr uint32_t SYSTICK = 0xE000E010;

// --- FPU (Floating Point Unit) Register Map ---
struct fpu_typedef {
    volatile uint32_t CPACR;    // Coprocessor access control register (For FPU)
    volatile uint32_t FPCCR;
    volatile uint32_t FPCAR;
    volatile uint32_t FPDSCR;
    volatile uint32_t FPSCR;
};
constexpr uint32_t FPU = 0xE000ED88;

#endif