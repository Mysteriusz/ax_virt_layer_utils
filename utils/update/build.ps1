$FILE_C = gci "$(Get-Location)\src" -recurse -file -filter "*.c"
$FILE_H = gci "$(Get-Location)\src" -recurse -file -filter "*.h"

$INCLUDE = @("/IC:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\full", "/ID:\ax_project\ax_virt_layer_utils\utils\base\ax_utility.h")
$DEFINE = @("/DUNICODE", "/DAX_WINDOWS", "/DAX_UM")

$OUTPUT_DIR = "$(Get-Location)\build"
$OUTPUT_BIN = "update.exe"

$FILE_O = @()
foreach ($FILE in $FILE_C) {
    $O = Join-Path $OUTPUT_DIR ($FILE.BaseName + ".obj")	
    cl $DEFINE $INCLUDE /c($FILE.FullName) /Fo:$O 
    $FILE_O += $O
}

$FLAGS = @("/SUBSYSTEM:CONSOLE")
$LIBS = "$env:LIBS_UM" + "$env:AX_PROJECT_PATH\ax_virt_layer_utils\utils\base\build\ax_utility.lib" -split ';'

cl /MT $DEFINE $FILE_O $LIBS /Fe"$OUTPUT_DIR\$OUTPUT_BIN" /link $FLAGS

