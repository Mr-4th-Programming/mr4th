
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

; rcx - n
; n*(2n + 1)*(n + 1)/6
sum_of_squares PROC
  ; rdx = 2n + 1
  lea rdx, [1 + rcx*2]
  
  ; r8 = n + 1
  lea r8, [1 + rcx]
  
  ; rax = n*(2n + 1)*(n + 1)
  mov rax, rcx
  imul rax, rdx
  imul rax, r8
  
  ; rax = n*(2n + 1)*(n + 1)/6
  xor rdx,rdx
  mov rcx,6
  idiv rcx
  
  ret
sum_of_squares ENDP


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

; rcx - return pointer
; rdx - min
; r8  - max
; r9  - target_product
;  assumptions:
;   rdx < r8
;   rdx*rdx <= target_product <= max*max
find_bounded_factors PROC
  ; clear result to zero
  mov qword ptr [rcx], 0
  mov qword ptr [rcx+8], 0
  
  ; use r11 as the min
  mov r11, rdx
  
  ; r10 = r11*r8
  mov r10, r11
  imul r10, r8
  
  ; if (r10 == r9) goto return_min_max;
  ; if (r10 < r9)  goto too_small;
  ; goto too_big
  cmp r10, r9
  jz return_min_max
  jl too_small
  jmp too_big
  
too_small:
  ; increase r11 so that:
  ;  r11*r8 >= r9 & (r11 - 1)*r8 < r9
  ;  r11 >= r9/r8 & r11 < r9/r8 + 1
  ;  r9/r8 <= r11 < r9/r8 + 1
  ;  r11 <- ceil(r9/r8)
  
  ; rax = r9, rdx = 0
  mov rax, r9
  xor rdx,rdx
  
  ; rax = floor((0:r9)/r8), rdx = (0:r9)%r8
  idiv r8
  
  ; if ((0:r9)%r8 == 0) goto too_small0;
  ; goto too_small1;
  test rdx,rdx
  jz too_small0
  
too_small1:
  ; r11 = floor((0:r9)/r8)
  mov r11, rax
  inc r11
  
  ; if (r11 > r8) goto return;
  cmp r11, r8
  jg return
  jmp too_big
  
too_small0:
  ; r11 = floor((0:r9)/r8)
  mov r11, rax
  
  ; if (r11 > r8) goto return;
  cmp r11, r8
  jg return
  jmp return_min_max
  
too_big:
  ; decrease r8 so that:
  ;  r11*r8 <= r9 & r11*(r8 + 1) > r9
  ;  r8 <= r9/r11 & r8 > (r9/r11) - 1
  ;  (r9/r11) - 1 < r8 <= r9/r11
  ;  r8 <- floor(r9/r11)
  
  ; rax = r9, rdx = 0
  mov rax, r9
  xor rdx,rdx
  
  ; r8 = floor((rdx:rax)/r11)
  idiv r11
  mov r8, rax
  
  ; if (r11 > r8) goto return;
  cmp r11, r8
  jg return
  
  ; if ((0:r9) == 0) goto return_min_max;
  ; goto too_small;
  test rdx, rdx
  jz return_min_max
  jmp too_small
  
return_min_max:
  mov [rcx], r11
  mov [rcx+8], r8
  
return:
  mov rax, rcx
  ret
find_bounded_factors ENDP


; rcx - A
; rdx - B
gcd_euclidean PROC
  ; transfer B into r8
  mov r8, rdx
  
  ; if (A <= B) goto loop0;
  cmp rcx,r8
  jle loop0
  ; swap
  mov r8, rcx
  mov rcx, rdx
  
loop0:
  ; if (A == 0) goto return
  test rcx,rcx
  jz return
  
  ; euclidean_step
  mov rax, r8
  xor rdx, rdx
  idiv rcx
  mov r8, rcx
  mov rcx, rdx
  
  jmp loop0
  
return:
  mov rax, r8
  ret
gcd_euclidean ENDP


; rcx - A
; rdx - B
lcm_euclidean PROC
  push rsi
  
  ; rsi = A*B
  mov rsi,rcx
  imul rsi,rdx
  
  ; call gcd(...)
  call gcd_euclidean
  
  ; rcx = gcd(...)
  mov rcx,rax
  
  ; rax = A*B/gcd(...)
  mov rax, rsi
  xor rdx, rdx
  idiv rcx
  
  pop rsi
  ret
lcm_euclidean ENDP


END
