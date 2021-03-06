#ifndef _FilterWheelDriverInterface_H
#define _FilterWheelDriverInterface_H

#include "driverrootinterface.h"
#include "linkinterface.h"
#include "deviceinfointerface.h"
#include "driverinfointerface.h"
#include "filterwheelmovetointerface.h"

/*!
\brief The FilterWheelDriverInterface allows an X2 implementor to a write X2 filter wheel driver.

\ingroup Driver

See the X2FilterWheel for an example.
*/
class FilterWheelDriverInterface : public DriverRootInterface, public LinkInterface, public HardwareInfoInterface, public DriverInfoInterface, public FilterWheelMoveToInterface 
{
public:
	virtual ~FilterWheelDriverInterface(){}

	/*!\name DriverRootInterface Implementation
	See DriverRootInterface.*/
	//@{ 
	virtual DeviceType							deviceType(void)							  {return DriverRootInterface::DT_FILTERWHEEL;}
	virtual int									queryAbstraction(const char* pszName, void** ppVal)			= 0;
	//@} 

	/*!\name DriverInfoInterface Implementation
	See DriverInfoInterface.*/
	//@{ 
	virtual void								driverInfoDetailedInfo(BasicStringInterface& str) const		= 0;
	virtual double								driverInfoVersion(void) const								= 0;
	//@} 

	/*!\name HardwareInfoInterface Implementation
	See HardwareInfoInterface.*/
	//@{ 
	virtual void deviceInfoNameShort(BasicStringInterface& str) const				= 0;
	virtual void deviceInfoNameLong(BasicStringInterface& str) const				= 0;	
	virtual void deviceInfoDetailedDescription(BasicStringInterface& str) const		= 0;
	virtual void deviceInfoFirmwareVersion(BasicStringInterface& str)				= 0;
	virtual void deviceInfoModel(BasicStringInterface& str)							= 0;
	//@} 

	/*!\name LinkInterface Implementation
	See LinkInterface.*/
	//@{ 
	virtual int									establishLink(void)						= 0;
	virtual int									terminateLink(void)						= 0;
	virtual bool								isLinked(void) const					= 0;
	//@} 

	/*!\name FilterWheelMoveToInterface Implementation
	See FilterWheelMoveToInterface.*/
	//@{ 
	virtual int									filterCount(int& nCount)							= 0;
	virtual int									defaultFilterName(const int& nIndex, BasicStringInterface& strFilterNameOut) = 0;
	virtual int									startFilterWheelMoveTo(const int& nTargetPosition)	= 0;
	virtual int									isCompleteFilterWheelMoveTo(bool& bComplete) const	= 0;
	virtual int									endFilterWheelMoveTo(void)							= 0;
	virtual int									abortFilterWheelMoveTo(void)						= 0;
	//@}

};

#endif