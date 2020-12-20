#include "File.h"

// 申请内存
VOID* alloc(ULONG uSize)
{
	return ExAllocatePoolWithTag(PagedPool, uSize, 'abcd');
}

// 重新分配空间
void* reAlloc(void* src, ULONG uSize)
{
	if (!src)
		return NULL;
	
	void* data = alloc(uSize);
	RtlCopyMemory(data, src, uSize);
	free(data);
	return data;
}

// 释放内存空间
void free(void* data)
{
	if (data)
		ExFreePoolWithTag(data, 'abcd');
}

// 创建文件
NTSTATUS createFile(wchar_t* wFilePath, ULONG access, ULONG share, ULONG openModel, BOOLEAN isDir, HANDLE* hFile)
{
	NTSTATUS status = STATUS_SUCCESS;

	IO_STATUS_BLOCK StatusBlock = { 0 };
	ULONG           ulShareAccess = share;
	ULONG ulCreateOpt = FILE_SYNCHRONOUS_IO_NONALERT;

	UNICODE_STRING path;
	RtlInitUnicodeString(&path, wFilePath);

	// 1. 初始化OBJECT_ATTRIBUTES的内容
	OBJECT_ATTRIBUTES objAttrib = { 0 };
	ULONG ulAttributes = OBJ_CASE_INSENSITIVE/*不区分大小写*/ | OBJ_KERNEL_HANDLE/*内核句柄*/;
	InitializeObjectAttributes(&objAttrib,    // 返回初始化完毕的结构体
		&path,      // 文件对象名称
		ulAttributes,  // 对象属性
		NULL, NULL);   // 一般为NULL

// 2. 创建文件对象
	ulCreateOpt |= isDir ? FILE_DIRECTORY_FILE : FILE_NON_DIRECTORY_FILE;

	status = ZwCreateFile(
		hFile,					// 返回文件句柄
		access,					// 文件操作描述
		&objAttrib,				// OBJECT_ATTRIBUTES
		&StatusBlock,			// 接受函数的操作结果
		0,						// 初始文件大小
		FILE_ATTRIBUTE_NORMAL,	// 新建文件的属性
		ulShareAccess,			// 文件共享方式
		openModel,				// 文件存在则打开不存在则创建
		ulCreateOpt,			// 打开操作的附加标志位
		NULL,					// 扩展属性区
		0);						// 扩展属性区长度

	return status;
}

// 获取文件大小
NTSTATUS getFileSize(HANDLE hFile, ULONG64* size)
{
	IO_STATUS_BLOCK ioStatus = { 0 };
	FILE_STANDARD_INFORMATION file = { 0 };
	NTSTATUS status;
	status = ZwQueryInformationFile(
		hFile,						// 文件句柄
		&ioStatus,					// 完成状态
		&file,						// 保存文件信息的缓冲区
		sizeof(file),				// 缓冲区的字节数
		FileStandardInformation);	// 要获取的信息类型
	if (STATUS_SUCCESS == status)
	{
		// 保存文件字节数
		*size = file.EndOfFile.QuadPart;
	}

	return status;
}

// 读取文件内容
NTSTATUS readFile(HANDLE hFile, ULONG offsetLow, ULONG offsetHig, ULONG sizeToRead, PVOID pBuff, ULONG* read)
{
	NTSTATUS status;
	IO_STATUS_BLOCK ioStatus = { 0 };
	LARGE_INTEGER offset;

	// 设置要读取的文件偏移
	offset.HighPart = offsetHig;
	offset.LowPart = offsetLow;

	status = ZwReadFile(
		hFile,			// 文件句柄
		NULL,			// 事件对象，用于异步IO
		NULL,			// APC 的完成通知例程，用于异步IO
		NULL,			// 完成通知例程的附加参数
		&ioStatus,		// IO 状态
		pBuff,			// 保存文件数据的缓冲区
		sizeToRead,		// 要读取的字节数
		&offset,		// 要读取的文件位置
		NULL);
	if (status == STATUS_SUCCESS)
		*read = ioStatus.Information;

	return status;
}

// 写入文件内容
NTSTATUS writeFile(HANDLE hFile, ULONG offsetLow, ULONG offsetHig, ULONG sizeToWrite, PVOID pBuff, ULONG* write)
{
	NTSTATUS status;
	IO_STATUS_BLOCK ioStatus = { 0 };
	LARGE_INTEGER offset;

	// 设置要写入的文件偏移
	offset.HighPart = offsetHig;
	offset.LowPart = offsetLow;

	status = ZwWriteFile(
		hFile,			// 文件句柄
		NULL,			// 事件对象，用于异步IO
		NULL,			// APC 的完成通知例程，用于异步IO
		NULL,			// 完成通知例程的附加参数
		&ioStatus,		// IO 状态
		pBuff,			// 写入到文件中的缓冲区
		sizeToWrite,		// 写入的字节数
		&offset,		// 写入到的文件偏移
		NULL);
	if (status == STATUS_SUCCESS)
		*write = ioStatus.Information;

	return status;
}

