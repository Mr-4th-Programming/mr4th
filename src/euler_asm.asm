
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
fibonacci_number PROC FRAME
  mov qword ptr [rsp + 8],rsi
.savereg rsi, 8
  sub rsp, 64
.allocstack 64
.endprolog
  
  mov qword ptr [rsp + 32], 1 ; a = [1, 0]
  mov qword ptr [rsp + 40], 0
  mov qword ptr [rsp + 48], 0 ; s = [0, 1]
  mov qword ptr [rsp + 56], 1
  
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
  lea rcx, [rsp + 32]
  lea rdx, [rsp + 48]
  call fibonacci_stepper_mul
  
skip_accumulator:
  ; n = n >> 1
  shr rsi, 1
  
  ; if n == 0 goto done;
  jz done
  
  ; sqr(s)
  lea rcx, [rsp + 48]
  call fibonacci_stepper_sqr
  
  ; goto loop0;
  jmp loop0
  
done:
  mov rax, [rsp + 40]
  add rsp, 64
  mov rsi, qword ptr [rsp + 8]
  ret
fibonacci_number ENDP

; rcx - n
; fib(n + 2) - 1
fibonacci_sigma PROC FRAME
  sub rsp,32
.allocstack 32
.endprolog
  
  add rcx, 2
  call fibonacci_number
  dec rax
  
  add rsp,32
  ret
fibonacci_sigma ENDP

; rcx - buffer (U64*)
; rdx - n (U64)
; r8  - m (U64)
;  assumptions:
;   buffer points to at least n + 1 U64s worth of free memory
;   m <= n
choose__asm PROC
  ; m' = n - m
  mov r9,rdx
  sub r9,r8
  
  ; r = min(m, m'), c = max(m, m')
  ;  r8 contains, r9 contains c
  mov rax,r8
  cmp r8,r9
  cmova r8,r9
  cmova r9,rax
  
  ; if (r == 0) goto return_1;
  test r8,r8
  jz return_1
  
  ; if (r == 1) goto return_n;
  cmp r8,1
  je return_n
  
  ; if (r & 1) goto r_odd_case;
  test r8,1
  jnz r_odd_case
  
  
  ;; EVEN CASE ;;
  ; i = 2
  mov rax,2
  
  ; k = 6
  mov r11,6
  
loop0_even:
  ; buffer[i] = k
  mov qword ptr [rcx + rax*8],r11
  ; i += 1
  inc rax
  
  ; k += i + 1
  inc r11
  add r11,rax
  
  ; if (i <= c) goto loop0_even;
  cmp rax,r9
  jbe loop0_even
  
  ; row = 4
  mov rax,4
  
  ; goto loop1
  jmp loop1
  
  
  ;; ODD CASE ;;
r_odd_case:
  ; i = 3
  mov rax,3
  
  ; minor_slot = 10
  mov rdx,10
  
  ; k = 20
  mov r11,20
  
loop0_odd:
  ; buffer[i] = k
  mov qword ptr [rcx + rax*8],r11
  ; i += 1
  inc rax
  
  ; minor_slot += i + 1
  inc rdx
  add rdx,rax
  
  ; k += minor_slot
  add r11,rdx
  
  ; if (i <= c) goto loop0_odd;
  cmp rax,r9
  jbe loop0_odd
  
  ; row = 5
  mov rax,5
  
loop1:
  ; if (row > r) goto return_buffer_c
  cmp rax,r8
  ja return_buffer_c
  
  ; col = row
  mov r10,rax
  
  ;; initialize the loop
  mov r11,qword ptr [rcx + r10*8 - 8]
  shl r11,1
  add r11,qword ptr [rcx + r10*8]
  mov rdx,r11
  shl rdx,1
  
  ; buffer[col] = major_slot
  mov qword ptr [rcx + r10*8],rdx
  
