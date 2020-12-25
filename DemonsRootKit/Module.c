#include "Module.h"

PPEB PsGetProcessPeb(IN PEPROCESS Process);

// ����ģ��
ULONG EnumModule(PVOID pOutputBuff, ULONG uBuffSize, PVOID pPid)
{
	ULONG uInputPid;
	NTSTATUS status;
	PEPROCESS proc;
	PULONG pObjectTab;
	ULONG uCount;
	ULONG uPid;
	PMODULE pModuleHead;
	PMODULE pModuleTemp;
	PMODULE pModuleTest;

	struct _PEB* peb;
	LDR_DATA_TABLE_ENTRY* pLdrHeader;
	LDR_DATA_TABLE_ENTRY* pLdrBegin;

	uCount = 0;
	uInputPid = *(ULONG*)pPid;
	status = STATUS_SUCCESS;
	pModuleHead = ExAllocatePoolWithTag(PagedPool, sizeof(MODULE), 'abcd');
	RtlZeroMemory(pModuleHead, sizeof(MODULE));
	pModuleTemp = pModuleHead;

	for (int i = 4; i < 90000; i += 4)
	{
		// ��ȡ���� EPROCESS
		status = PsLookupProcessByProcessId((HANDLE)i, &proc);

		// �ж��Ƿ�Ϊ������
		pObjectTab = (ULONG*)((ULONG)proc + 0xf4);
		if (!NT_SUCCESS(status) || !(*pObjectTab))
			continue;

		uPid = *(ULONG*)((ULONG)proc + 0xB4);

		// �ж� PID �Ƿ���ͬ
		if (uPid != uInputPid)
			continue;

		// ��ʼ����ģ��
		peb = PsGetProcessPeb(proc);

		// ��ǰ�߳��л������½��̶���
		KeAttachProcess(proc);

		// ��ȡ LDR ��
		pLdrHeader = (LDR_DATA_TABLE_ENTRY*)peb->Ldr->InLoadOrderModuleList.Flink;
		pLdrBegin = (LDR_DATA_TABLE_ENTRY*)pLdrHeader->InLoadOrderLinks.Blink;
		
		do 
		{
			RtlCopyMemory(pModuleTemp->cFullDllName, pLdrHeader->FullDllName.Buffer, pLdrHeader->FullDllName.Length);
			pModuleTemp->uSize = pLdrHeader->SizeOfImage;
			pModuleTemp->uStartAddress = (ULONG)pLdrHeader->DllBase;
			pModuleTest = ExAllocatePoolWithTag(PagedPool, sizeof(MODULE), 'abcd');
			RtlZeroMemory(pModuleTest, sizeof(MODULE));
			pModuleTemp->Next = pModuleTest;
			pModuleTemp = pModuleTest;
			pLdrHeader = (LDR_DATA_TABLE_ENTRY*)pLdrHeader->InLoadOrderLinks.Flink;
			uCount++;
		} while (pLdrBegin!=pLdrHeader);

		// ����ҿ�����
		KeDetachProcess();

		// �ݼ����ü���
		ObDereferenceObject(proc);
		break;
	}

	ULONG uOutputSize = uCount * sizeof(MODULE);

	// �����Ҫ�Ĵ�С�ȴ����Ĵ�С�󣬾ͷ���ʵ����Ҫ�Ĵ�С
	PMODULE pModule = pOutputBuff;
	if (uOutputSize <= uBuffSize)
	{
		pModuleTemp = pModuleHead;
		for (ULONG i = 0; i < uCount; i++)
		{
			pModule[i].uSize = pModuleTemp->uSize;
			pModule[i].uStartAddress = pModuleTemp->uStartAddress;
			RtlCopyMemory(pModule[i].cFullDllName, pModuleTemp->cFullDllName, 256 * 2);
			pModuleTemp = pModuleTemp->Next;
		}
	}

	// ����ռ�
	pModuleTemp = pModuleHead;
	for (ULONG i = 0; i < uCount; i++)
	{
		ExFreePoolWithTag(pModuleTemp, 'abcd');
		pModuleTemp = pModuleTemp->Next;
	}

	return uOutputSize;
}
