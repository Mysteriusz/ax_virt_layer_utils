param(
	[switch]$um = $false,
	[switch]$km = $false,
	[switch]$ax_update_exe = $true,
	[switch]$v = $false,
	[switch]$ax_utility_lib = $true
)
$lastexitcode = 0

if ((. ./values.ps1 -v:$v -km:$km -um:$um) -ne 0){
	return
}

$build_table = [hashtable]@{}
if ($ax_update_exe) {$build_table.Add("ax_update_exe", "./utils/ax_update_exe/build.ps1")}
if ($ax_utility_lib) {$build_table.Add("ax_utility_lib", "./utils/ax_utility_lib/build.ps1")}

$timer = [Diagnostics.Stopwatch]::StartNew()
foreach ($script in $build_table.GetEnumerator()){
	$WORKING_NAME = $script.Key

	$timer.Restart()
	. (resolve-path $script.Value)
	if ($lastexitcode -ne 0){
		return
	}

	MSG -msg "Build completed in: $($timer.Elapsed)"
}