// 复制文件
NTSTATUS copyFile(wchar_t* srcPath, wchar_t* destPath)
{
	HANDLE hSrc = (HANDLE)-1;
	HANDLE hDest = (HANDLE)-1;
	NTSTATUS status = STATUS_SUCCESS;
	ULONG64 srcSize = 0;
	ULONG size = 0;
	char* pBuff = NULL;

	__try
	{
		// 打开源文件
		status = createFile(
			srcPath,
			GENERIC_READ,
			FILE_SHARE_READ,
			FILE_OPEN_IF,
			FALSE,
			&hSrc);
		if (STATUS_SUCCESS != status)
			__leave;

		// 获取源文件大小
		if (STATUS_SUCCESS != getFileSize(hSrc, &srcSize))
			__leave;

		// 分配内存空间保存源文件的数据
		pBuff = ExAllocatePoolWithTag(PagedPool, (ULONG)srcSize, 'abcd');
		if (pBuff == NULL)
			__leave;

		// 读取源文件的数据到内存中
		status = readFile(hSrc, 0, 0, (ULONG)srcSize, pBuff, &size);
		if (STATUS_SUCCESS != status || size != (ULONG)srcSize)
			__leave;

		// 打开目标文件
		status = createFile(
			destPath,
			GENERIC_WRITE,
			FILE_SHARE_READ,
			FILE_CREATE,
			FALSE,
			&hDest);
		if (STATUS_SUCCESS != status)
			__leave;

		// 将源文件的数据写入到目标文件
		status = writeFile(hDest, 0, 0, (ULONG)srcSize, pBuff, &size);
		if (STATUS_SUCCESS != status || size != srcSize)
			__leave;
	}
	__finally
	{
		// 关闭源文件
		if (hSrc != (HANDLE)-1)
			ZwClose(hSrc);

		// 关闭目标文件
		if (hDest != (HANDLE)-1)
			ZwClose(hDest);

		// 释放缓冲区
		if (pBuff)
			ExFreePoolWithTag(pBuff, 'abcd');
	}

	return status;
}

// 拷贝文件
NTSTATUS moveFile(wchar_t* srcPath, wchar_t* destPath)
{
	NTSTATUS status = STATUS_SUCCESS;

	// 拷贝一份文件
	status = copyFile(srcPath, destPath);

	// 如果拷贝成功，删除源文件
	if (status == STATUS_SUCCESS)
		status = removeFile(srcPath);

	return status;
}

// 删除文件
NTSTATUS removeFile(wchar_t* filePath)
{
	UNICODE_STRING path;
	RtlInitUnicodeString(&path, filePath);

	// 初始化 OBJECT_ATTRIBUTES 的内容
	OBJECT_ATTRIBUTES objAttrib = { 0 };
	ULONG             ulAttributes = OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE;
	InitializeObjectAttributes(
		&objAttrib,    // 返回初始化完毕的结构体
		&path,		  // 文件对象名称
		ulAttributes,  // 对象属性
		NULL,          // 根目录(一般为NULL)
		NULL);         // 安全属性(一般为NULL)
		
	// 删除指定文件 / 文件夹
	return ZwDeleteFile(&objAttrib);
}

