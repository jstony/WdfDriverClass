#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#ifdef __cplusplus
extern "C"{					//���ʹ��C++���������˱���Ԥ����������߱�������C��ʽ���뺯����
#endif

#pragma warning(disable:4200)  // nameless struct/union
#pragma warning(disable:4201)  // nameless struct/union
#pragma warning(disable:4115)  // named typedef in parenthesis
#pragma warning(disable:4214)  // bit field types other than int


#include <ntddk.h>
#include <wdf.h>
#define NTSTRSAFE_LIB
#include <ntstrsafe.h>

#include <ntddstor.h>
#include <mountdev.h>
#include <ntddvol.h>
#include "WdfDriverClass.h"

/*
//ע��PCIE��������
#define REGISTER_CPCIEBASE_CLASS(DriverName) \
	CPCIEbase* GetCPCIEbase(){\
	return  (CPCIEbase*)new(NonPagedPool, 'GETD') DriverName();\
}
*/

//ע��PCIE���������
#define REGISTER_WDFDRIVER_CLASS_NULL()  \
	WdfDriverClass* GetWdfDriverClass(){\
	return  new(NonPagedPool, 'GETD') WdfDriverClass();\
}

	// WDF�豸����
typedef struct _DEVICE_CONTEXT {
		PVOID par1;
		PVOID par2;
		PVOID par3;
		PVOID par4;
		int nLength;
		WDFDRIVER hDriver;

		WDFDEVICE           Device;                 // �����豸���� 
		WDFDMAENABLER       DmaEnabler;             // A WDFDMAENABLER handle
		WDFREQUEST          CurrentRequest;         // A WDFREQUEST handle
		WDFINTERRUPT        WdfInterrupt;

		ULONG               MaximumTransferLength;  // Maximum transfer length for adapter
		ULONG               MaximumPhysicalPages;   // Maximum number of breaks adapter

		ULONG               Intcsr;                 // Accumulated interrupt flags

		//PREG5933            Regs;                   // S5933 registers ptr

		PUCHAR              PortBase;               // I/O port base address
		ULONG               PortCount;              // Number of assigned ports
		BOOLEAN             PortMapped;             // TRUE if mapped port addr

} DEVICE_CONTEXT, *PDEVICE_CONTEXT;

	// WDF��������
typedef struct {
		PVOID par1;
		PVOID par2;
		PVOID par3;
		PVOID par4;
}DRIVER_CONTEXT, *PDRIVER_CONTEXT;

typedef struct _REQUEST_CONTEXT{

	WDFREQUEST          Request;
	WDFDMATRANSACTION   DmaTransaction;

} REQUEST_CONTEXT, *PREQUEST_CONTEXT;

#ifndef _WORD_DEFINED
#define _WORD_DEFINED
	typedef unsigned short  WORD;
	typedef unsigned int	UINT;
#endif

//===============================��������
// ������ȡ������ĺ���ָ�룬
// ͨ��GetDeviceContext�ܴ���һ��ܶ���ȡ��һ��ָ��ṹ��DEVICE_CONTEXT��ָ�롣
// ʹ��GetDeviceContext��ǰ���ǣ�Ŀ������Ѿ�������һ������ΪDEVICE_CONTEXT�Ļ����顣
// �ڱ������У�������PnpAdd������Ϊ�豸��������DEVICE_CONTEXT�����顣
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, GetDeviceContext);

// ������ȡ������ĺ���ָ�룬
// ͨ��GetDeviceContext�ܴ���һ��ܶ���ȡ��һ��ָ��ṹ��DEVICE_CONTEXT��ָ�롣
// ʹ��GetDeviceContext��ǰ���ǣ�Ŀ������Ѿ�������һ������ΪDEVICE_CONTEXT�Ļ����顣
// �ڱ������У�������PnpAdd������Ϊ�豸��������DEVICE_CONTEXT�����顣
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DRIVER_CONTEXT, GetDriverContext);
//���������ĺ���ָ��
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(REQUEST_CONTEXT, GetRequestContext);

//===============================���ú���
NTSTATUS																	//������ں�������
		DriverEntry(
		IN PDRIVER_OBJECT  DriverObject, 
		IN PUNICODE_STRING  RegistryPath
		);


#if (DBG)
	__inline void KDBG(int nLevel, char* msg, ...)		//�������������������ڵ���ʱ�����Ϣ nLevel��ʾ��Ϣ����Ҫ�̶�, msgָ����Ϣ������
	{
		static char csmsg[1024];

		va_list argp;
		va_start(argp, msg);
		vsprintf(csmsg, msg, argp);
		va_end(argp);

		KdPrintEx((DPFLTR_DEFAULT_ID, nLevel, "PCIE Driver:"));// ��ʽ��ͷ
		KdPrintEx((DPFLTR_DEFAULT_ID, nLevel, csmsg));	 // Log body
		KdPrintEx((DPFLTR_DEFAULT_ID, nLevel, "\n"));	 // ����
	}
#else
	__inline void KDBG(int nLevel, char* msg, ...)
	{
	}
#endif


#ifdef __cplusplus
}
#endif


#endif