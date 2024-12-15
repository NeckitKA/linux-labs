print:
    pusha ; сохраняем все регистры в стек
.print_loop:
    cmp byte [bx], 0 ; проверяем на равенство нулевому символу
    jz .done ; если равен, то идем на метку .done
    cmp byte [bx], 92 ; проверяем на символ переноса строки
    jz .newline 
    mov ah, 0x0e ; иначе рисуем символ
    mov al, [bx]
    int 0x10
    inc bx
    jmp .print_loop ; безусловный переход, то есть идем в начало
.newline:
inc bx
cmp byte [bx], 110
jz .newDrop
   mov ah, 0x0e ; иначе рисуем символ
    mov al, [bx]
    int 0x10
    inc bx
    jmp .print_loop ; безусловный переход, то есть идем в начало
.newDrop:
mov al, 10
    int 0x10
    mov al, 13
    int 0x10
inc bx
mov ah, 0x0e ; иначе рисуем символ
    mov al, [bx]
    int 0x10
    inc bx
    jmp .print_loop ; безусловный переход, то есть идем в начало
.done:
    popa ; восстанавливаем все регистры из стека
    ret
