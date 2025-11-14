# Function

This function is a part of AX_IO_FS_INT interface.

## • Description

Read file BOM given path.

## • Signature
```c
axres io_fbom(
	_in const c16		*path,
	_out io_file_enc	*buf
);
```

## • Arguments

### ```path```
Path to the file.

### ```buf```
Output buffer for the encoding found.

## • Return value

**AX_SUCC** on success.
Error code on failure.

