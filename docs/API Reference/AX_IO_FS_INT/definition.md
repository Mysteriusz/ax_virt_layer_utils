# AX_IO_FS_INT

Interface providing file I/O abstractions for any build mode/platform.

## • Functions

|Function link|Function description|
|-|-|
|[_enc_size](./_enc_size.md)|Size in bytes of the encoding character|
|[_bom_size](./_bom_size.md)|Size in bytes of the Byte Order Marker|
|(WIN64) [_io_file_conv_win64](./_io_file_conv_win64.md)|Access mask to **File Access Rights Constants**|
|[_io_file_conv](./_io_file_conv.md)|Access to POSIX access string|
|[io_finv](./io_finv.md)|Invalidate file structure|
|[io_foff](./io_foff.md)|Change file pointer|
|[io_fbom](./io_fbom.md)|Get file Byte Order Mark|
|[io_fex](./io_fex.md)|Check file existence|
|[io_fsize](./io_fsize.md)|Get file size|
|[io_fo](./io_fo.md)|Open file|
|[io_fo_tmp](./io_fo_tmp.md)|Open temporary file|
|[io_fc](./io_fc.md)|Close file|
|[io_fr](./io_fr.md)|Read from file|
|[io_fw](./io_fw.md)|Write to file|
|[io_fres](./io_fres.md)|Resize file|
|[io_ftrans](./io_ftrans.md)|Transfer bytes between files|
|[io_fmmap](./io_fmmap.md)|Map file to memory|
|[io_funmap](./io_funmap.md)|Unmap file from memory|

## • Type definitions

<a name="io_file_acc"></a>
### ```io_file_acc```
```c
typedef u16 io_file_acc;
```
Base type: unsigned short</br>
Size: 16 bits

Access mask for I/O files.

#### Enum values
```c
enum io_file_acc{
	// Read
	IO_FILE_R = 0x01,
	// Write
	IO_FILE_W = 0x02,
	// Create(If doesnt exist)
	IO_FILE_C = 0x04,
	IO_FILE_RW = IO_FILE_R | IO_FILE_W,
	IO_FILE_RWC = IO_FILE_R | IO_FILE_W | IO_FILE_C,
};
```

<a name="io_file_enc"></a>
### ```io_file_enc```
```c
typedef u16 io_file_enc;
```
Base type: unsigned int</br>
Size: 32 bits

Basic identifier for encoding types.

#### Enum values
```c
enum io_file_enc{
	UTF8LE = 10,
	UTF8 = 11,
	UTF8BE = 12,

	UTF16LE = 20,	
	UTF16 = 21, 
	UTF16BE = 22,

	UTF32LE = 40,
	UTF32 = 41,
	UTF32BE = 42,
};
```

<a name="io_file_inf"></a>
### ```io_file_inf```
```c
typedef u16 io_file_inf;
```
Base type: unsigned short</br>
Size: 16 bits

Bit mask for file information.

#### Enum values
```c
enum io_file_inf{
	IO_FILE_MAP = 0x01,
	IO_FILE_ENC = 0x02,
};
```

## • Structs

### ```_io_fmap```
```c
typedef struct _io_fmap{ 
	void *root; 

#if defined(AX_WIN64)
	HANDLE hdl;
#elif defined(AX_LINUX)
#error "TODO"
#endif // defined(AX_WIN64)

	u32 size;
} io_fmap;
```
Base type: none</br>
Size: (AX_WIN64: 24 bytes)</br>
This structure provides a description of an allocated memory map.</br>
The map can be created using [io_fmmap](./io_fmmap.md).

#### Field breakdown

|Field name|Field description|
|-|-|
|root|Pointer to the beggining of the memory map|
|**(AX_WIN64)** hdl|Windows handle of the map|
|size|Size in bytes of the memory map|

<a name="io_file"></a>
### ```_io_file```
```c
typedef struct _io_file{
	c16 *path;
#if defined(AX_UM)
	void *hdl;
#elif defined(AX_KM)

#if defined(AX_WIN64)
	HANDLE hdl;
#elif defined(AX_LINUX)
	void *hdl;
#endif

#endif
	io_file_acc acc; // File access bit mask
	io_file_inf inf; // File info bit mask
	io_fmap	map; // Memory mapped file
	io_file_enc enc; // Optional field (default = UTF16LE)
	u64 offset; // (IN BYTES) Changes after each R/W (default = _bom_size(enc))
} io_file;
```
Base type: none</br>
Size: (AX_WIN64: 56 bytes)</br>
This structure provides a file descriptor containing all access data.</br>
The map can be created using [io_fo](./io_fo.md).

#### Field breakdown

|Field name|Field description|
|-|-|
|path|Path to the file provided on open|
|hdl|Handle of the file|
|acc|Access mask provided on open|
|inf|Information mask provided on open|
|map|Inlined **io_fmap** structure opened automatically on **io_fo**|
|enc|File encoding|
|offset|Current file pointer offset in bytes|

## • Macros

### • Values

<a name="IO_FILE_MAX"></a>
#### ```IO_FILE_MAX```
Value:
```c
GB(2) 
```
Maximum file size in bytes

#### ```IO_FILE_CHUNK```
Value:
```c
0x400 // (1024 UTF8) (512 UTF16) (256 UTF32)
```
Unified size of file chunk when processing in chunks

#### ```BOMS```
Value:
```c
#define UTF16LE_BOM 	0xFEFF
#define UTF16BE_BOM	0xFFFE
#define UTF32LE_BOM	0xFEFF0000
#define UTF32BE_BOM	0x0000FFFE
#define UTF8LE_BOM	0xBFBBEF
#define UTF8BE_BOM	0xEFBBBF
```
All bom`s in little and big endian

