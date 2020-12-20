#include "DeviceControl.h"

PDRIVER_OBJECT g_pDriver;
ULONG g_uPid;

// 创建设备对象
NTSTATUS CreateDevice(PDRIVER_OBJECT pDriverObject)
{
	// 创建需要用到的变量
	NTSTATUS status = STATUS_SUCCESS;
	PDEVICE_OBJECT pDeviceObject = NULL;

	// 初始化设备对象
	UNICODE_STRING DeviceName = { 0 };
	RtlInitUnicodeString(&DeviceName, NAME_DEVICE);

	// 创建设备对象的函数
	status = IoCreateDevice(
		pDriverObject,			// 所属驱动对象，创建后会被添加到它的 DeviceObject 链表中
		0,						// 设备的扩展空间大小，分配的空间会被 DeviceExtension 指向
		&DeviceName,			// 设备对象的名称
		FILE_DEVICE_UNKNOWN,	// 一般说明它是一个和硬件无关的虚拟设备
		0,						// 设备的属性信息
		TRUE,					// 是否独占 / R3是否可访问
		&pDeviceObject);		// 创建出的设备对象被保存到的地方

	// 通过 NT_SUCCESS 判断创建是否成功
	if (!NT_SUCCESS(status))
	{
		KdPrint(("设备对象创建失败"));
		IoDeleteDevice(pDeviceObject);
		return status;
	}

	// 设备对象的名称只能被内核程序解析，为了让R3应用识别
	// 需要设置符号链接名
	UNICODE_STRING SysLinkName = { 0 };
	RtlInitUnicodeString(&SysLinkName, NAME_SYMBOL);
	status = IoCreateSymbolicLink(&SysLinkName, &DeviceName);

	// 通过 NT_SUCCESS 判断创建是否成功
	if (!NT_SUCCESS(status))
	{
		KdPrint(("设备对象创建失败"));
		IoDeleteDevice(pDeviceObject);
		return status;
	}

	return status;
}

