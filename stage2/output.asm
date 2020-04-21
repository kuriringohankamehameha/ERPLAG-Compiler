SECTION .bss
base: resb 10000
SECTION .data
fmt: db "Output:%d", 10, 0
intFormat:     db        "%d", 0
int_inMsg:    db        "Enter an integer value" , 10, 0
true_msg:    db        "TRUE" , 10, 0
false_msg:    db        "FALSE" , 10, 0
extern printf
extern scanf
section .text
global main
driver:
sub rsp,8
MOV EAX,0
mov [base+24], eax
MOV EAX, 5
mov [base+12], eax
MOV EAX, 9
mov [base+16], eax
lea rdi, [int_inMsg]
xor rax, rax
call printf
lea rdi, [intFormat]
lea rsi, [base+0]
xor rax, rax
call scanf
lea rdi, [int_inMsg]
xor rax, rax
call printf
lea rdi, [intFormat]
lea rsi, [base+4]
xor rax, rax
call scanf
MOV EAX, [base + 0]
push rax
MOV EAX, [base + 4]
push rax
MOV EAX, [base + 16]
pop rbx
IMUL     EBX
push rax
MOV EAX, [base + 12]
push rax
MOV EAX, [base + 16]
pop rbx
SUB EBX,EAX
MOV     EAX, EBX
push rax
MOV EAX, [base + 4]
pop rbx
IMUL     EBX
push rax
MOV EAX, [base + 12]
push rax
MOV EAX, 2
pop rbx
IMUL     EBX
push rax
MOV EAX, [base + 16]
push rax
MOV EAX, [base + 0]
pop rbx
IMUL     EBX
pop rbx
SUB EBX,EAX
MOV     EAX, EBX
pop rbx
ADD EAX,EBX
pop rbx
ADD EAX,EBX
pop rbx
ADD EAX,EBX
mov [base+8], eax
MOV EAX, [base + 8]
PUSH RAX
MOV EAX,0
POP RBX
CMP EBX,EAX
JG X1
MOV EAX,0
JMP E1
X1:
MOV EAX,1
E1:
push rax
MOV EAX, [base + 12]
PUSH RAX
MOV EAX,0
POP RBX
CMP EBX,EAX
JLE X2
MOV EAX,0
JMP E2
X2:
MOV EAX,1
E2:
push rax
MOV EAX, [base + 0]
PUSH RAX
MOV EAX,0
POP RBX
CMP EBX,EAX
JL X3
MOV EAX,0
JMP E3
X3:
MOV EAX,1
E3:
push rax
MOV EAX, [base + 24]
pop rcx
AND EAX,ECX
pop rcx
AND EAX,ECX
pop rcx
OR EAX,ECX
mov [base+24], eax
mov eax, [base+8]
mov rdi, fmt
mov rsi, [base+8]
call printf
mov eax, [base+24]
cmp eax,0
JE X4
lea rdi, [true_msg]
xor rax, rax
call printf
jmp E4
X4:
lea rdi, [false_msg]
xor rax, rax
call printf
E4:
add rsp,8
ret
main:
sub rsp,8
call driver
add rsp,8
ret
