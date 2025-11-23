package loader

import(
	"golang.org/x/exp/mmap"
	"fmt"
	"strings"
)

type frame_type uint32
const(
	F_ROOT = 0x00
	F_ARRAY = 0x01
	F_VALUE = 0x02
)

type frame struct{
	f_type frame_type
	f_sig string 
	f_con string // Context tree starting from root.x
}

var ROOT_FRAME = &frame{
	f_type: F_ROOT,
	f_sig: "",
	f_con: "root",
}

var CONTROL_CHARS = []byte{
	'{',
	'}',
	'=',
	'\x20',
	'\t',
	'\n',
	'\r',
}

func FrameTypeDecide(a_frame *frame, a_char byte) (r_type frame_type, r_err crt_err){
	switch(a_frame.f_type){
	case F_ARRAY:
	case F_ROOT:
		if a_char == '{'{
			r_type = F_ARRAY
		}else{
			r_type = F_VALUE
		}
	default:
		return 0,CRT_INV_SYN
	}

	return r_type,CRT_SUCC
}

func FrameSearch(
	a_reader *mmap.ReaderAt,
	a_frame *frame,
	a_off int,
) (r_frame *frame, r_off int, r_err crt_err){
	var buf *frame = &frame{}
	var i int = a_off	
	var map_len int = a_reader.Len()
	var new_con strings.Builder

	for i < map_len{
		c := a_reader.At(i)

		switch(c){
		case '{': // Frame opening
			new_sig := strings.Trim(new_con.String(), string(CONTROL_CHARS))

			buf.f_sig = new_sig
			buf.f_con = a_frame.f_con + "." + new_sig
			fmt.Println("Processing frame at: - "+buf.f_con)
			ft,err := FrameTypeDecide(a_frame, c)
			if err != CRT_SUCC{
				return nil,0,CRT_INV_SYN
			}

			buf.f_type = ft
			return buf,i + 1,CRT_SUCC
		case '}': // Frame close
			return nil,i + 1,CRT_SUCC
		default:
			new_con.WriteByte(c)
		}
		i++
	}

	return buf,i,CRT_SUCC
}

func FrameLoad(a_doc *doc) (r_frame *frame, r_err crt_err){
	if doc_inv(a_doc){
		return nil,CRT_INV_DOC
	}

	// Initial root and stack
	var f_root *frame = ROOT_FRAME
	var f_stack *stack = &stack{}
	f_stack.Push(f_root)

	// Initialize file map
	reader,err := mmap.Open(a_doc.path)
	if err != nil{
		return nil,CRT_INV_DOC
	}
	var map_len int = reader.Len()
	var i int = 0
	var prev *frame = f_root

	// Iterate over all frames
	for i < map_len{
		// Find next frame
		buf,ni,err := FrameSearch(reader, prev, i)
		// Pop current frame (buffer is nil)
		if buf == nil && err == CRT_SUCC{
			buf = f_stack.Pop().(*frame)

			if len(*f_stack) == 0{
				break
			}
			prev = (*f_stack)[len(*f_stack)-1].(*frame)
		}else if err != CRT_SUCC{
			panic(2)
		}else{
			f_stack.Push(buf)
			prev = buf
		}

		i = ni
	}

	return f_root,CRT_SUCC
}

