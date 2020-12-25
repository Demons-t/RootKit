#include "IDT.h"

// ����IDT
BOOLEAN GetIdtInfo(PVOID pOutputBuff)
{
	IDT_INFO idt = { 0,0,0 };
	PIDT_ENTRY pIdtEntry = NULL;
	PIDT_ENTRY pOutput = (PIDT_ENTRY)pOutputBuff;

	// ��ȡ IDT ���ַ
	__asm sidt idt;

	// ��ȡ IDT �������ַ
	pIdtEntry = (PIDT_ENTRY)MAKELONG(idt.uLowIdtBase, idt.uHighIdtBase);

	// ��ȡ IDT ��Ϣ
	for (ULONG i = 0; i < 0x100; i++)
	{
		pOutput[i].uOffsetLow = pIdtEntry[i].uOffsetLow;		// �͵�ַ
		pOutput[i].uOffsetHigh = pIdtEntry[i].uOffsetHigh;		// �ߵ�ַ
		pOutput[i].uSelector = pIdtEntry[i].uSelector;			// ��ѡ����
		pOutput[i].GateType = pIdtEntry[i].GateType;			// ����
		pOutput[i].DPL = pIdtEntry[i].DPL;						// ��Ȩ��
	}

	return TRUE;
}
