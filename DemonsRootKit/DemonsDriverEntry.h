#pragma once
#include "DeviceControl.h"
#include "KernelReload.h"
#include "SysenterHook.h"

// ж�غ���
NTSTATUS DriverUnload(PDRIVER_OBJECT pDriverObject);

// �����������ں���
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath);