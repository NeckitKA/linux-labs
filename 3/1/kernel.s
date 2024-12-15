mov ah, 0x0e ; будет запрашивать вывод к консоль биоса
mov al, 'I' ; ну и так на каждую букву
int 0x10
mov al, 'f' 
int 0x10 ; буква l два раза поэтому просто два раза вызываем прерывание
mov al, ' '
int 0x10
mov al, 'y'
int 0x10
mov al, 'o'
int 0x10
mov al, 'u'
int 0x10
mov al, ' ' 
int 0x10
mov al, 'g'
int 0x10
mov al, 'a' 
int 0x10
mov al, 'z'
int 0x10
mov al, 'e'
int 0x10
mov al, 10
int 0x10
mov al, 13
int 0x10
mov al, 'l'
int 0x10
mov al, 'o'
int 0x10
mov al, 'n'
int 0x10
mov al, 'g'
int 0x10
mov al, ' '
int 0x10
mov al, 'i'
int 0x10
mov al, 'n'
int 0x10
mov al, 't'
int 0x10
mov al, 'o'
int 0x10
mov al, ' '
int 0x10
mov al, 'a'
int 0x10
mov al, 'b'
int 0x10
mov al, 'y'
int 0x10
mov al, 's'
int 0x10
mov al, 's'
int 0x10
mov al, ' ' 
int 0x10
mov al, '^'
int 0x10
mov al, 95
int 0x10
mov al, '^'
int 0x10
; бесконечный цикл, те самый (eb fe) сверху
loop:
    jmp loop 

; заполняем 510 нулевыми байтами минус размер в байтах кода выше
times 510-($-$$) db 0
; те самые магические байты в конце сектора
dw 0xaa55 
