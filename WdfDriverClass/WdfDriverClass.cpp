#include "WdfDriverClass.h"
#include "NewDelete.h"   //����ָ�룬��������������������Դ

REGISTER_WDFDRIVER_CLASS_NULL()
WdfDriverClass::WdfDriverClass(void)
{
	m_nSize = sizeof(WdfDriverClass); //��ȡ��С
	m_pDriverContext = NULL;
	m_pDeviceContext = NULL; //��ָֹ����ָ
	KdPrint(("PCIEBase")); //���ں˵���������һ����Ϣ
}


WdfDriverClass::~WdfDriverClass(void)
{
}

WdfDriverClass* WdfDriverClass::GetWdfDriverClassFromDriver(WDFDRIVER Driver)
{
	PDRIVER_CONTEXT pContext = GetDriverContext(Driver);
	return (WdfDriverClass*)pContext->par1;
}

WdfDriverClass* WdfDriverClass::GetWdfDriverClassFromDevice(WDFDEVICE Device)
{
	PDEVICE_CONTEXT pContext = GetDeviceContext(Device);
	return (WdfDriverClass*)pContext->par1;
}

//�豸����ʱ��Ӧ
NTSTATUS WdfDriverClass::PnpAdd_Common(IN WDFDRIVER Driver, IN PWDFDEVICE_INIT DeviceInit)
{
	//����1 Driver Ϊ������� ��������

	PDRIVER_CONTEXT pContext = GetDriverContext(Driver);		//����public.h�к궨��ĺ���ʵ�ֶ�DRIVER_CONTEXT�ṹ��ָ��Ļ�ȡ
	WdfDriverClass* pThis = (WdfDriverClass*)pContext->par1;
	return pThis->PnpAdd(DeviceInit);
}

//���õ�Դ����¼��ص�
void WdfDriverClass::InitPnpPwrEvents(WDF_PNPPOWER_EVENT_CALLBACKS* pPnpPowerCallbacks)
{
	pPnpPowerCallbacks->EvtDevicePrepareHardware = PnpPrepareHardware_common; // ��ʼ��
	pPnpPowerCallbacks->EvtDeviceReleaseHardware = PnpReleaseHardware_common; // ֹͣ
	pPnpPowerCallbacks->EvtDeviceSurpriseRemoval = PnpSurpriseRemove_common;  // �쳣�Ƴ�
	pPnpPowerCallbacks->EvtDeviceD0Entry	= PwrD0Entry_common; // ����D0��Դ״̬������״̬����������β��롢���߻���
	pPnpPowerCallbacks->EvtDeviceD0Exit = PwrD0Exit_common;  // �뿪D0��Դ״̬������״̬�����������߻��豸�Ƴ�
}

// �����൱��WDM�е�AddDevice������
// ����PNP�������׵���屻���õĻص���������Ҫ�����Ǵ����豸����
// �豸������ϵͳ���������ʽ�������ⲿ��ĳ������Ӳ���豸��
NTSTATUS WdfDriverClass::PnpAdd(IN PWDFDEVICE_INIT DeviceInit)
{
	NTSTATUS status;
	WDFDEVICE device;
	int nInstance = 0;
	PDEVICE_CONTEXT pContext = NULL;
	WDF_OBJECT_ATTRIBUTES attributes;
	WDF_PNPPOWER_EVENT_CALLBACKS pnpPowerCallbacks;
	WDF_DEVICE_PNP_CAPABILITIES pnpCapabilities;
	WCHAR	enumeratorName[64] = {0};
	ULONG	returnSize;
	UNICODE_STRING	unicodeEnumName, temp;
	WDF_OBJECT_ATTRIBUTES  attributesForRequests;

	//PAGED_CODE(); //PAGED_CODE��������ȷ�������߳�������һ�������ҳ���㹻��IRQL����
	// Get the device enumerator name to detect whether the device being
	// added is a PCI device or root-enumerated non pnp ISA device.
	// It's okay to WDM functions when there is no appropriate WDF
	// interface is available.
	//
	status = WdfFdoInitQueryProperty(DeviceInit,
		DevicePropertyEnumeratorName,		//��ȡ�豸����
		sizeof(enumeratorName),
		enumeratorName,
		&returnSize);

	if(!NT_SUCCESS(status)){
		return status;
	}

	WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributesForRequests, REQUEST_CONTEXT);
	WdfDeviceInitSetRequestAttributes(DeviceInit, &attributesForRequests);//����Ĭ�϶������ԣ������Խ������ڴӿ�ܶ����г��е�IO Request ����

	RtlInitUnicodeString(&unicodeEnumName, enumeratorName);
	RtlInitUnicodeString(&temp, L"PCI");
	if(RtlCompareUnicodeString(&unicodeEnumName, &temp, TRUE) != 0) {
		//
		// It's not a PCI device.
		//
		ASSERTMSG("Unknown device", FALSE); //����豸û��ʶ��
		return STATUS_DEVICE_CONFIGURATION_ERROR;  //�����豸��֤����
	}
	//�豸��֤�ɹ�
	WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&pnpPowerCallbacks);  //��ʼ����Դ�ص��ṹ��
	InitPnpPwrEvents(&pnpPowerCallbacks);  //���õ�Դ�ص�
	WdfDeviceInitSetPnpPowerEventCallbacks(DeviceInit, &pnpPowerCallbacks);

}

