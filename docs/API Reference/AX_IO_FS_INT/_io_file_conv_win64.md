# Function

This function is a part of AX_IO_FS_INT interface.

## • Description

> [!IMPORTANT]
> This function is available **ONLY** for AX_WIN64 builds.</br>
> For POSIX compatibility check [_io_file_conv](./_io_file_conv.md)

Convert access mask to Windows [File Access Rights Constants](https://learn.microsoft.com/en-us/windows/win32/fileio/file-access-rights-constants)

## • Signature
```c
static inline const DWORD _io_file_conv_win64(
	_in io_file_acc 		acc	
);
```

## • Arguments

### ```acc```
Access mask to convert as [io_file_acc](./definition.md#io_file_acc).

## • Return value

DWORD File Access Rights Values representative of io_file_acc mask.

