package main

import (
	"fmt"
)

func main() {
	var i int
	//var j int
	var d float32
	var s string

	// Scan是以空格和换行作为分隔
	//fmt.Scan(&i, &d, &s)

	//fmt.Scanf("%d %f\n%s", &i, &d, &s)
	//fmt.Println(i, d, s)

	//fmt.Scanf("%3d%2d", &i, &j)
	//fmt.Println(i, j)

	//fmt.Scanln(&i, &d, &s)
	//fmt.Println(i, d, s)

	// 以上的方法都不可以一次性读取一行
	// Reader提供的方法读取到的值都是byte，rune或者string类型的，
	// 用Reader来读取int或者float32等类型的话还不如直接用fmt包的Scan方法。

	// 看到bufio包

}