// �����豸�����ĵ�Դ������
NTSTATUS WdfDriverClass::InitPowerManagement()
{
	NTSTATUS status = STATUS_SUCCESS;
	return status;
}

NTSTATUS WdfDriverClass::PnpPrepareHardware_common( 
	IN WDFDEVICE Device, 
	IN WDFCMRESLIST ResourceList, 
	IN WDFCMRESLIST ResourceListTranslated 
	)
{
	WdfDriverClass* pThis = WdfDriverClass::GetWdfDriverClassFromDevice(Device);
	ASSERT(pThis);
	return pThis->PnpPrepareHardware(ResourceList, ResourceListTranslated);
}

NTSTATUS WdfDriverClass::PnpPrepareHardware(IN WDFCMRESLIST ResourceList, IN WDFCMRESLIST ResourceListTranslated)
{
	NTSTATUS status;
	PDEVICE_CONTEXT pContext = NULL; 
	ULONG ulNumRes = 0;
	ULONG ulIndex;
	CM_PARTIAL_RESOURCE_DESCRIPTOR*  pResDes = NULL;

	//
	// The Resources collection is not used for PCI devices, since the PCI
	// bus driver manages the device's PCI Base Address Registers.
	//

	UNREFERENCED_PARAMETER( ResourceList );//�������������δ���ò����ľ���


	pContext = GetDeviceContext(m_hDevice);

	// �����豸
	//status = ConfigureUsbDevice();
	if(!NT_SUCCESS(status))
		return status;

	// ��ȡPipe���
	//status = GetUsbPipes();
	if(!NT_SUCCESS(status))
		return status;

	// ��ʼ��Դ���ԣ�
	status = InitPowerManagement();

	// ��ϵͳ��Դ�б����ǲ���ʵ���ԵĲ�����������ӡһЩ��Ϣ��
	// ������ȫ���԰��������Щ���붼ע�͵���
	ulNumRes = WdfCmResourceListGetCount(ResourceList);
	KDBG(DPFLTR_INFO_LEVEL, "ResourceListCount:%d\n", ulNumRes);

	// ���������������µ�ö����Щϵͳ��Դ������ӡ�����ǵ����������Ϣ��
	for(ulIndex = 0; ulIndex < ulNumRes; ulIndex++)
	{
		pResDes = WdfCmResourceListGetDescriptor(ResourceList, ulIndex);		
		if(!pResDes){
			return STATUS_DEVICE_CONFIGURATION_ERROR;
		}

		switch (pResDes->Type) 
		{
		case CmResourceTypeMemory:
			KDBG(DPFLTR_INFO_LEVEL, "System Resource��CmResourceTypeMemory\n");
			break;

		case CmResourceTypePort:
			KDBG(DPFLTR_INFO_LEVEL, "System Resource��CmResourceTypePort\n");
			break;

		case CmResourceTypeInterrupt:
			KDBG(DPFLTR_INFO_LEVEL, "System Resource��CmResourceTypeInterrupt\n");
			break;

		default:
			KDBG(DPFLTR_INFO_LEVEL, "System Resource��Others %d\n", pResDes->Type);
			break;
		}
	}

	return STATUS_SUCCESS;
}

NTSTATUS WdfDriverClass::PnpReleaseHardware_common(IN WDFDEVICE    Device,IN WDFCMRESLIST ResourceList)
{
	WdfDriverClass* pThis = WdfDriverClass::GetWdfDriverClassFromDevice(Device);
	ASSERT(pThis);
	return pThis->PnpReleaseHardware(ResourceList);
}

