#include <liscensedinterfaces/basicstringinterface.h>
#include <liscensedinterfaces/loggerinterface.h>
#include <liscensedinterfaces/sberrorx.h>
#include "visibility.h"
#include "FilterWheel.h"

extern "C" EXPORT int sbPlugInName2(BasicStringInterface & str) {
    str = "qhycfw3";
    return SB_OK;
}

extern "C" EXPORT int sbPlugInFactory2(
    const char * pDisplayName,
    const int & instanceNum,
    SerXInterface * pSerial,
    TheSkyXFacadeForDriversInterface * pTheSky,
    SleeperInterface * pSleeper,
    BasicIniUtilInterface * pSettings,
    LoggerInterface * pLogger,
    MutexInterface * pMutex,
    TickCountInterface * pTickCounter,
    void ** ppObjectOut
) {
    try {
        *ppObjectOut = new FilterWheel(
            instanceNum,
            pSerial,
            pTheSky,
            pSleeper,
            pSettings,
            pLogger,
            pMutex,
            pTickCounter
        );

        return SB_OK;
    }
    catch (...) {
        return ERR_CMDFAILED;
    }
}