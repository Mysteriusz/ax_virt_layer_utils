package main

import (
	"crt/serializer"
	"fmt"
)

func main(){
	doc,_ := loader.FindAtDefault()
	_,err := loader.SymParse(doc)
	fmt.Println(err)
	//fmt.Println(doc)
}

