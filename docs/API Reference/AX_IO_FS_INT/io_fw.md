# Function

This function is a part of AX_IO_FS_INT interface.

## • Description

Write to file at it`s current byte offset.

## • Signature
```c
axres io_fw(
	_in io_file		*file,
	_in u32			size,
	_in void 		*buf,
	_out_opt u32		*writ // Bytes written
);
```

## • Arguments

### ```file```
File structure to write to previously opened with [io_fo](./io_fo.md). 

### ```size```
Size of the **buf**.

### ```buf```
Pointer to the write buffer.

### ```writ```
Bytes written to file. (optional)

## • Return value

**AX_SUCC** if write was successfull.

Error code if write failed.

## • Remarks

Write happens at file byte offset indicated by io_file->offset.
Learn more at [io_file definition](./definition.md#io_file).

