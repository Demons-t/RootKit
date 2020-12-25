#include "SsdtHook.h"

// SSDT hook ��������

// Ҫ�����Ľ���pid
ULONG g_Pid = 0;

#pragma pack(1)
typedef struct _ServiceDesriptorEntry
{
	ULONG* ServiceTableBase;        // ������ַ
	ULONG* ServiceCounterTableBase; // �������ַ
	ULONG NumberOfServices;         // ������ĸ���
	UCHAR* ParamTableBase;          // �������ַ
}SSDTEntry, * PSSDTEntry;
#pragma pack()
// ����ԭ��
typedef NTSTATUS(NTAPI* FnZwOpenProcess)(PHANDLE,
	ACCESS_MASK,
	POBJECT_ATTRIBUTES,
	PCLIENT_ID);


// ����SSDT,ֱ����������ʹ��
NTSYSAPI SSDTEntry KeServiceDescriptorTable;

// ����ԭʼ������ַ
FnZwOpenProcess g_OldZwOpenProcess;

// ���˺���
NTSTATUS NTAPI MyZwOpenProcess(
	__out PHANDLE  ProcessHandle,
	__in ACCESS_MASK  DesiredAccess,
	__in POBJECT_ATTRIBUTES  ObjectAttributes,
	__in_opt PCLIENT_ID  ClientId
)
{
	// �ж��Ƿ������ǵĺ���
	if ((ULONG)ClientId->UniqueProcess == g_Pid)
		DesiredAccess = 0;

	// ����ԭʼ����
	return g_OldZwOpenProcess(ProcessHandle,
		DesiredAccess,
		ObjectAttributes,
		ClientId);
}


// �ر�ҳ����
void OffPageProcted()
{
	_asm
	{
		mov eax, cr0;
		and eax, ~0x10000;
		mov cr0, eax;
	}

}

// ��ҳ����
void OnPageProcted()
{
	_asm
	{
		mov eax, cr0;
		or eax, 0x10000;
		mov cr0, eax;
	}
}


// SSDT hook����
void InstallSsdtHook()
{
	//1.����ԭʼ������ַ
	g_OldZwOpenProcess =
		(FnZwOpenProcess)KeServiceDescriptorTable.ServiceTableBase[0xBE];

	// �ر�ҳ����
	OffPageProcted();

	// 2. �滻���ǵĹ��˺���
	InterlockedExchange(
		&KeServiceDescriptorTable.ServiceTableBase[0xBE],
		(LONG)MyZwOpenProcess);

	// ��ҳ����
	OnPageProcted();
}

// SSDT unhook����
void UnInstaSsdtllHook()
{
	// �ر�ҳ����
	OffPageProcted();

	// 2. �滻���ǵĹ��˺���
	InterlockedExchange(
		&KeServiceDescriptorTable.ServiceTableBase[0xBE],
		(LONG)g_OldZwOpenProcess);

	// ��ҳ����
	OnPageProcted();
}

