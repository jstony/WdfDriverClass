#include "WdfDriverClass.h"
#include "NewDelete.h"   //重载指针，用于驱动对象中申请资源

REGISTER_WDFDRIVER_CLASS_NULL()
WdfDriverClass::WdfDriverClass(void)
{
	m_nSize = sizeof(WdfDriverClass); //获取大小
	m_pDriverContext = NULL;
	m_pDeviceContext = NULL; //防止指针乱指
	KdPrint(("PCIEBase")); //向内核调试器发送一条消息
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

//设备插入时响应
NTSTATUS WdfDriverClass::PnpAdd_Common(IN WDFDRIVER Driver, IN PWDFDEVICE_INIT DeviceInit)
{
	//参数1 Driver 为输入参数 驱动对象

	PDRIVER_CONTEXT pContext = GetDriverContext(Driver);		//利用public.h中宏定义的函数实现对DRIVER_CONTEXT结构体指针的获取
	WdfDriverClass* pThis = (WdfDriverClass*)pContext->par1;
	return pThis->PnpAdd(DeviceInit);
}

//设置电源相关事件回调
void WdfDriverClass::InitPnpPwrEvents(WDF_PNPPOWER_EVENT_CALLBACKS* pPnpPowerCallbacks)
{
	pPnpPowerCallbacks->EvtDevicePrepareHardware = PnpPrepareHardware_common; // 初始化
	pPnpPowerCallbacks->EvtDeviceReleaseHardware = PnpReleaseHardware_common; // 停止
	pPnpPowerCallbacks->EvtDeviceSurpriseRemoval = PnpSurpriseRemove_common;  // 异常移除
	pPnpPowerCallbacks->EvtDeviceD0Entry	= PwrD0Entry_common; // 进入D0电源状态（工作状态），比如初次插入、或者唤醒
	pPnpPowerCallbacks->EvtDeviceD0Exit = PwrD0Exit_common;  // 离开D0电源状态（工作状态），比如休眠或设备移除
}

// 作用相当于WDM中的AddDevice函数。
// 他是PNP过程中首当其冲被调用的回调函数。主要任务是创建设备对象。
// 设备对象在系统中以软件形式，代表外部的某个物理硬件设备。
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

	//PAGED_CODE(); //PAGED_CODE这个宏可以确保调用线程运行在一个允许分页的足够低IRQL级别
	// Get the device enumerator name to detect whether the device being
	// added is a PCI device or root-enumerated non pnp ISA device.
	// It's okay to WDM functions when there is no appropriate WDF
	// interface is available.
	//
	status = WdfFdoInitQueryProperty(DeviceInit,
		DevicePropertyEnumeratorName,		//获取设备名称
		sizeof(enumeratorName),
		enumeratorName,
		&returnSize);

	if(!NT_SUCCESS(status)){
		return status;
	}

	WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributesForRequests, REQUEST_CONTEXT);
	WdfDeviceInitSetRequestAttributes(DeviceInit, &attributesForRequests);//设置默认对象属性，此属性将运用于从框架队列中出列的IO Request 对象

	RtlInitUnicodeString(&unicodeEnumName, enumeratorName);
	RtlInitUnicodeString(&temp, L"PCI");
	if(RtlCompareUnicodeString(&unicodeEnumName, &temp, TRUE) != 0) {
		//
		// It's not a PCI device.
		//
		ASSERTMSG("Unknown device", FALSE); //如果设备没有识别
		return STATUS_DEVICE_CONFIGURATION_ERROR;  //返回设备验证错误
	}
	//设备验证成功
	WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&pnpPowerCallbacks);  //初始化电源回调结构体
	InitPnpPwrEvents(&pnpPowerCallbacks);  //设置电源回调
	WdfDeviceInitSetPnpPowerEventCallbacks(DeviceInit, &pnpPowerCallbacks);

}

// 配置设备驱动的电源管理功能
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

	UNREFERENCED_PARAMETER( ResourceList );//避免编译器关于未引用参数的警告


	pContext = GetDeviceContext(m_hDevice);

	// 配置设备
	//status = ConfigureUsbDevice();
	if(!NT_SUCCESS(status))
		return status;

	// 获取Pipe句柄
	//status = GetUsbPipes();
	if(!NT_SUCCESS(status))
		return status;

	// 初始电源策略，
	status = InitPowerManagement();

	// 对系统资源列表，我们不做实质性的操作，仅仅打印一些信息。
	// 读者完全可以把下面的这些代码都注释掉。
	ulNumRes = WdfCmResourceListGetCount(ResourceList);
	KDBG(DPFLTR_INFO_LEVEL, "ResourceListCount:%d\n", ulNumRes);

	// 下面我们饶有兴致地枚举这些系统资源，并打印出它们的相关名称信息。
	for(ulIndex = 0; ulIndex < ulNumRes; ulIndex++)
	{
		pResDes = WdfCmResourceListGetDescriptor(ResourceList, ulIndex);		
		if(!pResDes){
			return STATUS_DEVICE_CONFIGURATION_ERROR;
		}

		switch (pResDes->Type) 
		{
		case CmResourceTypeMemory:
			KDBG(DPFLTR_INFO_LEVEL, "System Resource：CmResourceTypeMemory\n");
			break;

		case CmResourceTypePort:
			KDBG(DPFLTR_INFO_LEVEL, "System Resource：CmResourceTypePort\n");
			break;

		case CmResourceTypeInterrupt:
			KDBG(DPFLTR_INFO_LEVEL, "System Resource：CmResourceTypeInterrupt\n");
			break;

		default:
			KDBG(DPFLTR_INFO_LEVEL, "System Resource：Others %d\n", pResDes->Type);
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

// 此函数类似于WDM中的PNP_MN_STOP_DEVICE函数，在设备移除时被调用。
// 当个函数被调用时候，设备仍处于工作状态。
NTSTATUS WdfDriverClass::PnpReleaseHardware(IN WDFCMRESLIST ResourceListTranslated)
{
	KDBG(DPFLTR_INFO_LEVEL, "[PnpReleaseHardware]");

	// 如果PnpPrepareHardware调用失败,UsbDevice为空；
	// 这时候直接返回即可。
/*	if (m_hUsbDevice == NULL)
		return STATUS_SUCCESS;

	// 取消USB设备的所有IO操作。它将连带取消所有Pipe的IO操作。
	WdfIoTargetStop(WdfUsbTargetDeviceGetIoTarget(m_hUsbDevice), WdfIoTargetCancelSentIo);

	// Deconfiguration或者“反配置”
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

//下面两个Power回调，和WDM中的PnpSetPower类似。
NTSTATUS WdfDriverClass::PwrD0Entry(IN WDF_POWER_DEVICE_STATE  PreviousState)
{
	KDBG(DPFLTR_INFO_LEVEL, "[PwrD0Entry] from %s", PowerName(PreviousState));
	return STATUS_SUCCESS;
}

// 离开D0状态
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

// 设备配置
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