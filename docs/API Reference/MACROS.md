# Global project macros

<a name="ax_um"></a>
## AX_UM
This macro indicates user mode build type.<br/>
It cannot be paired with [AX_KM](#ax_km)

<a name="ax_km"/></a>
## AX_KM
This macro indicates kernel mode build type.<br/>
It cannot be paired with [AX_UM](#ax_um)

<a name="ax_linux"/></a>
## AX_LINUX
This macro indicates build for linux.<br/>
It cannot be paired with [AX_WIN64](#ax_win64)

<a name="ax_win64"/></a>
## AX_WIN64
This macro indicates build for windows.<br/>
It cannot be paired with [AX_LINUX](#ax_linux)

<a name="ax_strict_buf_size"/></a>
## AX_STRICT_BUF_SIZE
This macro makes the build check buffer sizes on any R/W function.<br/>
It`s auto-defined for [AX_KM](#ax_km) build mode.

