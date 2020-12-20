#include "IDT.h"

// 遍历IDT
BOOLEAN GetIdtInfo(PVOID pOutputBuff)
{
	IDT_INFO idt = { 0,0,0 };
	PIDT_ENTRY pIdtEntry = NULL;
	PIDT_ENTRY pOutput = (PIDT_ENTRY)pOutputBuff;

	// 获取 IDT 表地址
	__asm sidt idt;

	// 获取 IDT 表数组地址
	pIdtEntry = (PIDT_ENTRY)MAKELONG(idt.uLowIdtBase, idt.uHighIdtBase);

	// 获取 IDT 信息
	for (ULONG i = 0; i < 0x100; i++)
	{
		pOutput[i].uOffsetLow = pIdtEntry[i].uOffsetLow;		// 低地址
		pOutput[i].uOffsetHigh = pIdtEntry[i].uOffsetHigh;		// 高地址
		pOutput[i].uSelector = pIdtEntry[i].uSelector;			// 段选择子
		pOutput[i].GateType = pIdtEntry[i].GateType;			// 类型
		pOutput[i].DPL = pIdtEntry[i].DPL;						// 特权级
	}

	return TRUE;
}
