// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Put your code here.
(INIT)
@8192 // Hack计算机的屏幕包括 256*512像素。 一个按键按下，其对应的 16bits 将全部设置为1
D = A
@index
M = D

(LOOP)
@index 
M = M - 1   // 0-8191 
D = M
@INIT
D;JLT   // 遍历完了，重置操作

@KBD
D = M
@WHITE
D;JEQ
@BLACK
0;JMP

(WHITE)
@SCREEN
D = A 
@index
A = D + M
M = 0
@LOOP
0;JMP

(BLACK)
@SCREEN
D = A 
@index
A = D + M
M = -1 
@LOOP
0;JMP
