#pragma once
#include "Data.h"

#define ListDirNext(Type,fileinfo) ((Type*)((ULONG_PTR)fileinfo + fileinfo->NextEntryOffset))
#define ListDirForEach(FileInfoType,fileInfo, iterator) \
	for (FileInfoType* iterator = fileInfo; \
		iterator->NextEntryOffset != 0;	 \
		iterator = ListDirNext(FileInfoType,iterator))

// 申请内存
VOID* alloc(ULONG uSize);

// 重新分配空间
void* reAlloc(void* src, ULONG uSize);

// 释放内存空间
void free(void* data);

// 创建文件
NTSTATUS createFile(
	wchar_t* wFilePath,
	ULONG access,
	ULONG share,
	ULONG openModel,
	BOOLEAN isDir,
	HANDLE* hFile);

// 获取文件大小
NTSTATUS getFileSize(HANDLE hFile, ULONG64* size);

// 读取文件内容
NTSTATUS readFile(
	HANDLE hFile,
	ULONG offsetLow,
	ULONG offsetHig,
	ULONG sizeToRead,
	PVOID pBuff,
	ULONG* read);

// 写入文件内容
NTSTATUS writeFile(
	HANDLE hFile,
	ULONG offsetLow,
	ULONG offsetHig,
	ULONG sizeToWrite,
	PVOID pBuff,
	ULONG* write);

// 拷贝文件
NTSTATUS copyFile(
	wchar_t* srcPath,
	wchar_t* destPath);

// 移动文件
NTSTATUS moveFile(
	wchar_t* srcPath,
	wchar_t* destPath);

// 删除文件
NTSTATUS removeFile(wchar_t* filePath);

// 列出一个目录下的文件和文件夹
NTSTATUS listDirGet(
	wchar_t* dir,
	FILE_BOTH_DIR_INFORMATION** fileInfo,
	ULONG maxFileCount);

// 获取一个目录下的第一个文件
NTSTATUS firstFile(
	wchar_t* dir,
	HANDLE* hFind,
	FILE_BOTH_DIR_INFORMATION* fileInfo,
	int nSize);

// 获取一个目录下的下一个文件
NTSTATUS nextFile(
	HANDLE hFind,
	FILE_BOTH_DIR_INFORMATION* fileInfo,
	int nSize);

// 释放空间
void listDirFree(FILE_BOTH_DIR_INFORMATION* fileInfo);

// 自定义的枚举类型
ULONG EnumFileData(
	PVOID pInputBuff, 
	PVOID pOutputBuff,
	ULONG outputSize);

// 获取第一个文件
ULONG EnumFirstFile(
	PVOID pInputBuff,
	PVOID pOutputBuff,
	ULONG outputSize);

// 获取下一个文件
ULONG EnumNextFile(PVOID pOutputBuff);

// 删除文件
BOOLEAN DeleteFiles(PVOID pFileName);