$build_dir = "$PSScriptRoot\build"

$flags = @("-Wall", "-Werror", "-O2", "-c", "-Wno-unused-function")
$define = @("-DAX_WIN32", "-DAX_UM")

$file_h = gci "$PSScriptRoot\src\" -recurse -file -filter "*.h"
$file_c = gci "$PSScriptRoot\src\" -recurse -file -filter "*.c"
$file_o = @()

foreach ($file in $file_c){
	$temp = "$build_dir\$($file.BaseName).obj"
	clang $flags $define $($file_h | foreach {"-I"+$_.DirectoryName}) $($file.FullName) -o $temp
	$file_o += $temp
}

llvm-ar rcs "$build_dir\ax_utility.lib" $file_o

copy-item -path $($file_h | select -expandproperty "FullName") -destination "$build_dir\headers"