// 列出一个目录下的文件和文件夹
NTSTATUS listDirGet(wchar_t* dir, FILE_BOTH_DIR_INFORMATION** fileInfo, ULONG maxFileCount)
{
	NTSTATUS status = STATUS_SUCCESS;
	IO_STATUS_BLOCK ioStatus = { 0 };
	HANDLE hDir = (HANDLE)-1;
	VOID* pBuff = NULL;

	__try
	{
		// 打开目录
		status = createFile(
			dir,
			GENERIC_READ,
			FILE_SHARE_READ,
			FILE_OPEN_IF,
			TRUE,
			&hDir);
		if (STATUS_SUCCESS != status)
			__leave;

		// 计算出保存一个文件信息所需的最大字节数 = 结构体大小 + 文件名大小
		ULONG signalFileInforSize = sizeof(FILE_BOTH_DIR_INFORMATION) + 267 * 2;

		// 计算出总空间字节数
		ULONG uTotalSize = signalFileInforSize * maxFileCount;

		// 申请内存空间
		pBuff = ExAllocatePoolWithTag(PagedPool, uTotalSize, 'abcd');
		if (pBuff == NULL)
			__leave;

		// 第一次调用，获取所需缓冲区字节数
		status = ZwQueryDirectoryFile(
			hDir,							// 目录句柄
			NULL,							// 事件对象
			NULL,							// 完成通知例程
			NULL,							// 完成通知例程附加参数
			&ioStatus,						// IO状态
			pBuff,							// 输出的文件信息
			uTotalSize,						// 文件信息缓冲区的字节数
			FileBothDirectoryInformation,	// 获取信息的类型
			FALSE,							// 是否只获取第一个
			0,
			TRUE);							//是否重新扫描目录

		// 保存缓冲区的内容首地址
		if (status == STATUS_SUCCESS)
			*fileInfo = (FILE_BOTH_DIR_INFORMATION*)pBuff;
	}
	__finally
	{
		if (hDir != (HANDLE)-1)
			ZwClose(hDir);

		if (status != STATUS_SUCCESS && pBuff != NULL)
			ExFreePoolWithTag(pBuff, 'abcd');
	}

	return status;
}

// 获取一个目录下的第一个文件
NTSTATUS firstFile(wchar_t* dir, HANDLE* hFind, FILE_BOTH_DIR_INFORMATION* fileInfo, int nSize)
{
	NTSTATUS status = STATUS_SUCCESS;
	IO_STATUS_BLOCK ioStatus = { 0 };

	// 打开目录
	status = createFile(
		dir,
		GENERIC_READ,
		FILE_SHARE_READ,
		FILE_OPEN_IF,
		TRUE,
		hFind);

	if (STATUS_SUCCESS != status)
		return status;

	// 第一次调用，获取所需缓冲区的字节数
	status = ZwQueryDirectoryFile(
		*hFind,							// 目录句柄
		NULL,							// 事件对象
		NULL,							// 完成通知例程
		NULL,							// 完成通知例程附加参数
		&ioStatus,						// IO状态
		fileInfo,						// 输出的文件信息
		nSize,							// 文件信息缓冲区的字节数
		FileBothDirectoryInformation,	// 获取信息的类型
		TRUE,							// 是否只获取第一个
		0,
		TRUE);							//是否重新扫描目录

	return status;
}

// 获取一个目录下的下一个文件
NTSTATUS nextFile(HANDLE hFind, FILE_BOTH_DIR_INFORMATION* fileInfo, int nSize)
{
	IO_STATUS_BLOCK ioStatus = { 0 };

	// 第一次调用，获取所需缓冲区字节数
	return ZwQueryDirectoryFile(
		hFind,							// 目录句柄
		NULL,							// 事件对象
		NULL,							// 完成通知例程
		NULL,							// 完成通知例程附加参数
		&ioStatus,						// IO状态
		fileInfo,						// 输出的文件信息
		nSize,							// 文件信息缓冲区的字节数
		FileBothDirectoryInformation,	// 获取信息的类型
		TRUE,							// 是否只获取第一个
		0,
		FALSE);							//是否重新扫描目录
}

// 释放空间
void listDirFree(FILE_BOTH_DIR_INFORMATION* fileInfo)
{
	ExFreePoolWithTag(fileInfo, 'abcd');
}

// 自定义的枚举类型
ULONG EnumFileData(PVOID pInputBuff, PVOID pOutputBuff, ULONG outputSize)
{
	NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING filePath;
	HANDLE hFile = NULL;
	ULONG uComplateSize = 0;

	// 初始化字符串
	RtlInitUnicodeString(&filePath, (PCWSTR)pInputBuff);

	// 打开文件
	status = createFile(
		(LPWSTR)pInputBuff,
		GENERIC_READ,
		FILE_SHARE_READ,
		FILE_OPEN_IF,
		FALSE,
		&hFile);
	if (STATUS_SUCCESS != status)
		uComplateSize = 0;

	// 获取文件大小
	ULONG64 fileSize = 0;
	status = getFileSize(hFile, &fileSize);
	if (STATUS_SUCCESS != status)
	{
		uComplateSize = 0;
		ZwClose(hFile);
	}

	// 判断输出缓冲区的大小是否能够容纳
	if (pOutputBuff == NULL || outputSize < fileSize)
	{
		// 如果缓冲区为 NULL 或者输出缓冲区的大小不够大
		// 则设置完成的字节数是所需字节数
		// 探后不对缓冲区进行输出，只设置
		// 缓冲区的大小，这样，DeviceIoControl
		// 返回之后，就能得到所需字节数
		uComplateSize = (ULONG)fileSize;
		ZwClose(hFile);
	}

	ULONG read = 0;
	readFile(hFile, 0, 0, (ULONG)fileSize, pOutputBuff, &read);
	ZwClose(hFile);

	// 设置读取到的字节数
	// 保存实际读取到的字节数
	uComplateSize = read;

	return uComplateSize;
}

