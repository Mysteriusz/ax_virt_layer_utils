# Function

This function is a part of AX_IO_FS_INT interface.

## • Description

Get file size in bytes given path.

## • Signature
```c
axres io_fsize(
	_in const c16		*path,
	_out u64		*size
);
```

## • Arguments

### ```path```
Path to the file.

### ```size```
File size buffer (in bytes).

## • Return value

**AX_SUCC** on success.

Error code on failure.

