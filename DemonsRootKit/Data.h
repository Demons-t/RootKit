#pragma once
#include <ntifs.h>

#define NAME_DEVICE L"\\Device\\demons"
#define NAME_SYMBOL L"\\??\\augen"

#define _CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)

#define IOCTL_BASE        0x800

#define DEMONS_CODE(code)			\
		CTL_CODE					\
		(							\
			FILE_DEVICE_UNKNOWN	,	\
			IOCTL_BASE + (code),	\
			METHOD_OUT_DIRECT,		\
			0						\
		)

typedef enum _DemonsCtlCOde
{
	GETPID				= DEMONS_CODE(0),
	ENUM_DRIVER			= DEMONS_CODE(1),
	HIDE_DRIVER			= DEMONS_CODE(2),
	PROCESS_FIRST		= DEMONS_CODE(3),
	PROCESS_NEXT		= DEMONS_CODE(4),
	TERMINATE_PROCESS	= DEMONS_CODE(5),
	ENUM_THREAD			= DEMONS_CODE(6),
	ENUM_IDT			= DEMONS_CODE(7),
	ENUM_GDT			= DEMONS_CODE(8),
	ENUM_SSDT			= DEMONS_CODE(9),
	HIDE_PROCESS		= DEMONS_CODE(10),
	FILE_DATA			= DEMONS_CODE(11),
	FILE_FIRST			= DEMONS_CODE(12),
	FILE_NEXT			= DEMONS_CODE(13),
	DELETE_FILE			= DEMONS_CODE(14),
	ENUM_REGISTRY		= DEMONS_CODE(15),
	NEW_REGISTRY		= DEMONS_CODE(16),
	DELETE_REGISTRY_KEY = DEMONS_CODE(17),
	ENUM_MODULE			= DEMONS_CODE(18),
}DemonsCtlCOde;

