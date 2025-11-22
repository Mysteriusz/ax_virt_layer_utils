package main

import (
	"crt/parser"
	"fmt"
)

func main(){
	doc,_ := loader.FindAtDefault()
	_,err := loader.FrameLoad(doc)
	fmt.Println(err)
}

