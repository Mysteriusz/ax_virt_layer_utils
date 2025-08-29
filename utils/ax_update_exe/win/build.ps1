$build_dir = "$PSScriptRoot\build"

$flags = @("-Wall", "-Werror", "-O2", "-std=c23")
$define = @("-DAX_WIN32", "-DAX_UM")

$file_lib_u = @("D:\ax_project\ax_virt_layer_utils\utils\ax_utility_lib\win\build\ax_utility.lib")
$file_h_u = @("-ID:\ax_project\ax_virt_layer_utils\utils\ax_utility_lib\win\build\headers")

$file_h = gci "$PSScriptRoot\src\" -recurse -file -filter "*.h"
$file_c = gci "$PSScriptRoot\src\" -recurse -file -filter "*.c"
$file_o = @()

foreach ($file in $file_c){
	$temp = "$build_dir\$($file.BaseName).obj"
	clang -c $define $file_h_u `
		$($file_h | foreach {"-I"+$_.DirectoryName}) `
		$($file.FullName) -o $temp

	$file_o += $temp
}

clang $flags $define $file_lib_u $file_h_u $($file_h | foreach {"-I"+$_.DirectoryName}) $file_o -ladvapi32 -o "$build_dir\ax_update.exe" "-Wl,/subsystem:console" 

