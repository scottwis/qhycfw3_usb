#ifndef _FocuserDriverInterface_H
#define _FocuserDriverInterface_H

#include "driverrootinterface.h"
#include "deviceinfointerface.h"
#include "driverinfointerface.h"
#include "linkinterface.h"
#include "focuser/focusergotointerface2.h"

/*!
\brief The FocuserDriverInterface allows an X2 implementor to a write X2 focuser driver.

\ingroup Driver

See the X2Focuser for an example.
*/
class FocuserDriverInterface : public DriverRootInterface, public LinkInterface, public HardwareInfoInterface, public DriverInfoInterface, public FocuserGotoInterface2
{
public:
	virtual ~FocuserDriverInterface(){}

	/*!\name DriverRootInterface Implementation
	See DriverRootInterface.*/
	//@{ 
	virtual DeviceType							deviceType(void)							  {return DriverRootInterface::DT_FOCUSER;}
	virtual int									queryAbstraction(const char* pszName, void** ppVal) = 0;
	//@} 

	/*!\name DriverInfoInterface Implementation
	See DriverInfoInterface.*/
	//@{ 
	virtual void								driverInfoDetailedInfo(BasicStringInterface& str) const		{};
	virtual double								driverInfoVersion(void) const								{return 0.0;}
	//@} 

	/*!\name HardwareInfoInterface Implementation
	See HardwareInfoInterface.*/
	//@{ 
	virtual void deviceInfoNameShort(BasicStringInterface& str) const				{};
	virtual void deviceInfoNameLong(BasicStringInterface& str) const				{};	
	virtual void deviceInfoDetailedDescription(BasicStringInterface& str) const		{};
	virtual void deviceInfoFirmwareVersion(BasicStringInterface& str)				{};
	virtual void deviceInfoModel(BasicStringInterface& str)							{};
	//@} 

	/*!\name LinkInterface Implementation
	See LinkInterface.*/
	//@{ 
	virtual int									establishLink(void)						= 0;
	virtual int									terminateLink(void)						= 0;
	virtual bool								isLinked(void) const					= 0;
	//@} 

	/*!\name FocuserGotoInterface2 Implementation
	See FocuserGotoInterface2.*/
	virtual int									focPosition(int& nPosition) 			=0;
	virtual int									focMinimumLimit(int& nMinLimit) 		=0;
	virtual int									focMaximumLimit(int& nMaxLimit)			=0; 
	virtual int									focAbort()								=0;

	virtual int								startFocGoto(const int& nRelativeOffset)	= 0;
	virtual int								isCompleteFocGoto(bool& bComplete) const	= 0;
	virtual int								endFocGoto(void)							= 0;

	virtual int								amountCountFocGoto(void) const					= 0;
	virtual int								amountNameFromIndexFocGoto(const int& nZeroBasedIndex, BasicStringInterface& strDisplayName, int& nAmount)=0;
	virtual int								amountIndexFocGoto(void)=0;
	//@}


};

#endif