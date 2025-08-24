param(
	[string]$machineString,
	[switch]$build,
	[switch]$status,
	[switch]$reboot
)

class machine{
	[System.Management.Automation.Runspaces.PSSession]$pss
	[Microsoft.Management.Infrastructure.CimSession]$cim
	[string]$address
	[string]$destination
	[string]$system
	[System.Management.Automation.PSCredential]$credential
	[System.Security.Cryptography.X509Certificates.X509Certificate2]$certificate
}

if (-not $PSBoundParameters.ContainsKey('machineString')){
	$machineString = $env:AX_VIRT_LAYER_MACHINES
}

$global:MACHINES_REQUESTED = $machineString -split ';'
$global:MACHINES_GLOBAL = "$env:AX_VIRT_LAYER_MACHINES"

$global:SERVICE_NAME = "$env:AX_VIRT_LAYER_NAME"
$global:SERVICE_BUILD = "$env:AX_VIRT_LAYER_BUILD_DIR"
$global:UPDATE_SERVICE_NAME = "$env:AX_VIRT_LAYER_UPDATE_NAME"

$global:SERVICE_FILE = "ax_virt_layer.sys"
$global:SIGNATURE_FILE = "ax_virt_layer.pfx"

$global:UPDATE_FILE = "ax_update.exe"
$global:UPDATE_BUILD = "D:\ax_project\ax_virt_layer_utils\utils\ax_update_exe\win\build"

$global:CONTROL_FILE = "ax_virt_control.exe"
$global:CONTROL_BUILD = "D:\ax_project\ax_virt_layer\control\build"

$global:def_username = "main"
$global:def_password = convertto-securestring "root" -asplaintext -force

function Register{
	param(
		[string]$machineString
	)

	if ($machineString -eq $null -or $machineString.Length -lt 3){
		write-host "Incorrect machine string." -foregroundcolor red
		return $null
	}

	[machine]$buffer = [machine]::new()

	$parsed = $machineString -split '\|'

	$buffer.address = $parsed[0]
	$buffer.system = $parsed[1]
	$buffer.destination = $parsed[2]
	$buffer.credential = new-object System.Management.Automation.PSCredential $($global:def_username, $global:def_password)
	
	try{
		$buffer.pss = new-pssession -computername $buffer.address -credential $buffer.credential -erroraction stop
		$buffer.cim = new-cimsession -computername $buffer.address -credential $buffer.credential -erroraction stop 
	}
	catch{
		write-host $_ -foregroundcolor yellow
		write-host "One of the connection was not established. Make sure PS-Remoting is enabled on both host and target machines and credentials are correct." -foregroundcolor red
		return $null
	}

	write-host "Machine registered." -foregroundcolor green
	return $buffer
}
function Sign{
	param(
		[machine]$machine
	)

	$sigPath = "$($global:SERVICE_BUILD)\$($global:SIGNATURE_FILE)"
	$sysPath = "$($global:SERVICE_BUILD)\$($global:SERVICE_FILE)"

	# Create and export cignature to build directory.
	try{
		# Check ceritifcate existance to prevent duplication.
		$machine.certificate = gci -path 'Cert:\CurrentUser\My' | where-object { $_.Subject -eq "CN=$SERVICE_NAME" }
		if ($machine.certificate -eq $null){
			$machine.certificate = new-selfsignedcertificate -type CodeSigningCert -keyexportpolicy Exportable -keyspec Signature -subject "CN=$SERVICE_NAME" -certstorelocation 'Cert:\CurrentUser\My'
		}
		export-pfxcertificate -cert $machine.certificate -filepath $sigPath -password $machine.credential.password -force | out-null
	} 
	catch{
		write-host "Certifcate creation failed." -foregroundcolor red
		return
	}

	# Sign the driver file using exported certificate.	
	try{
		signtool sign /f $sigPath /p $machine.credential.GetNetworkCredential().Password /fd SHA256 $sysPath | out-null
	}
	catch{
		write-host "Driver signing failed." -foregroundcolor red
		return
	}

	write-host "Driver signed." -foregroundcolor green
}
function SetupDirectory{
	param(
		[machine]$machine
	)

	invoke-command -session $machine.pss -erroraction stop -scriptblock{
		param(
			[string]$path,
			[string]$directory
		)
		ni -path (join-path $path $directory) -itemtype Directory -force
	} -argumentlist $machine.destination, "update" | out-null
}
function Install{
	param(
		[machine]$machine
	)

	#Sign -machine $machine
	SetupDirectory -machine $machine

	$tocopy = [string[]](
	"$($global:UPDATE_BUILD)\$($global:UPDATE_FILE)"
	)
	
	cpi -path $tocopy -destination "$($machine.destination)\update" -tosession $machine.pss -force | out-null
}

function Status{
	param(
		[machine]$machine
	)

	invoke-command -session $machine.pss -erroraction stop -scriptblock{
		param(
			[string]$service
		)
		sc.exe queryex $service
	} -argumentlist $global:SERVICE_NAME 
}
function Reboot{
	param(
		[machine]$machine
	)

	invoke-command -session $machine.pss -erroraction stop -scriptblock{
		param(
			[string]$service
		)
		restart-computer -force
	} 
}

[machine]$m0 = Register -machineString $MACHINES_REQUESTED[0]
Install -machine $m0

if ($reboot){
	Reboot -machine $m0
}
