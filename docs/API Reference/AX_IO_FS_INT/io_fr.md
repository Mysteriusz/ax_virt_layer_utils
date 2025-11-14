# Function

This function is a part of AX_IO_FS_INT interface.

## • Description

Read from file at it`s current byte offset.

## • Signature
```c
axres io_fr(
	_in io_file		*file,
	_in u32			size,
	_in_out void 		*buf,
	_out_opt u32		*read // Bytes read
);
```

## • Arguments

### ```file```
File structure to read from previously opened with [io_fo](./io_fo.md). 

### ```size```
Size of the **buf**.

### ```buf```
Pointer to the receiving buffer.

### ```read```
Bytes read from file. (optional)

## • Return value

**AX_SUCC** if read was successfull.

Error code if read failed.

## • Remarks

Read happens from file byte offset indicated by io_file->offset.
Learn more at [io_file definition](./definition.md#io_file).

