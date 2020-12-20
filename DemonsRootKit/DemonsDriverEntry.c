#include "DemonsDriverEntry.h"

// 卸载函数
#pragma code_seg("PAGE")
NTSTATUS  DriverUnload(PDRIVER_OBJECT pDriverObject)
{
	// 卸载 SysenterHook
	UnInstallHook();

	// 删除设备对象的时候，必须先删除符号名，再删除设备对象
	UNICODE_STRING SymLinkName = { 0 };
	RtlInitUnicodeString(&SymLinkName, NAME_SYMBOL);
	IoDeleteSymbolicLink(&SymLinkName);

	// 可以通过驱动对象的链表找到设备对象，进行删除
	IoDeleteDevice(pDriverObject->DeviceObject);

	// 卸载内核重载
	UnHook();

	// 卸载 SSDT HOOK
	// UnInstaSsdtllHook();
	
	return STATUS_SUCCESS;
}

// 驱动程序的入口函数
#pragma code_seg("INIT")
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
	UNREFERENCED_PARAMETER(pRegistryPath);
	
	g_pDriver = pDriverObject;

	// 卸载驱动的回调函数
	pDriverObject->DriverUnload = DriverUnload;                                                                                

	// 创建设备对象
	CreateDevice(pDriverObject);

	// 设置派遣函数
	SetDispatch(pDriverObject);

	// SSDT HOOK
	// InstallSsdtHook();

	// 重载内核
	KernelReload();

	// 安装 SysenterHook
	InstallHook();

	return STATUS_SUCCESS;
}
