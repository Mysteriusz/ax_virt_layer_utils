# AX_IO_MEMORY

Interface providing memory I/O.

## • Functions

|Function link|Function description|
|-|-|
|[_sfmemcmp](./_sfmemcmp.md)|Safely compare 2 buffers|

## • Macros

### • Functions

#### ```axmalloc(size)```
Value:
```c
CAT(CAT(axmalloc_,AX_PLATFORM),CAT(_,AX_MODE))(size, __LINE__, (c8*)__FUNCTION__)
```
Allocate memory with platform/mode specific function.

#### ```axfree(size)```
Value:
```c
CAT(CAT(axfree_,AX_PLATFORM),CAT(_,AX_MODE))(size, __LINE__, (c8*)__FUNCTION__)
```
Free memory with platform/mode specific function.

