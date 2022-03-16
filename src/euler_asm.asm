
; TODO: above/below for unsigned integers

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
; rdx - primes_list (TODO: edx?)
; r8d - first       (TODO: r8d?)
; r9d - opl         (TODO: r9d?)
; [rsp + 0x28] - node
prime_sieve__asm PROC
  push r15
  push r14
  push r13
  push r12
  sub rsp,16
  
  ; save primes list at [rsp]
  mov [rsp],rdx
  
  ; save counter at [rsp + 1]
  mov qword ptr [rsp+8],0
  
  ; move node to r15
  mov r15, [rsp + 58h]
  
  ; if (node != 0) goto skip_emit_2;
  test r15,r15
  jnz skip_emit_2;
  
  ; emit 2
  mov dword ptr [rdx],2
  mov qword ptr [rsp+8],1
skip_emit_2:
  
  ; M' = (first even)?(first + 1):(first);
  or r8d,1
  
  ; move opl into r12
  mov r12,r9
  
  ; ibound = (opl - M' + 1)/2
  sub r9,r8
  inc r9
  shr r9,1
  
  ; table_memory_bound = table_memory + ibound
  mov r13,rcx
  add r13,r9
  
mixing0:
  ; if (node == 0) goto done_mixing;
  test r15,r15
  jz done_mixing
  
  ; r14 = node->array.v
  mov r14, [r15 + 8]
  ; r10 = node->array.v + node->array.count
  mov r10, [r15 + 16]
  lea r10, [r14 + r10*4]
  
mixing1:
  ; if (r14 >= r10) goto mixing0_next;
  cmp r14,r10
  jge mixing0_next
  
  ; p = *r14
  mov r11d, [r14]
  
  ; if (p == 2) goto mixing1_next;
  cmp r11,2
  je mixing1_next;
  
  ; if (p*p >= opl) goto done_mixing;
  mov rax,r11
  imul rax,rax
  cmp rax,r12
  jge done_mixing
  
  ; k = floor((M' + p - 1)/p)
  mov rax,r8
  add rax,r11
  dec rax
  xor rdx,rdx
  idiv r11
  
  ; if (k < p) k = p;
  cmp rax,r11
  cmovl rax,r11
  
  ; if (k even) k += 1;
  or rax,1
  
  ; x = k*p
  imul rax,r11
  
  ; j = (x - M')/2
  sub rax,r8
  shr rax,1
  
  ; ptr = table_memory + j
  add rax, rcx
  
mixing2:
  ; if (ptr >= table_memory_bound) goto mixing1_next;
  cmp rax,r13
  jge mixing1_next;
  
  ; mark x in the table
  mov byte ptr [rax],1
  
  ; j += p, goto mixing2
  add rax,r11
  jmp mixing2
  
mixing1_next:
  ; increment r14
  add r14,4
  jmp mixing1
  
mixing0_next:
  ; node = node->next
  mov r15,[r15]
  jmp mixing0
  
done_mixing:
  
  ; retrieve base of primes_list
  mov rdx,[rsp]
  
  ; retrieve counter
  mov rax,[rsp+8]
  
  ; ptr = table_memory
  mov r12, rcx
  
scanning0:
  ; mark = *ptr
  movzx r15, byte ptr [r12]
  
  ; if (mark != 0) goto scanning0_next;
  test r15,r15
  jnz scanning0_next;
  
  ; j = ptr - table_memory
  mov r14, r12
  sub r14, rcx
  
  ; p = M' + 2*j
  lea r14, [r8 + r14*2]
  
  ; emit p
  mov dword ptr [rdx + rax*4], r14d
  inc rax
  
  ; j' = (p*p - M')/2
  mov  r10,r14
  imul r10,r10
  sub  r10,r8
  shr  r10,1
  
  ; ptr' = table_memory + j'
  add r10, rcx
  
scanning1:
  ; if (ptr' >= table_memory_bound) goto scanning0_next;
  cmp r10,r13
  jge scanning0_next;
  
  ; *(ptr') = 1
  mov byte ptr [r10], 1
  
  ; ptr' += p
  add r10,r14
  jmp scanning1
  
scanning0_next:
  ; ptr += 1
  inc r12
  
  ; if (ptr < table_memory_bound) goto scanning0;
  cmp r12,r13
  jl scanning0
  
  add rsp,16
  pop r12
  pop r13
  pop r14
  pop r15
  ret
prime_sieve__asm ENDP



; rcx - table_memory
; rdx - count (TODO: edx?)
;  assumptions:
;   rcx is a pointer to 4*rdx bytes, cleared to zero
factorization_table__asm PROC
  ; i = 0
  xor rax,rax
  
loop0:
  ; if (i >= count) goto done;
  cmp rax,rdx
  jge done
  
  ; f = table_memory[i]
  mov r9d, dword ptr [rcx + rax*4]
  
  ; if (f != 0) goto loop0_next
  test r9,r9
  jnz loop0_next
  
  ; n = 3 + 2*i
  lea r8, [3 + rax*2]
  
  ; table_memory[i] = n
  mov dword ptr [rcx + rax*4], r8d
  
  ; r9 = n*n
  mov r9,r8
  imul r9,r9
  
  ; j = (n*n - 3)/2
  sub r9,3
  shr r9,1
  
loop1:
  ; if (j >= count) goto loop0_next;
  cmp r9,rdx
  jge loop0_next
  
  ; table_memory[j] = n
  mov dword ptr [rcx + r9*4], r8d
  
  ; increment
  add r9,r8
  jmp loop1
  
loop0_next:
  ; increment
  inc rax
  jmp loop0

done:
  ret
factorization_table__asm ENDP


; rcx - fact_tbl
; rdx - count (TODO: edx?)
; r8d - n     (TODO: r8d?)
get_prime_factor PROC
  ; if (n even) goto return_2;
  test r8d,1
  jz return_2
  
  ; if (n == 1) goto return_1;
  cmp r8d,1
  je return_1
  
  ; j = (n - 3)/2
  sub r8d,3
  shr r8d,1
  
  ; if (j >= count) goto return_0;
  cmp r8d,edx
  jge return_0
  
  ; f = fact_tbl[j]
  mov eax, dword ptr [rcx + r8*4]
  ret
  
return_2:
  mov rax,2
  ret
  
return_1:
  mov rax,1
  ret
  
return_0:
  xor rax,rax
  ret
get_prime_factor ENDP


; ecx - n (TODO: edx)
; edx - f (TODO: r8d)
max_divide PROC
  
  ; move f into r8
  mov r8d,edx
  ; move n into r10
  mov r10d,ecx
  ; r = 0
  xor r9,r9
  
loop0:
  ; compute n/f and n%f
  mov rax,r10
  xor rdx,rdx
  idiv r8
  
  ; if ((remainder) != 0) goto done;
  test rdx,rdx
  jnz done
  
  ; r += 1
  inc r9
  
  ; n = (quotient)
  mov r10d,eax
  
  jmp loop0
  
done:
  mov rax,r10
  shl rax,32
  or  rax,r9
  ret
max_divide ENDP


; rcx - fact_tbl
; edx - count
; r8d - n
divisor_count PROC
  push rsi
  push rdi
  push r12
  push r13
  
  ; move fact_tbl to rsi
  mov rsi, rcx
  
  ; move count to rdi
  mov edi, edx
  
  ; move n to r12
  mov r12d, r8d
  
  ; accum = 1
  mov r13d,1
  
loop0:
  ; if (n <= 1) goto done;
  cmp r12d,1
  jbe done
  
  ; call get_prime_factor(fact_tbl, count, n)
  mov rcx, rsi
  mov edx, edi
  mov r8d, r12d
  call get_prime_factor
  
  ; f is in rax
  
  ; call max_divide(n, f)
  mov ecx, r12d
  mov edx, eax
  call max_divide
  
  ; {r,n'} are in rax
  
  ; accum *= (r + 1)
  mov ecx,eax
  inc ecx
  imul r13d,ecx
  
  ; n = n'
  shr rax,32
  mov r12d,eax
  
  jmp loop0
  
done:
  
  mov rax,r13

  pop r13
  pop r12
  pop rdi
  pop rsi
  ret
divisor_count ENDP



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


; rcx - return pointer (pointer to EulerData)
; rdx - text (pointer to String8)
; r8  - out (ptr)
euler_data_from_text__asm PROC
  sub rsp,8
  
  ; store og return pointer for later
  mov [rsp],rcx
  
  ; result.v = out
  mov [rcx],r8
  
  ; ptr = text.str
  mov rax,[rdx]
  
  ; opl = ptr + text.size
  mov rdx,[rdx + 8]
  lea rdx,[rax + rdx]
  
  ; line_count = 0
  xor r10,r10
  
loop0:
  ; if (ptr >= opl) goto done0;
  cmp rax,rdx
  jae done0
  
  ; c = *ptr
  movzx rcx, byte ptr [rax]
  
  ; if (c == '\n') goto inc_line_counter;
  cmp rcx,0ah
  je inc_line_counter
  
  ; d = c - '0'
  sub rcx,'0'
  
  ; if (d <= 9) goto process_digit;
  cmp rcx,9
  jbe process_digit
  
  jmp loop0_next 
  
inc_line_counter:
  ; line_count += 1
  inc r10
  jmp loop0_next
  
process_digit:
  ; *out_ptr = d, out_ptr += 1
  mov [r8],rcx
  inc r8
  
loop0_next:
  ; increment (ptr += 1, goto loop0)
  inc rax
  jmp loop0
  
done0:
  
  mov rax,[rsp]
  
  ; result.count = out_ptr - out;
  mov rdx,[rax]
  sub r8,rdx
  mov [rax + 8],r8
  
  ; result.line_count = line_count;
  mov [rax + 16],r10
  
  add rsp,8
  ret
euler_data_from_text__asm ENDP


; rcx - return pointer (pointer to EulerData)
; rdx - text (pointer to String8)
; r8  - out (ptr)
euler_data_from_text_2dig__asm PROC
  sub rsp,8
  
  ; store og return pointer for later
  mov [rsp],rcx
  
  ; result.v = out
  mov [rcx],r8
  
  ; ptr = text.str
  mov rax,[rdx]
  
  ; opl = ptr + text.size
  mov rdx,[rdx + 8]
  lea rdx,[rax + rdx]
  
  ; line_count = 0
  xor r10,r10
  
  ; dig_count = 0
  xor r9,r9
  
  ; accum = 0
  xor r11,r11
  
loop0:
  ; if (ptr >= opl) goto done0;
  cmp rax,rdx
  jae done0
  
  ; c = *ptr
  movzx rcx, byte ptr [rax]
  
  ; if (c == '\n') goto inc_line_counter;
  cmp rcx,0ah
  je inc_line_counter
  
  ; d = c - '0'
  sub rcx,'0'
  
  ; if (d <= 9) goto process_digit;
  cmp rcx,9
  jbe process_digit
  
  jmp loop0_next 
  
inc_line_counter:
  ; line_count += 1
  inc r10
  jmp loop0_next
  
process_digit:
  ; accum *= 10
  imul r11,10
  
  ; accum += d
  add r11,rcx
  
  ; dig_count += 1
  inc r9
  
  ; if (dig_count != 2) goto loop0_next
  cmp r9,2
  jne loop0_next
  
  ; *out_ptr = accum, out_ptr += 1
  mov [r8],r11
  inc r8
  
  ; accum = 0, dig_count = 0
  xor r11,r11
  xor r9,r9
  
loop0_next:
  ; increment (ptr += 1, goto loop0)
  inc rax
  jmp loop0
  
done0:
  
  mov rax,[rsp]
  
  ; result.count = out_ptr - out;
  mov rdx,[rax]
  sub r8,rdx
  mov [rax + 8],r8
  
  ; result.line_count = line_count;
  mov [rax + 16],r10
  
  add rsp,8
  ret
euler_data_from_text_2dig__asm ENDP


END
