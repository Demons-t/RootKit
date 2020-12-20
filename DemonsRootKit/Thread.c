#include "Thread.h"

// 遍历线程
ULONG EnumThread(PVOID pOutputBuff, ULONG uOutputSize, PVOID pInputBuff)
{
	PETHREAD _thread;
	PKTHREAD _kthread;
	PLIST_ENTRY LClink;
	PLIST_ENTRY LNink;
	CLIENT_ID* _CLIENT_ID;
	char BasePriority;
	UCHAR State;
	ULONG ThreadID = 0;
	ULONG ThreadStartAddress;

	// 获取到需要查询的PID
	ULONG dwInputPID;
	dwInputPID = *(ULONG*)pInputBuff;
	NTSTATUS Status;
	Status = STATUS_SUCCESS;
	PEPROCESS pProc;
	ULONG ulCount;
	ulCount = 0;
	PTHREAD_INFO pThreadHead = ExAllocatePoolWithTag(PagedPool, sizeof(THREAD_INFO), 'abcd');

	RtlZeroMemory(pThreadHead, sizeof(THREAD_INFO));
	pThreadHead->Next = NULL;
	PTHREAD_INFO pThreadTemp = pThreadHead;
	for (int i = 4; i < 90000; i += 4)
	{
		// 尝试获取进程EPROCESS
		Status = PsLookupProcessByProcessId((HANDLE)i, &pProc);

		// 判断是否为死进程
		ULONG* pulObjectTable = (ULONG*)((ULONG)pProc + 0xf4);
		if (!NT_SUCCESS(Status) || !(*pulObjectTable))
			continue;

		ULONG* pulPID = (ULONG*)((ULONG)pProc + 0xB4);
		ULONG ulPID = *pulPID;

		// 判断PID是否相同
		if (ulPID != dwInputPID)
			continue;

		// 开始遍历该进程的线程
		LClink = (PLIST_ENTRY)((ULONG)pProc + 0x188);
		LNink = LClink->Flink;

		while (LNink != LClink)
		{
			_thread = (PETHREAD)((ULONG)LNink - 0x268);
			_kthread = (PKTHREAD)_thread;
			_CLIENT_ID = (PCLIENT_ID)((ULONG)_thread + 0x22C);

			// TID
			ThreadID = (ULONG)(_CLIENT_ID->UniqueThread);

			// StartAddress
			ThreadStartAddress = *(ULONG*)((ULONG)_thread + 0x260);

			// BasePriority
			BasePriority = *(char*)((ULONG)_kthread + 0x57);

			// State
			State = *(UCHAR*)((ULONG)_kthread + 0x68);

			pThreadTemp->ulTID = ThreadID;
			pThreadTemp->ulStartAddress = ThreadStartAddress;
			pThreadTemp->ulBasePriority = BasePriority;
			pThreadTemp->ulStatus = State;

			PTHREAD_INFO pThreadTemp2 = ExAllocatePoolWithTag(PagedPool, sizeof(THREAD_INFO), 'abcd');
			RtlZeroMemory(pThreadTemp2, sizeof(THREAD_INFO));
			pThreadTemp2->Next = NULL;
			pThreadTemp->Next = pThreadTemp2;
			pThreadTemp = pThreadTemp2;
			LNink = LNink->Flink;

			// 先获取到这个线程数量
			ulCount++;
		}
		ObDereferenceObject(pProc);
		break;
	}
	ULONG OutPutSize = ulCount * sizeof(THREAD_INFO);
	// 如果需要的大小比传进来的大小大，则返回需要的大小
	PTHREAD_INFO OutThread = pOutputBuff;
	if (OutPutSize <= uOutputSize)
	{
		pThreadTemp = pThreadHead;
		for (ULONG i = 0; i < ulCount; ++i)
		{
			// TID
			OutThread[i].ulTID = pThreadTemp->ulTID;

			// StartAddress
			OutThread[i].ulStartAddress = pThreadTemp->ulStartAddress;

			// Status
			OutThread[i].ulStatus = pThreadTemp->ulStatus;

			// BasePriority
			OutThread[i].ulBasePriority = pThreadTemp->ulBasePriority;

			pThreadTemp = pThreadTemp->Next;
		}
	}
	// 清理空间
	pThreadTemp = pThreadHead;
	for (ULONG i = 0; i <= ulCount; ++i)
	{
		ExFreePoolWithTag(pThreadTemp, 'abcd');
		pThreadTemp = pThreadTemp->Next;
	}

	return OutPutSize;
}
