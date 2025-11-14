# Function

This function is a part of AX_IO_FS_INT interface.

## • Description

Check if file exists given path.

## • Signature
```c
axres io_fex(
	_in const c16		*path	
);
```

## • Arguments

### ```path```
Path to the file

## • Return value

**AX_SUCC** if file exists.

Error code if file doesn`t exist.