typedef struct _LDR_DATA_TABLE_ENTRY {
	struct _LIST_ENTRY InLoadOrderLinks;                                    //0x0
	struct _LIST_ENTRY InMemoryOrderLinks;                                  //0x8
	struct _LIST_ENTRY InInitializationOrderLinks;                          //0x10
	PVOID DllBase;                                                          //0x18
	PVOID EntryPoint;                                                       //0x1c
	ULONG SizeOfImage;                                                      //0x20
	struct _UNICODE_STRING FullDllName;                                     //0x24
	struct _UNICODE_STRING BaseDllName;                                     //0x2c
	// ... 后面还有字段，因为不需要使用，所以不做考虑
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

// 声明函数原型和结构体
typedef struct _PEB_LDR_DATA {
	ULONG                   Length;
	BOOLEAN                 Initialized;
	PVOID                   SsHandle;
	LIST_ENTRY              InLoadOrderModuleList;
	LIST_ENTRY              InMemoryOrderModuleList;
	LIST_ENTRY              InInitializationOrderModuleList;
} PEB_LDR_DATA, * PPEB_LDR_DATA;

// 下面的结构体是PEB部分字段.
typedef struct _PEB_SHORT {
	BOOLEAN                 InheritedAddressSpace;
	BOOLEAN                 ReadImageFileExecOptions;
	BOOLEAN                 BeingDebugged;
	BOOLEAN                 Spare;
	HANDLE                  Mutant;
	PVOID                   ImageBaseAddress;
	PPEB_LDR_DATA           Ldr;
}PEB_SHORT, *PPEB_SHORT;

// 驱动结构体
#pragma pack(1)
typedef struct _DRIVER
{
	ULONG uDllBase;					// 基址
	ULONG uSize;					// 大小
	char cName[256 * 2];			// 驱动名
	char cFullDllName[256 * 2];		// 路径
	struct _DRIVER* Next;
}DRIVER, *PDRIVER;
#pragma pack()

// 进程结构体
#pragma pack(1)
typedef struct _PROCESS_INFO
{
	HANDLE hProcessHandle;
	CHAR wName[256];				// 名字
	unsigned int uPid;				// PID
	unsigned int uPPid;				// 父进程ID
	WCHAR cFullDllName[256];		// 路径

}PROCESS_INFO, *PPROCESS_INFO;
#pragma pack()

// 文件结构体
#pragma pack(1)
typedef	 struct _FILE_INFO
{
	HANDLE	hFindHandle;

	ULONG dwFileAttributes;
	ULONG64 ftCreationTime;
	ULONG64 ftLastAccessTime;
	ULONG64 ftLastWriteTime;
	ULONG nFileSizeHigh;
	ULONG nFileSizeLow;
	WCHAR  cFileName[266];
}FILE_INFO, * PFILE_INFO;
#pragma pack()

// idtr寄存器指向这个结构体
#pragma pack(1)
typedef struct _IDT_INFO
{
	UINT16 uIdtLimit;		// IDT 范围
	UINT16 uLowIdtBase;		// IDT 低地址
	UINT16 uHighIdtBase;	// IDT 高基址
}IDT_INFO, *PIDT_INFO;
#pragma pack()

// IDT 表中描述符的结构体
#pragma pack(1)
typedef struct _IDT_ENTRY
{
	UINT16	uOffsetLow;			// 处理程序低地址偏移
	UINT16	uSelector;			// 端选择器
	UINT8	uReserved;			// 保留
	UINT8	GateType : 4;		// 中断类型
	UINT8	StorageSegment : 1;	// 为 0 是中断门
	UINT8	DPL : 2;			// 特权级
	UINT8	Present : 1;		// 未使用中断可置为0
	UINT16	uOffsetHigh;		// 处理程序高地址偏移
}IDT_ENTRY,*PIDT_ENTRY;
#pragma pack()

// gdtr 寄存器指向这个结构体
#pragma pack(1)
typedef struct _GDT_INFO
{
	UINT16	uGdtLimit;			// GDT 范围
	UINT16	uLowGdtBase;		// GDT 低基址
	UINT16	uHighGdtBase;		// GDT 高基址
}GDT_INFO, *PGDT_INFO;
#pragma pack()

// 段描述符的结构
#pragma pack(1)
typedef struct _GDT
{
	UINT64 Limit0_15 : 16;	// 界限
	UINT64 base0_23 : 24;	// 基址
	UINT64 TYPE : 4;		// 类型
	UINT64 S : 1;			// S 位置(为0则是系统段，1是代码段或者数据段)
	UINT64 DPL : 2;			// DPL 特权级
	UINT64 P : 1;			// P位置 段存在
	UINT64 Limit16_19 : 4;	// 界限
	UINT64 AVL : 1;			// AVL 系统软件可用位
	UINT64 O : 1;			// O
	UINT64 D_B : 1;			// D_B 默认大小
	UINT64 G : 1;			// G 颗粒度
	UINT64 Base24_31 : 8;	// Base
}GDT,*PGDT;
#pragma pack()

// 系统描述符表结构体
#pragma pack(1)
typedef struct _ServiceDesriptorEntry
{
	PVOID ServiceTableBase;        // 服务表基址
	PULONG ServiceCounterTableBase; // 计数表基址
	ULONG NumberOfServices;         // 表中项的个数
	ULONG ParamTableBase;          // 参数表基址
}SSDTEntry, * PSSDTEntry;
#pragma pack()

// ssdt
#pragma pack(1)
typedef struct _SSDT
{
	ULONG uAddress;			// 加载基址
	ULONG uSysCallIndex;	// 调用号
}SSDT, *PSSDT;
#pragma pack()

//线程信息结构体
#pragma pack(1)
typedef struct _THREAD_INFO
{
	ULONG ulTID;			// 线程ID
	ULONG ulBasePriority;	// 优先级
	ULONG ulStartAddress;	// 线程的起始地址
	ULONG ulStatus;			// 状态
	struct _THREAD_INFO* Next;	//
}THREAD_INFO, * PTHREAD_INFO;
#pragma pack()

// 注册表结构体
#pragma pack(1)
typedef struct _REGISTRY
{
	ULONG uType;				// 类型，0是子项，1是值
	char cKeyName[256 * 2];		// 项的名字
	char cValueName[256 * 2];	// 值的名字
	ULONG uValueType;			// 值的类型
	UCHAR uValue[256];			// 值
}REGISTRY, * PREGISTRY;
#pragma pack()

// 模块结构体
#pragma pack(1)
typedef struct _MODULE
{
	char cFullDllName[256 * 2];	// 模块路径
	ULONG uStartAddress;		// 模块基地址
	ULONG uSize;				// 模块大小
	struct _MODULE* Next;
}MODULE, * PMODULE;
#pragma pack()

struct _PEB
{
	UCHAR InheritedAddressSpace;                                            //0x0
	UCHAR ReadImageFileExecOptions;                                         //0x1
	UCHAR BeingDebugged;
	UCHAR BitField;
	void* Mutant;                                                           //0x4
	void* ImageBaseAddress;                                                 //0x8
	struct _PEB_LDR_DATA* Ldr;
};