loop2:
  ; col += 1
  inc r10
  
  ; if (col > c) goto loop2_done;
  cmp r10,r9
  ja loop2_done
  
  ; minor_slot += buffer[col]
  add r11,qword ptr [rcx + r10*8]
  ; major_slot += minor_slot
  add rdx,r11
  ; buffer[col] = major_slot
  mov qword ptr [rcx + r10*8],rdx
  
  ; goto loop2
  jmp loop2
  
loop2_done:
  
  ; row += 2
  add rax,2
  
  ; goto loop1
  jmp loop1
  
return_buffer_c:
  mov rax,qword ptr [rcx + r9*8]
  ret
  
return_1:
  mov rax,1
  ret
  
return_n:
  mov rax,rdx
  ret
choose__asm ENDP


; rcx - table (U64*)
; rdx - count (U64)
fill_factorial_table PROC
  ; i = 0
  xor rax,rax
  ; n = 1
  mov r8,1
  
  ; 
loop0:
  ; if (i >= count) goto done
  cmp rax,rdx
  jae done
  
  ; table[i] = n
  mov [rcx + rax*8],r8
  ; i += 1
  ; n *= i + 1
  add rax,2
  imul r8,rax
  dec rax
  
  ; goto loop0
  jmp loop0
  
done:
  ret
fill_factorial_table ENDP


; rcx          - table_memory (B8*)
; rdx          - primes_list  (U32*)
; r8d          - first        (U32)
; r9d          - opl          (U32)
; [rsp + 0x28] - node         (ListNode_U32*)
prime_sieve__asm PROC FRAME
  push r15
.pushreg r15
  push r14
.pushreg r14
  push r13
.pushreg r13
  push r12
.pushreg r12
  sub rsp,16
.allocstack 16
.endprolog
  
  ; extend first and opl to 64-bits
  mov r8d,r8d
  mov r9d,r9d
  
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
  
  ; ptr = node->array.v
  mov r14, [r15 + 8]
  ; ptr_opl = node->array.v + node->array.count
  mov r10, [r15 + 16]
  lea r10, [r14 + r10*4]
  
mixing1:
  ; if (ptr >= ptr_opl) goto mixing0_next;
  cmp r14,r10
  jae mixing0_next
  
  ; p = *ptr
  mov r11d, dword ptr [r14]
  
  ; if (p == 2) goto mixing1_next;
  cmp r11,2
  je mixing1_next;
  
  ; if (p*p >= opl) goto done_mixing;
  mov rax,r11
  imul rax,rax
  cmp rax,r12
  jae done_mixing
  
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
  jae mixing1_next;
  
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
  jae scanning0_next;
  
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
  jb scanning0
  
  add rsp,16
  pop r12
  pop r13
  pop r14
  pop r15
  ret
prime_sieve__asm ENDP


; rcx - table_memory (U32*)
; rdx - count        (U32)
;  assumptions:
;   rcx is a pointer to 4*edx bytes, cleared to zero
factorization_table__asm PROC
  ; zero extend count to 64-bit
  mov edx,edx
  
  ; i = 0
  xor rax,rax
  
loop0:
  ; if (i >= count) goto done;
  cmp rax,rdx
  jae done
  
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
  jae loop0_next
  
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


; rcx - fact_tbl (U32*)
; edx - count    (U32)
; r8d - n        (U32)
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
  jae return_0
  
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


; ecx - n
; edx - f
max_divide PROC
  
  ; move f into r8
  mov r8d,edx
  ; r = 0
  xor r9d,r9d
  
loop0:
  ; compute n/f and n%f
  xor edx,edx
  mov eax,ecx
  idiv r8d
  
  ; if ((remainder) != 0) goto done;
  test edx,edx
  jnz done
  
  ; r += 1
  inc r9d
  
  ; n = (quotient)
  mov ecx,eax
  
  jmp loop0
  
done:
  mov eax,ecx
  shl rax,32
  or  rax,r9
  ret
max_divide ENDP


; rcx - fact_tbl (U32*)
; edx - count    (U32)
; r8d - n        (U32)
divisor_count PROC FRAME
  push rsi
