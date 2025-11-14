# AX_TYPE_INT

This interface contains all basic types, macro functions, and</br>
macros used by other interfaces.

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
typedef unsigned char u8;
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
To learn more about result codes read: [AX_ERROR_CODE_INT](./AX_ERROR_CODE_INT/definition.md)

## Structs

<a name="axres_s"></a>
### _axres_s
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

### Markers

|Marker name|Marker value|Marker decription|
|-|-|-|
|_opt| |Optional variable|
|_in| |Input variable|
|_in_opt| |Optional Input variable|
|_out| |Output variable|
|_out_opt| |Optional Output variable|
|_in_out| |Input/Output variable|
|_in_out_opt| |Optional Input/Output variable|
|_eval| |Variable must be evaluated POST intial call and reused|
|_prepass|[static 1]|Autopass (variable not null and can be used without assertions)|
|_free| |Heap allocated variable (should be freed)|
|_inline_force|__force_inline</br> OR</br> __attribute__((always_inline))|Value depends on the compiler|

### Values

#### null
Value:
```c
    (0)
```

#### nullptr
Value:
```c
    ((void*)0)
```

### Functions

#### unref(p)
Value:
```c
    ((void)p)
```
Dereferece unreferced variable

#### addr(v)
Value:
```c
    (&(long long[]){v}) 
```
Address of the value (v)

#### asrt(expr)
Value:
```c
    (((expr) == false) ? (void)__builtin_trap() : (void)null) 
```
Runtime assertion (expr)

#### chkf(v,f)
Value:
```c
    (((v) & (f)) != 0)
```
Check bit flag (f) in the value (v)

#### astp(t,v)
Value:
```c
    *((t*)((void*)&(v)))
```
Cast value (v) to type (t)

#### cntd(i)
Value:
```c
    ((u32)log_b((i), 10) + 1)
```
Count digit count in an intger (i)

#### bin(n)
Value:
```c
    (1 << (n)) 
```
Set (n)-th bit to 1

#### offo(f,s)
Value:
```c
    ((u32)&(((s*)nullptr)->f))
```
Offset of field (f) in the structure (s)

#### offo_n(f,s)
Value:
```c
    ((u32)&((s)->f) - (u64)(s))
```
Offset of field (f) in the structure (s)

