/************************************************************************/
/* ������ܵ�������ļ�
 �˷�������Ҫ������ʵ��wdf������ܣ������Ժ��ʹ��,�Ժ���Ҫ�õ�ʱ��Ӵ���Ļ���������
 ���༴�ɡ�
*/
/************************************************************************/
#ifdef __cplusplus
extern "C"{
#endif

#include "public.h"
#pragma alloc_text(INIT, DriverEntry)

#ifdef __cplusplus
}
#endif

WdfDriverClass* GetWdfDriverClass();

VOID WdfDriverClassDestroy(IN WDFOBJECT  Object)
{
	PVOID pBuf = WdfMemoryGetBuffer((WDFMEMORY)Object, NULL);		//�ͷ��ڴ���Դ
	delete pBuf;
}

NTSTATUS WdfDriverClass::DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	WDF_OBJECT_ATTRIBUTES attributes;
	WDF_DRIVER_CONFIG config;

	// ���ڲ������sizeof(DEVICE_CONTEXT)��ṹ�峤��
	WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, DRIVER_CONTEXT);	//��ʼ�������Ŀռ�

	// ����WDF������������������������Զ�ò�����Ҳ������¶������ȷʵ�����š�
	// ���ǿ��԰�������һֻ�����ŵ��ְɡ�
	// DriverEntry��������ֳ�ʼ�������������е�С�˿�����һģһ������ֱ���޶��¡�
	// ���߿��Ծݴ˲ο�����Scasi���ļ����ˡ�NDIS��StreamPort�����͵�С�˿�����������������ġ�
	WDF_DRIVER_CONFIG_INIT(&config, WdfDriverClass::PnpAdd_Common);  //ע��EvtDriverDeviceAdd�¼��ص�����,configΪ���صı���ʼ����Ľṹ��

	NTSTATUS status = WdfDriverCreate(DriverObject, // WDF��������
		RegistryPath,
		&attributes,
		&config, // ���ò���
		&m_hDriver);

	if(NT_SUCCESS(status))
	{
		m_pDriverContext = GetDriverContext(m_hDriver);
		ASSERT(m_pDriverContext);
		m_pDriverContext->par1 = (PVOID)this;	

		WDFMEMORY hMemDrv;
		WDF_OBJECT_ATTRIBUTES_INIT(&attributes);			//��ʼ���ṹ��
		attributes.ParentObject = m_hDriver;						//�����ϲ㸸�������������
		attributes.EvtDestroyCallback  = WdfDriverClassDestroy;
		WdfMemoryCreatePreallocated(&attributes, (PVOID)this, GetSize(), &hMemDrv);
	}

	return status;
}


extern "C" NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT  DriverObject, 
	IN PUNICODE_STRING  RegistryPath
	)
{
	WdfDriverClass* myDriver = GetWdfDriverClass();//new(NonPagedPool, 'CY01')DrvClass();
	if(myDriver == NULL)return STATUS_UNSUCCESSFUL;
	return myDriver->DriverEntry(DriverObject, RegistryPath);
}