.pushreg rsi
  push rdi
.pushreg rdi
  push r12
.pushreg r12
  push r13
.pushreg r13
  sub rsp,32
.allocstack 32
.endprolog
  
  ; move fact_tbl to rsi
  mov rsi, rcx
  
  ; move count to edi
  mov edi, edx
  
  ; move n to r12d
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
  
  mov eax,r13d
  
  add rsp,32
  pop r13
  pop r12
  pop rdi
  pop rsi
  ret
divisor_count ENDP



; rcx - return pointer (Pair_U64*)
; rdx - min            (U64)
; r8  - max            (U64)
; r9  - target_product (U64)
;  assumptions:
;   rdx < r8
;   rdx*rdx <= target_product <= max*max
find_bounded_factors PROC
  ; clear result to zero
  mov qword ptr [rcx], 0
  mov qword ptr [rcx+8], 0
  
  ; move min to r11
  mov r11, rdx
  
  ; product = min*max
  mov r10, r11
  imul r10, r8
  
  ; if (product == target_product) goto return_min_max;
  ; if (product < target_product)  goto too_small;
  ; goto too_big
  cmp r10, r9
  jz return_min_max
  jb too_small
  jmp too_big
  
too_small:
  ; increase min so that:
  ;  min*max >= target_product & (min - 1)*max < target_product
  ;  min >= target_product/max & min < target_product/max + 1
  ;  target_product/max <= min < target_product/max + 1
  ;  min <- ceil(target_product/max)
  
  ; q,r = target_product div max
  xor rdx,rdx
  mov rax, r9
  idiv r8
  
  ; if (r == 0) goto too_small0;
  ; goto too_small1;
  test rdx,rdx
  jz too_small0
  
too_small1:
  ; min = floor((0:target_product)/max)
  mov r11, rax
  inc r11
  
  ; if (min > max) goto return;
  cmp r11, r8
  ja return
  jmp too_big
  
too_small0:
  ; min = floor((0:target_product)/max)
  mov r11, rax
  
  ; if (min > max) goto return;
  cmp r11, r8
  ja return
  jmp return_min_max
  
too_big:
  ; decrease max so that:
  ;  min*max <= target_product & min*(max + 1) > target_product
  ;  max <= target_product/min & max > (target_product/min) - 1
  ;  (target_product/min) - 1 < max <= target_product/min
  ;  max <- floor(target_product/min)
  
  ; max = floor((0:target_product)/min)
  xor rdx,rdx
  mov rax, r9
  idiv r11
  mov r8, rax
  
  ; if (min > max) goto return;
  cmp r11, r8
  ja return
  
  ; if ((0:target_product) == 0) goto return_min_max;
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


; rcx - A (U64)
; rdx - B (U64)
gcd_euclidean PROC
  ; transfer B into r8
  mov r8, rdx
  
  ; if (A <= B) goto loop0;
  cmp rcx,r8
  jbe loop0
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


; rcx - A (U64)
; rdx - B (U64)
lcm_euclidean PROC FRAME
  mov qword ptr [rsp + 8],rsi
.savereg rsi,8
  sub rsp,32
.allocstack 32
.endprolog
  
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
  
  add rsp,32
  mov rsi,qword ptr [rsp + 8]
  ret
lcm_euclidean ENDP


; rcx - return pointer (EulerData*)
; rdx - text           (String8*)
; r8  - out            (U8*)
euler_data_from_text__asm PROC
  ; store og return pointer for later
  mov [rsp + 8],rcx
  
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
  
  mov rax,[rsp + 8]
  
  ; result.count = out_ptr - out;
  mov rdx,[rax]
  sub r8,rdx
  mov [rax + 8],r8
  
  ; result.line_count = line_count;
  mov [rax + 16],r10
  
  ret
euler_data_from_text__asm ENDP


