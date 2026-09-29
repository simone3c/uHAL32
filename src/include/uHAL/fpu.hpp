#ifndef _uHAL_SCB_
#define _uHAL_SCB_

#include "uHAL/common.hpp"
namespace uHAL{

struct fpu{
    enum class coproc_n{
        cp10,
        cp11
    };
    enum class coproc_access_mode{
        denied,
        privileged,
        reserved,
        full
    };
    static void set_coproc_access(coproc_n cp, coproc_access_mode md);
};
}

#endif