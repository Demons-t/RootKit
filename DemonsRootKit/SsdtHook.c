#include "SsdtHook.h"

// SSDT hook 保护自身

// 要保护的进程pid
ULONG g_Pid = 0;

#pragma pack(1)
typedef struct _ServiceDesriptorEntry
{
	ULONG* ServiceTableBase;        // 服务表基址
	ULONG* ServiceCounterTableBase; // 计数表基址
	ULONG NumberOfServices;         // 表中项的个数
	UCHAR* ParamTableBase;          // 参数表基址
}SSDTEntry, * PSSDTEntry;
#pragma pack()
// 函数原型
typedef NTSTATUS(NTAPI* FnZwOpenProcess)(PHANDLE,
	ACCESS_MASK,
	POBJECT_ATTRIBUTES,
	PCLIENT_ID);


// 导入SSDT,直接声明就能使用
NTSYSAPI SSDTEntry KeServiceDescriptorTable;

// 保存原始函数地址
FnZwOpenProcess g_OldZwOpenProcess;

// 过滤函数
NTSTATUS NTAPI MyZwOpenProcess(
	__out PHANDLE  ProcessHandle,
	__in ACCESS_MASK  DesiredAccess,
	__in POBJECT_ATTRIBUTES  ObjectAttributes,
	__in_opt PCLIENT_ID  ClientId
)
{
	// 判断是否是我们的函数
	if ((ULONG)ClientId->UniqueProcess == g_Pid)
		DesiredAccess = 0;

	// 调用原始函数
	return g_OldZwOpenProcess(ProcessHandle,
		DesiredAccess,
		ObjectAttributes,
		ClientId);
}


// 关闭页保护
void OffPageProcted()
{
	_asm
	{
		mov eax, cr0;
		and eax, ~0x10000;
		mov cr0, eax;
	}

}

// 开页保护
void OnPageProcted()
{
	_asm
	{
		mov eax, cr0;
		or eax, 0x10000;
		mov cr0, eax;
	}
}


// SSDT hook函数
void InstallSsdtHook()
{
	//1.保存原始函数地址
	g_OldZwOpenProcess =
		(FnZwOpenProcess)KeServiceDescriptorTable.ServiceTableBase[0xBE];

	// 关闭页保护
	OffPageProcted();

	// 2. 替换我们的过滤函数
	InterlockedExchange(
		&KeServiceDescriptorTable.ServiceTableBase[0xBE],
		(LONG)MyZwOpenProcess);

	// 开页保护
	OnPageProcted();
}

// SSDT unhook函数
void UnInstaSsdtllHook()
{
	// 关闭页保护
	OffPageProcted();

	// 2. 替换我们的过滤函数
	InterlockedExchange(
		&KeServiceDescriptorTable.ServiceTableBase[0xBE],
		(LONG)g_OldZwOpenProcess);

	// 开页保护
	OnPageProcted();
}

