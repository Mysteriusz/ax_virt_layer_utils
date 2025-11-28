param(
	[switch]$verbose = $false,
	[switch]$km = $false,
	[switch]$um = $false
)

if ($km -and $um){
	write-host "Cannot initialize both modes."
}

if ($um){
	. ./values_um.ps1
}elseif ($km){
	. ./values_km.ps1
}

$BUILD_NAME = "UNK"
$DIR_BUILD = "${PMIN}_${MMIN}"

$LIB_HEADERS = resolve-path "./utils/ax_utility_lib/build/${DIR_BUILD}/headers"
$LIB_BUILD = resolve-path "./utils/ax_utility_lib/build/${DIR_BUILD}/"

function MSG{
	param(
		[Parameter(Mandatory=$true)]
		[string]$msg,
		[System.ConsoleColor]$color = 7 # Gray
	)

	write-host "[$BUILD_NAME] " -nonewline
	write-host $msg -foregroundcolor $color
}
function MSG_V{
	param(
		[Parameter(Mandatory=$true)]
		[string]$msg,
		[System.ConsoleColor]$color = 7 # Gray
	)

	if ($verbose -eq $false){
		return
	}

	write-host "[$BUILD_NAME] " -nonewline
	write-host $msg -foregroundcolor $color
}

