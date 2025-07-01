section .data
    min_font_size: dd 8          ; Minimum font size constant
    font_increment: dd 2         ; Font size increment/decrement value

section .text
    global increase_font_size
    global decrease_font_size
    global toggle_bold

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
toggle_bold:
  xor byte[rdi], 1                ; boldTextEnabled = !boldTextEnabled
  ret
