section .data
    min_font_size: dd 8          ; Minimum font size constant
    font_increment: dd 2         ; Font size increment/decrement value

section .text
    global increase_font_size
    global decrease_font_size
    global toggle
    global _invert_color

; int increase_font_size(int* currentFontSize)
; Returns the new font size
increase_font_size:
  mov eax, [rdi]                  ; eax = currentFontSize
  mov ecx, [rel font_increment]   ; ecx = font_increment
  add eax, ecx                    ; eax += ecx
  mov [rdi], eax                  ; currentFontSize = eax
  ret

; int decrease_font_size(int* currentFontSize)
; Returns the new font size
decrease_font_size:
  mov eax, [rdi]                  ; eax = currentFontSize
  mov ecx, [rel font_increment]   ; ecx = font_increment
  sub eax, ecx                    ; eax -= ecx (calculate new size)
  mov edx, [rel min_font_size]    ; edx = min_font_size
  cmp eax, edx                    ; if new_size < min_font_size
  jl .no_decrease                 ; don't decrease
  mov [rdi], eax                  ; currentFontSize = eax
  ret
.no_decrease:
  mov eax, [rdi]                  ; eax = currentFontSize
  ret

; void toggle_bold(uint8_t* bold_flag)
toggle:
  xor byte[rdi], 1                ; boldTextEnabled = !boldTextEnabled
  ret

; void invert_color(const GdkRGBA* input, GdkRGBA* output)
; rdi = input
; rsi = output
_invert_color:
  ; Load 1.0 into xmm0
  mov rax, 0x3FF0000000000000   ; 1.0 in double IEEE754
  movq xmm0, rax
  ; red: xmm1 = [RDI] input.red
  movsd xmm1, qword [rdi]        ; load input->red
  subsd xmm2, xmm2               ; clear xmm2 (to be safe)
  subsd xmm2, xmm2               ; clear xmm2 again (just safe)
  movapd xmm2, xmm0              ; xmm2 = 1.0
  subsd xmm2, xmm1               ; xmm2 = 1.0 - red
  movsd qword [rsi], xmm2       ; output->red = xmm2
  ; green: load input->green ([rdi+8])
  movsd xmm1, qword [rdi+8]
  movapd xmm2, xmm0              ; xmm2 = 1.0
  subsd xmm2, xmm1               ; xmm2 = 1.0 - green
  movsd qword [rsi+8], xmm2     ; output->green
  ; blue: load input->blue ([rdi+16])
  movsd xmm1, qword [rdi+16]
  movapd xmm2, xmm0              ; xmm2 = 1.0
  subsd xmm2, xmm1               ; xmm2 = 1.0 - blue
  movsd qword [rsi+16], xmm2    ; output->blue
  ; alpha: copy input->alpha ([rdi+24]) to output->alpha ([rsi+24])
  movsd xmm1, qword [rdi+24]
  movsd qword [rsi+24], xmm1
  ret
