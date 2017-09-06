package main

import (
	"fmt"
)

func test(v string) string {
	return v
}

func expand(s string, f func(string) string) string {
	return f(s)
}

func main() {
	v := "hi all"
	f := test
	fmt.Println(expand(v, f))
}
