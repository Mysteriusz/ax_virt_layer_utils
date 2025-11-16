# Function

This function is a part of AX_IO_FS_INT interface.

## • Description

Update file pointer to provided byte offset.

## • Signature
```c
bool io_foff(
	_in io_file 		*file,
	_in u32 		offset
);
```

## • Arguments

### ```file```
File to update the offset.

### ```offset```
New byte offset for the file.

## • Return value

**true** if file pointer has been updated successfully.</br>
**false** if file pointer update failed.

## • Remarks

If requested byte offset is bigger than the file,</br>
the function will fail.

To resize the file checkout [io_fres](./io_fres.md).

