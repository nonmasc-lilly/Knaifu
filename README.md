# THE KNAIFU

the knaifu is my custom cpu architecture.
it is rather generic.

It has 8 instructions

000. set r,i      : sets r to i {z}
001. addrHL r1,r2 : sets H to r1 and L to r2
010. mem r        : if null is low set HL to r, else set r to HL
011. swp r1,r2    : sets r1 to r2 {z}
100. add r1,r2    : adds r2 to r1 and stores it in r1 {zc}
101. not r,i      : negates r1 and adds i then stores it in r1 {z}
110. or r1,r2     : bitwise ors r1 with r2 and stores it in r1 {z}
111. jz           : if the {z} flag is high set IHIL to HL
The knaifu is an 8 bit machine with a 16 bit address space
this means that it has an 8 bit dataline with a 16 bit address
line. It has 8 registers.

000. A : 8 bit general
001. B : 8 bit general
010. C : 8 bit general
100. H|(011.)L : 2, 8 bit general/address registers
110. IH|(101.)IL : 2, 8 bit instruction registers
111. flags : 8 bit flags register

Instructions are read in as 8 bit blocks. With the following format:

IIIRRRNN OOOOOOOO

I = instruction
R = register
N = null bit, if either of these are high then we should not read
    the next byte as the same instruction, if both are high we
    should run a nop cycle
O = option byte, for instructions that require more than one arg

option is assigned one in the event of a null bit operation

if the value at the address [FFFF] is FF then the knaifu will halt

for example we could have the truth machine as follows:
```
0000|set A,$0F    ; a = $f
0002|set B,1      ; b = 1
0004|addrHL A,B   ; HL = ab
0006|[null] mem A ; a = [HL]
0007|swp C,A      ; c = a, a = c
0009|set A,0      ; a = $18
000B|set B,$24    ; b = 0
000D|addrHL a,b   ; HL = ab
000F|swp A,C      ; a = c, c = a
0011|[null] jz    ; if ~a goto HL
0012|set A,2      ; a = 0
0014|set B,$FF    ; b = $ff
0016|addrHL A,B   ; HL = ab
0018|set A,0      ; a = 0
001A|mem A        ; [HL] = a
    |             ; <halt>
001C|set A,$FF    ; a = $ff
001E|set B,$FF    ; b = $ff
0020|addrHL A,B   ; HL = ab
0022|mem A        ; [HL] = a
0024|set C, 0     ; truth:
0026|set A, 0     ; loop: a = c
0028|add A, C     ; ^^^
002A|set B, 2     ; b = 2
002C|addrHL A,B   ; HL = ab
002E|set A, 1     ; a = 1
0030|mem A        ; [HL] = a
0032|add C, A     ; c += a
0034|set A, 0     ; a = 0
0036|set B, $26   ; b = $26
0038|addrHL A,B   ; HL = ab
003A|set A, 0     ; a = 0 (set {z})
003B|[null] jz    ; jz (jun $0026)
```

The knaifu will begin execution at [0000] and continue until
the value at [FFFF] is equal to FF.
