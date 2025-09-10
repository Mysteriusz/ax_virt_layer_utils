param(
	[switch]$km
)

$build_dir = "$PSScriptRoot\build"

if ($km){

$cc = "cl"
$flags = @(
	"Wall",
	"O2",
	"IC:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\km\full"
)
$define = @(
	"DAX_WIN64",
	"DAX_KM"
)
$aalias = "/"
$ialias = "/c"
$oalias = "/Fo"

}else{

$cc = "clang"
$flags = @(
	"Wall",
	"Werror",
	"O2",
	"std=c23",
	"Wno-unused-function",
	"-target=x86_64-pc-windows-msvc"
)
$define = @(
	"DAX_WIN64",
	"DAX_UM"
)
$aalias = "-"
$ialias = "-c"
$oalias = "-o"

}

$flags = $flags | foreach { $aalias + $_ }
$define = $define | foreach { $aalias + $_ }

$file_h = gci "$PSScriptRoot\src\" -recurse -file -filter "*.h"
$file_c = gci "$PSScriptRoot\src\" -recurse -file -filter "*.c"
$file_o = @()

foreach ($file in $file_c){
	$temp = "$build_dir\$($file.BaseName).obj"

	$arg = @(
		$flags
		$ialias
		$file.FullName
		$oalias+$temp
		$define
		$($file_h | foreach {$aalias+"I"+$_.DirectoryName})
	)
	& $cc $arg

	if ($lastexitcode -ne 0){
		exit
	}

	$file_o += $temp
}

llvm-ar rcs "$build_dir\ax_utility.lib" $file_o

copy-item -path $($file_h | select -expandproperty "FullName") -destination "$build_dir\headers"

