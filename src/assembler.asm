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

section .text
global buscar_respuesta

buscar_respuesta:
    push rbp
    mov rbp, rsp
    push rdi  ; Guardar puntero a la pregunta
    push rsi  ; Guardar puntero al buffer de respuesta
    push rdx  ; Guardar idioma

    ; Abrir archivo
    mov rax, 2          ; syscall open
    mov rdi, filename
    xor rsi, rsi        ; O_RDONLY
    xor rdx, rdx
    syscall
    
    cmp rax, 0
    jl .no_respuesta    ; Si hay error, saltar a no_respuesta

    mov [fd], rax       ; Guardar file descriptor

.leer_linea:
    ; Leer una línea del archivo
    mov rax, 0          ; syscall read
    mov rdi, [fd]
    mov rsi, buffer
    mov rdx, 4096
    syscall

    cmp rax, 0
    jle .no_respuesta    ; Si hay error o fin de archivo

    ; Procesar la línea
    mov rcx, rax        ; rax contiene bytes leídos
    mov rsi, buffer
    mov rdi, pregunta_comp
    
.copiar_pregunta:
    mov al, [rsi]
    cmp al, '|'
    je .fin_pregunta
    mov [rdi], al
    inc rsi
    inc rdi
    loop .copiar_pregunta

.fin_pregunta:
    mov byte [rdi], 0   ; Null-terminar la pregunta
    inc rsi             ; Saltar el delimitador

    ; Comparar preguntas
    mov rdi, pregunta_comp
    mov rsi, [rbp - 8]  ; Recuperar puntero a la pregunta original
    call comparar_cadenas
    test rax, rax
    jnz .leer_linea     ; Si no coincide, leer siguiente línea


    ; Copiar respuesta a buffer temporal
    mov rdi, respuesta_temp
.copiar_respuesta:
    mov al, [rsi]
    cmp al, '|'
    je .fin_respuesta
    mov [rdi], al
    inc rsi
    inc rdi
    jmp .copiar_respuesta

.fin_respuesta:
    mov byte [rdi], 0   ; Null-terminar la respuesta
    inc rsi             ; Saltar el delimitador

    ; Comparar idioma
    mov rdi, idioma_comp
.copiar_idioma:
    mov al, [rsi]
    cmp al, 10          ; Salto de línea
    je .fin_idioma
    mov [rdi], al
    inc rsi
    inc rdi
    jmp .copiar_idioma

.fin_idioma:
    mov byte [rdi], 0   ; Null-terminar el idioma

    ; Convertir idioma_comp a entero y comparar con edx
    call convertir_idioma
    cmp eax, edx
    jne .leer_linea     ; Si no coincide, leer siguiente línea

    ; Si llegamos aquí, hemos encontrado una coincidencia
    mov rsi, respuesta_temp
    mov rdi, [rbp + 16] ; Puntero al buffer de respuesta
    call copiar_cadena

    jmp .cerrar_archivo

.no_respuesta:
    mov rsi, no_respuesta
    mov rdi, [rbp + 16] ; Puntero al buffer de respuesta
    call copiar_cadena


.cerrar_archivo:
    mov rax, 3          ; syscall close
    mov rdi, [fd]
    syscall

    pop rdx
    pop rsi
    pop rdi
    pop rbp
    ret

comparar_cadenas:
    ; rdi: cadena 1, rsi: cadena 2
    xor rcx, rcx
.loop:
    mov al, [rdi + rcx]
    cmp al, [rsi + rcx]
    jne .no_igual
    test al, al
    jz .igual
    inc rcx
    jmp .loop
.igual:
    xor rax, rax  ; Devolver 0 si son iguales
    ret
.no_igual:
    mov rax, 1    ; Devolver 1 si no son iguales
    ret

convertir_idioma:
    ; Convertir la cadena de idioma en idioma_comp a un entero
    xor eax, eax
    mov rcx, 3  ; Asumimos que el idioma tiene 3 caracteres (por ejemplo, "ESP", "ENG")
.loop:
    shl eax, 8
    mov al, [rdi]
    inc rdi
    loop .loop
    ret

copiar_cadena:
    ; Copiar la cadena de rsi a rdi
.loop:
    mov al, [rsi]
    mov [rdi], al
    inc rsi
    inc rdi
    test al, al
    jnz .loop
    ret