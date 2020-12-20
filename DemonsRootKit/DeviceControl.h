#pragma once
#include "Driver.h"
#include "Process.h"
#include "IDT.h"
#include "Gdt.h"
#include "SSDT.h"
#include "Thread.h"
#include "File.h"
#include "SsdtHook.h"
#include "Registry.h"
#include "Module.h"

extern PDRIVER_OBJECT g_pDriver;
extern ULONG g_uPid;

// 创建设备对象
NTSTATUS CreateDevice(PDRIVER_OBJECT pDriverObject);

// 默认的消息派遣函数
NTSTATUS DefaultDispatch(PDEVICE_OBJECT pDeviceObject, PIRP pIrp);

NTSTATUS  Create(PDEVICE_OBJECT pDeviceObject, PIRP pIrp);

NTSTATUS  Close(PDEVICE_OBJECT pDeviceObject, PIRP pIrp);

NTSTATUS DeviceIoControlDispatch(PDEVICE_OBJECT pDeviceObject, PIRP pIrp);

// 设置派遣函数
VOID SetDispatch(PDRIVER_OBJECT pDriverObject);