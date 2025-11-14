# Function

This function is a part of AX_IO_FS_INT interface.

## • Description

Close a file.

## • Signature
```c
axres io_fc(
	_in io_file		*file
);
```

## • Arguments

### ```file```
File structure to close previously opened with [io_fo](./io_fo.md).

## • Return value

**AX_SUCC** if file closed successfull.

Error code if failed to close. 

