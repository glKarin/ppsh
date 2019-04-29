#include "../src/log.h"

using namespace KMPlayer;

#define TEST_INFO(x)                                         \
   debugLog() << "[01;33mINFO:[01;36m " << #x << "[00m" << endl

#define TEST_VERIFY(x)                                       \
    do {                                                     \
        if ( x )                                             \
            debugLog() << "[01;32mPASSED ";                \
        else                                                 \
            debugLog() << "[01;31mFAILED ";                \
        debugLog() << "[00mVerify '" << #x << "'" << endl; \
    } while (false)
