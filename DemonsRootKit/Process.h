#pragma once
#include "Data.h"

// ��������
ULONG EnumProcess(PVOID pOutPut, ULONG uBuffSize, PVOID pInputBuff, ULONG uCode);

// ���ؽ���
VOID HideProcess(PVOID pPid);

// ��������
VOID KillProcess(PVOID pPid);