#include "base_inc.h"

#include "base_inc.cpp"

#include <stdio.h>
#include <stdlib.h>

#include "base_memory_malloc.cpp"

#define EvalPrint(x) printf("%s = %d\n", #x, (S32)(x))
#define EvalPrintLL(x) printf("%s = %lld\n", #x, (S64)(x))
#define EvalPrintU(x) printf("%s = %u\n", #x, (U32)(x))
#define EvalPrintX(x) printf("%s = %x\n", #x, (U32)(x))
#define EvalPrintULL(x) printf("%s = %llu\n", #x, (U64)(x))
#define EvalPrintF(x) printf("%s = %e [%f]\n", #x, (F64)(x), (F64)(x))
#define EvalPrintB(x) printf("%s = %s\n", #x, (char*)((x)?"true":"false"))
#define EvalPrintS(x) printf("%s = %s\n", #x, (char*)(x))
#define EvalPrintStr8(x) printf("%s = %.*s\n", #x, str8_expand(x))

struct Node{
    Node *next;
    Node *prev;
    
    U64 x;
};

struct TestStruct{
    int a;
    int b;
    int c;
    int d;
};

int main(){
    M_Arena arena = m_make_arena(m_malloc_base_memory());
    
    // 1 000000 000000 000000
    //  100 00000000 00000000
    //    4    0   0    0   0
    U8 utf8_test[] = { 0x40, 0xF1, 0x80, 0x80, 0x80, };
    
    StringDecode dec1 = str_decode_utf8(utf8_test, ArrayCount(utf8_test));
    StringDecode dec2 = str_decode_utf8(utf8_test + dec1.size,
                                        ArrayCount(utf8_test) - dec1.size);
    EvalPrintX(dec1.codepoint);
    EvalPrintX(dec2.codepoint);
    
    str_encode_utf8(utf8_test, 5000);
    StringDecode dec3 = str_decode_utf8(utf8_test, 4);
    EvalPrintU(dec3.codepoint);
    
    String8 test_string_array[] = {
        str8_lit("A"),
        str8_lit("Bee"),
        str8_lit("Let's get some 90923923 in here!"),
        // TODO(allen): test with direct string comparison
        //str8_lit("\x01\x02\x03\x04\x05\x06\x07\x08\n\t\r"),
    };
    
    for (U32 i = 0; i < ArrayCount(test_string_array); i += 1){
        {
            String32 string_32 = str32_from_str8(&arena, test_string_array[i]);
            String8 string_8 = str8_from_str32(&arena, string_32);
            EvalPrintStr8(string_8);
        }
        
        {
            String16 string_16 = str16_from_str8(&arena, test_string_array[i]);
            String8 string_8 = str8_from_str16(&arena, string_16);
            EvalPrintStr8(string_8);
        }
    }
    
    
    String8 test_string = str8_pushf(&arena, "Hello %dth World!", 55);
    
    EvalPrintStr8(test_string);
    EvalPrintStr8(str8_prefix(test_string, 5));
    EvalPrintStr8(str8_postfix(test_string, 6));
    EvalPrintStr8(str8_skip(test_string, 3));
    EvalPrintStr8(str8_chop(test_string, 3));
    
    String8List list = {};
    str8_list_push(&arena, &list, str8_lit("Part 1"));
    str8_list_push(&arena, &list, str8_lit("Part 2"));
    str8_list_push(&arena, &list, str8_lit("Part 3"));
    
    StringJoin join_params = {};
    join_params.mid = str8_lit("/");
    String8 joined = str8_join(&arena, &list, &join_params);
    EvalPrintStr8(joined);
    
    String8List split = str8_split(&arena, joined, (U8*)" ", 1);
    for (String8Node *node = split.first;
         node != 0;
         node = node->next){
        EvalPrintStr8(node->string);
    }
    
    
    U64 node_count = 10;
    Node *nodes = push_array(&arena, Node, node_count);
    for (U64 i = 0; i < node_count; i += 1){
        nodes[i].x = i;
    }
    
    {
        Node *first = 0;
        Node *last = 0;
        for (U64 i = 0; i < node_count; i += 1){
            SLLStackPush(first, &nodes[i]);
        }
        SLLStackPop(first);
        SLLStackPop(first);
        SLLStackPop(first);
        
        for (Node *node = first;
             node != 0;
             node = node->next){
            EvalPrint(node->x);
        }
    }
    
    
    
    int foo[100];
    for (int i = 0; i < ArrayCount(foo); i += 1){
        foo[i] = i;
    }
    
    EvalPrint(ArrayCount(foo));
    
    int bar[100];
    MemoryCopyArray(bar, foo);
    EvalPrint(bar[50]);
    EvalPrint(MemoryMatch(foo, bar, sizeof(foo)));
    MemoryZeroArray(bar);
    EvalPrint(bar[50]);
    EvalPrint(MemoryMatch(foo, bar, sizeof(foo)));
    
    EvalPrint(OffsetOfMember(TestStruct, a));
    EvalPrint(OffsetOfMember(TestStruct, b));
    EvalPrint(OffsetOfMember(TestStruct, c));
    EvalPrint(OffsetOfMember(TestStruct, d));
    
    TestStruct t = {1, 2, 3, 4};
    EvalPrint(t.a);
    EvalPrint(t.d);
    MemoryZeroStruct(&t);
    EvalPrint(t.a);
    EvalPrint(t.d);
    
    EvalPrint(Min(1, 10));
    EvalPrint(Min(100, 10));
    EvalPrint(Max(1, 10));
    EvalPrint(Max(100, 10));
    EvalPrint(Clamp(1, 10, 100));
    EvalPrint(Clamp(1, 0, 100));
    EvalPrint(Clamp(1, 500, 100));
    
    EvalPrint(min_S8);
    EvalPrint(min_S16);
    EvalPrint(min_S32);
    EvalPrintLL(min_S64);
    
    EvalPrint(max_S8);
    EvalPrint(max_S16);
    EvalPrint(max_S32);
    EvalPrintLL(max_S64);
    
    EvalPrintU(max_U8);
    EvalPrintU(max_U16);
    EvalPrintU(max_U32);
    EvalPrintULL(max_U64);
    
    EvalPrintF(machine_epsilon_F32);
    EvalPrintF(machine_epsilon_F64);
    
    EvalPrintF(inf_F32());
    EvalPrintF(neg_inf_F32());
    
    EvalPrintF(inf_F64());
    EvalPrintF(neg_inf_F64());
    
    EvalPrintF(abs_F32(-100.f));
    EvalPrintF(abs_F32(100.f));
    EvalPrintF(abs_F32(0));
    EvalPrintF(abs_F32(neg_inf_F32()));
    
    EvalPrintF(abs_F64(-100.));
    EvalPrintF(abs_F64(100.));
    EvalPrintF(abs_F64(0));
    EvalPrintF(abs_F64(neg_inf_F64()));
    
    EvalPrintF(sqrt_F32(100.f));
    EvalPrintF(sin_F32(tau_F32*.3f));
    EvalPrintF(cos_F32(tau_F32*.3f));
    EvalPrintF(tan_F32(0.5f));
    EvalPrintF(ln_F32(e_F32));
    
    EvalPrintF(sqrt_F64(100.));
    EvalPrintF(sin_F64(tau_F64*.3));
    EvalPrintF(cos_F64(tau_F64*.3));
    EvalPrintF(tan_F64(0.5));
    EvalPrintF(ln_F64(e_F64));
    
    EvalPrintF(lerp(0, 0.3f, 1.f));
    EvalPrintF(lerp(10.f, 0.5f, 100.f));
    EvalPrintF(unlerp(0, 0.3f, 1.f));
    EvalPrintF(unlerp(10.f, lerp(10.f, 0.5f, 100.f), 100.f));
    
    EvalPrintB(intr_contains(i2f32(20, 20, 200, 200), v2f32(100, 199)));
    EvalPrintB(intr_contains(i2f32(20, 20, 200, 200), v2f32(100, 201)));
    EvalPrintB(intr_contains(i2f32(20, 20, 200, 200), v2f32(19, 199)));
    
    EvalPrintS(string_from_operating_system(operating_system_from_context()));
    EvalPrintS(string_from_day_of_week(DayOfWeek_Wednesday));
    
    return(0);
}