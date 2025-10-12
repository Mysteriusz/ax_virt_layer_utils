build_dir="$(dirname $BASH_SOURCE)/build"
header_dir="$(dirname $BASH_SOURCE)/build/headers"

readarray -t file_c < <(find "$(dirname $BASH_SOURCE)/src" -name "*.c" -type f -exec realpath {} +)
readarray -t file_h < <(find "$(dirname $BASH_SOURCE)/src" -name "*.h" -type f -exec realpath {} +)

include_dirs=()
for f in "${file_h[@]}"; do
    	dir=$(dirname "$f")
    	[[ ! " ${include_dirs[*]} " =~ " $dir " ]] && include_dirs+=("$dir")
done

file_o=()
function compile(){
	file="$1"
	output="$build_dir/$(basename $file .c).o"

	$cmd \
	    	"${flags[@]/#/$aalias}" \
		"${define[@]/#/$dalias}" \
	    	"$salias" "$file" \
	    	"$oalias" "$output" \
	    	"${include_dirs[@]/#/$ialias}"

	if [[ $? != 0 ]]; then
		kill 0
		exit 2
	fi
	echo "$output"

	return 0
}

for file in ${file_c[@]}; do
	compile $file &
done

wait

for file in ${file_h[@]}; do
    cp "$file" "$header_dir/"
done

# Create static .lib file
llvm-ar rcs "$build_dir/ax_utility.lib" $(find $build_dir -maxdepth 1 -type f -name "*.o")
if [[ $? != 0 ]]; then
	exit
fi

# Copy headers
cp "${file[@]}" "$build_dir/headers/" -f

