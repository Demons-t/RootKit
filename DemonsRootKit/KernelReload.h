#pragma once
#include <ntifs.h>
#include <ntimage.h>

// 打开文件
HANDLE KernelCreateFile(
	IN PUNICODE_STRING pstrFile, // 文件路径符号链接
	IN BOOLEAN bIsDir);   // 是否为文件夹

// 获取文件大小
ULONG64 KernelGetFileSize(IN HANDLE hfile);

// 读取文件
ULONG64 KernelReadFile(
	IN  HANDLE         hfile,    // 文件句柄
	IN  PLARGE_INTEGER Offset,   // 从哪里开始读取
	IN  ULONG          ulLength, // 读取多少字节
	OUT PVOID          pBuffer);  // 保存数据的缓存

// 搜索内存特征
void* SearchMemory(char* buf, int BufLenth, char* Mem, int MaxLenth);

// 关闭页保护
void OffProtected();

// 开启页保护
void OnProtected();


// 通过名称获取模块地址
ULONG32 MyGetModuleHandle(PUNICODE_STRING pModuleName);


// 读取内核模块到内存中
void ReadKernelToBuf(PWCHAR pPath, PUCHAR* pBuf);


// 展开内核PE文件
void ZKKernel(PUCHAR* pZkBUf, PUCHAR buf);


// 修复新内核重定位
void FixReloc(PUCHAR ZkBuf, PUCHAR OldBase);


// 修复旧SSDT表
void FixSSDT(PUCHAR pZKBuf, PUCHAR OldBase);


// 获取KiFastCallEntry函数
ULONG GetKiFastCallEntry();


// hook中过滤函数
ULONG   FilterFun(ULONG SSdtBase, PULONG OldFun, ULONG Id);


// hookKiFastCallEntry函数
void OnHookKiFastCallEntry();


// 内核重载 开始
void KernelReload();


// 卸载内核钩子
void UnHook();