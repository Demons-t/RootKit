#include "SysenterHook.h"

#include <ntddk.h>

// ԭʼ����
ULONG g_OldKiFastCallEntry = 0;
// Ҫ�����Ľ���PID
ULONG g_uPid = 0;

// 1. ��ȡԭʼKiFastCallEntry����
//			1.1 ecx ����Ĵ�����
//			1.2 rdmsr ����ָ��
//			1.3 eax ���ر���ԭʼ����

// 2. �����ǹ��˺����滻��msr 0x176�żĴ����� 
//     2.1 ecx ����Ĵ�����,eax д����˺���
//	   2.2 wrmsr ����ָ��

// 3. ж�ع���
//     3.1 ecx ����Ĵ�����,eax д��ԭʼ����
//	   2.2 wrmsr ����ָ��


// ���˺���
void _declspec(naked) MyKiFastCallEntry()
{
	// ���� ZwOpenProcess,���ú�eax == 0x0BE
	// edx�����û�ջ
	//  [edx + 0x00] : ���ص�ַ1
	//	[edx + 0x04] : ���ص�ַ2
	//	[edx + 0x08] : ����1    ProcessHandle
	//	[edx + 0x0c] : ����2    DesiredAccess
	//	[edx + 0x10] : ����3    ObjectAttributes
	//	[edx + 0x14] : ����3    ClientId

	_asm
	{
		pushad;					// ����Ĵ���
		cmp eax, 0x0BE;			// �Ƿ���ZwOpenProcess ����
		jne CallEnd;			// ��������
		mov eax, [edx + 0x14];	// ��ȡ���ĸ����� ClientId
		mov eax, [eax];			// ClientId->ProcessId
		cmp eax, g_uPid;			//�ж��Ƿ�Ҫ����Ľ���
		jne CallEnd;
		mov[edx + 0x0c], 0;		// ��Ȩ�޸�Ϊ0���޷�����

	CallEnd:
		popad;						//�ָ��Ĵ���
		jmp g_OldKiFastCallEntry;	//����ԭʼ����
	}
}

// ��װ����
void InstallHook()
{
	// ��ȡԭʼ����
	_asm
	{
		mov ecx, 0x176;
		rdmsr;							//��msr176�Ĵ��������ݱ���eax
		mov g_OldKiFastCallEntry, eax; //����ԭʼ������ַ
	}
	// ���ù���
	_asm
	{
		mov ecx, 0x176;
		mov eax, MyKiFastCallEntry; // ���˺���
		wrmsr;						//д�뵽msr0x176�Ĵ���
	}
}

// ж�ع���
void UnInstallHook()
{
	// ���ù���
	_asm
	{
		mov ecx, 0x176;
		mov eax, g_OldKiFastCallEntry;  // ���˺���
		wrmsr;							//д�뵽msr0x176�Ĵ���
	}
}