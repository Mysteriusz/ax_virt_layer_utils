# Function

This function is a part of AX_IO_FS_INT interface.

## • Description

Invalidate file structure and check the expected encoding.

## • Signature
```c
bool io_finv(
	_in io_file 		*file,
	_in_opt io_file_enc 	exp_enc // Pass expected encoding
);
```

## • Arguments

### ```file```
File structure to invalidate.

### ```exp_enc```
Expected encoding.</br>
(to skip the encoding check the value should be **0**)

## • Return value

**true** if file structure is invalid.</br>
**false** if file structure is valid.

## • Remarks

Expected encoding does not have to be the exact encoding used on file opening.</br>
If file was opened with encoding of **UTF16LE** the value can be **UTF16** or even **UTF16BE**.

