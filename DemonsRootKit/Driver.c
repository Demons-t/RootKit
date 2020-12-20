#include "Driver.h"

// 清理空间
VOID FreePool(PDRIVER pTemp, PDRIVER pDriverHead, ULONG uCount)
{
	// 清理空间
	pTemp = pDriverHead;
	for (ULONG i = 0; i < uCount; i++)
	{
		ExFreePoolWithTag(pTemp, 'abcd');
		pTemp = pTemp->Next;
	}
}

// 遍历驱动
ULONG EnumDriver(PVOID pOutBuff, ULONG uCoutSize, PDRIVER_OBJECT pDriverObject)
{
	// 指针指向的就是当前驱动在链表中对应的项
	PLDR_DATA_TABLE_ENTRY pCurrent = (PLDR_DATA_TABLE_ENTRY)pDriverObject->DriverSection;
	PLDR_DATA_TABLE_ENTRY pItem = pCurrent;

	// 申请堆空间
	PDRIVER pDriverHead = ExAllocatePoolWithTag(PagedPool, sizeof(DRIVER), 'abcd');
	
	// 初始化为0
	RtlZeroMemory(pDriverHead, sizeof(DRIVER));
	
	pDriverHead->Next = NULL;
	PDRIVER pTemp = pDriverHead;
	ULONG uCount = 0;
	do 
	{
		RtlCopyMemory(pTemp->cName, pCurrent->BaseDllName.Buffer, pCurrent->BaseDllName.Length);
		pTemp->uSize = pCurrent->SizeOfImage;
		pTemp->uDllBase = (ULONG)pCurrent->DllBase;
		RtlCopyMemory(pTemp->cFullDllName, pCurrent->FullDllName.Buffer, pCurrent->FullDllName.Length);

		// 获取当前元素指向的下一个元素
		pCurrent = (PLDR_DATA_TABLE_ENTRY)(pCurrent->InLoadOrderLinks.Flink);
		PDRIVER pEnumDriver = NULL;
		pEnumDriver = ExAllocatePoolWithTag(PagedPool, sizeof(DRIVER), 'abcd');
		RtlZeroMemory(pEnumDriver, sizeof(DRIVER));
		pTemp->Next = pEnumDriver;
		pEnumDriver->Next = NULL;
		pTemp = pEnumDriver;                                          
		uCount++;

	} while (pItem!=pCurrent);

	ULONG uOutputSize = uCount * sizeof(DRIVER);

	// 如果传入的size大于实际需要的size，就返回实际需要的size
	PDRIVER pOutputSize = pOutBuff;
	if (uOutputSize <= uCoutSize)
	{
		pTemp = pDriverHead;
		for (ULONG i = 0; i < uCount; i++)
		{
			RtlCopyMemory(pOutputSize[i].cName, pTemp->cName, 256 * 2);
			pOutputSize[i].uDllBase = pTemp->uDllBase;
			pOutputSize[i].uSize = pTemp->uSize;
			RtlCopyMemory(pOutputSize[i].cFullDllName, pTemp->cFullDllName, 256 * 2);
			pTemp = pTemp->Next;
		}
	}

	// 清理空间
	FreePool(pTemp, pDriverHead, uCount);

	return uOutputSize;
}

// 隐藏驱动
UINT8 HideDriver(UNICODE_STRING uDriverName, PDRIVER_OBJECT pDriverObject)
{
	PLDR_DATA_TABLE_ENTRY pEntry = (PLDR_DATA_TABLE_ENTRY)pDriverObject->DriverSection;
	PLDR_DATA_TABLE_ENTRY pFirstEntry;
	pFirstEntry = pEntry;

	do 
	{
		// 比较名字是否相同，如果相同，则断链
		if (!RtlCompareUnicodeString(&uDriverName, &(pEntry->BaseDllName), FALSE))
		{
			// 将当前的驱动从链表中摘除(断链\摘链)
			pEntry->InLoadOrderLinks.Blink->Flink = pEntry->InLoadOrderLinks.Flink;
			pEntry->InLoadOrderLinks.Flink->Blink = pEntry->InLoadOrderLinks.Blink;
			pEntry->InLoadOrderLinks.Blink = NULL;
			pEntry->InLoadOrderLinks.Flink = NULL;
			break;
		}

		// 链表往前走
		pEntry = (PLDR_DATA_TABLE_ENTRY)pEntry->InLoadOrderLinks.Flink;

	} while (pFirstEntry !=pEntry);

	return FALSE;
}
