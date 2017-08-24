// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 8.
//!+

// Dup1 prints the text of each line that appears more than
// once in the standard input, preceded by its count.
package main

import (
	"bufio"
	"fmt"
	"os"
)

func main() {
	// 内置函数make创建空map
	counts := make(map[string]int)
	// 程序使用短发量声明创建 bufio.Scanner 类垄癿发量 input。
	input := bufio.NewScanner(os.Stdin)
	// Scan 函数在读到一行时返回true,在无输入时返回false。
	for input.Scan() {
		//读取的内容可以调用input.Text()得到
		counts[input.Text()]++
	}
	// NOTE: ignoring potential errors from input.Err()
	for line, n := range counts {
		if n > 1 {
			fmt.Printf("%d\t%s\n", n, line)
		}
	}
}

//!-
