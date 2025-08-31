$build_dir = "$PSScriptRoot\build"

$flags = @(
	"-Wall",
	"-Werror",
	"-O2",
	"-c",
	"-std=c23"
	"-Wno-unused-function",
	"--target=x86_64-pc-windows-msvc"
)
$define = @("-DAX_WIN32", "-DAX_UM", "-D_AMD64_")

$file_h = gci "$PSScriptRoot\src\" -recurse -file -filter "*.h"
$file_c = gci "$PSScriptRoot\src\" -recurse -file -filter "*.c"
$file_o = @()

foreach ($file in $file_c){
	$temp = "$build_dir\$($file.BaseName).obj"
	clang $flags $define $($file_h | foreach {"-I"+$_.DirectoryName}) $file_h_add $($file.FullName) -o $temp
	$file_o += $temp
}

llvm-ar rcs "$build_dir\ax_utility.lib" $file_o

copy-item -path $($file_h | select -expandproperty "FullName") -destination "$build_dir\headers"

