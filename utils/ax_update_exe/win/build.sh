build_dir="$(dirname $BASH_SOURCE)/build"
header_dir="$(dirname $BASH_SOURCE)/build/headers"

file_lib_u=("D:/ax_project/ax_virt_layer_utils/utils/ax_utility_lib/win/build/ax_utility.lib")
file_h_u=("D:/ax_project/ax_virt_layer_utils/utils/ax_utility_lib/win/build/headers")
file_h_u=${file_h_u[@]/#/$ialias}

readarray -t file_h < <(find "$(dirname $BASH_SOURCE)/src/" -name "*.h" -type f -exec realpath {} +)
readarray -t file_c < <(find "$(dirname $BASH_SOURCE)/src/" -name "*.c" -type f -exec realpath {} +)

for file in ${file_c[@]}; do
	temp="$build_dir/$(basename $file .c).o"
	$cmd $salias $define_a $file_h_u ${file_h_u[@]/#/$ialias} ${file_h[@]/#/$ialias} $file $oalias $temp
	if [[ $? != 0 ]]; then
		exit
	fi
done

$cmd $flags_a $define_a $file_lib_u $file_h_u ${file_h[@]/#/$ialias} $(find "$build_dir" -name "*.o" -type f -exec realpath {} +) -ladvapi32 $oalias "$build_dir/ax_update.exe" "-Wl,/subsystem:console"
if [[ $? != 0 ]]; then
	exit
fi

