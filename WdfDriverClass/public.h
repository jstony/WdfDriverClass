#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#ifdef __cplusplus
extern "C"{					//如果使用C++编译器，此编译预处理命令告诉编译器以C方式编译函数。
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
//注册PCIE驱动子类
#define REGISTER_CPCIEBASE_CLASS(DriverName) \
	CPCIEbase* GetCPCIEbase(){\
	return  (CPCIEbase*)new(NonPagedPool, 'GETD') DriverName();\
}
*/

//注册PCIE驱动类基类
#define REGISTER_WDFDRIVER_CLASS_NULL()  \
	WdfDriverClass* GetWdfDriverClass(){\
	return  new(NonPagedPool, 'GETD') WdfDriverClass();\
}

	// WDF设备环境
typedef struct _DEVICE_CONTEXT {
		PVOID par1;
		PVOID par2;
		PVOID par3;
		PVOID par4;
		int nLength;
		WDFDRIVER hDriver;

		WDFDEVICE           Device;                 // 创建设备对象 
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

	// WDF驱动环境
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

//===============================函数声明
// 定义析取环境块的函数指针，
// 通过GetDeviceContext能从任一框架对象取得一个指向结构体DEVICE_CONTEXT的指针。
// 使用GetDeviceContext的前提是，目标对象已经定义了一个名称为DEVICE_CONTEXT的环境块。
// 在本工程中，我们在PnpAdd函数中为设备对象定义了DEVICE_CONTEXT环境块。
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, GetDeviceContext);

// 定义析取环境块的函数指针，
// 通过GetDeviceContext能从任一框架对象取得一个指向结构体DEVICE_CONTEXT的指针。
// 使用GetDeviceContext的前提是，目标对象已经定义了一个名称为DEVICE_CONTEXT的环境块。
// 在本工程中，我们在PnpAdd函数中为设备对象定义了DEVICE_CONTEXT环境块。
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DRIVER_CONTEXT, GetDriverContext);
//定义请求块的函数指针
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(REQUEST_CONTEXT, GetRequestContext);

//===============================配置函数
NTSTATUS																	//驱动入口函数声明
		DriverEntry(
		IN PDRIVER_OBJECT  DriverObject, 
		IN PUNICODE_STRING  RegistryPath
		);


#if (DBG)
	__inline void KDBG(int nLevel, char* msg, ...)		//定义内联函数，用于在调试时输出信息 nLevel表示消息的重要程度, msg指向消息的内容
	{
		static char csmsg[1024];

		va_list argp;
		va_start(argp, msg);
		vsprintf(csmsg, msg, argp);
		va_end(argp);

		KdPrintEx((DPFLTR_DEFAULT_ID, nLevel, "PCIE Driver:"));// 格式化头
		KdPrintEx((DPFLTR_DEFAULT_ID, nLevel, csmsg));	 // Log body
		KdPrintEx((DPFLTR_DEFAULT_ID, nLevel, "\n"));	 // 换行
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