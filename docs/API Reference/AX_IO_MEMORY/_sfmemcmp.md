# Function

This function is a part of AX_IO_MEMORY interface.

## • Description

Compare 2 buffers with their respective sizes.

## • Signature
```c
static i8 _sfmemcmp(
	_in const u8 a _prepass,
	_in const u8 b _prepass,
	_in u32 a_n,
	_in u32 b_n
);
```

## • Arguments

### ```a```
Pointer to first buffer.

### ```b```
Pointer to second buffer.

### ```a_n```
Size of **a** buffer.

### ```b_n```
Size of **b** buffer.

## • Return value

If buffers are identical then return value is **0**.

If</br>
    ```a_n > b_n```</br>
OR</br> 
    ```a > b```</br>
return value is **-1**.

If</br>
    ```a_n < b_n```</br>
OR</br>
    ```a < b```</br>
return value is **1**.


