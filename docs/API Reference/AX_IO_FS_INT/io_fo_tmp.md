# Function

This function is a part of AX_IO_FS_INT interface.

## • Description

Open temporary file.

## • Signature
```c
axres io_fo_tmp(
	_out io_file		**buf
);
```

## • Arguments

### ```buf```
Buffer receiving the file pointer.

## • Return value

**AX_SUCC** if temporary file was created and opened successfully.</br>
**AX_UNK_ERR** may happen if an empty OS temporary file path was not found. 

Error code if failed to open. 

## • Remarks

Temporary files are always opened with [IO_FILE_RWC](./definition.md#io_file_acc) access.

