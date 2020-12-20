#pragma once
#include "DeviceControl.h"
#include "KernelReload.h"
#include "SysenterHook.h"

// 卸载函数
NTSTATUS DriverUnload(PDRIVER_OBJECT pDriverObject);

// 驱动程序的入口函数
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath);