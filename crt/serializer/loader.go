package loader

import(
	"os"
	"path/filepath"
	"log"
	"fmt"
	"regexp"
	"io"
	"bufio"
)

func FindAtDefault() (r_doc *doc, r_err ser_err) {
	_ = log.Println
	_ = fmt.Println
	_ = regexp.MustCompile
	_ = io.ErrUnexpectedEOF
	_ = bufio.NewScanner

	var fp *os.File = nil
	var fs string = ""

	var err error
	for _, val := range DOC_DEF {
		fs,err = os.Executable()
		if err != nil{
			panic(1)
		}
		fs = filepath.Join(filepath.Dir(fs), val)

		fp,err = os.OpenFile(fs, os.O_RDONLY, 0)
		if err != nil{
			break
		}
	}
	if fp == nil || fs == "" || err != nil{
		return nil,SER_NOT_FND
	}

	found := &doc{file: fp, path: fs}
	return found,SER_SUCC
}

func doc_inv(a_doc *doc) bool{
	if a_doc == nil{
		return true
	}
	
	_,err := os.Lstat(a_doc.path)
	if err != nil || filepath.Ext(a_doc.path) != DOC_EXT{
		return true
	}

	if a_doc.file == nil{
		return true
	}

	return false
}

func SymLineState(a_scan *bufio.Scanner, a_state sym_state) (r_state sym_state, r_err ser_err){
	if a_scan == nil{
		return SYM_UNK,SER_INV_DOC
	}

	line := a_scan.Text()	
	var found bool = false

	for _,sym_kvp := range SYM_MAP{
	fmt.Println(line)
		// a_state allows this sym_kvp mapped regex
		if sym_kvp.i_state & a_state != 0{
			found,_ = regexp.MatchString(sym_kvp.regex, line)
		}

		if found == true{
			break;
		}
	}

	if found == false{
		return SYM_UNK,SER_INV_SYN
	}

	return SYM_FIELD,SER_SUCC
}

func SymParse(a_doc *doc) (r_sym sym_doc, r_err ser_err){
	if doc_inv(a_doc){
		return sym_doc{},SER_INV_DOC
	}

	var sym = sym_doc{}

	var sym_state sym_state = SYM_UNK
	var sym_err ser_err = SER_SUCC

	scanner := bufio.NewScanner(a_doc.file)
	for scanner.Scan(){
		sym_state,sym_err = SymLineState(scanner, sym_state)

		if sym_err != SER_SUCC || sym_state == SYM_UNK{
			return sym_doc{},SER_INV_SYN
		}
	}

	return sym,SER_SUCC
}

