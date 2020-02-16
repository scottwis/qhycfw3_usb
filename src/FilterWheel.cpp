#include <cxxabi.h>
#include <limits.h>
#define UNW_LOCAL_ONLY
#include <libunwind.h>
#include <stdlib.h>

#include <liscensedinterfaces/basicstringinterface.h>
#include <liscensedinterfaces/basiciniutilinterface.h>
#include <liscensedinterfaces/theskyxfacadefordriversinterface.h>
#include <liscensedinterfaces/sleeperinterface.h>
#include <liscensedinterfaces/loggerinterface.h>
#include <liscensedinterfaces/mutexinterface.h>
#include <liscensedinterfaces/tickcountinterface.h>
#include <liscensedinterfaces/sberrorx.h>
#include <stdio.h>
#include "FilterWheel.h"

static const char * DRIVER_NAME = "QHYCFW3 USB X2 Filter Wheel Driver";
static const char * PARENT_KEY = "qhycfw3-usb";
static const char * PORT_KEY = "port";
static const char * NUM_POSITIONS_KEY = "positions";
static const int DEFAULT_NUM_POSITIONS = 0;
static const int MAX_NUM_POSITIONS = 16;

#if defined(_WIN32)
static const char * DEF_PORT_NAME = "COM1";
#elif defined(__APPLE__)
static const char * DEF_PORT_NAME = "/dev/tty.SLAB_USBtoUART";
#elif defined(__linux_)
static const char * DEF_PORT_NAME = "/dev/ttyUSB0";
#endif

class FilterWheel::Impl {
public:
    Impl(
        const int instanceNum,
        SerXInterface * pSerial,
        TheSkyXFacadeForDriversInterface * pTheSky,
        SleeperInterface * pSleeper,
        BasicIniUtilInterface * pSettings,
        LoggerInterface * pLogger,
        MutexInterface * pMutex,
        TickCountInterface * pTickCounter
    ) :
        linked(false),
        numPositions(DEFAULT_NUM_POSITIONS),
        instanceNum(instanceNum),
        pSerial(pSerial),
        pTheSky(pTheSky),
        pSleeper(pSleeper),
        pSettings(pSettings),
        pLogger(pLogger),
        pMutex(pMutex),
        pTickCounter(pTickCounter)
    {
    }

    ~Impl() {
        printf("FilterWheel::Impl::~Impl()\n");
        /*delete pSerial;
        delete pTheSky;
        delete pSleeper;
        delete pSettings;
        delete pLogger;
        delete pMutex;
        delete pTickCounter;*/
    }

    void loadSettings() {
        X2MutexLocker lock(pMutex);

        memset(port, 0, sizeof(port));
        pSettings->readString(PARENT_KEY, PORT_KEY, DEF_PORT_NAME, port, sizeof(port));

        printf("loadSettings: port == %s\n", port);

        //numPositions = pSettings->readInt(PARENT_KEY, NUM_POSITIONS_KEY, DEFAULT_NUM_POSITIONS);
    }

    bool linked;
    int numPositions;
    //TODO: replace with std::string...
    char port[NAME_MAX];
    const int instanceNum;
    SerXInterface * pSerial;
    TheSkyXFacadeForDriversInterface * pTheSky;
    SleeperInterface * pSleeper;
    BasicIniUtilInterface * pSettings;
    LoggerInterface * pLogger;
    MutexInterface * pMutex;
    TickCountInterface * pTickCounter;
};

FilterWheel::FilterWheel(
    const int instanceNum,
    SerXInterface * pSerial,
    TheSkyXFacadeForDriversInterface * pTheSky,
    SleeperInterface * pSleeper,
    BasicIniUtilInterface * pSettings,
    LoggerInterface * pLogger,
    MutexInterface * pMutex,
    TickCountInterface * pTickCounter
) : m_pImpl(std::make_shared<Impl>(
    instanceNum,
    pSerial,
    pTheSky,
    pSleeper,
    pSettings,
    pLogger,
    pMutex,
    pTickCounter
)) {
    printf("FilterWheel::FilterWheel(...) this == %p\n", this);
    m_pImpl->loadSettings();
}