// 默认的消息派遣函数
NTSTATUS DefaultDispatch(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDeviceObject);

	// 设置消息的处理状态
	pIrp->IoStatus.Status = STATUS_SUCCESS;

	// 设置消息处理的内容长度
	pIrp->IoStatus.Information = 0;

	// 通知操作已经完成，完成后不提高当前的 IRQL
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS  Create(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDeviceObject);
	KdPrint(("驱动被创建了\n"));
	// 设置IRP完成状态
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS  Close(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDeviceObject);
	KdPrint(("驱动被关闭了\n"));
	// 设置IRP完成状态
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS DeviceIoControlDispatch(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDeviceObject);
	NTSTATUS ntStatus = STATUS_SUCCESS;
	ULONG ulComplateSize = 0;

	// 获取本层 IO 栈
	PIO_STACK_LOCATION pStack = IoGetCurrentIrpStackLocation(pIrp);

	// 得到输入缓冲区字节数
	ULONG uCinSize = pStack->Parameters.DeviceIoControl.InputBufferLength;

	// 得到输出缓冲区字节数
	ULONG uCoutSize = pStack->Parameters.DeviceIoControl.OutputBufferLength;

	// 获取通知码
	ULONG uCode = pStack->Parameters.DeviceIoControl.IoControlCode;

	// 获取输入缓冲区
	PVOID pCinBuff = pIrp->AssociatedIrp.SystemBuffer;

	// 获取输出缓冲区
	// 如果控制码指定了`METHOD_OUT_DIRECT`, 那么输出缓冲区就以MDL的形式传入
	// 如果在调用DeviceIoContral给输出缓冲区传递了NULL时,`MdlAddress` 也会为NULL 
	PVOID pCoutBuff = NULL;
	if (pIrp->MdlAddress && METHOD_FROM_CTL_CODE(uCode) & METHOD_OUT_DIRECT)
	{
		pCoutBuff = MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, 0);
	}

	// 根据控制码来进行操作
	switch (uCode)
	{
	case GETPID:
	{
		ULONG uPID;
		uPID = *(ULONG*)pCinBuff;
		g_uPid = uPID;
		g_Pid = uPID;
		KdPrint(("获取到PID: %d\n", uPID));
		KdPrint(("g_Pid = %d\n", g_Pid));
		KdPrint(("g_uPid = %d\n", g_uPid));
		break;
	}
	case ENUM_DRIVER:
	{
		// 遍历驱动
		ulComplateSize = EnumDriver(pCoutBuff, uCoutSize, g_pDriver);
		break;
	}
	case HIDE_DRIVER:
	{
		PWCHAR pInputBuf = (PWCHAR)pCinBuff;
		UNICODE_STRING uDriverName = { 0 };
		RtlInitUnicodeString(&uDriverName, pInputBuf);
		UINT8 uRet;

		// 隐藏驱动
		uRet = HideDriver(uDriverName, g_pDriver);
		break;
	}
	case PROCESS_FIRST:
	case PROCESS_NEXT:
	{
		// 遍历进程
		ulComplateSize = EnumProcess(pCoutBuff, uCoutSize, pCinBuff, uCode);
		break;
	}
	case TERMINATE_PROCESS:
	{
		// 结束进程
		KillProcess(pCinBuff);
		break;
	 }
	case ENUM_IDT:
	{
		// 遍历 IDT
		ulComplateSize = GetIdtInfo(pCoutBuff);
		break;
	}
	case ENUM_GDT:
	{
		// 遍历 GDT
		ulComplateSize = GetGdtInfo(pCoutBuff, uCoutSize);
		break;
	}
	case ENUM_SSDT:
	{
		// 遍历 ssdt
		ulComplateSize = EnumSsdt(pCoutBuff, uCoutSize);
		break;
	}
	case ENUM_THREAD:
	{
		// 遍历线程
		ulComplateSize = EnumThread(pCoutBuff, uCoutSize, pCinBuff);
		break;
	}
	case HIDE_PROCESS:
	{
		// 隐藏进程
		HideProcess(pCinBuff);
		break;
	}
	case FILE_DATA:
	{
		// 遍历自定义文件数据
		ulComplateSize = EnumFileData(pCinBuff, pCoutBuff, uCoutSize);
		break;
	}
	case FILE_FIRST:
	{
		// 获取第一个文件
		ulComplateSize = EnumFirstFile(pCinBuff, pCoutBuff, uCoutSize);
		break;
	}
	case FILE_NEXT:
	{
		// 如果输入缓冲区和输出缓冲区不是结构体 FILE_INFO,就退出
		if (uCinSize != sizeof(FILE_INFO) || uCoutSize != uCinSize)
			break;

		// 获取下一个文件
		ulComplateSize = EnumNextFile(pCoutBuff);
		break;
	}
	case DELETE_FILE:
	{
		// 删除文件
		DeleteFiles(pCinBuff);
		break;
	}
	case ENUM_REGISTRY:
	{
		// 遍历注册表
		ulComplateSize = EnumRegister(pCoutBuff, uCoutSize, pCinBuff);
		break;
	}
	case NEW_REGISTRY:
	{
		// 创建子项
		NewReg(pCinBuff);
		break;
	}
	case DELETE_REGISTRY_KEY:
	{
		// 删除子项
		DeleteRegKey(pCinBuff);
		break;
	}
	case ENUM_MODULE:
	{
		// 遍历模块
		ulComplateSize = EnumModule(pCoutBuff, uCoutSize, pCinBuff);
		break;
	}
	default:
		break;
	}

	// 设置消息的处理状态
	pIrp->IoStatus.Status = ntStatus;

	// 设置消息处理的内容长度
	pIrp->IoStatus.Information = ulComplateSize;

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return ntStatus;
}

// 设置派遣函数
VOID SetDispatch(PDRIVER_OBJECT pDriverObject)
{
	// 为所有的 IRP 消息设置默认的派遣函数
	for (int i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++)
		pDriverObject->MajorFunction[i] = DefaultDispatch;

	// 创建设备对象之后，可以通过驱动对象设置消息的派遣函数(消息响应)
	// 消息是由设备对象产生的，但统一由驱动对象进行处理
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = Create;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = Close;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceIoControlDispatch;
}
