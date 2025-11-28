param(
	[switch]$um = $false,
	[switch]$km = $false,
	[switch]$ax_update_exe = $true,
	[switch]$ax_utility_lib = $true
)

. ./values.ps1 -km:$km -um:$um

$build_table = [hashtable]@{}
if ($ax_update_exe) {$build_table.Add("ax_update_exe", "./utils/ax_update_exe/build.ps1")}
if ($ax_utility_lib) {$build_table.Add("ax_utility_lib", "./utils/ax_utility_lib/build.ps1")}

foreach ($script in $build_table.GetEnumerator()){
	$BUILD_NAME = $script.Key
	. (resolve-path $script.Value)
	if ($lastexitcode -ne 0){
		return -1
	}
}

