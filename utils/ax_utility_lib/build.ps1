$files_c = $(gci "$PSScriptRoot\src" -file -r -filter "*.c")
$files_h = $(gci "$PSScriptRoot\src" -file -r -filter "*.h")

$build_path = "$PSScriptRoot\build\${DIR_BUILD}\"
$headers_path = "$PSScriptRoot\build\${DIR_BUILD}\headers"
$output_lib = "$PSScriptRoot\build\${DIR_BUILD}\ax_utility_lib.lib"

$files_o = @()

foreach ($src in $files_c){
	$out = $build_path+$($src.Name -replace "\.[^.]+$")+".o"
	MSG -msg "Compiling file: $($src.FullName)" -color Yellow -opt

	& $CC `
		$PREF_SRC $src.FullName `
		($GLOB_INC | foreach {$PREF_INC+$_}) `
		($GLOB_DEF | foreach {$PREF_DEF+$_}) `
		($GLOB_FLAG | foreach {$PREF_BASE+$_}) `
		($files_h | select -expandproperty DirectoryName | get-unique | foreach {$PREF_INC+$_}) `
		($PREF_OUT+$out)

	if ($lastexitcode -ne 0){
		MSG -msg "Compilation failed with code: $lastexitcode" -color Red
		return 1
	}

	$files_o += $out

	MSG -msg "Successfully compiled to: $out" -color Green -opt

}

ar rcs $output_lib $files_o
if ($lastexitcode -ne 0){
	MSG -msg "Static library build failed with code: $lastexitcode" -color Red
	return 1
}
MSG -msg "Static library created at: ${output_lib}" -color Blue

$files_h | foreach {cp $_ $headers_path}