// 获取第一个文件
ULONG EnumFirstFile(PVOID pInputBuff, PVOID pOutputBuff, ULONG outputSize)
{
	HANDLE hFind = NULL;
	NTSTATUS status = STATUS_SUCCESS;
	ULONG uComplateSize = 0;

	// 定义一个缓冲区，用于保存输出的文件内容
	char tempBuff[sizeof(FILE_BOTH_DIR_INFORMATION) + 267 * 2];
	FILE_BOTH_DIR_INFORMATION* pInfo = (FILE_BOTH_DIR_INFORMATION*)tempBuff;

	// 获取第一个文件
	status = firstFile((wchar_t*)pInputBuff, &hFind, pInfo, sizeof(tempBuff));

	if (status == STATUS_SUCCESS)
	{
		if (pOutputBuff != NULL && outputSize >= sizeof(FILE_INFO))
		{
			PFILE_INFO pFindInfo = (PFILE_INFO)pOutputBuff;
			pFindInfo->hFindHandle = hFind;

			// 保存信息
			RtlCopyMemory(pFindInfo->cFileName, pInfo->FileName, pInfo->FileNameLength);
			pFindInfo->cFileName[pInfo->FileNameLength / 2] = 0;
			
			pFindInfo->dwFileAttributes = pInfo->FileAttributes;
			pFindInfo->ftCreationTime = pInfo->CreationTime.QuadPart;
			pFindInfo->ftLastAccessTime = pInfo->LastAccessTime.QuadPart;
			pFindInfo->ftLastWriteTime = pInfo->LastWriteTime.QuadPart;
			pFindInfo->nFileSizeHigh = pInfo->EndOfFile.HighPart;
			pFindInfo->nFileSizeLow = pInfo->EndOfFile.LowPart;

			// 设置完成的字节数
			uComplateSize = sizeof(FILE_INFO);
		}
	}

	return uComplateSize;
}

// 获取下一个文件
ULONG EnumNextFile(PVOID pOutputBuff)
{
	NTSTATUS status = STATUS_SUCCESS;
	ULONG uComplateSize = 0;

	// 获取输出缓冲区
	PFILE_INFO pFindInfo = (PFILE_INFO)pOutputBuff;

	// 定义一个结构体，用于保存输出的文件内容
	char tempBuff[sizeof(FILE_BOTH_DIR_INFORMATION) + 267 * 2];
	FILE_BOTH_DIR_INFORMATION* pInfo = (FILE_BOTH_DIR_INFORMATION*)tempBuff;

	// 获取第一个文件
	status = nextFile(pFindInfo->hFindHandle, pInfo, sizeof(tempBuff));

	if (status == STATUS_SUCCESS)
	{
		RtlCopyMemory(pFindInfo->cFileName, pInfo->FileName, pInfo->FileNameLength);
		pFindInfo->cFileName[pInfo->FileNameLength / 2] = 0;

		pFindInfo->dwFileAttributes = pInfo->FileAttributes;
		pFindInfo->ftCreationTime = pInfo->CreationTime.QuadPart;
		pFindInfo->ftLastAccessTime = pInfo->LastAccessTime.QuadPart;
		pFindInfo->ftLastWriteTime = pInfo->LastWriteTime.QuadPart;
		pFindInfo->nFileSizeHigh = pInfo->EndOfFile.HighPart;
		pFindInfo->nFileSizeLow = pInfo->EndOfFile.LowPart;

		// 设置完成的字节数
		uComplateSize = sizeof(FILE_INFO);
	}
	else
	{
		ZwClose(pFindInfo->hFindHandle);
	}

	return uComplateSize;
}

// 删除文件
BOOLEAN DeleteFiles(PVOID pFileName)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	UNICODE_STRING StrFile;
	RtlInitUnicodeString(&StrFile, pFileName);

	OBJECT_ATTRIBUTES objAttribute = { 0 };
	ULONG ulAttributes = OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE;

	InitializeObjectAttributes(&objAttribute, &StrFile, ulAttributes, NULL, NULL);

	// 删除指定文件/文件夹
	status = ZwDeleteFile(&objAttribute);

	return TRUE;
}
