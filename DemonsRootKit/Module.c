#include "Module.h"

PPEB PsGetProcessPeb(IN PEPROCESS Process);

// 遍历模块
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
		// 获取进程 EPROCESS
		status = PsLookupProcessByProcessId((HANDLE)i, &proc);

		// 判断是否为死进程
		pObjectTab = (ULONG*)((ULONG)proc + 0xf4);
		if (!NT_SUCCESS(status) || !(*pObjectTab))
			continue;

		uPid = *(ULONG*)((ULONG)proc + 0xB4);

		// 判断 PID 是否相同
		if (uPid != uInputPid)
			continue;

		// 开始遍历模块
		peb = PsGetProcessPeb(proc);

		// 当前线程切换到的新进程对象
		KeAttachProcess(proc);

		// 获取 LDR 链
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

		// 解除挂靠进程
		KeDetachProcess();

		// 递减引用计数
		ObDereferenceObject(proc);
		break;
	}

	ULONG uOutputSize = uCount * sizeof(MODULE);

	// 如果需要的大小比传进的大小大，就返回实际需要的大小
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

	// 清理空间
	pModuleTemp = pModuleHead;
	for (ULONG i = 0; i < uCount; i++)
	{
		ExFreePoolWithTag(pModuleTemp, 'abcd');
		pModuleTemp = pModuleTemp->Next;
	}

	return uOutputSize;
}
