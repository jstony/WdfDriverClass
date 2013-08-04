#pragma once
//注意编译时，不能使用纯C标志来编译此文件，因为函数重载是C++才有的内容
__forceinline void* __cdecl operator new(size_t size,
										 POOL_TYPE pool_type,
										 ULONG pool_tag) 
{
	ASSERT((pool_type < MaxPoolType) && (0 != size));
	if(size == 0)return NULL;

	// 中断级检查。分发级别和以上，只能分配非分页内存。
	ASSERT(pool_type ==  NonPagedPool || 
		(KeGetCurrentIrql() < DISPATCH_LEVEL));

	PVOID pRet = ExAllocatePoolWithTag(pool_type, size, pool_tag);
	if(pRet) RtlZeroMemory(pRet, size);

	return pRet;
}

__forceinline void __cdecl operator delete(void* pointer) 
{
	ASSERT(pointer);
	if (NULL != pointer)
		ExFreePool(pointer);
}

