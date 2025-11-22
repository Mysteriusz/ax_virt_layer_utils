package loader

import(
	"os"
)

// File search order
var DOC_DEF = []string{
	".crtcfg",
}
var DOC_EXT string = ".crtcfg"

type doc struct{
	file *os.File
	path string
}

var DOC_MAX_SIZE uint32 = (1 << 21) // 1 MB

type crt_err int32
const (
	CRT_SUCC = iota
	CRT_NOT_FND
	CRT_INV_DOC
	CRT_INV_SYN
	CRT_READ_ERR
	CRT_TOO_BIG
)

