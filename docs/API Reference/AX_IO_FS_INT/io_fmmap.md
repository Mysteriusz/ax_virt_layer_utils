# Function

This function is a part of AX_IO_FS_INT interface.

## • Description

Open a file map in memory.

## • Signature
```c
axres io_fmmap(
	_in void 		*hdl,
	_in bool 		term,
	_in_out io_fmap		*map
);
```

## • Arguments

### ```hdl```
POSIX compatible file handle.

### ```term```
If map should be null-terminated.</br>
(8 bytes more allocation)

### ```map```
Stack buffer for the map.

## • Return value

**AX_SUCC** if map was created successfully.

Error code if map wasn`t created.

