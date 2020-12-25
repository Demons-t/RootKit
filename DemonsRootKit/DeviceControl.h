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

// �����豸����
NTSTATUS CreateDevice(PDRIVER_OBJECT pDriverObject);

// Ĭ�ϵ���Ϣ��ǲ����
NTSTATUS DefaultDispatch(PDEVICE_OBJECT pDeviceObject, PIRP pIrp);

NTSTATUS  Create(PDEVICE_OBJECT pDeviceObject, PIRP pIrp);

NTSTATUS  Close(PDEVICE_OBJECT pDeviceObject, PIRP pIrp);

NTSTATUS DeviceIoControlDispatch(PDEVICE_OBJECT pDeviceObject, PIRP pIrp);

// ������ǲ����
VOID SetDispatch(PDRIVER_OBJECT pDriverObject);