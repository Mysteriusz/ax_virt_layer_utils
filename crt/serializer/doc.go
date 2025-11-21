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

type ser_err int32
const (
	SER_SUCC = iota
	SER_NOT_FND
	SER_INV_DOC
	SER_INV_SYN
	SER_READ_ERR
)

