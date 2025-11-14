# Function

This function is a part of AX_IO_FS_INT interface.

## • Description

Get byte size given the encoding.

## • Signature
```c
static inline u32 _enc_size(
	_in io_file_enc 		enc
);
```

## • Arguments

### ```enc```
Encoding to check as [io_file_enc](./definition.md#io_file_enc).

## • Return value

Encoding return value map:</br>
UTF8LE, UTF8, UTF8BE -> 1</br>
UTF16LE, UTF16, UTF16BE -> 2</br>
UTF32LE, UTF32, UTF32BE -> 4

In case of an invalid argument the value is **0**.

