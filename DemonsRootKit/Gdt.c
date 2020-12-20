#include "Gdt.h"

ULONG GetGdtInfo(PVOID pOutputBuff, ULONG uBuffSize)
{
	GDT_INFO gdt = { 0,0,0 };
	PGDT pGdtEntry = NULL;
	PGDT pOutput = (PGDT)pOutputBuff;

	// 获取 GDT 表地址
	__asm sgdt gdt;

	// 获取GDT 表数组地址
	ULONG uNum = gdt.uGdtLimit / 8;
	pGdtEntry = (PGDT)MAKELONG(gdt.uLowGdtBase, gdt.uHighGdtBase);
	ULONG uRetNum = 0;

	for (ULONG i = 0; i < uNum; i++)
	{
		// 如果段不存在，就不遍历
		if(pGdtEntry[i].P == 0)
			continue;
		uRetNum++;
	}

	if (uRetNum * sizeof(GDT) > uBuffSize)
		return uRetNum * sizeof(GDT);
	
	for (ULONG i = 0; i < uRetNum; i++)
	{
		// 如果段不存在，就不遍历
		if(pGdtEntry[i].P == 0)
			continue;
		RtlCopyMemory(&pOutput[i], &pGdtEntry[i], sizeof(GDT));
	}

	return uRetNum * sizeof(GDT);
}
