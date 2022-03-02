
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
  
  ; r8 = n1*k2
  mov r8, [rcx]
  imul r8, [rdx + 8]
  
  ; r9 = n2*k2
  mov r9, [rcx + 8]
  imul r9, [rdx + 8]
  
  ; p1 = n1*k1 + n2*k2 = (rax + r9)
  mov [rcx], rax
  add [rcx], r9
  
  ; rax = n3*k3
  mov rax, [rcx + 16]
  imul rax, [rdx + 16]
  
  ; p3 = n2*k2 + n3*k3 = (r9 + rax)
  mov [rcx + 16], r9
  add [rcx + 16], rax
  
  ; rax = n2*k3
  mov rax, [rcx + 8]
  imul rax, [rdx + 16]
  
  ; p2 = n1*k2 + n2*k3 = (r8 + rax)
  mov [rcx + 8], r8
  add [rcx + 8], rax
  
  ret
fibonacci_stepper_mul ENDP

; rcx - dst
fibonacci_stepper_sqr PROC
  ; rax = n1 + n3
  mov rax, [rcx]
  add rax, [rcx + 16]
  
  ; rax = n2*(n1 + n3), rdx = n2*n2
  mov rdx, [rcx + 8]
  imul rax, rdx
  imul rdx, rdx
  
  ; p2 = n2*(n1 + n3) = rax
  mov [rcx + 8], rax
  
  ; p1 = n1*n1 + n2*n2
  mov rax, [rcx]
  imul rax, rax
  add rax, rdx
  mov [rcx], rax
  
  ; p3 = n3*n3 + n2*n2
  mov rax, [rcx + 16]
  imul rax, rax
  add rax, rdx
  mov [rcx + 16], rax
  
  ret  
fibonacci_stepper_sqr ENDP

; rcx - n
fibonacci_number PROC
  push rsi
  sub rsp, 48
  mov qword ptr [rsp],      1 ; a = [1, 0, 1]
  mov qword ptr [rsp + 8],  0
  mov qword ptr [rsp + 16], 1
  mov qword ptr [rsp + 24], 0 ; s = [0, 1, 1]
  mov qword ptr [rsp + 32], 1
  mov qword ptr [rsp + 40], 1
  
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
  lea rdx, [rsp + 24]
  call fibonacci_stepper_mul
  
skip_accumulator:
  ; n = n >> 1
  shr rsi, 1
  
  ; if n == 0 goto done;
  jz done
  
  ; sqr(s)
  lea rcx, [rsp + 24]
  call fibonacci_stepper_sqr
  
  ; goto loop0;
  jmp loop0
  
done:
  mov rax, [rsp + 8]
  add rsp, 48
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

euler2 PROC
  ret
euler2 ENDP

; 3*TRI(999/3) + 5*TRI(999/5) - 15*TRI(999/15)
euler1 PROC
  sub rsp, 8
  ; 3 part
  mov rcx, 333        ; rcx = floor(999/3)
  call triangle_number; rax = tri(999/3)
  imul rax, 3         ; rax = 3*tri(999/3)
  mov [rsp], rax      ; [rsp] = 3*tri(999/3)
  ; 5 part
  mov rcx, 199        ; rcx = floor(999/5)
  call triangle_number; rax = tri(999/5)
  imul rax, 5         ; rax = 5*tri(999/5)
  add [rsp], rax      ; [rsp] = 3*tri(999/3) + 5*tri(999/5)
  ; 15 part
  mov rcx, 66         ; rcx = floor(999/15)
  call triangle_number; rax = tri(999/15)
  imul rax, 15        ; rax = 15*tri(999/15)
  sub [rsp], rax      ; [rsp] = 3*tri(999/3) + 5*tri(999/5) - 15*tri(999/15)
  mov rax, [rsp]
  add rsp, 8
  ret
euler1 ENDP


END
