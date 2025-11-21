package loader

// Parsed symbol types
type sym_doc struct{
}
type sym_state int32
const(
	SYM_MODE = 0x01
	SYM_FIELD = 0x02
	SYM_LIST = 0x04
	SYM_UNK = 0x08
	SYM_EMP = 0x08
)

var SYM_MAP = []struct{
	i_state sym_state // Initial required state flag
	regex string // Appropriate regex
}{
	{
		i_state: SYM_UNK | SYM_EMP, // Unknown or empty symbol state required
		regex: "(?P<ROOT>[A-Z]*)[.](?P<FIELD>[A-Z_]*)([\x20\t]*[=][\x20\t]*)(?:\\{\\}?)", // Only next state possible is mode
	},
}

