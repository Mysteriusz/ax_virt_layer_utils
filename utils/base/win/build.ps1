$FILE_C = gci "$(Get-Location)\src" -recurse -file -filter "*.c"

$FILE_H = gci "$(Get-Location)\src" -recurse -file -filter "*.h"
$FILE_SHARED_H = gci "$(Get-Location)\..\shared" -recurse -file -filter "*.h"

$INCLUDE = @(
	"/IC:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um\full"
	$($FILE_SHARED_H | select -expandproperty DirectoryName | foreach {"/I"+$_})
	$($FILE_H | select -expandproperty DirectoryName | foreach {"/I"+$_})
)

$DEFINE = @("/DUNICODE", "/DAX_WINDOWS", "/DAX_UM")

$OUTPUT_DIR = "$(Get-Location)\build"
$OUTPUT_BIN = "ax_utility.lib"

$FILE_O = @()
foreach ($FILE in $FILE_C) {
    $O = Join-Path $OUTPUT_DIR ($FILE.BaseName + ".obj")	
    cl $DEFINE $INCLUDE /c($FILE.FullName) /Fo:$O
    $FILE_O += $O
}

lib $FILE_O /OUT:"$OUTPUT_DIR\$OUTPUT_BIN" 

$OUTPUT_H = $env:AX_VIRT_H
[string[]]$OUTPUT_FILE_H = @(
	$($FILE_H | select -expandproperty FullName) 
	$($FILE_SHARED_H | select -expandproperty FullName)
	"$OUTPUT_DIR\$OUTPUT_BIN" 
)

cpi -path $OUTPUT_FILE_H -destination $OUTPUT_H -force

