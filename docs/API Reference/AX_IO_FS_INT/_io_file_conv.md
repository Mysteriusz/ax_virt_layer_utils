# Function

This function is a part of AX_IO_FS_INT interface.

## • Description

Convert access mask to POSIX access string.

## • Signature
```c
static inline const c16 *_io_file_conv(
	_in io_file_acc 		acc	
);
```

## • Arguments

### ```acc```
Access mask to convert as [io_file_acc](./definition.md#io_file_acc).

## • Return value

Access return value map:</br>
IO_FILE_R -> r</br>
IO_FILE_W -> a</br>
IO_FILE_C -> w

In case of an invalid argument the value is **nullptr**.</br>
The returned mode will always be a **b** (binary) mode.

