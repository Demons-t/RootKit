#pragma once

#include <ntddk.h>

extern ULONG g_Pid;

// �ر�ҳ����
void OffPageProcted();


// ��ҳ����
void OnPageProcted();


// SSDT hook����
void InstallSsdtHook();


// SSDT unhook����
void UnInstaSsdtllHook();