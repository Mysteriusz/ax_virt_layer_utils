# Function

This function is a part of AX_IO_FS_INT interface.

## • Description

> [!WARNING]
> TEMP: File has to be marked with BOM.

Open a file with Byte Order Mark.

## • Signature
```c
axres io_fo(
	_in const c16		*path,
	_in io_file_acc		acc,
	_out io_file		**buf
);
```

## • Arguments

### ```path```
Path to the file.

### ```acc```
File access mask.

### ```buf```
Empty pointer to receive the file heap.

## • Return value

**AX_SUCC** if file opening was successfull.

Error code if failed to open. 

## • Remarks

If **acc** argument has **IO_FILE_C** bit on</br>
the file will be created.

