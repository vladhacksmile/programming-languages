%include "lib.inc"
%include "words.inc"

%define BUFFER_SIZE 256

%define SUCCESSFUL_EXIT_CODE 0
%define ERROR_EXIT_CODE 1

global _start

extern find_word

section .rodata ; секция Read Only Data, здесь хранятся сообщения, ну а зачем им права на изменение?
	message: db "Please, enter key: ", 10, 0
	message_error_key: db "Sorry, key not found!", 10, 0
	message_error_key_len: db "Sorry, but key length too long!", 10, 0

section .text

_start:
	sub rsp, BUFFER_SIZE
	mov rdi, rsp ; положим указатель в rdi на проаллоцируемый буффер
	mov rsi, BUFFER_SIZE ; положим в rsi размер буфера
	call read_word
	test rax, rax
	je .outOfBuffer
	mov rdi, peak ; положим в rdi указатель на peak
	mov rsi, rsp ; положим в rsi указатель буффер
	call find_word ; читаем слово при помощи функции из dict.asm
	add rsp, BUFFER_SIZE
	test rax, rax
	je .key_not_found
	mov rdi, rax
	push rdi
	call string_length
	pop rdi
	add rdi, rax ; добавляем длину строки к указателю на начало слово
	inc rdi ; про нуль-терминированный символ тоже не забываем
	call print_string
	mov rdi, SUCCESSFUL_EXIT_CODE
	call exit
.outOfBuffer: ; вывод сообщения о выходах за пределы допустимой длины буфера
	add rsp, BUFFER_SIZE
	mov rdi, message_error_key_len
	call print_error
	mov rdi, ERROR_EXIT_CODE
	call exit
.key_not_found: ; вывод сообщения, если ключ не нашелся
	mov rdi, message_error_key
	call print_error
	mov rdi, ERROR_EXIT_CODE
	call exit
