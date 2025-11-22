package loader

type stack []any

func (s *stack) Push(a_v any){
	*s = append(*s, a_v)
}
func (s *stack) Pop() (r_v any){
	l := len(*s)
	if l == 0{
		return nil
	}

	r_v = (*s)[l-1]
	*s = (*s)[:l-1]
	return r_v
}

