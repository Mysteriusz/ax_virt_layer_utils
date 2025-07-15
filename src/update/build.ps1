$FILE_C = gci "$(Get-Location)\src" -recurse -file -filter "*.c"
$FILE_H = gci "$(Get-Location)" -recurse -file -filter "*.h"
$FILE_LIB_C = gci "$(Get-Location)\lib" -recurse -file -filter "*.c"

$INCLUDE = @("/IC:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\full")
$DEFINE = @("/DUNICODE")

$OUTPUT_DIR = "$(Get-Location)\build"
$OUTPUT_BIN = "update.exe"
$OUTPUT_LIB = "update.lib"

$FILE_O = @()
foreach ($FILE in $FILE_C) {
    $O = Join-Path $OUTPUT_DIR ($FILE.BaseName + ".obj")	
    cl $DEFINE $INCLUDE /c($FILE.FullName) /Fo:$O 
    $FILE_O += $O
}

$FILE_LIB_O = @()
foreach ($FILE in $FILE_LIB_C){
    $O = Join-Path $OUTPUT_DIR ($FILE.BaseName + ".obj")
    cl $DEFINE $INCLUDE /c($FILE.FullName) /Fo:$O
    $FILE_LIB_O += $O
}
lib $FILE_LIB_O /OUT:"$OUTPUT_DIR\$OUTPUT_LIB"

$FLAGS = @("/SUBSYSTEM:CONSOLE")
$LIBS = $env:LIBS_UM + "$OUTPUT_DIR\$OUTPUT_LIB" -split ';'

cl /MT $DEFINE $FILE_O $LIBS /Fe"$OUTPUT_DIR\$OUTPUT_BIN" /link $FLAGS


