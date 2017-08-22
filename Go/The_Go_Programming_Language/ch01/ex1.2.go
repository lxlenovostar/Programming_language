// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

package main

import (
	"fmt"
	"os"
	"strconv"
)

func main() {
	for tmp, arg := range os.Args[1:] {
		fmt.Printf("%v\n", strconv.Itoa(tmp)+" "+arg)
	}
}
