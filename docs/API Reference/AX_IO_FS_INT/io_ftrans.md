# Function

This function is a part of AX_IO_FS_INT interface.

## • Description

Transfer bytes from file to file.

> [!IMPORTANT]
> Both R/W happen from each file pointer.

## • Signature
```c
axres io_ftrans(
	_in io_file		*from,
	_in io_file		*to,
	_in u32			size,
	_out_opt u32		*trans // Bytes transfered
);
```

## • Arguments

### ```from```
Sender file structure.

### ```to```
Receiver file structure.

### ```size```
Size in bytes to transfer.</br>
If size is **0** size is set to file size of the **from**.

### ```trans```
Successfully transfered bytes count buffer.

## • Return value

**AX_SUCC** if transfer was successfull.

Error code if file transfer failed.

