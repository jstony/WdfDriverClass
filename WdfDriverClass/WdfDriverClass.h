#pragma once
#include "public.h"
class WdfDriverClass
{
public:
	WdfDriverClass(void);
	virtual ~WdfDriverClass(void);
	int GetSize(){return m_nSize;}  //获得对象大小
	//驱动基类入口函数,实现代码在main.cpp中
	virtual NTSTATUS DriverEntry(IN PDRIVER_OBJECT  DriverObject, IN PUNICODE_STRING  RegistryPath);

	//////////////////////////////////////////////////////////////////////////
	//增加设备函数
	static NTSTATUS PnpAdd_Common(IN WDFDRIVER  Driver, IN PWDFDEVICE_INIT  DeviceInit);
	virtual NTSTATUS PnpAdd(IN PWDFDEVICE_INIT  DeviceInit);

	//////////////////////////////////////////////////////////////////////////
	//电源相关函数回调
	virtual void InitPnpPwrEvents(WDF_PNPPOWER_EVENT_CALLBACKS*);//初始化PNP设备电源状态回调
	virtual NTSTATUS InitPowerManagement(); //初始化电源管理

	//////////////////////////////////////////////////////////////////////////
	//电源相关事件回调
	static NTSTATUS
		PnpPrepareHardware_common(
		IN WDFDEVICE    Device,
		IN WDFCMRESLIST ResourceList,
		IN WDFCMRESLIST ResourceListTranslated
		);

	virtual NTSTATUS
		PnpPrepareHardware(
		IN WDFCMRESLIST ResourceList,
		IN WDFCMRESLIST ResourceListTranslated
		);

	static NTSTATUS
		PnpReleaseHardware_common(
		IN WDFDEVICE    Device,
		IN WDFCMRESLIST ResourceList
		);		//通用框架，以后类类需要改的话，重载虚函即可

	virtual NTSTATUS
		PnpReleaseHardware(
		IN WDFCMRESLIST ResourceList
		);  //释放设备

	static VOID 
		PnpSurpriseRemove_common(
		IN WDFDEVICE  Device
		);

	virtual VOID PnpSurpriseRemove();

	static NTSTATUS 
		PwrD0Entry_common(
		IN WDFDEVICE  Device, 
		IN WDF_POWER_DEVICE_STATE  PreviousState
		);

	virtual NTSTATUS 
		PwrD0Entry(
		IN WDF_POWER_DEVICE_STATE  PreviousState
		);

	static NTSTATUS 
		PwrD0Exit_common(
		IN WDFDEVICE  Device, 
		IN WDF_POWER_DEVICE_STATE  TargetState
		);

	virtual NTSTATUS 
		PwrD0Exit(
		IN WDF_POWER_DEVICE_STATE  TargetState
		);
	//////////////////////////////////////////////////////////////////////////
	//通过对象 获取设备类
	static WdfDriverClass* GetWdfDriverClassFromDriver(WDFDRIVER Driver);
	static WdfDriverClass* GetWdfDriverClassFromDevice(WDFDEVICE Device);

protected:
	int m_nSize;//记录此驱动类的大小
	WDFDRIVER m_hDriver;
	WDFDEVICE m_hDevice;
	PDRIVER_CONTEXT m_pDriverContext;
	PDEVICE_CONTEXT m_pDeviceContext;
};

