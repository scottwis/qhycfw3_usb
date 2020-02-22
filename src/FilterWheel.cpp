#include <limits.h>
#include <liscensedinterfaces/basicstringinterface.h>
#include <liscensedinterfaces/basiciniutilinterface.h>
#include <liscensedinterfaces/theskyxfacadefordriversinterface.h>
#include <liscensedinterfaces/sleeperinterface.h>
#include <liscensedinterfaces/loggerinterface.h>
#include <liscensedinterfaces/mutexinterface.h>
#include <liscensedinterfaces/tickcountinterface.h>
#include <liscensedinterfaces/sberrorx.h>
#include <liscensedinterfaces/x2guiinterface.h>
#include <stdio.h>
#include <string>
#include "FilterWheel.h"

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
        moving(false),
        currentCommand(0),
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
        delete pSerial;
        delete pTheSky;
        delete pSleeper;
        delete pSettings;
        delete pLogger;
        delete pMutex;
        delete pTickCounter;
    }

    void loadSettings() {
        X2MutexLocker lock(pMutex);
        char buf[NAME_MAX];
        memset(buf, 0, sizeof(buf));
        pSettings->readString(PARENT_KEY, PORT_KEY, DEF_PORT_NAME, buf, sizeof(buf));
        port = buf;
        numPositions = pSettings->readInt(PARENT_KEY, NUM_POSITIONS_KEY, DEFAULT_NUM_POSITIONS);
    }

    int writeAndWaitForEcho(char c) {
        unsigned long n;
        int rc = pSerial->writeFile(&c, sizeof(c), n);

        if (rc != SB_OK) {
            return rc;
        }

        rc = pSerial->flushTx();

        if (rc != SB_OK) {
            return rc;
        }

        char readBuf = 0;
        rc = pSerial->readFile(&readBuf, sizeof(readBuf), n, 2000);

        if (rc != SB_OK) {
            return rc;
        }

        if (n != 1 || readBuf != c) {
            return ERR_CMDFAILED;
        }

        return SB_OK;
    }

    int write(char c) {
        unsigned long n;
        int rc = pSerial->writeFile(&c, sizeof(c), n);

        if (rc != SB_OK) {
            return rc;
        }

        if (n != sizeof(c)) {
            return ERR_CMDFAILED;
        }
        return SB_OK;
    }

    int read(char c) {
        unsigned long n;
        char buf = 0;
        int rc = pSerial->readFile(&buf, sizeof(buf), n, 2000);

        if (rc != SB_OK) {
            return rc;
        }

        if (n != sizeof(buf) || buf != c) {
            return ERR_CMDFAILED;
        }

        return SB_OK;
    }

    bool linked;
    int numPositions;
    std::string port;
    const int instanceNum;
    bool moving;
    char currentCommand;

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
    m_pImpl->loadSettings();
}

FilterWheel::~FilterWheel() {
}

int FilterWheel::queryAbstraction(const char * pszName, void ** ppVal) {
    X2MutexLocker(m_pImpl->pMutex);

    *ppVal = nullptr;

    if (strcmp(pszName, SerialPortParams2Interface_Name) == 0) {
        *ppVal = dynamic_cast<SerialPortParams2Interface *>(this);
    }

    if (strcmp(pszName, ModalSettingsDialogInterface_Name) == 0) {
        *ppVal = dynamic_cast<ModalSettingsDialogInterface *>(this);
    }

    if (strcmp(pszName, X2GUIEventInterface_Name) == 0) {
        *ppVal = dynamic_cast<X2GUIEventInterface *>(this);
    }
    return SB_OK;
}

void FilterWheel::driverInfoDetailedInfo(BasicStringInterface & str) const {
    str = "none";
}

double FilterWheel::driverInfoVersion() const {
    return 0.1;
}

void FilterWheel::deviceInfoNameShort(BasicStringInterface & str) const {
    str = "QHYCFW3 USB";
}

void FilterWheel::deviceInfoNameLong(BasicStringInterface & str) const {
    str = "QHYCFW3 USB";
}

void FilterWheel::deviceInfoDetailedDescription(BasicStringInterface & str) const {
    str = "filter wheel";
}

void FilterWheel::deviceInfoFirmwareVersion(BasicStringInterface & str) {
    str = "unknown";
}

void FilterWheel::deviceInfoModel(BasicStringInterface & str) {
    str = "qhycfw3";
}