static void backtrace() {
    printf("-----------------------------------------------\n");

    unw_cursor_t cursor;
    unw_context_t context;

    // Initialize cursor to current frame for local unwinding.
    unw_getcontext(&context);
    unw_init_local(&cursor, &context);

    while (unw_step(&cursor) > 0) {
        unw_word_t offset, pc;
        unw_get_reg(&cursor, UNW_REG_IP, &pc);
        if (pc == 0) {
            break;
        }
        printf("0x%px:", (void *)pc);

        char sym[256];
        if (unw_get_proc_name(&cursor, sym, sizeof(sym), &offset) == 0) {
            int status;
            char * pRealName = abi::__cxa_demangle(sym, 0, 0, &status);
            if (status == 0) {
                printf(" (%s", pRealName);
                ::free(pRealName);
            }
            else {
                printf(" (%s", sym);
            }

            printf("+0x%llx)\n", offset);
        } else {
            printf(" -- error: unable to obtain symbol name for this frame\n");
        }
    }

    printf("-----------------------------------------------\n");
}

FilterWheel::~FilterWheel() {
    printf("FilterWheel::~FilterWheel() this == %p\n", this);
    backtrace();
}

int FilterWheel::queryAbstraction(const char * pszName, void ** ppVal) {
    X2MutexLocker(m_pImpl->pMutex);

    printf("queryAbstraction: pszName == %s, ppVal = %p\n", pszName, ppVal);
    if (! ppVal) {
        return SB_OK;
    }

    *ppVal = nullptr;

    if (strcmp(pszName, DriverRootInterface_Name) == 0) {
        *ppVal = dynamic_cast<DriverInfoInterface *>(this);
        return SB_OK;
    }
    else if (strcmp(pszName, LinkInterface_Name) == 0) {
        *ppVal = dynamic_cast<LinkInterface *>(this);
        return SB_OK;
    }
    else if (strcmp(pszName, HardwareInfoInterface_Name) == 0) {
        *ppVal = dynamic_cast<HardwareInfoInterface*>(this);
        return SB_OK;
    }
    else if (strcmp(pszName, DriverInfoInterface_Name)) {
        *ppVal = dynamic_cast<DriverInfoInterface*>(this);
        return SB_OK;
    }
    else if (strcmp(pszName, FilterWheelMoveToInterface_Name) == 0) {
        *ppVal = dynamic_cast<FilterWheelMoveToInterface *>(this);
        return SB_OK;
    }
    else if (strcmp(pszName, SerialPortParams2Interface_Name) == 0) {
        *ppVal = dynamic_cast<SerialPortParams2Interface *>(this);
        return SB_OK;
    }

    return ERR_NOT_IMPL;
}

void FilterWheel::driverInfoDetailedInfo(BasicStringInterface & str) const {
    str = DRIVER_NAME;
}

double FilterWheel::driverInfoVersion(void) const {
    return 1.0;
}

void FilterWheel::deviceInfoNameShort(BasicStringInterface & str) const {
    str = DRIVER_NAME;
}

void FilterWheel::deviceInfoNameLong(BasicStringInterface & str) const {
    str = DRIVER_NAME;
}

void FilterWheel::deviceInfoDetailedDescription(BasicStringInterface & str) const {
    str = DRIVER_NAME;
}

void FilterWheel::deviceInfoFirmwareVersion(BasicStringInterface & str) {
    str = DRIVER_NAME;
}

void FilterWheel::deviceInfoModel(BasicStringInterface & str) {
    str = DRIVER_NAME;
}