; rcx - return pointer (EulerData*)
; rdx - text           (String8*)
; r8  - out            (U8*)
euler_data_from_text_2dig__asm PROC
  ; store og return pointer for later
  mov [rsp + 8],rcx
  
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
  
  mov rax,[rsp + 8]
  
  ; result.count = out_ptr - out;
  mov rdx,[rax]
  sub r8,rdx
  mov [rax + 8],r8
  
  ; result.line_count = line_count;
  mov [rax + 16],r10
  
  ret
euler_data_from_text_2dig__asm ENDP

; rcx - result pointer (U64x3*)
; rdx - a              (U64x3*)
; r8  - b              (U64x3*)
add_u64x3 PROC
  ; c0,r0 = a0 + b0
  xor r9,r9
  mov rax, qword ptr [rdx]
  add rax, qword ptr [r8]
  setc r9b
  
  ; result[0] = r0
  mov qword ptr [rcx], rax
  
  ; /c1,/r1 = a1 + b1
  xor r10,r10
  mov rax, qword ptr [rdx + 8]
  add rax, qword ptr [r8 + 8]
  setc r10b
  
  ; ^c1,r1 = /r1 + c0
  xor r11,r11
  add rax, r9
  setc r11b
  
  ; result[1] = r1
  mov qword ptr [rcx + 8], rax
  
  ; c1 = ^c1 | /c1
  or r10,r11
  
  ; /r2 = a2 + b2
  ; r2 = /r2 + c1
  mov rax, qword ptr [rdx + 16]
  add rax, qword ptr [r8 + 16]
  add rax, r10
  
  ; result[2] = r2
  mov qword ptr [rcx + 16], rax
  
  ; return the result pointer
  mov rax, rcx
  ret
add_u64x3 ENDP

; rcx - result pointer (U64x3*)
; rdx - a              (U64x3*)
; r8  - b              (U64)
add_small_u64x3 PROC
  ; c0,r0 = a0 + b
  xor r9,r9
  mov rax, qword ptr [rdx]
  add rax, r8
  setc r9b
  
  ; result[0] = r0
  mov qword ptr [rcx], rax
  
  ; /r1 = a1
  mov rax, qword ptr [rdx + 8]
  ; c1,r1 = /r1 + c0
  xor r10,r10
  add rax, r9
  setc r10b
  ; result[1] = r1
  mov qword ptr [rcx + 8], rax
  
  ; /r2 = a2
  mov rax, qword ptr [rdx + 16]
  ; r2 = /r2 + c1
  add rax, r10
  ; result[2] = r2
  mov qword ptr [rcx + 16], rax
  
  ; return the result pointer
  mov rax, rcx
  ret
add_small_u64x3 ENDP

; rcx - result pointer (U64x3*)
; rdx - a              (U64x3*)
; r8  - b              (U64)
mul_small_u64x3 PROC
  
  ; move a to r9
  mov r9,rdx
  
  ; c0,r0 = a0*b (c0:rdx, r0:rax)
  mov rax, qword ptr [r9]
  mul r8
  
  ; result[0] = r0
  mov qword ptr [rcx], rax
  
  ; move c0 to r10
  mov r10, rdx
  
  ; /c1,/r1 = a1*b (/c1:rdx, /r1:rax)
  mov rax, qword ptr [r9 + 8]
  mul r8
  
  ; ^c1,r1 = /r1 + c0
  xor r11,r11
  add rax,r10
  setc r11b
  
  ; result[1] = r1
  mov qword ptr [rcx + 8], rax
  
  ; c1 = /c1 + ^c1
  add r11, rdx
  
  ; /r2 = a2*b
  mov rax, qword ptr [r9 + 16]
  mul r8
  
  ; r2 = /r2 + c1
  add rax,r11
  
  ; result[2] = r2
  mov qword ptr [rcx + 16], rax
  
  ; return the result pointer
  mov rax, rcx
  ret
mul_small_u64x3 ENDP

