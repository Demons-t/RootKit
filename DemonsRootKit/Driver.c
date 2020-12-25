#include "Driver.h"

// ����ռ�
VOID FreePool(PDRIVER pTemp, PDRIVER pDriverHead, ULONG uCount)
{
	// ����ռ�
	pTemp = pDriverHead;
	for (ULONG i = 0; i < uCount; i++)
	{
		ExFreePoolWithTag(pTemp, 'abcd');
		pTemp = pTemp->Next;
	}
}

// ��������
ULONG EnumDriver(PVOID pOutBuff, ULONG uCoutSize, PDRIVER_OBJECT pDriverObject)
{
	// ָ��ָ��ľ��ǵ�ǰ�����������ж�Ӧ����
	PLDR_DATA_TABLE_ENTRY pCurrent = (PLDR_DATA_TABLE_ENTRY)pDriverObject->DriverSection;
	PLDR_DATA_TABLE_ENTRY pItem = pCurrent;

	// ����ѿռ�
	PDRIVER pDriverHead = ExAllocatePoolWithTag(PagedPool, sizeof(DRIVER), 'abcd');
	
	// ��ʼ��Ϊ0
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

		// ��ȡ��ǰԪ��ָ�����һ��Ԫ��
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

	// ��������size����ʵ����Ҫ��size���ͷ���ʵ����Ҫ��size
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

	// ����ռ�
	FreePool(pTemp, pDriverHead, uCount);

	return uOutputSize;
}

// ��������
UINT8 HideDriver(UNICODE_STRING uDriverName, PDRIVER_OBJECT pDriverObject)
{
	PLDR_DATA_TABLE_ENTRY pEntry = (PLDR_DATA_TABLE_ENTRY)pDriverObject->DriverSection;
	PLDR_DATA_TABLE_ENTRY pFirstEntry;
	pFirstEntry = pEntry;

	do 
	{
		// �Ƚ������Ƿ���ͬ�������ͬ�������
		if (!RtlCompareUnicodeString(&uDriverName, &(pEntry->BaseDllName), FALSE))
		{
			// ����ǰ��������������ժ��(����\ժ��)
			pEntry->InLoadOrderLinks.Blink->Flink = pEntry->InLoadOrderLinks.Flink;
			pEntry->InLoadOrderLinks.Flink->Blink = pEntry->InLoadOrderLinks.Blink;
			pEntry->InLoadOrderLinks.Blink = NULL;
			pEntry->InLoadOrderLinks.Flink = NULL;
			break;
		}

		// ������ǰ��
		pEntry = (PLDR_DATA_TABLE_ENTRY)pEntry->InLoadOrderLinks.Flink;

	} while (pFirstEntry !=pEntry);

	return FALSE;
}
