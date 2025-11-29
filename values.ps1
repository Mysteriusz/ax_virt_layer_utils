param(
	[switch]$v = $false,
	[switch]$km = $false,
	[switch]$um = $false
)

function MSG{
	param(
		[Parameter(Mandatory=$true)]
		[string]$msg,
		[System.ConsoleColor]$color = 7, # Gray
		[switch]$opt = $false
	)

	if ($opt -and !$v){
		return
	}

	write-host "[$WORKING_NAME] " -nonewline
	write-host $msg -foregroundcolor $color
}

$WORKING_NAME = "values"

if ($km -and $um){
	MSG -msg "Cannot initialize both modes." -color Red
	return 1
}elseif (!$km -and !$um){
	MSG -msg "Build mode not set." -color Red
	return 1
}

if ($um){
	. ./values_um.ps1
}elseif ($km){
	. ./values_km.ps1
}

$DIR_BUILD = "${PMIN}_${MMIN}"
$LIB_HEADERS = resolve-path "./utils/ax_utility_lib/build/${DIR_BUILD}/headers"
$LIB_BUILD = resolve-path "./utils/ax_utility_lib/build/${DIR_BUILD}/"

return 0