; rcx - result pointer (U64x3_DivR*)
; rdx - n              (U64x3*)
; r8  - d              (U64)
div_small_u64x3 PROC
  ; move n into r9
  mov r9,rdx
  
  ; (q2,r2) = n2 div d
  mov rax, qword ptr [r9 + 16]
  xor rdx,rdx
  div r8
  
  ; result[2] = q2
  mov qword ptr [rcx + 16], rax
  
  ; (q1,r1) = r2:n1 div d
  mov rax, qword ptr [r9 + 8]
  ;; note: move r2 into high bits is a noop (rdx->rdx)
  div r8
  
  ; result[1] = q1
  mov qword ptr [rcx + 8], rax
  
  ; (q0,r0) = r1:n0 div d
  mov rax, qword ptr [r9]
  ;; note: move r2 into high bits is a noop (rdx->rdx)
  div r8
  
  ; result[0] = q0
  mov qword ptr [rcx], rax
  
  ; result.r = r0
  mov qword ptr [rcx + 24], rdx
  
  ; return the result pointer
  mov rax,rcx
  ret
div_small_u64x3 ENDP


; rcx - result pointer (U64x3*)
; rdx - digits         (U8*)
; r8  - count          (U64)
u64x3_from_dec PROC FRAME
  push r12
.pushreg r12
  push r13
.pushreg r13
  push r14
.pushreg r14
  push rbx
.pushreg rbx
  sub rsp,32
.allocstack 32
.endprolog
  
  ; clear result to zer0
  mov qword ptr [rcx],0
  mov qword ptr [rcx + 8],0
  mov qword ptr [rcx + 16],0
  
  ; move result pointer to rbx
  mov rbx,rcx
  
  ; ptr = digits
  mov r12, rdx
  
  ; opl = ptr + count
  mov r13,r12
  add r13,r8
  
loop0_begin:
  ; if (ptr >= opl) goto done
  cmp r12,r13
  jae done
  
  ; x = 0
  xor r14,r14
  ; m = 1
  mov r8,1
  
loop0:
  ; x *= 10, x += *ptr
  imul r14,10
  movzx rax, byte ptr [r12]
  add r14, rax
  
  ; m *= 10
  imul r8,10
  
  ; ptr += 1
  inc r12
  
  ; if (m >= 1000000000000000000) goto loop1
  mov rdx,1000000000000000000
  cmp r8,rdx
  jae loop1
  
  ; if (ptr >= opl) goto loop1
  cmp r12,r13
  jae loop1
  
  ; goto loop0
  jmp loop0
  
loop1:
  ; call mul_small_u64x3(result pointer, result pointer, m)
  mov rcx,rbx
  mov rdx,rbx
  ;mov r8,r8
  call mul_small_u64x3
  
  ; call add_small_u64x3(result pointer, result pointer, x)
  mov rcx,rbx
  mov rdx,rbx
  mov r8,r14
  call add_small_u64x3
  
  jmp loop0_begin
  
done:
  
  ; return result pointer
  mov rax, rbx
  
  add rsp,32
  pop rbx
  pop r14
  pop r13
  pop r12
  ret
u64x3_from_dec ENDP


; rcx - buffer (U8*)
; rdx - x      (U64x3*)
dec_from_u64x3__asm PROC FRAME
  push r12
.pushreg r12
  push r13
.pushreg r13
  push r14
.pushreg r14
  sub rsp,64
.allocstack 64
.endprolog
  
  ; move buffer into r12
  mov r12,rcx
  
  ; ptr = buffer
  mov r13,r12
  
  ; move x to rsp + 32 "by value"
  mov rax,[rdx]
  mov [rsp + 32],rax
  mov rax,[rdx + 8]
  mov [rsp + 40],rax
  mov rax,[rdx + 16]
  mov [rsp + 48],rax
  
loop0:
  
  ; if (x[0] == 0 && x[1] == 0 && x[2] == 0) goto finish;
  mov rax,[rsp + 32]
  or  rax,[rsp + 40]
  or  rax,[rsp + 48]
  test rax,rax
  jz finish
  
  ; call div_small_u64x3(div-result, x, 10)
  lea rcx,[rsp + 32]
  lea rdx,[rsp + 32]
  mov r8,10
  call div_small_u64x3
  
  ; d = div_result.r
  mov rcx,[rsp + 56]
  
  ; *ptr = d
  mov byte ptr [r13], cl
  
  ; ptr += 1
  inc r13
  
  ; goto loop0
  jmp loop0
  
