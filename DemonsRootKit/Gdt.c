#include "Gdt.h"

ULONG GetGdtInfo(PVOID pOutputBuff, ULONG uBuffSize)
{
	GDT_INFO gdt = { 0,0,0 };
	PGDT pGdtEntry = NULL;
	PGDT pOutput = (PGDT)pOutputBuff;

	// ��ȡ GDT ���ַ
	__asm sgdt gdt;

	// ��ȡGDT �������ַ
	ULONG uNum = gdt.uGdtLimit / 8;
	pGdtEntry = (PGDT)MAKELONG(gdt.uLowGdtBase, gdt.uHighGdtBase);
	ULONG uRetNum = 0;

	for (ULONG i = 0; i < uNum; i++)
	{
		// ����β����ڣ��Ͳ�����
		if(pGdtEntry[i].P == 0)
			continue;
		uRetNum++;
	}

	if (uRetNum * sizeof(GDT) > uBuffSize)
		return uRetNum * sizeof(GDT);
	
	for (ULONG i = 0; i < uRetNum; i++)
	{
		// ����β����ڣ��Ͳ�����
		if(pGdtEntry[i].P == 0)
			continue;
		RtlCopyMemory(&pOutput[i], &pGdtEntry[i], sizeof(GDT));
	}

	return uRetNum * sizeof(GDT);
}
