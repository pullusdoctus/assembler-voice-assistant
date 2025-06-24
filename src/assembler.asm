section .data
    filename db "db.txt", 0
    no_respuesta db "No se encontró respuesta", 0
    delimitador db "|", 0

section .bss
    fd resq 1          ; File descriptor
    buffer resb 4096   ; Buffer para leer el archivo
    pregunta_comp resb 256  ; Buffer para comparar preguntas
    respuesta_temp resb 1024 ; Buffer temporal para respuesta
    idioma_comp resb 32 ; Buffer para comparar idiomas
    pregunta_orig resq 1    ; Guardar puntero original a pregunta
    respuesta_orig resq 1   ; Guardar puntero original a respuesta
    idioma_orig resq 1      ; Guardar idioma original

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
    
    ; Guardar parámetros en variables globales para acceso fácil
    mov [pregunta_orig], rdi
    mov [respuesta_orig], rsi
    mov [idioma_orig], rdx
    
    ; Abrir archivo
    mov rax, 2          ; syscall open
    mov rdi, filename
    xor rsi, rsi        ; O_RDONLY
    xor rdx, rdx
    syscall
    
    cmp rax, 0
    jl .no_respuesta    ; Si hay error, saltar a no_respuesta
    mov [fd], rax       ; Guardar file descriptor

.leer_archivo_completo:
    ; Leer todo el archivo
    mov rax, 0          ; syscall read
    mov rdi, [fd]
    mov rsi, buffer
    mov rdx, 4095       ; Dejar espacio para null terminator
    syscall
    
    cmp rax, 0
    jle .no_respuesta   ; Si hay error o archivo vacío
    
    ; Null-terminar el buffer
    mov byte [buffer + rax], 0
    
    ; Procesar línea por línea
    mov rsi, buffer     ; rsi apunta al inicio del buffer
    
.procesar_linea:
    ; Verificar si llegamos al final
    cmp byte [rsi], 0
    je .no_respuesta
    
    ; Copiar pregunta hasta el primer delimitador
    mov rdi, pregunta_comp
    call copiar_hasta_delimitador
    
    ; rsi ahora apunta después del primer '|'
    ; Comparar preguntas
    mov rdi, pregunta_comp
    mov rsi, [pregunta_orig]
    call comparar_cadenas
    
    ; Restaurar rsi para continuar procesando esta línea
    mov rsi, rbx        ; rbx contiene la posición después del '|'
    
    test rax, rax
    jnz .saltar_linea   ; Si no coincide, saltar a siguiente línea
    
    ; Las preguntas coinciden, copiar respuesta
    mov rdi, respuesta_temp
    call copiar_hasta_delimitador
    
    ; rsi ahora apunta después del segundo '|'
    ; Copiar idioma
    mov rdi, idioma_comp
    call copiar_hasta_nueva_linea
    
    ; Convertir idioma y comparar
    mov rdi, idioma_comp
    call convertir_idioma_simple
    
    cmp rax, [idioma_orig]
    jne .saltar_linea   ; Si no coincide el idioma, continuar buscando
    
    ; ¡Encontramos una coincidencia completa!
    mov rsi, respuesta_temp
    mov rdi, [respuesta_orig]
    call copiar_cadena
    jmp .cerrar_archivo

.saltar_linea:
    ; Avanzar hasta la siguiente línea
    call avanzar_a_siguiente_linea
    jmp .procesar_linea

.no_respuesta:
    mov rsi, no_respuesta
    mov rdi, [respuesta_orig]
    call copiar_cadena

.cerrar_archivo:
    ; Cerrar archivo
    mov rax, 3          ; syscall close
    mov rdi, [fd]
    syscall
    
    ; Restaurar registros
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret

copiar_hasta_delimitador:
    ; Entrada: rsi = fuente, rdi = destino
    ; Salida: rsi = posición después del '|', rbx = posición después del '|'
    push rdi
.loop:
    mov al, [rsi]
    cmp al, '|'
    je .fin
    cmp al, 0
    je .fin
    cmp al, 10          ; nueva línea
    je .fin
    mov [rdi], al
    inc rsi
    inc rdi
    jmp .loop
.fin:
    mov byte [rdi], 0   ; Null-terminar
    cmp al, '|'
    jne .no_incrementar
    inc rsi             ; Saltar el '|'
.no_incrementar:
    mov rbx, rsi        ; Guardar posición actual
    pop rdi
    ret

copiar_hasta_nueva_linea:
    ; Entrada: rsi = fuente, rdi = destino
    ; Salida: rsi = posición después del salto de línea
    push rdi
.loop:
    mov al, [rsi]
    cmp al, 10          ; nueva línea
    je .fin
    cmp al, 13          ; retorno de carro
    je .fin
    cmp al, 0
    je .fin
    mov [rdi], al
    inc rsi
    inc rdi
    jmp .loop
.fin:
    mov byte [rdi], 0   ; Null-terminar
    pop rdi
    ret

avanzar_a_siguiente_linea:
    ; Avanzar rsi hasta el siguiente salto de línea
.loop:
    mov al, [rsi]
    cmp al, 0
    je .fin
    inc rsi
    cmp al, 10          ; nueva línea
    jne .loop
.fin:
    ret

comparar_cadenas:
    ; rdi: cadena 1, rsi: cadena 2
    ; Retorna: 0 si son iguales, 1 si diferentes
.loop:
    mov al, [rdi]
    mov bl, [rsi]
    cmp al, bl
    jne .no_igual
    test al, al
    jz .igual
    inc rdi
    inc rsi
    jmp .loop
.igual:
    xor rax, rax  ; Retornar 0 si son iguales
    ret
.no_igual:
    mov rax, 1    ; Retornar 1 si no son iguales
    ret

convertir_idioma_simple:
    ; Convertir cadena numérica simple a entero
    ; Entrada: rdi = cadena
    ; Salida: rax = entero
    xor rax, rax
    xor rcx, rcx
.loop:
    mov cl, [rdi]
    test cl, cl
    jz .fin
    cmp cl, '0'
    jl .fin
    cmp cl, '9'
    jg .fin
    sub cl, '0'         ; Convertir carácter a dígito
    imul rax, rax, 10   ; Multiplicar resultado por 10
    add rax, rcx        ; Agregar nuevo dígito
    inc rdi
    jmp .loop
.fin:
    ret

copiar_cadena:
    ; Copiar cadena de rsi a rdi
.loop:
    mov al, [rsi]
    mov [rdi], al
    test al, al
    jz .fin
    inc rsi
    inc rdi
    jmp .loop
.fin:
    ret
