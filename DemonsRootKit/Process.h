#pragma once
#include "Data.h"

// 遍历进程
ULONG EnumProcess(PVOID pOutPut, ULONG uBuffSize, PVOID pInputBuff, ULONG uCode);

// 隐藏进程
VOID HideProcess(PVOID pPid);

// 结束进程
VOID KillProcess(PVOID pPid);