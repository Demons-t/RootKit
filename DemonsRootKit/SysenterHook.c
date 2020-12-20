#include "SysenterHook.h"

#include <ntddk.h>

// 原始函数
ULONG g_OldKiFastCallEntry = 0;
// 要保护的进程PID
ULONG g_uPid = 0;

// 1. 获取原始KiFastCallEntry函数
//			1.1 ecx 保存寄存器号
//			1.2 rdmsr 调用指令
//			1.3 eax 返回保存原始函数

// 2. 将我们过滤函数替换到msr 0x176号寄存器中 
//     2.1 ecx 保存寄存器号,eax 写入过滤函数
//	   2.2 wrmsr 调用指令

// 3. 卸载钩子
//     3.1 ecx 保存寄存器号,eax 写入原始函数
//	   2.2 wrmsr 调用指令


// 过滤函数
void _declspec(naked) MyKiFastCallEntry()
{
	// 过滤 ZwOpenProcess,调用号eax == 0x0BE
	// edx保存用户栈
	//  [edx + 0x00] : 返回地址1
	//	[edx + 0x04] : 返回地址2
	//	[edx + 0x08] : 参数1    ProcessHandle
	//	[edx + 0x0c] : 参数2    DesiredAccess
	//	[edx + 0x10] : 参数3    ObjectAttributes
	//	[edx + 0x14] : 参数3    ClientId

	_asm
	{
		pushad;					// 保存寄存器
		cmp eax, 0x0BE;			// 是否是ZwOpenProcess 函数
		jne CallEnd;			// 结束过滤
		mov eax, [edx + 0x14];	// 获取第四个参数 ClientId
		mov eax, [eax];			// ClientId->ProcessId
		cmp eax, g_uPid;			//判断是否要保存的进程
		jne CallEnd;
		mov[edx + 0x0c], 0;		// 将权限改为0，无法访问

	CallEnd:
		popad;						//恢复寄存器
		jmp g_OldKiFastCallEntry;	//调用原始函数
	}
}

// 安装钩子
void InstallHook()
{
	// 获取原始函数
	_asm
	{
		mov ecx, 0x176;
		rdmsr;							//将msr176寄存器的内容保存eax
		mov g_OldKiFastCallEntry, eax; //保存原始函数地址
	}
	// 设置钩子
	_asm
	{
		mov ecx, 0x176;
		mov eax, MyKiFastCallEntry; // 过滤函数
		wrmsr;						//写入到msr0x176寄存器
	}
}

// 卸载钩子
void UnInstallHook()
{
	// 设置钩子
	_asm
	{
		mov ecx, 0x176;
		mov eax, g_OldKiFastCallEntry;  // 过滤函数
		wrmsr;							//写入到msr0x176寄存器
	}
}