#pragma once
#include "Data.h"

// ����ռ�
VOID FreePool(PDRIVER pTemp, PDRIVER pDriverHead, ULONG uCount);

// ��������
ULONG EnumDriver(PVOID pOutBuff, ULONG uCoutSize, PDRIVER_OBJECT pDriverObject);

// ��������
UINT8 HideDriver(UNICODE_STRING uDriverName, PDRIVER_OBJECT pDriverObject);

