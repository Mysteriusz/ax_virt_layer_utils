$CC = "gcc"

$PMIN = "WIN"
$MMIN = "UM"
$PFULL = "windows"
$MFULL = "usermode"

$PREF_INC = "-I"
$PREF_LIB = "-L"
$PREF_DEF = "-D"
$PREF_SRC = "-c"
$PREF_OUT = "-o"
$PREF_BASE = "-"

$GLOB_DEF = @(
	"AX_UM",
	"AX_${PMIN}64"
)
$GLOB_FLAG = @(
	"-target=x86_64-pc-${PFULL}-${CC}"
)

