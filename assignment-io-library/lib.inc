section .text

; Принимает код возврата и завершает текущий процесс
exit:
    mov rax, 60
    syscall

; Принимает указатель на нуль-терминированную строку, возвращает её длину
string_length:
	push rbx
	xor rbx, rbx
	mov cl, [rdi]
	test cl, cl
	jne .strLenLoop
	jmp .strLenOut
.strLenLoop:
	inc rbx
	mov cl, [rdi+rbx]
	test cl, cl
	jne .strLenLoop
.strLenOut:
	mov rax, rbx
	pop rbx
	ret

; Принимает указатель на нуль-терминированную строку, выводит её в stdout
print_string:
	mov rsi, rdi
	call string_length
	mov rdx, rax
	mov rax, 1
	mov rdi, 1
	syscall
    ret

; Переводит строку (выводит символ с кодом 0xA)
print_newline:
    mov rdi, 0xA

; Принимает код символа и выводит его в stdout
print_char:
	push rdi
	mov rdi, rsp
	call print_string
	pop rdi
    ret

; Выводит беззнаковое 8-байтовое число в десятичном формате 
; Совет: выделите место в стеке и храните там результаты деления
; Не забудьте перевести цифры в их ASCII коды.
print_uint:
    mov rax, rdi
    mov r10, rsp ; сохраним текущее состояние стека
    push 0 ; положим на стек нуль-терминатор
	.divisionInt:
		xor rdx, rdx ; обнулим остаток
    	mov rcx, 10 ; положим делитель в rcx
    	div rcx ; разделим на число из rcx
    	add rdx, 48 ; прибавляем 48 для преобразования в ASCII код
    	dec rsp ; декремент значения стек поинтера
    	mov [rsp], dl ; положим младший байт rdx (остатка от деления)
    	cmp rax, 0 ; дошли до нуль-терминатора?
    	jne .divisionInt ; нет? повторяем! да? идем дальше
	mov rdi, rsp ; положим указатель составленной в памяти строку в rdi
	call print_string ; вызовем вывод текста
	mov rsp, r10 ; вернем стек на место
	ret
    
; Выводит знаковое 8-байтовое число в десятичном формате 
print_int:
	cmp rdi, 0
	jl .change_sign
	jmp print_uint
.change_sign:
	neg rdi
	push rdi
	mov rdi, 0x2D
	call print_char
	pop rdi
	jmp print_uint

; Принимает два указателя на нуль-терминированные строки, возвращает 1 если они равны, 0 иначе
string_equals:
	xor rax, rax
	xor r8, r8
	xor r10, r10
.check:
	mov r8b, [rdi + r10]
	cmp r8b, [rsi + r10]
	jne .complete
	inc r10
	test r8, r8
	jne .check
	mov rax, 1
.complete:
    ret

; Читает один символ из stdin и возвращает его. Возвращает 0 если достигнут конец потока
read_char:
	push 0
	xor rax, rax
	xor rdi, rdi
	mov rsi, rsp
	mov rdx, 1
	syscall
	pop rax
    ret

; Принимает: адрес начала буфера, размер буфера
; Читает в буфер слово из stdin, пропуская пробельные символы в начале, .
; Пробельные символы это пробел 0x20, табуляция 0x9 и перевод строки 0xA.
; Останавливается и возвращает 0 если слово слишком большое для буфера
; При успехе возвращает адрес буфера в rax, длину слова в rdx.
; При неудаче возвращает 0 в rax
; Эта функция должна дописывать к слову нуль-терминатор

read_word:
	xor r8, r8
	xor r10, r10
.wordReader:
	push rdi
	push rsi
	call read_char
	pop rsi
	pop rdi
	test rax, rax
	je .complete
	cmp rax, 0x20
	je .space_trigger
	cmp rax, 0x9
	je .space_trigger
	cmp rax, 0xA
	je .space_trigger
	mov r8, 0x1
	cmp r10, rsi
	je .outofBuffer
	mov [rdi + r10], rax
	inc r10
	jmp .wordReader
.space_trigger:
	cmp r8, 0x1
	je .complete
	jmp .wordReader
.outofBuffer:
	xor rax, rax
.complete:
	mov rdx, r10
	mov rax, rdi
    ret
 

; Принимает указатель на строку, пытается
; прочитать из её начала беззнаковое число.
; Возвращает в rax: число, rdx : его длину в символах
; rdx = 0 если число прочитать не удалось
parse_uint:
	xor rax, rax
	xor rdx, rdx
	xor r10, r10
.read_digit:
	mov r10b, [rdi + rdx] ; значение по указателю rdi + текущее смещение в длине
	sub r10, 0x30 ; переведем ASCII в число
	
	; проверим промежуток от 0 до 9, чтобы убедиться, что это цифра
	cmp r10, 0
	jnae .complete
	cmp r10, 9
	jnbe .complete

	inc rdx ; не завершили функцию? увеличим счетчик символов
	
	; проделаем обратные действия по сравнению с print_uint
	imul rax, 10 ; умножим на 10
	add rax, r10 ; добавим нашу циферку в rax к остальным
	jmp .read_digit ; проверим следующий символ
.complete:
	ret

; Принимает указатель на строку, пытается
; прочитать из её начала знаковое число.
; Если есть знак, пробелы между ним и числом не разрешены.
; Возвращает в rax: число, rdx : его длину в символах (включая знак, если он был) 
; rdx = 0 если число прочитать не удалось
parse_int:
	xor r10, r10
	mov r10b, [rdi]
	cmp r10, 0x2D
	je .change_sign
	jmp parse_uint
.change_sign:
	push rdi
	mov rdi, 0x2D
	call print_char
	pop rdi
	inc rdi
	call parse_uint
	inc rdx
	ret

; Принимает указатель на строку, указатель на буфер и длину буфера
; Копирует строку в буфер
; Возвращает длину строки если она умещается в буфер, иначе 0
string_copy:
    xor rax, rax
    xor r10, r10
.stringCopier:
	; rdi указатель на строку
	; rsi указатель на буфер
	; rdx длина буфера
    mov r10b, [rdi + rax]
    mov [rsi + rax], r10b
    cmp rax, rdx
    je .outOfBuffer
    test r10, r10
    je .complete
    inc rax
    jmp .stringCopier
.outOfBuffer:
	xor rax, rax
.complete:
	ret
