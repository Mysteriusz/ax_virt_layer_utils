param(
	[switch]$wr,
	[switch]$km
)

$build_order = [string[]]@(
	"utils\ax_utility_lib\win\build.ps1"
	"utils\ax_update_exe\win\build.ps1"
)

$location = $(get-location).Path
for ($i = 0; $i -lt $build_order.length; $i++){
	$script = "$location\$($build_order[$i])" 
	if (test-path $script){
    		& $script
	}
}

if ($wr){
	& "$location\winrun.ps1"
}