// �˺���������WDM�е�PNP_MN_STOP_DEVICE���������豸�Ƴ�ʱ�����á�
// ��������������ʱ���豸�Դ��ڹ���״̬��
NTSTATUS WdfDriverClass::PnpReleaseHardware(IN WDFCMRESLIST ResourceListTranslated)
{
	KDBG(DPFLTR_INFO_LEVEL, "[PnpReleaseHardware]");

	// ���PnpPrepareHardware����ʧ��,UsbDeviceΪ�գ�
	// ��ʱ��ֱ�ӷ��ؼ��ɡ�
/*	if (m_hUsbDevice == NULL)
		return STATUS_SUCCESS;

	// ȡ��USB�豸������IO��������������ȡ������Pipe��IO������
	WdfIoTargetStop(WdfUsbTargetDeviceGetIoTarget(m_hUsbDevice), WdfIoTargetCancelSentIo);

	// Deconfiguration���ߡ������á�
	WDF_USB_DEVICE_SELECT_CONFIG_PARAMS  configParams;
	WDF_USB_DEVICE_SELECT_CONFIG_PARAMS_INIT_DECONFIG(&configParams);
	return WdfUsbTargetDeviceSelectConfig(m_hUsbDevice, WDF_NO_OBJECT_ATTRIBUTES, &configParams);*/
}

VOID WdfDriverClass::PnpSurpriseRemove_common( IN WDFDEVICE  Device)
{
	WdfDriverClass* pThis = WdfDriverClass::GetWdfDriverClassFromDevice(Device);
	ASSERT(pThis);
	pThis->PnpSurpriseRemove();
}

VOID WdfDriverClass::PnpSurpriseRemove()
{
}

NTSTATUS WdfDriverClass::PwrD0Entry_common(IN WDFDEVICE  Device, 	IN WDF_POWER_DEVICE_STATE  PreviousState)
{
	WdfDriverClass* pThis = WdfDriverClass::GetWdfDriverClassFromDevice(Device);
	ASSERT(pThis);
	return pThis->PwrD0Entry(PreviousState);
}

//��������Power�ص�����WDM�е�PnpSetPower���ơ�
NTSTATUS WdfDriverClass::PwrD0Entry(IN WDF_POWER_DEVICE_STATE  PreviousState)
{
	KDBG(DPFLTR_INFO_LEVEL, "[PwrD0Entry] from %s", PowerName(PreviousState));
	return STATUS_SUCCESS;
}

// �뿪D0״̬
NTSTATUS WdfDriverClass::PwrD0Exit(IN WDF_POWER_DEVICE_STATE  TargetState)
{
	KDBG(DPFLTR_INFO_LEVEL, "[PwrD0Exit] to %s", PowerName(TargetState));
	return STATUS_SUCCESS;
}

NTSTATUS WdfDriverClass::PwrD0Exit_common(IN WDFDEVICE  Device, IN WDF_POWER_DEVICE_STATE  PreviousState)
{
	WdfDriverClass* pThis = WdfDriverClass::GetWdfDriverClassFromDevice(Device);
	ASSERT(pThis);
	return pThis->PwrD0Exit(PreviousState);
}

// �豸����
NTSTATUS WdfDriverClass::ConfigurePcieDevice()
{

}

char* PowerName(WDF_POWER_DEVICE_STATE PowerState)
{
	char *name;

	switch(PowerState)
	{
	case WdfPowerDeviceInvalid :	
		name = "PowerDeviceUnspecified"; 
		break;
	case WdfPowerDeviceD0:			
		name = "WdfPowerDeviceD0"; 
		break;
	case WdfPowerDeviceD1:			
		name = "WdfPowerDeviceD1"; 
		break;
	case WdfPowerDeviceD2:		
		name = "WdfPowerDeviceD2"; 
		break;
	case WdfPowerDeviceD3:		
		name = "WdfPowerDeviceD3"; 
		break;
	case WdfPowerDeviceD3Final:	
		name = "WdfPowerDeviceD3Final";
		break;
	case WdfPowerDevicePrepareForHibernation:									
		name = "WdfPowerDevicePrepareForHibernation"; 
		break;
	case WdfPowerDeviceMaximum:		
		name = "WdfPowerDeviceMaximum";
		break;
	default:					
		name = "Unknown Power State";
		break;
	}

	return name;
}