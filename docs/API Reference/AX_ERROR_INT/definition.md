# AX_ERROR_INT

Interface providing error handling.

## • Functions

|Function link|Function description|
|-|-|
|[ax_log](./ax_log.md)|Log axres with head|
|[ax_log_msg](./ax_log_msg.md)|Log axres with head and message|

## • Macros

### • Values

#### ```AX_LOG_HEAD```
Value:
```c
u"--------AX_LOG--------"
```
AX_LOG debug label

#### ```AX_LOG_META```
Value:
```c
u"--------METADATA--------"
```
AX_LOG_META debug label

### • Functions

#### ```AX_RES_S(r)```
Value:
```c
(*(axres_s*)((axres*)addr(r)))
```
Cast axres code to [_axres_s](../AX_TYPE_INT.md#axres_s) structure.

#### ```AX_ERR(r)```
Value:
```c
((AX_RES_S(r).err) != AX_SUCC)
```
Check if axres code is successfull.

#### ```axcheck(r, ...)```
Value:
```c
if(AX_ERR(r)){__VA_ARGS__; return ((axres)r);}
```
axres(r) return code on error alias.

#### ```axcheck_r(r, fr, ...)```
Value:
```c
if(AX_ERR(r)){__VA_ARGS__; return (fr);}
```
axres(r) custom return code(fr) on error alias.

#### ```axcheck_g(r, gt, ...)```
Value:
```c
if(AX_ERR(r)){__VA_ARGS__; goto gt;}
```
axres(r) goto(gt) on error alias.

#### ```axcheck_b(r, ...)```
Value:
```c
if(AX_ERR(r)){__VA_ARGS__; break;}
```
axres(r) break on error alias.


