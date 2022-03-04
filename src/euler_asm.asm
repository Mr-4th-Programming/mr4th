
.code

; rcx - n
; n*(n + 1)/2
triangle_number PROC
  mov rax, rcx ; rax = n
  add rax, 1   ; rax = n + 1
  mul rcx      ; rax = (n + 1)*n
  shr rax, 1   ; rax = (n + 1)*n/2
  ret
triangle_number ENDP

; rcx - dst, rdx - src
fibonacci_stepper_mul PROC
  ; rax = n1*k1
  mov rax, [rcx]
  imul rax, [rdx]
  
  ; r8 = n2*k2
  mov r8, [rcx + 8]
  imul r8, [rdx + 8]
  
  ; r9 = n1*k2
  mov r9, [rcx]
  imul r9, [rdx + 8]
  
  ; p1 = n1*k1 + n2*k2 = (rax + r8)
  mov [rcx], rax
  add [rcx], r8
  
  ; rax = n2*(k1 + k2)
  mov rax, [rdx]
  add rax, [rdx + 8]
  imul rax, [rcx + 8]
  
  ; p2 = n1*k2 + n2*(k1 + k2) = (r9 + rax)
  mov [rcx + 8], r9
  add [rcx + 8], rax
  
  ret
fibonacci_stepper_mul ENDP

; rcx - dst
fibonacci_stepper_sqr PROC
  mov rax, [rcx]    ; rax = n1
  mov r8, [rcx + 8] ; r8  = n2
  mov r9, rax
  imul r9, r8       ; r9  = n1*n2
  imul rax, rax     ; rax = n1*n1
  imul r8, r8       ; r8  = n2*n2
  
  ; p1 = n1*n1 + n2*n2
  add rax, r8       ; rax = n1*n1 + n2*n2
  mov [rcx], rax
  
  ; p2 = 2*n1*n2 + n2*n2
  lea rax, [r8 + r9*2]       ; rax = 2*n1*n2 + n2*n2
  mov [rcx + 8], rax
  ret  
fibonacci_stepper_sqr ENDP

; rcx - n
fibonacci_number PROC
  push rsi
  sub rsp, 32
  mov qword ptr [rsp],      1 ; a = [1, 0]
  mov qword ptr [rsp + 8],  0
  mov qword ptr [rsp + 16], 0 ; s = [0, 1]
  mov qword ptr [rsp + 24], 1
  
  ; relocate n
  mov rsi, rcx
  
  ; if n == 0 goto done;
  test rsi, rsi
  jz done
  
loop0:
  ; if (n&1) == 0 goto skip_accumulator;
  test rsi, 1
  jz skip_accumulator
  
  ; mul(a,s)
  mov rcx, rsp
  lea rdx, [rsp + 16]
  call fibonacci_stepper_mul
  
skip_accumulator:
  ; n = n >> 1
  shr rsi, 1
  
  ; if n == 0 goto done;
  jz done
  
  ; sqr(s)
  lea rcx, [rsp + 16]
  call fibonacci_stepper_sqr
  
  ; goto loop0;
  jmp loop0
  
done:
  mov rax, [rsp + 8]
  add rsp, 32
  pop rsi
  ret
fibonacci_number ENDP

; rcx - n
; fib(n + 2) - 1
fibonacci_sigma PROC
  add rcx, 2
  call fibonacci_number
  sub rax, 1
  ret
fibonacci_sigma ENDP


; rcx - table_memory
; rdx - primes_list
; r8d - max_value
;  assumptions:
;   r8d > 2
;   rcx points to r8d available bytes, cleared to zero
;   rdx points to 8*r8d available bytes
prime_sieve__asm PROC
  ; r8 = (r8d - 3)/2
  sub r8, 3
  shr r8, 1
  
  ; emit 2
  mov dword ptr [rdx],2
  mov rax,1
  
  ; r9=0
  xor r9,r9
  
loop0:
  
  ; r10 = table_memory[r9]
  movzx r10, byte ptr [rcx + r9]
  
  ; if (r10 != 0) goto next;
  test r10,r10
  jnz next
  
  ; r10 = 3 + r9*2
  lea r10, [3 + r9*2]
  
  ; emit r10
  mov dword ptr [rdx + rax*4],r10d
  inc rax
  
  ; r11 = (r10*r10 - 3)/2
  mov r11, r10
  imul r11, r11
  sub r11, 3
  shr r11, 1
  
  ; if (r11 > r8) goto next;
  cmp r11,r8
  jg next
  
loop1:
  ; table_memory[r11] = 1
  mov byte ptr [rcx + r11], 1
  
  ; r11 += r10
  add r11, r10
  ; if (r11 <= r8) goto loop1;
  cmp r11,r8
  jle loop1
  
next:
  ; r9 += 1
  inc r9
  ; if (r9 <= r8) goto loop0;
  cmp r9,r8
  jle loop0
  
  ret
prime_sieve__asm ENDP

END
