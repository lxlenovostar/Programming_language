// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 33.
//!+

// Echo4 prints its command-line arguments.
package main

import (
	"flag"
	"fmt"
	"strings"
)

// n 和 sep分别是指向对应命令行标志参数变量的指针
// 第一个参数是命令行标志参数的名字
var n = flag.Bool("n", false, "omit trailing newline")
var sep = flag.String("s", " ", "separator")

func main() {
	// parse the command line into the defined flags.
	flag.Parse()
	// 对于非标志参数的普通命令行参数可以通过调用flag.Args()函数来访问
	fmt.Print(strings.Join(flag.Args(), *sep))
	if !*n {
		fmt.Println()
	}
}

//!-
