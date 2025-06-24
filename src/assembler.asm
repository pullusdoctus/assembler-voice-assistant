section .data
    filename db "src/db.txt", 0
    no_respuesta db "No se encontró respuesta", 0
    delimitador db "|", 0

section .bss
    fd resq 1
    buffer resb 4096
    pregunta_comp resb 256
    respuesta_temp resb 1024
    idioma_comp resb 32
    pregunta_orig resq 1
    respuesta_orig resq 1
    idioma_orig resq 1

section .text
global buscar_respuesta

buscar_respuesta:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13
    push r14
    push r15
    
    ; Guardar parámetros
    mov [pregunta_orig], rdi
    mov [respuesta_orig], rsi
    mov [idioma_orig], rdx
    
    ; Abrir archivo
    mov rax, 2
    mov rdi, filename
    xor rsi, rsi
    xor rdx, rdx
    syscall
    
    cmp rax, 0
    jl .no_respuesta
    mov [fd], rax
    
    ; Leer archivo
    mov rax, 0
    mov rdi, [fd]
    mov rsi, buffer
    mov rdx, 4095
    syscall
    
    cmp rax, 0
    jle .no_respuesta
    
    ; Null-terminar buffer
    mov byte [buffer + rax], 0
    
    ; Procesar contenido
    mov rsi, buffer
    
.procesar_linea:
    ; Verificar fin de archivo
    cmp byte [rsi], 0
    je .no_respuesta
    
    ; Saltar líneas vacías
    cmp byte [rsi], 10
    je .siguiente_linea
    cmp byte [rsi], 13
    je .siguiente_linea
    
    ; Verificar si es comentario (línea que empieza con #)
    cmp byte [rsi], '#'
    je .saltar_comentario
    
    ; Verificar si la línea contiene al menos un '|' (formato válido)
    push rsi
    call verificar_formato_valido
    pop rsi
    test rax, rax
    jz .saltar_linea  ; Si no tiene formato válido, saltar
    
    ; Copiar pregunta hasta '|'
    mov rdi, pregunta_comp
    push rsi
    call copiar_hasta_delimitador
    mov rbx, rsi  ; Guardar posición después del '|'
    pop rsi
    
    ; Verificar que se extrajo algo
    cmp byte [pregunta_comp], 0
    je .saltar_linea
    
    ; Comparar preguntas
    mov rdi, pregunta_comp
    mov rsi, [pregunta_orig]
    call comparar_cadenas
    
    test rax, rax
    jnz .saltar_linea
    
    ; Preguntas coinciden, continuar con respuesta
    mov rsi, rbx  ; Restaurar posición después del primer '|'
    mov rdi, respuesta_temp
    call copiar_hasta_delimitador
    mov rbx, rsi  ; Guardar posición después del segundo '|'
    
    ; Verificar que se extrajo respuesta
    cmp byte [respuesta_temp], 0
    je .saltar_linea
    
    ; Copiar idioma
    mov rsi, rbx
    mov rdi, idioma_comp
    call copiar_hasta_nueva_linea
    
    ; Verificar que se extrajo idioma
    cmp byte [idioma_comp], 0
    je .saltar_linea
    
    ; Convertir y comparar idioma
    mov rdi, idioma_comp
    call convertir_idioma_simple
    
    cmp rax, [idioma_orig]
    jne .saltar_linea
    
    ; Copiar respuesta
    mov rsi, respuesta_temp
    mov rdi, [respuesta_orig]
    call copiar_cadena
    jmp .cerrar_archivo

.saltar_comentario:
    call avanzar_a_siguiente_linea
    jmp .procesar_linea

.siguiente_linea:
    inc rsi
    jmp .procesar_linea

.saltar_linea:
    call avanzar_a_siguiente_linea
    jmp .procesar_linea

.no_respuesta:
    mov rsi, no_respuesta
    mov rdi, [respuesta_orig]
    call copiar_cadena

.cerrar_archivo:
    mov rax, 3
    mov rdi, [fd]
    syscall
    
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret

verificar_formato_valido:
    ; rsi = línea a verificar
    ; Retorna 1 si tiene al menos dos '|', 0 si no
    push rdi
    mov rdi, rsi
    xor rax, rax  ; contador de '|'
    
.loop:
    mov cl, [rdi]
    cmp cl, 0
    je .fin
    cmp cl, 10    ; nueva línea
    je .fin
    cmp cl, 13    ; retorno de carro
    je .fin
    cmp cl, '|'
    jne .continuar
    inc rax
.continuar:
    inc rdi
    jmp .loop
    
.fin:
    ; Necesitamos al menos 2 '|' para formato válido: pregunta|respuesta|idioma
    cmp rax, 2
    jge .valido
    xor rax, rax  ; No válido
    jmp .salir
.valido:
    mov rax, 1    ; Válido
.salir:
    pop rdi
    ret

copiar_hasta_delimitador:
    ; rsi = fuente, rdi = destino
    ; Salida: rsi = después del '|'
    push rax
    
.loop:
    mov al, [rsi]
    cmp al, '|'
    je .fin
    cmp al, 0
    je .fin
    cmp al, 10
    je .fin
    cmp al, 13
    je .fin
    mov [rdi], al
    inc rsi
    inc rdi
    jmp .loop
    
.fin:
    mov byte [rdi], 0
    cmp al, '|'
    jne .no_skip
    inc rsi
.no_skip:
    pop rax
    ret

copiar_hasta_nueva_linea:
    ; rsi = fuente, rdi = destino
    push rax
    
.loop:
    mov al, [rsi]
    cmp al, 10
    je .fin
    cmp al, 13
    je .fin
    cmp al, 0
    je .fin
    mov [rdi], al
    inc rsi
    inc rdi
    jmp .loop
    
.fin:
    mov byte [rdi], 0
    pop rax
    ret

avanzar_a_siguiente_linea:
    push rax
    
.loop:
    mov al, [rsi]
    cmp al, 0
    je .fin
    inc rsi
    cmp al, 10
    jne .loop
    
.fin:
    pop rax
    ret

comparar_cadenas:
    ; rdi = cadena1, rsi = cadena2
    ; Retorna 0 si iguales, 1 si diferentes
    push rbx
    
.loop:
    mov al, [rdi]
    mov bl, [rsi]
    cmp al, bl
    jne .diferentes
    test al, al
    jz .iguales
    inc rdi
    inc rsi
    jmp .loop
    
.iguales:
    xor rax, rax
    pop rbx
    ret
    
.diferentes:
    mov rax, 1
    pop rbx
    ret

convertir_idioma_simple:
    ; rdi = cadena numérica
    ; Salida: rax = número
    push rbx
    push rcx
    
    xor rax, rax
.loop:
    mov cl, [rdi]
    test cl, cl
    jz .fin
    cmp cl, '0'
    jl .fin
    cmp cl, '9'
    jg .fin
    sub cl, '0'
    imul rax, 10
    movzx rbx, cl
    add rax, rbx
    inc rdi
    jmp .loop
    
.fin:
    pop rcx
    pop rbx
    ret

copiar_cadena:
    ; rsi = fuente, rdi = destino
    push rax
    
.loop:
    mov al, [rsi]
    mov [rdi], al
    test al, al
    jz .fin
    inc rsi
    inc rdi
    jmp .loop
    
.fin:
    pop rax
    ret
