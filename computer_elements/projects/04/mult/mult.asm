// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)

// Put your code here.
// R0 循环 R1 次累加
@2
M = 0       // R2 = 0

@0
D = M      // check R0
@END
D;JLT

@1
D = M      // check R1 这个检查没有那么必要
@END
D;JLT

(LOOP)
    @1
    M = M - 1
    D = M       // R1 - 1
    @END
    D;JLT

    @0
    D = M
    @2
    M = D + M
    @LOOP
    0;JMP

(END)
    @END
    0;JMP