int FilterWheel::establishLink(void) {
    X2MutexLocker locker(m_pImpl->pMutex);

    printf("establishLink: attempting to open: %s\n", m_pImpl->port);
    int rc = m_pImpl->pSerial->open(m_pImpl->port);

    if (rc != SB_OK) {
        printf("establishLink: open failed %i\n", rc);
        return rc;
    }

    char buf = '0';
    unsigned long n = 0;

    //1. Write a zero to the device. This tells it to seek to position 0.
    printf("establishLink: trying initial write\n" );
    rc = m_pImpl->pSerial->writeFile((void *)&buf, sizeof(buf), n);

    if (rc != SB_OK) {
        printf("establishLink: initial write failed: %i\n", rc);
        m_pImpl->pSerial->purgeTxRx();
        m_pImpl->pSerial->close();
        return rc;
    }

    printf("establishLink: flushing send buffer\n");
    rc = m_pImpl->pSerial->flushTx();

    if (rc != SB_OK) {
        printf("establishLink: flushing send buffer failed: %i\n", rc);
        m_pImpl->pSerial->purgeTxRx();
        m_pImpl->pSerial->close();
    }
    //2. Wait for up to 2 seconds to get a 0 back. This means we have successfully seeked to position 0. If we don't
    //get back a success, close the serial port and return a failure code.
    buf = 0;
    rc = m_pImpl->pSerial->readFile((void *)&buf, sizeof(buf), n, 2000);

    if (rc != SB_OK || n != 1) {
        printf("establishLink: read after initial write failed: %i\n", rc);
        m_pImpl->pSerial->purgeTxRx();
        m_pImpl->pSerial->close();
        return rc;
    }

    printf("establishLink: initial read succeeded\n");

    //3. If we have the number of positions set to "auto", try to determine the number of positions.
    if (m_pImpl->numPositions <= 0) {
        int i = 0;
        for (; i < MAX_NUM_POSITIONS; ++i) {
            if (i <= 9) {
                buf = '0' + i;
            }
            else {
                buf = 'A' + (i - 10);
            }
            printf("establishLink: writing value '%c'\n", buf);

            rc = m_pImpl->pSerial->writeFile((void *)&buf, sizeof(buf), n);
            if (rc != SB_OK) {
                printf("establishLink: writing value %c failed: %i\n", buf, rc);
                m_pImpl->pSerial->purgeTxRx();
                m_pImpl->pSerial->close();
                return rc;
            }

            rc = m_pImpl->pSerial->flushTx();
            if (rc != SB_OK) {
                printf(
                    "establishLink: flushing the write buffer failed. Stopping search for number of devices: %i. i == %i\n",
                    rc,
                    i
                );
                break;
            }

            rc = m_pImpl->pSerial->readFile((void *)&buf, sizeof(buf), n, 1000);
            if (rc != SB_OK || n != 1) {
                printf(
                    "establishLink: reading from the device failed. Stopping search for number of devices: %i. i == %i\n",
                    rc,
                    i
                );
                break;
            }
        }
        m_pImpl->numPositions = i;
        rc = m_pImpl->pSettings->writeInt(PARENT_KEY, NUM_POSITIONS_KEY, m_pImpl->numPositions);

        if (rc != SB_OK) {
            printf("establishLink: call to pSettings->writeInt failed: %i\n", rc);
            m_pImpl->pSerial->purgeTxRx();
            m_pImpl->pSerial->close();
            return rc;
        }
    }

    printf("establishLink: returning true. Num positions == %i\n", m_pImpl->numPositions);
    m_pImpl->linked = true;

    return SB_OK;
}

int FilterWheel::terminateLink(void) {
    X2MutexLocker locker(m_pImpl->pMutex);

    if (m_pImpl->linked) {
        printf("terminateLink: terminating link\n");
        m_pImpl->pSerial->purgeTxRx();
        m_pImpl->pSerial->close();
        m_pImpl->linked = false;
    }
    else {
        printf("terminateLink: not linked");
    }
    return SB_OK;
}

bool FilterWheel::isLinked(void) const {
    X2MutexLocker locker(m_pImpl->pMutex);
    return m_pImpl->linked;
}

int FilterWheel::filterCount(int & nCount) {
    X2MutexLocker locker(m_pImpl->pMutex);
    return m_pImpl->numPositions;
}

int FilterWheel::defaultFilterName(const int & nIndex, BasicStringInterface & strFilterNameOut) {
    strFilterNameOut = "";
    return SB_OK;
}

int FilterWheel::startFilterWheelMoveTo(const int & nTargetPosition) {
    return ERR_NOT_IMPL;
}

int FilterWheel::isCompleteFilterWheelMoveTo(bool & bComplete) const {
    return ERR_NOT_IMPL;
}

int FilterWheel::endFilterWheelMoveTo(void) {
    return ERR_NOT_IMPL;
}

int FilterWheel::abortFilterWheelMoveTo(void) {
    return ERR_NOT_IMPL;
}

void FilterWheel::portName(BasicStringInterface & str) const {
    printf("portName\n");
    X2MutexLocker lock(m_pImpl->pMutex);
    str = m_pImpl->port;
}

void FilterWheel::setPortName(const char * szPort) {
    printf("setPortName: %s\n", szPort);
    X2MutexLocker lock(m_pImpl->pMutex);

    strncpy(m_pImpl->port, szPort, sizeof(m_pImpl->port));
}

unsigned int FilterWheel::baudRate() const {
    return 9600;
}

void FilterWheel::setBaudRate(unsigned int i) {
}

bool FilterWheel::isBaudRateFixed() const {
    return true;
}

SerXInterface::Parity FilterWheel::parity() const {
    return SerXInterface::B_NOPARITY;
}

void FilterWheel::setParity(const SerXInterface::Parity & parity) {
}

bool FilterWheel::isParityFixed() const {
    return true;
}
