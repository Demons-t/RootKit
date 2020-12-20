#pragma once

#include <ntddk.h>

extern ULONG g_Pid;

// 关闭页保护
void OffPageProcted();


// 开页保护
void OnPageProcted();


// SSDT hook函数
void InstallSsdtHook();


// SSDT unhook函数
void UnInstaSsdtllHook();