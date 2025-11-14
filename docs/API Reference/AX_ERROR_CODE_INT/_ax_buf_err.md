# Function

This function is a part of AX_ERROR_CODE_INT interface.

## • Description

Invalidate buffer size against expected size.

## • Signature
```c
static inline axres _ax_buf_err(
	    u64 		    size,
	    u64 		    buf_size	
);
```

## • Arguments

### ```size```
Defines expected size

### ```buf_size```
Defines checked buffer size

## • Return value

If buffer size was correct then return value is **AX_SUCC**.

If **(buf_size < size)** then return value is **AX_BUF_TOO_SMALL**.

## • Remarks

In case [AX_STRICT_BUF_SIZE](../MACROS.md#ax_strict_buf_size) was defined on build,</br>
and **(buf_size > size)** return value is **AX_BUF_TOO_BIG**.

