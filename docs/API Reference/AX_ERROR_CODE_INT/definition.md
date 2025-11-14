# AX_ERROR_CODE_INT

Interface providing error codes and code specific error handling.

## • Functions

|Function link|Function description|
|-|-|
|[_ax_buf_err](./_ax_buf_err.md)|Invalidate buffer size against expected size|

## • Macros

All error codes are mapped as following:</br>
    0x01 - 0x0f -> INV[ALID] codes</br>
    0x10 - 0x1f -> BUF[FER] codes</br>
    0x20 - 0x3f -> NOT codes</br>
    0x40 - 0x7f -> ACC[ESS] codes</br>
    0x80 - 0xfff -> UNK[NOWN] codes

> [!WARNING]
> Code mappings may change so it`s only safe to use provided code macros.

### • Values

#### • AX_SUCC
Value:
```c
    ((axres)0x00)
```
Successfull operation

#### • AX_INV_ARG
Value:
```c
    ((axres)0x01)
```
Invalid argument provided

#### • AX_INV_DATA
Value:
```c
    ((axres)0x02)
```
Invalid argument data provided

#### • AX_INV_BUF
Value:
```c
    ((axres)0x03)
```
Invalid buffer passed

#### • AX_INV_CODE
Value:
```c
    ((axres)0x04)
```
Invalid code received

#### • AX_INV_FILE
Value:
```c
    ((axres)0x05)
```
Invalid file structure

#### • AX_INV_ENC
Value:
```c
    ((axres)0x06)
```
Invalid file encoding

#### • AX_INV_FMT
Value:
```c
    ((axres)0x07)
```
Invalid value format

#### • AX_INV_IND
Value:
```c
    ((axres)0x08)
```
Invalid index provided

#### • AX_INV_MEM
Value:
```c
    ((axres)0x09)
```
Corrupted memory

#### • AX_INV_PATH
Value:
```c
    ((axres)0x0a)
```
Invalid path provided

#### • AX_BUF_TOO_SMALL
Value:
```c
    ((axres)0x10)
```
Too small buffer provided

#### • AX_BUF_TOO_BIG
Value:
```c
    ((axres)0x11)
```
Too big buffer provided

#### • AX_NOT_FND
Value:
```c
    ((axres)0x20)
```
Value not found

#### • AX_NOT_IMP
Value:
```c
    ((axres)0x21)
```
Logic not implemented

#### • AX_ACC_DEN
Value:
```c
    ((axres)0x40)
```
Access denied

#### • AX_UNK_ERR
Value:
```c
    ((axres)0x80)
```
Unknown error

