$location = $(get-location).Path

$path = $location+"\utils\ax_update_exe\win\build\ax_update.exe"

if (test-path($path)){
	& $path
}