finish:
  ; set return value to (ptr - buffer)
  mov rax,r13
  sub rax,r12
  
  ; ptra = ptr
  ; ptrb = buffer
  ; (ptra: r12, ptrb: r13)
  ; ptrb -= 1
  dec r13
reverse_loop:
  ; if (ptra >= ptrb) goto done
  cmp r12, r13
  jae done
  
  ; swap
  mov dl, byte ptr [r12]
  mov cl, byte ptr [r13]
  mov byte ptr [r12], cl
  mov byte ptr [r13], dl
  
  ; ptra += 1, ptrb -= 1
  inc r12
  dec r13
  
  ; goto reverse_loop
  jmp reverse_loop
  
done:
  
  add rsp,64
  pop r14
  pop r13
  pop r12
  ret
dec_from_u64x3__asm ENDP

; rcx - a (U64xN*)
; rdx - b (U64)
;  assumption: a has an extra slot of memory available
mul_small_in_place_u64xn PROC FRAME
  mov [rsp + 8],r12
.savereg r12, 8
.endprolog
  
  ; move b into r9
  mov r9,rdx
  
  ; f = 0
  xor r8,r8
  
  ; i = 0
  xor r10,r10
  
  ; save a.size in r12
  mov r12,[rcx]
  
loop0:
  ; if (i >= a.size) goto done
  cmp r10,r12
  jae done
  
  ; l,h = a[i]*b
  mov rax, qword ptr [rcx + r10*8 + 8]
  mul r9
  
  ; c,r = l + f
  xor r11,r11
  add rax,r8
  setc r11b
  
  ; a[i] = r
  mov qword ptr [rcx + r10*8 + 8], rax
  
  ; f = h + c
  mov r8,r11
  add r8,rdx
  
  ; i += 1
  inc r10
  
  ; goto loop0
  jmp loop0
  
done:
  ; if (f == 0) goto return;
  test r8,r8
  jz return
  
  ; a[a.size] = f
  mov qword ptr [rcx + r12*8 + 8], r8
  
  ; a.size += 1
  inc r12
  mov qword ptr [rcx],r12
  
return:
  mov r12,[rsp + 8]
  ret
mul_small_in_place_u64xn ENDP

; rcx - n (U64xN*)
; rdx - d (U64)
div_small_in_place_u64xn PROC
  ; move d into r8
  mov r8,rdx
  
  ; r = 0
  xor rdx,rdx
  
  ; i = n.size
  mov r9, qword ptr [rcx]
  
loop0:
  ; if (i == 0) goto done;
  test r9,r9
  jz done
  
  ; i -= 1
  dec r9
  
  ; q,r = r:n[i] div d
  mov rax, qword ptr [rcx + r9*8 + 8]
  div r8
  
  ; n[i] = q
  mov qword ptr [rcx + r9*8 + 8], rax
  
  ; goto loop0
  jmp loop0
  
done:
  
  ; if (n.size > 0 && n[n.size - 1] == 0) n.size -= 1
  mov r9, qword ptr [rcx]
  test r9,r9
  jz return
  mov r8, qword ptr [rcx + r9*8]
  test r8,r8
  jnz return
  
  dec r9
  mov qword ptr [rcx], r9
  
return:
  ; set result value to last remainder
  mov rax,rdx
  ret
div_small_in_place_u64xn ENDP

