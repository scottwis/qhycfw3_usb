#pragma once

#include <liscensedinterfaces/filterwheeldriverinterface.h>
#include <liscensedinterfaces/x2guiinterface.h>
#include <liscensedinterfaces/modalsettingsdialoginterface.h>
#include <liscensedinterfaces/serialportparams2interface.h>

#include <memory>

class SerXInterface;
class TheSkyXFacadeForDriversInterface;
class SleeperInterface;
class BasicIniUtilInterface;
class LoggerInterface;
class MutexInterface;
class TickCountInterface;

class FilterWheel :
    public FilterWheelDriverInterface,
    public SerialPortParams2Interface,
    public ModalSettingsDialogInterface,
    public X2GUIEventInterface
{
public:
    FilterWheel(
        const int instanceNum,
        SerXInterface * pSerial,
        TheSkyXFacadeForDriversInterface * pTheSky,
        SleeperInterface * pSleeper,
        BasicIniUtilInterface * pSettings,
        LoggerInterface * pLogger,
        MutexInterface * pMutex,
        TickCountInterface * pTickCounter
    );

    FilterWheel() = delete;
    FilterWheel(const FilterWheel & src) = delete;
    FilterWheel(FilterWheel && src) = delete;
    ~FilterWheel() override;

    FilterWheel & operator=(const FilterWheel & rhs) = delete;
    FilterWheel & operator=(FilterWheel && rhs) = delete;

    int queryAbstraction(const char * pszName, void ** ppVal) override;
    void driverInfoDetailedInfo(BasicStringInterface & str) const override;
    double driverInfoVersion() const override;
    void deviceInfoNameShort(BasicStringInterface & str) const override;
    void deviceInfoNameLong(BasicStringInterface & str) const override;
    void deviceInfoDetailedDescription(BasicStringInterface & str) const override;
    void deviceInfoFirmwareVersion(BasicStringInterface & str) override;
    void deviceInfoModel(BasicStringInterface & str) override;
    int establishLink() override;
    int terminateLink() override;
    bool isLinked() const override;
    int filterCount(int & nCount) override;
    int defaultFilterName(const int & nIndex, BasicStringInterface & strFilterNameOut) override;
    int startFilterWheelMoveTo(const int & nTargetPosition) override;
    int isCompleteFilterWheelMoveTo(bool & bComplete) const override;
    int endFilterWheelMoveTo() override;
    int abortFilterWheelMoveTo() override;
    void portName(BasicStringInterface & str) const override;
    void setPortName(const char * szPort) override;
    unsigned int baudRate() const override;
    void setBaudRate(unsigned int i) override;
    bool isBaudRateFixed() const override;
    SerXInterface::Parity parity() const override;
    void setParity(const SerXInterface::Parity & parity) override;
    bool isParityFixed() const override;
    int initModalSettingsDialog() override;
    int execModalSettingsDialog() override;
    void uiEvent(X2GUIExchangeInterface * pUiEx, const char * pszEvent) override;
private:
    class Impl;
    std::shared_ptr<Impl> m_pImpl;
};