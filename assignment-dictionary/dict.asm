global find_word

extern string_equals

section .text

find_word:
		add rdi, 8 ; смещение для прохода по списку
		call string_equals
		sub rdi, 8
		cmp rax, 1
		jz .word_found
		mov rdi, [rdi] ; положим значение по адресу
		test rdi, rdi
		je .word_not_found
		jmp find_word
    .word_found:
		add rdi, 8
		mov rax, rdi ; положим в регистр rax адрес начала вхождения в словарь
		ret
	.word_not_found:
		xor rax, rax ; обнулим регистр rax, так как ничего не нашли
		ret
