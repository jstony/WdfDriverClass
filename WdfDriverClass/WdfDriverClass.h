#pragma once
#include "public.h"
class WdfDriverClass
{
public:
	WdfDriverClass(void);
	virtual ~WdfDriverClass(void);
	int GetSize(){return m_nSize;}  //��ö����С
	//����������ں���,ʵ�ִ�����main.cpp��
	virtual NTSTATUS DriverEntry(IN PDRIVER_OBJECT  DriverObject, IN PUNICODE_STRING  RegistryPath);

	//////////////////////////////////////////////////////////////////////////
	//�����豸����
	static NTSTATUS PnpAdd_Common(IN WDFDRIVER  Driver, IN PWDFDEVICE_INIT  DeviceInit);
	virtual NTSTATUS PnpAdd(IN PWDFDEVICE_INIT  DeviceInit);

	//////////////////////////////////////////////////////////////////////////
	//��Դ��غ����ص�
	virtual void InitPnpPwrEvents(WDF_PNPPOWER_EVENT_CALLBACKS*);//��ʼ��PNP�豸��Դ״̬�ص�
	virtual NTSTATUS InitPowerManagement(); //��ʼ����Դ����

	//////////////////////////////////////////////////////////////////////////
	//��Դ����¼��ص�
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
		);		//ͨ�ÿ�ܣ��Ժ�������Ҫ�ĵĻ��������麯����

	virtual NTSTATUS
		PnpReleaseHardware(
		IN WDFCMRESLIST ResourceList
		);  //�ͷ��豸

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
	//ͨ������ ��ȡ�豸��
	static WdfDriverClass* GetWdfDriverClassFromDriver(WDFDRIVER Driver);
	static WdfDriverClass* GetWdfDriverClassFromDevice(WDFDEVICE Device);

protected:
	int m_nSize;//��¼��������Ĵ�С
	WDFDRIVER m_hDriver;
	WDFDEVICE m_hDevice;
	PDRIVER_CONTEXT m_pDriverContext;
	PDEVICE_CONTEXT m_pDeviceContext;
};