; rcx - buffer (U8*)
; rdx - x      (U64xN*) (we're allowed to modify this in place)
dec_from_u64xn__asm PROC FRAME
  push r12
.pushreg r12
  push r13
.pushreg r13
  push r14
.pushreg r14
  sub rsp,32
.allocstack 32
.endprolog
  
  ; move buffer into r12
  mov r12,rcx
  
  ; ptr = buffer
  mov r13,r12
  
  ; move x to r14
  mov r14,rdx
  
loop0:
  
  ; if (*x == 0) goto finish;
  mov rax, qword ptr [r14]
  test rax,rax
  jz finish
  
  ; call div_small_in_place_u64xn(x, 10)
  mov rcx,r14
  mov rdx,10
  call div_small_in_place_u64xn
  
  ; *ptr = remainder
  mov byte ptr [r13], al
  
  ; ptr += 1
  inc r13
  
  ; goto loop0
  jmp loop0
  
  
finish:
  ; set return value to (ptr - buffer)
  mov rax,r13
  sub rax,r12
  
  ; ptra = ptr
  ; ptrb = buffer
  ; (ptra: r12, ptrb: r13)
  
  ; ptrb -= 1
  dec r13
  
reverse_loop:
  ; if (ptra >= ptrb) goto done
  cmp r12, r13
  jae done
  
  ; swap
  mov dl, byte ptr [r12]
  mov cl, byte ptr [r13]
  mov byte ptr [r12], cl
  mov byte ptr [r13], dl
  
  ; ptra += 1, ptrb -= 1
  inc r12
  dec r13
  
  ; goto reverse_loop
  jmp reverse_loop
  
done:
  
  add rsp,32
  pop r14
  pop r13
  pop r12
  ret
dec_from_u64xn__asm ENDP

memmove PROTO

; rcx - array (U64*)
; rdx - index (U64)
darray_delete PROC FRAME
  sub rsp,32
.allocstack 32
.endprolog
  
  ; count = array.count
  mov rax, qword ptr [rcx - 8]
  
  ; if (index >= count) goto skip
  cmp rdx,rax
  jae skip
  
  ; new_count = count - 1
  dec rax
  
  ; array.count = new_count
  mov qword ptr [rcx - 8], rax
  
  ; move array to r8, index to r9
  mov r8,rcx
  mov r9,rdx
  
  ; call memmove(rcx + index, rcx + index + 1, (count - index)*8)
  lea rcx,[r8 + r9*8]
  lea rdx,[r8 + r9*8 + 8]
  mov r8,rax
  sub r8,r9
  shl r8,3
  call memmove
  
skip:
  add rsp,32
  ret
darray_delete ENDP


; rcx - fact_table  (U64*)
; rdx - label_array (U64*)
; r8  - perm_out    (U64*)
euler24 PROC FRAME
  push r12
.pushreg r12
  push r13
.pushreg r13
  push r14
.pushreg r14
  push r15
.pushreg r15
  push rsi
.pushreg rsi
  sub rsp,32
.allocstack 32
.endprolog
  
  ; index = 999999
  mov r12,999999
  
  ; move fact_table into r15
  mov r15, rcx
  
  ; move label_array into r10
  mov r13,rdx
  
  ; move perm_out into rsi
  mov rsi,r8
  
  ; i = 8
  mov r14,8
  
loop0:
  ; perm_count = fact_table[i]
  mov r8,qword ptr [r15 + r14*8]
  
  ; group_index,new_index = index/perm_count
  mov rax,r12
  xor rdx,rdx
  div r8
  
  ; *perm_out = label_array[group_index]
  mov rcx,qword ptr [r13 + rax*8]
  mov qword ptr [rsi],rcx
  
  ; perm_out += 1
  add rsi,8
  
  ; index = new_index
  mov r12,rdx
  
  ; call darray_delete(label_array, group_index)
  mov rcx,r13
  mov rdx,rax
  call darray_delete
  
  ; i -= 1
  dec r14
  
  ; if (i <= 8) goto loop0
  test r14,8
  jbe loop0
  
  ; *perm_out = label_array[0]
  mov rcx,qword ptr [r13]
  mov qword ptr [rsi],rcx
  
  add rsp,32
  pop rsi
  pop r15
  pop r14
  pop r13
  pop r12
  ret
euler24 ENDP

END

