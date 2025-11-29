$CC = "gcc"

$PMIN = "WIN"
$MMIN = "KM"
$PFULL = "windows"
$MFULL = "kernelmode"

$PREF_INC = "-I"
$PREF_LIB = "-L"
$PREF_DEF = "-D"
$PREF_SRC = "-c"
$PREF_OUT = "-o"
$PREF_BASE = "-"

$GLOB_DEF = @(
	"AX_KM",
	"AX_${PMIN}64"
)
$GLOB_INC = @(
	"C:\msys64\ucrt64\include\ddk\"
)
$GLOB_FLAG = @(
	"-target=x86_64-w64-mingw32-clang",
	"Wno-pragma-pack"
)

