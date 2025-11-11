# AX_TYPE_INT
---

This interface contains all basic types, macro gates, and macros used by
other interfaces.

> [!IMPORTANT]
> To use this interface it`s required to define following macros:<br/>
>       [AX_UM](./MACROS.md#ax_um) OR [AX_KM](./MACROS.md#ax_km)<br/>
>       AND<br/>
>       [AX_WIN64](./MACROS.md#ax_win64) OR [AX_LINUX](./MACROS.md#ax_linux)<br/>

To learn more read [macro reference document](./MACROS.md).

## Type definitions

Following base types are defined:

### i8
```c
typedef signed char i8;
```
Base type: signed char</br>
Size: 8 bits

### i16
```c
typedef signed char i16;
```
Base type: signed short</br>
Size: 16 bits

### i32
```c
typedef signed int i32;
```
Base type: signed int</br>
Size: 32 bits

### i64
```c
typedef signed long long i64;
```
Base type: signed long long</br>
Size: 64 bits

### u8
```c
typedef signed char u8;
```
Base type: unsigned char</br>
Size: 8 bits

### u16
```c
typedef unsigned char u16;
```
Base type: unsigned short</br>
Size: 16 bits

### u32
```c
typedef unsigned int u32;
```
Base type: unsigned int</br>
Size: 32 bits

### u64
```c
typedef unsigned long long u64;
```
Base type: unsigned long long</br>
Size: 64 bits

<a name="iu64"></a>
### iu64
```c
typedef union{
	i64 sig_64;
	u64 unsig_64;

	i32 sig_32;
	u32 unsig_32;

	i16 sig_16;
	u16 unsig_16;

	i8 sig_8;
	u8 unsig_8;
} iu64;
```
Base type: u64 (unsigned long long)</br>
Size: 64 bits</br>
This type acts as a universal number.

---

### c8
```c
typedef unsigned char c8; // ansi
```
Base type: unsigned char (ansi character)</br>
Size: 8 bits

### c16
```c
typedef unsigned short c16; // utf-16
```
Base type: unsigned short (utf-16 character)</br>
Size: 16 bits

---

<a name="axres"></a>
### axres
```c
typedef u64 axres;
```
Base type: u64 (unsigned long long)</br>
This type is the default result code holder.

## Structs

<a name="axres_s"></a>
```c
typedef struct _axres_s{ 
	// 32-bit block

	u16 err : 12; // max 4095 (0xfff)
	u16 meta : 4; // flag per bit
	u16 reseverd0;

	// 32-bit block

	u32 reserved1 : 32;
} axres_s;
```
Base type: [axres](#axres)</br>
This type is a breakdown of the axres result code.

#### Field breakdown

|Field name|Field description|
|-|-|
|err|Main result code intger|
|meta|Bitmask metadata|
|reserved0|Reserved for future use|
|reserved1|Reserved for future use|

## Macros


