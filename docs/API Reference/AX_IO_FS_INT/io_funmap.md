# Function

This function is a part of AX_IO_FS_INT interface.

## • Description

Close a memory file map.

## • Signature
```c
axres io_funmap(
	_in_out io_fmap		*map
);
```

## • Arguments

### ```map```
File map previously opened with [io_fmmap](./io_fmmap.md).

## • Return value

**AX_SUCC** if map was closed successfully.

Error code if map wasn`t closed.

