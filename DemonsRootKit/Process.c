#include "Process.h"

CHAR* PsGetProcessImageFileName(IN PEPROCESS Process);
PPEB PsGetProcessPeb(IN PEPROCESS Process);

// 遍历进程
ULONG EnumProcess(PVOID pOutPut, ULONG uBuffSize, PVOID pInputBuff, ULONG uCode)
{
	NTSTATUS status;
	status = STATUS_SUCCESS;
	PEPROCESS proc = NULL;
	PROCESS_INFO fpi = { 0 };
	PPROCESS_INFO pInBuff = (PPROCESS_INFO)pInputBuff;
	RtlZeroMemory(pOutPut, uBuffSize);
	PPROCESS_INFO pOutBuff = (PPROCESS_INFO)pOutPut;
	ULONG i = uCode == PROCESS_FIRST ? 4 : (ULONG)pInBuff->hProcessHandle;
	ULONG uCount;
	uCount = 0;
	for (; i < 0xFFFF; i += 4)
	{
		if (status == PsLookupProcessByProcessId((HANDLE)i, &proc))
		{
			// 必须清空
			RtlZeroMemory(&fpi, sizeof(PROCESS_INFO));
			fpi.hProcessHandle = (HANDLE)(i + sizeof(HANDLE));
			
			// 获取进程id
			fpi.uPid = i;

			// 获取父进程ID
			ULONG* PPID = (ULONG*)((ULONG)proc + 0x140);
			ULONG uPPid = *PPID;
			fpi.uPPid = uPPid;

			// 进程挂靠，获取第一个模块的名字
			PPEB_SHORT peb = (PPEB_SHORT)PsGetProcessPeb(proc);
			KAPC_STATE state;
			KeStackAttachProcess(proc, &state);
			if (peb)
			{
				PPEB_LDR_DATA pebLdr = peb->Ldr;
				// 获取模块链表
				PLIST_ENTRY pModList = &pebLdr->InLoadOrderModuleList;
				PLIST_ENTRY pModuleListBegin;
				pModuleListBegin  = pModList;
				PLDR_DATA_TABLE_ENTRY pebDataTableEntry = (PLDR_DATA_TABLE_ENTRY)pModList->Flink;

				// 获取路径
				UNICODE_STRING fullName = pebDataTableEntry->FullDllName;
				RtlCopyMemory(fpi.cFullDllName, fullName.Buffer, fullName.Length);
				fpi.cFullDllName[fullName.Length / 2] = 0;

				// 获取名字
				RtlCopyMemory(fpi.wName, PsGetProcessImageFileName(proc), strlen(PsGetProcessImageFileName(proc)));
			}
			if (pOutBuff != NULL && uBuffSize >= sizeof(PROCESS_INFO))
			{
				RtlMoveMemory(pOutBuff, &fpi, sizeof(PROCESS_INFO));
			}
			if(uCode==PROCESS_NEXT)
				pOutBuff++;

			uCount = sizeof(fpi);

			// 解除进程挂靠
			KeUnstackDetachProcess(&state);

			// 引用计数-1
			ObDereferenceObject(proc);
		}
	}

	if (i == 0xFFFF)
		uCount = 0;

	return uCount * sizeof(PROCESS_INFO);
}

// 隐藏进程
VOID HideProcess(PVOID pPid)
{
	ULONG ulInputPID;
	ULONG EnumPID;
	PEPROCESS SystemEProcess;
	PLIST_ENTRY ulStartAddr;
	PLIST_ENTRY ulTemp;
	ulInputPID = *(ULONG*)pPid;
	SystemEProcess = PsGetCurrentProcess();
	ulStartAddr = (PLIST_ENTRY)((ULONG)SystemEProcess + 0xB8);
	ulTemp = ulStartAddr;
	do
	{
		EnumPID = *(ULONG*)((ULONG)ulTemp - 0x4);
		// 如果找到了则隐藏起来
		if (EnumPID == ulInputPID)
		{
			ulTemp->Blink->Flink = ulTemp->Flink;
			ulTemp->Flink->Blink = ulTemp->Blink;
			ulTemp->Flink = NULL;
			ulTemp->Blink = NULL;
			break;
		}
		ulTemp = ulTemp->Flink;
	} while (ulTemp != ulStartAddr);
}

// 结束进程
VOID KillProcess(PVOID pPid)
{
	ULONG pid = *(int*)pPid;
	// 若通过PID能找到EPROCESS
	PEPROCESS proc = NULL;
	if (NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)pid, &proc)))
	{
		HANDLE hProcess = NULL;
		CLIENT_ID Clientld = { 0 };
		OBJECT_ATTRIBUTES objAttribute = { sizeof(OBJECT_ATTRIBUTES) };
		Clientld.UniqueProcess = (HANDLE)pid;
		Clientld.UniqueThread = 0;
		ZwOpenProcess(&hProcess, 1, &objAttribute, &Clientld);//获取进程句柄
		if (hProcess)
		{
			ZwTerminateProcess(hProcess, 0);
			ZwClose(hProcess);
		}
		ObDereferenceObject(proc);// 递减引用计数
	}
}