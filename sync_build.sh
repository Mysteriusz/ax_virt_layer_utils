declare -A CC_CLANG=(
	[name]="clang"
	[cmd]="clang"
)

# Supported compilers
compilers=(CC_CLANG)

# 1 - um; 2 - km
export mode=0

# Global shared
export name=""
export cmd=""
export aalias=""
export dalias=""
export ialias=""
export salias=""
export oalias=""
export flags=()
export define=()

function match_cc() {
	str="$1"
	for cc in ${compilers[@]}; do
		name=$(eval "echo \${${cc}[name]}")
		cmd=$(eval "echo \${${cc}[cmd]}")
		if [[ $name == $str ]]; then
			echo $cmd
			return 0
		fi
	done

	return 1
}

while getopts "kuc:" opt; do
	case $opt in
	k)
		if [[ $mode != 0 ]]; then
			echo "ERROR: Multiple build modes defined"
			exit 1
		fi
		echo "Kernel mode"
		mode=2
		;;
	u)
		if [[ $mode != 0 ]]; then
			echo "ERROR: Multiple build modes defined"
			exit 1compile
		fi
		echo "User mode"
		mode=1
		;;
	c)
		cmd=$(match_cc "$OPTARG")
		if [[ "$?" != 0 ]]; then
			echo "ERROR: Unsupported compiler" 
			exit 1
		fi
		echo "Compiler:" $cmd
		;;
	esac
done

if [[ $mode == 1 ]]; then # User mode variables
	flags=(
		"Wall"
		"Werror"
		"O2"
		"std=c23"
		"Wno-unused-function"
		"-target=x86_64-pc-windows-msvc"
	)
	define=(
		"AX_WIN64"
		"AX_UM"
	)
	aalias="-"
	dalias="-D"
	ialias="-I"
	salias="-c"
	oalias="-o"
elif [[ $mode == 2 ]]; then # Kernel mode variables
	echo ""
else
	echo "ERROR: Unknown mode"
	exit 1
fi

define_a=${define[@]/#/$dalias}
flags_a=${flags[@]/#/$aalias}

# Full paths to build scripts in-order
build_order=(
	"D:\ax_project\ax_virt_layer_utils\utils\ax_utility_lib\win\build.sh"
	"D:\ax_project\ax_virt_layer_utils\utils\ax_update_exe\win\build.sh"
)

for build in ${build_order[@]}; do
	source $build
done