int FilterWheel::establishLink() {
    X2MutexLocker locker(m_pImpl->pMutex);

    int rc = m_pImpl->pSerial->open(m_pImpl->port.c_str());

    if (rc != SB_OK) {
        printf("qhycfw3: establishLink: open '%s' failed: %i\n", m_pImpl->port.c_str(), rc);
        return rc;
    }

    rc = m_pImpl->writeAndWaitForEcho('0');
    if (rc != SB_OK) {
        printf("qhycfw3: establishLink: initial write to device failed: %i\n", rc);
        m_pImpl->pSerial->purgeTxRx();
        m_pImpl->pSerial->close();
        return rc;
    }

    //3. If we have the number of positions set to "auto", try to determine the number of positions.
    if (m_pImpl->numPositions <= 0) {
        printf("qhycfw3: establishLink: searching for the number of filter slots\n");
        int i = 1;
        for (; i < MAX_NUM_POSITIONS; ++i) {
            char expected = toCommand(i);

            rc = m_pImpl->writeAndWaitForEcho(expected);

            if (rc != SB_OK) {
                break;
            }
        }
        m_pImpl->numPositions = i;
        rc = m_pImpl->pSettings->writeInt(PARENT_KEY, NUM_POSITIONS_KEY, m_pImpl->numPositions);

        if (rc != SB_OK) {
            printf("qhycfw3: establishLink: failed to serialize number of positions to settings file: %i\n", rc);
            m_pImpl->pSerial->purgeTxRx();
            m_pImpl->pSerial->close();
            return rc;
        }
        else {
            printf(
                "qhycfw3: establishLink: search completed. number of filter wheel positions == %i\n",
                m_pImpl->numPositions
            );
        }

        rc = m_pImpl->writeAndWaitForEcho('0');
        if (rc != SB_OK) {
            printf("qhycfw3: establishLink: reset to position 0 failed: %i\n", rc);
            return rc;
        }
    }
    else {
        printf("qhycfw3: establishLink: using cached number of filter slots %i\n", m_pImpl->numPositions);
    }

    m_pImpl->linked = true;

    return SB_OK;
}

int FilterWheel::terminateLink() {
    X2MutexLocker locker(m_pImpl->pMutex);

    if (m_pImpl->linked) {
        m_pImpl->pSerial->purgeTxRx();
        m_pImpl->pSerial->close();
        m_pImpl->linked = false;
    }
    return SB_OK;
}

bool FilterWheel::isLinked() const {
    X2MutexLocker locker(m_pImpl->pMutex);
    return m_pImpl->linked;
}

int FilterWheel::filterCount(int & nCount) {
    X2MutexLocker locker(m_pImpl->pMutex);
    nCount = m_pImpl->numPositions;
    return SB_OK;
}

int FilterWheel::defaultFilterName(const int & nIndex, BasicStringInterface & strFilterNameOut) {
    strFilterNameOut = "";
    return SB_OK;
}

int FilterWheel::startFilterWheelMoveTo(const int & nTargetPosition) {
    X2MutexLocker locker(m_pImpl->pMutex);

    if (m_pImpl->moving) {
        return ERR_CMD_IN_PROGRESS_FW;
    }

    if (nTargetPosition < 0 || nTargetPosition >= m_pImpl->numPositions) {
        return ERR_INDEX_OUT_OF_RANGE;
    }
    m_pImpl->currentCommand = toCommand(nTargetPosition);

    int rc = m_pImpl->write(m_pImpl->currentCommand);
    m_pImpl->moving = (rc == SB_OK);
    return rc;
}

int FilterWheel::isCompleteFilterWheelMoveTo(bool & bComplete) const {
    X2MutexLocker locker(m_pImpl->pMutex);

    if (!m_pImpl->moving) {
        return ERR_CMDFAILED;
    }

    int rc = m_pImpl->read(m_pImpl->currentCommand);

    if (rc == SB_OK) {
        bComplete = true;
        return SB_OK;
    }
    bComplete = false;
    return ERR_CMDFAILED;
}

int FilterWheel::endFilterWheelMoveTo() {
    X2MutexLocker locker(m_pImpl->pMutex);
    if (m_pImpl->moving) {
        m_pImpl->moving = false;
        return SB_OK;
    }
    return ERR_CMDFAILED;
}

int FilterWheel::abortFilterWheelMoveTo() {
    return ERR_NOT_IMPL;
}

void FilterWheel::portName(BasicStringInterface & str) const {
    X2MutexLocker lock(m_pImpl->pMutex);
    try {
        str = m_pImpl->port.c_str();
    }
    catch(...) {

    }
}

void FilterWheel::setPortName(const char * szPort) {
    X2MutexLocker lock(m_pImpl->pMutex);
    try {
        m_pImpl->port = szPort;
    }
    catch(...) {

    }
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

char FilterWheel::toCommand(int targetPosition) {
    if (targetPosition < 10) {
        return '0' + targetPosition;
    }
    return 'A' + (targetPosition - 10);
}

int FilterWheel::initModalSettingsDialog() {
    return SB_OK;
}

int FilterWheel::execModalSettingsDialog() {
    X2ModalUIUtil uiutil(this, m_pImpl->pTheSky);

    auto pUi = uiutil.X2UI();
    int rc  = pUi->loadUserInterface("qhycfw3_usb.ui", deviceType(),m_pImpl->instanceNum);

    if (rc != SB_OK) {
        return rc;
    }

    X2MutexLocker locker(m_pImpl->pMutex);
    auto pUiEx = pUi->X2DX();

    if (!pUiEx) {
        return ERR_CMDFAILED;
    }
    pUiEx->setCurrentIndex("comboBox", m_pImpl->numPositions);

    bool ok = false;
    rc = pUi->exec(ok);

    if (rc == SB_OK) {
        return m_pImpl->pSettings->writeInt(PARENT_KEY, NUM_POSITIONS_KEY, m_pImpl->numPositions);
    }
    return rc;
}

void FilterWheel::uiEvent(X2GUIExchangeInterface * pUiEx, const char * pszEvent) {
    if (strcmp(pszEvent, "on_pushButtonOK_clicked") == 0) {
        m_pImpl->numPositions = pUiEx->currentIndex("comboBox");
        if (m_pImpl->numPositions < 0) {
            m_pImpl->numPositions = 0;
        }
    }
}
