# Function

This function is a part of AX_IO_FS_INT interface.

## • Description

Resize file to size.

## • Signature
```c
axres io_fres(
	_in io_file		*file,
	_in u32			size
);
```

## • Arguments

### ```file```
File structure to resize.

### ```size```
New requested file size.

## • Return value

**AX_SUCC** File was resized successfully.

Error code if file resize failed.

