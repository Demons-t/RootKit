#include "SSDT.h"

// ���� ssdt
ULONG EnumSsdt(PVOID pOutputBuff, ULONG uBuffSize)
{
	// ��ȡ��ǰ����ִ�е��߳�
	PETHREAD pThread = PsGetCurrentThread();

	PSSDTEntry ssdt = (PSSDTEntry) * (ULONG*)((ULONG)pThread + 0xbc);
	ULONG uNum = ssdt->NumberOfServices;

	if (uBuffSize < uNum * sizeof(SSDT))
		return uNum * sizeof(SSDT);

	PSSDT pSsdt = (PSSDT)pOutputBuff;
	PLONG pAddr = ssdt->ServiceTableBase;

	ULONG i = 0;
	while (i < uNum) 
	{
		pSsdt[i].uAddress = pAddr[i];
		pSsdt[i].uSysCallIndex = i;
		++i;
	}
	

	return uNum * sizeof(SSDT);
}
