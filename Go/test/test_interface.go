package main

import (
	"fmt"
)

type Animal interface {
	Speak() string
}

type Dog struct {
}

func (d Dog) Speak() string {
	return "Woof!"
}

type Cat struct {
}

//func (c Cat) Speak() string {
//	return "Meow!"
//}

func (c *Cat) Speak() string {
	return "Meow!"
}

type Llama struct {
}

func (l Llama) Speak() string {
	return "?????"
}

type JavaProgrammer struct {
}

func (j JavaProgrammer) Speak() string {
	return "Design patterns!"
}

func main() {
	//animals := []Animal{Dog{}, new(Cat), Llama{}, JavaProgrammer{}}
	animals := []Animal{new(Dog), new(Cat), Llama{}, JavaProgrammer{}}
	for _, animal := range animals {
		fmt.Println(animal.Speak())
	}
}
