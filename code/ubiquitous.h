#ifndef UBIQUITOUS_H //Created on October 18th 2020 8:32 pm
#define UBIQUITOUS_H

enum {
    ARCH_X32,  
    ARCH_X64,  
    ARCH_ARM32,
    ARCH_ARM64,    
};

//TODO figure this out from compilier flags
#define ARCH ARCH_X64


#include "stdint.h"
#include "float.h"

#define true  1
#define false 0 

#define WORDSIZE 8
#define null  nullptr
#define EMPTY(...) 

#define STRINGIFY2(x) #x
#define STRINGIFY(x) STRINGIFY2(x)

#define ENUM_STRING(x, ...) strlit(#x)
#define ENUM_STRING_OFFSET(x, offset, ...) { (char *)#x + (offset), (s32)sizeof(#x)-1-(offset), (s32)sizeof(#x)-(offset) }

#define ENUM_VALUE(x,  ...) x
#define ENUM_FLAG(x, flag)  x = (flag)  



#if DEBUG_BUILD
        #define WRITE_TO_NULL_POINTER (*(int *)0 = 1)
        #define assert(expr)       ((expr) ? 1 : (__debugbreak(), WRITE_TO_NULL_POINTER)) 
        #define if_assert(cond, expr) assert ((cond) ? ((expr)!=0) : 1);
        #define defer_assert(expr) defer { assert (expr); };
        #define cassert(expr, ...) static_assert (expr, "cassert("#expr"): " __VA_ARGS__)
        #define breakpoint __debugbreak()
#else
        #define WRITE_TO_NULL_POINTER
        #define assert(expr) __noop()   
        #define if_assert(cond, expr) 
        #define defer_assert(expr) 
        #define cassert(expr, ...) static_assert (expr, #expr __VA_ARGS__)
        #define breakpoint 
#endif

//#define countof(array) (sizeof(array) / sizeof(array[0]))
#define countof(array) _countof(array) //NOTE msvc specific, nicer bc it catches errors when passing raw pointer to it
#define internal      static
#define global        static

#define NAMESPACE_BEGIN(name) namespace name {
#define NAMESPACE_END }

#define let  auto
#define then 
#define NEGATE(x) ((x) = !(x))




//NOTE don't put these on the same line unless you want to use __COUNTER__
#define UNIQUE_FILE_LOCATION_STRING_INTERNAL2(file, linenum) file "@" #linenum
#define UNIQUE_FILE_LOCATION_STRING_INTERNAL(file, linenum) UNIQUE_FILE_LOCATION_STRING_INTERNAL2(file, linenum)
#define UNIQUE_FILE_LOCATION_STRING strlit(UNIQUE_FILE_LOCATION_STRING_INTERNAL(__FILE__, __LINE__))

#define index_in_range(x, count) ((x) >= 0 && (x) < (count))

//NOTE we use this just to not confuse 4coder from thinking a type is a function...
#define CONSTRUCTOR(typename_funame) typename_funame##::##typename_funame
#define DESTRUCTOR(typename_funame)  typename_funame##::~##typename_funame

typedef int8_t  s8;
typedef int16_t s16; 
typedef int32_t s32;
typedef int64_t s64; 

typedef int8_t  bool8;
typedef int16_t bool16; 
typedef int32_t bool32;
typedef int64_t bool64; 

typedef uint8_t  u8;
typedef uint16_t u16; 
typedef uint32_t u32; 
typedef uint64_t u64; 

typedef s32    boolint; 
typedef s8     boolint8; 
typedef s16    boolint16; 
typedef s32    boolint32; 
typedef s64    boolint64; 

typedef s32    nil; 
typedef s8     b8;
typedef s16    b16;
typedef s32    b32;
typedef s64    b64;
typedef size_t uintptr; 
typedef float  f32;
typedef double f64;

#define PI    3.14159265358979323846f
#define TAU   6.28318530717958647692f

internal f32
to_degrees(f32 radians) {
    f32 t = radians / TAU;
    return 360.0f * t;
}

internal f32
to_radians(f32 degrees) {
    f32 t = degrees / 360.0f;
    return TAU * t;
}

//#define SQRT2 1.41421356237f //TODO get better approx
#define SQRT2 1.41421356237309504880168872420969807856967187537694807317667973799f
#define SQUARED(x) ((x)*(x))
#define CUBED(x) ((x)*(x)*(x))


struct Memory_Arena;
//NOTE if you rearrange these members, you have update macros below to make sure they're still correct
struct Push_Size_Args {
    u32 alignment; //   = 0;
    bool zero_out_memory; // = false;
};

//NOTE people are required to define this function, it's basically a first class citizen for us
inline Memory_Arena *get_temp_memory();


internal void *push_size_internal(Memory_Arena *arena, u32 size, u32 default_alignment, Push_Size_Args args = {});

#define push_size(arena, size, ...)                     push_size_internal(arena, size,                 1,             __VA_ARGS__ )
#define push_struct(arena,  type, ...)          (type *)push_size_internal(arena, sizeof(type),         alignof(type), __VA_ARGS__ )
#define push_array(arena, type, count, ...)     (type *)push_size_internal(arena, sizeof(type)*(count), alignof(type), __VA_ARGS__ )

#define push_size_memzero(arena, size)                  push_size_internal(arena, size,                 1,             {0, true} )
#define push_struct_memzero(arena,  type)       (type *)push_size_internal(arena, sizeof(type),         alignof(type), {0, true} )
#define push_array_memzero(arena, type, count)  (type *)push_size_internal(arena, sizeof(type)*(count), alignof(type), {0, true} )




//#define ZERO_STRUCT(var)      zero_memory(var, sizeof (*(var)))
//#define COPY_STRUCT(dst, src) copy_memory(dst, src, sizeof(*(src)))

//TODO intrinsic for this?
internal s32
signof(f32 x)
{       return (x < 0) ? -1 : ((x > 0) ? 1 : 0);        }

//TODO intrinsic for this?
internal s32
signof(s32 x)
{       return (x < 0) ? -1 : ((x > 0) ? 1 : 0);        }

internal f32
squared(f32 x) { return x*x; }


#define U8_MAX  UINT8_MAX
#define U16_MAX UINT16_MAX
#define U32_MAX UINT32_MAX
#define U64_MAX UINT64_MAX

#define u8_MAX  UINT8_MAX
#define u16_MAX UINT16_MAX
#define u32_MAX UINT32_MAX
#define u64_MAX UINT64_MAX
//the mins are just for some macro magic I do sometimes
#define u8_MIN  0
#define u16_MIN 0
#define u32_MIN 0
#define u64_MIN 0


template<typename TYPE> inline TYPE
get_max_value(TYPE t);

template<> inline u32
get_max_value<u32>(u32 d) {
    return U32_MAX;
}

template<> inline u64
get_max_value<u64>(u64 d) {
    return U64_MAX;
}


//#define UINTPTR_MAX UINTPTR_MAX

//#define U64_MAX INTPTR_MAX

#define S8_MIN  INT8_MIN
#define S8_MAX  INT8_MAX
#define S16_MIN INT16_MIN
#define S16_MAX INT16_MAX
#define S32_MIN INT32_MIN
#define S32_MAX INT32_MAX
#define S64_MIN INT64_MIN
#define S64_MAX INT64_MAX


#define s8_MIN  INT8_MIN
#define s8_MAX  INT8_MAX
#define s16_MIN INT16_MIN
#define s16_MAX INT16_MAX
#define s32_MIN INT32_MIN
#define s32_MAX INT32_MAX
#define s64_MIN INT64_MIN
#define s64_MAX INT64_MAX

#define F32_MAX FLT_MAX   //TODO does this cause interrupts/exceptions when using?

#define IS_ODD(x)  ((x) & 1)
#define IS_EVEN(x) (!IS_ODD(x))
#define IS_POW2(x) (((x) & ((x) - 1)) == 0)

#define CONCAT_INTERNAL(x,y) x##y
#define CONCAT(x,y) CONCAT_INTERNAL(x,y)

#define SWAP(a, b)        do{ auto  CONCAT(_temp, __LINE__) = a; a = b; b = CONCAT(_temp, __LINE__); } while(0)
#define SWAPt(a, b, type) do{ _type CONCAT(_temp, __LINE__) = a; a = b; b = CONCAT(_temp, __LINE__); } while(0)

#define min2(a, b) (((a) < (b)) ? (a) : (b))
#define max2(a, b) (((a) > (b)) ? (a) : (b))

#define set_min2(lval, rval) ((lval) = min2((lval), (rval)))
#define set_max2(lval, rval) ((lval) = max2((lval), (rval)))

internal f32
clamp(f32 value, f32 min, f32 max) {
    assert (min <= max);
    return (value < min) ? min : ((value > max) ? max : value);
}

internal void
clamp(f32 *value, f32 min, f32 max) { *value = clamp(*value, min, max); }

internal s32
clamp(s32 value, s32 min, s32 max)
{
    assert (min <= max);
    return (value < min) ? min : ((value > max) ? max : value);
}

internal void
clamp(s32 *value, s32 min, s32 max) { *value = clamp(*value, min, max); }

internal u32
clamp(u32 value, u32 min, u32 max)
{
    assert (min <= max);
    return (value < min) ? min : ((value > max) ? max : value);
}

internal void
clamp(u32 *value, u32 min, u32 max) { *value = clamp(*value, min, max); }

internal f32
divide(f32 a, f32 b, f32 failsafe = 0.0f) {
    if (b > 0.0001f) return a / b;
    else             return failsafe;
        
}



internal f32 
minval2(f32 a, f32 b) { return (a < b) ? a : b; }

internal f32 
maxval2(f32 a, f32 b) { return (a > b) ? a : b; }

internal s32 
minval2(s32 a, s32 b) { return (a < b) ? a : b; }

internal s32 
maxval2(s32 a, s32 b) { return (a > b) ? a : b; }


internal f32
lerp(f32 a, f32 b, f32 t)
{   return a + (b - a)*t;   }

internal f32
unlerp(f32 a, f32 b, f32 value)
{
    f32 divisor = (b - a);
    assert (divisor);
    return (value - a) / divisor;
}


internal f32
unlerp(s32 a, s32 b, s32 value) {
    s32 divisor = (b - a);
    assert (divisor);
    return (f32)(value - a) / (f32)divisor;
}

internal f32
remap(f32 in_min, f32 in_max, f32 in, f32 out_min, f32 out_max) {
    assert (in_min != in_max && out_min != out_max);
    f32 result = (in - in_min) / (in_max - in_min); // convert to 0-1 range
    result = out_min + (out_max-out_min)*result; //put it in our desired range
    //NOTE this is the exact same thing as result = lerp(out_min, out_max, unlerp(in_min, in_max, in));
    return result;
}

internal f32 absval(f32 x);

//NOTE this is the exact same thing as result = lerp(out_min, out_max, unlerp01(in_min, in_max, in));
internal f32 //input and output parameters clamped to 01
remap01(f32 in_min, f32 in_max, f32 in, f32 out_min, f32 out_max) {
    f32 in_dist = in_max - in_min;
    assert (absval(in_dist) >= 0.001f);
    
    f32 out_dist = out_max - out_min;
    assert (absval(out_dist) >= 0.001f);
    
    f32 t = clamp((in - in_min) / in_dist, 0.0f, 1.0f); // convert to 0-1 range
    f32 result = out_min + t*out_dist; //put it in our desired range
    return result;
}

internal void
clamp_hi(f32 *value, f32 max) { if (*value > max) *value = max; }

internal void
clamp_lo(f32 *value, f32 min) { if (*value < min) *value = min; }

#define CLAMP_HI(var, max) (((var) > (max)) ? ((var) = (max)) : (var))
#define CLAMP_LO(var, min) (((var) < (min)) ? ((var) = (min)) : (var))
#define CLAMP(var, min, max) (assert (max > min), CLAMP_LO(var, min), CLAMP_HI(var, max))
//#define for_index_inc(s32, index_name, count)     for(s32 index_name = 0; index_name < (count); index_name += 1)
//#define for_index_dec(s32, index_name, count)     for(s32 index_name = (count)-1; index_name >= 0; index_name -= 1)

#define it_foreach(it_name, it_constructor) for(auto it_name = it_constructor; it_name.valid(); it_name.get_next())
#define for_index_inc(type, i, count)     for(type i = 0; i < (count); i += 1)
#define for_index_dec(type, i, count)     for(type i = (count); ((i) --) > 0;   )

#define for_c_array_forward(varname, array) for(let varname = (array); varname < ((array) + countof(array)); varname += 1)
#define for_c_array_reverse(varname, array) for(let varname = (array) + countof(array)-1; varname >= array; varname -= 1)

#define for_array_forward(var, _array) for (let var = (_array).e;               var < ((_array).e + (_array).for_array_get_count()); var += 1)
#define for_array_reverse(var, array) if ((array).count > 0) for (let var = (array).e + (array).for_array_get_count() - 1; var >= (array).e;                  var -= 1)

#define for_array_forward_at(var, at, array) for (let var = (array).for_array_assert_contains(at); var < ((array).e + (array).for_array_get_count()); var += 1)
#define for_array_reverse_at(var, at, array) for (let var = (array).for_array_assert_contains(at); var >= (array).e; var -= 1)


#define for_ll_forward(varname, lst) for (auto varname = (lst).get_head(); varname; varname = (lst).get_next(varname)) 
#define for_ll_reverse(varname, lst) for (auto varname = (lst).get_tail(); varname; varname = (lst).get_prev(varname))
//note for sll and dll this could be dangerous because it may not be part of the same list 
#define for_ll_forward_at(e, sentinel, at) for (auto e = (sentinel).for_ll_not_sentinel(at); e; e = (sentinel).get_next(e)) 
#define for_ll_reverse_at(e, sentinel, at) for (auto e = (sentinel).for_ll_not_sentinel(at); e; e = (sentinel).get_prev(e))

#define sll_foreach_remove(varname, lst)  for(let varname = (lst).pop_head(); varname; varname = (lst).pop_head())


#if 0 //TODO come up with better name for this guy
//NOTE if we want sll/dll_foreach and dll_foreach_reverse to inherently be able to remove elements no problem, we could define the for loop like this...
#define dll_foreach2(varname, lst) for(let varname = (lst).head, next_varname = (lst).get_next(varname); \
    /**/                               varname; \
    /**/                               (varname = next_varname), (next_varname = (lst).get_next(next_varname)))
#endif
 

#define DLL_SENTINEL(type, nodes_member) DLL_Sentinel<type, offsetof(type, nodes_member)>


template <typename TYPE>
internal u64
array_pointer_index64(TYPE *array, u32 array_size, TYPE *ptr) {
    assert (ptr >= array && ptr < (array + array_size));
    u64 index64 = (u64)(ptr - array);
    return index64;
}

template <typename TYPE, u32 SIZE>
internal u64
array_pointer_index64(TYPE (&array)[SIZE], TYPE *ptr) { 
    return array_pointer_index64(&array[0], SIZE, ptr); 
}

template <typename TYPE>
internal u32
array_pointer_index32(TYPE *array, u32 array_size, TYPE *ptr) {
    u64 index64 = array_pointer_index64(array, array_size, ptr);
    assert (index64 <= U32_MAX);
    return (u32)index64;
}

template <typename TYPE, u32 SIZE>
internal u32
array_pointer_index32(TYPE (&array)[SIZE], TYPE *ptr) {
    return array_pointer_index32(&array[0], SIZE, ptr); 
}

#define ptr_in_base_size(pointer,  base, size)  ( (u8 *)(pointer) >= (u8 *)(base) && (u8 *)(pointer) < ((u8 *)(base) + (size)))
#define ptr_in_base_count(pointer, base, count) ( (uintptr)(pointer) >= (uintptr)(base) && (uintptr)(pointer) < (uintptr)((base) + (count)) )

struct Range_f32 {
    f32 min, max;
};


struct Range_s32 {
    s32 min, max;
};

internal nil
is_valid(Range_s32 range) //NOTE we allow "null" ranges where max == min
{       return (range.max >= range.min);        }

internal Range_s32 
range_s32(s32 min, s32 max)
{
    Range_s32 range = {min, max};
    assert (is_valid(range));
    return range;
}

internal s32
get_length(Range_s32 range)
{
    s32 length = range.max - range.min;
    assert (length >= 0);
    return length;
}

internal nil
in_range(Range_s32 range, s32 x)
{       return (x >= range.min) && (x < range.max);     }

internal nil
in_range(Range_s32 range, Range_s32 subrange)
{       return in_range(range, subrange.min) && in_range(range, subrange.max - 1);      }




static Range_s32
operator+(Range_s32 range, s32 offset)
{       return range_s32(range.min + offset, range.max + offset);       }

static void
operator+=(Range_s32 &range, s32 offset)
{       range = range + offset; }

static Range_s32
operator-(Range_s32 range, s32 offset)
{       return range_s32(range.min - offset, range.max - offset);       }

static void
operator-=(Range_s32 &range, s32 offset)
{       range = range - offset; }


struct Defer_Statement_Dummy_Type {}; //just to ensure we call correct overload with operator+

template <class LAMBDA> 
struct Defer_Statement { 
    LAMBDA code; 
    ~Defer_Statement() 
    { code(); }     
};

template <class LAMBDA> Defer_Statement<LAMBDA> 
operator+(Defer_Statement_Dummy_Type, LAMBDA code)
{ return {code}; }

#define defer auto CONCAT(__defer, __LINE__) = Defer_Statement_Dummy_Type{} + [&]()


#define BYTE_PTR_DIFF(b, a)  (((b) > (a)) ? ((uintptr)(b)-(uintptr)(a)) : (uintptr)0)
#define COUNT_PTR_DIFF(b, a) (((b) > (a)) ? (uintptr)((b)-(a))          : (uintptr)0)



internal s32 
to_s32(uintptr value) { 
    assert (value <= S32_MAX);  
    return (s32)(value); 
}

internal s32 
to_s32(char *p) { 
    assert (p >= (char *)0);
    return to_s32((uintptr)p);
}

internal s32 
to_s32(u32 value) { 
    assert (value <= S32_MAX); 
    return (s32)(value); 
}

internal u32
to_u32(uintptr value)
{ assert (value <= U32_MAX); return (u32)(value); }

internal u32 
to_u32(s32 value)
{ assert (value >= 0); return (u32)(value); }

internal u8
to_u8(s32 value)
{ assert ((value >= 0) && (value <= U8_MAX)); return (u8)(value); }

internal u16
to_u16(u32 value) { assert (value <= U16_MAX); return (u16)(value); }


#define SLL(type, next)       Singly_Linked_List<type, offsetof(type, next)>
#define DLL(type, prev, next) Doubly_Linked_List<type, offsetof(type, prev), offsetof(type, next)>

//NOTE I think we could use C++ pointer-to-member operator here but that syntax is fucking abyzmal. wtf...
//hopefully the compilier can just easily optimize the member dereference
template <typename TYPE, u32 NEXT_BYTE_OFFSET>
struct Singly_Linked_List {
    TYPE *head;
    s32  count;
    //NOTE var must be address of the var, so pointer
    #define NEXT(varptr) (*((TYPE **)((u8 *)(varptr) + NEXT_BYTE_OFFSET)))
    #define PREV(varptr) (*((TYPE **)((u8 *)(varptr) + PREV_BYTE_OFFSET)))
    
    //NOTE this is defined for my sll_foreach macro
    inline TYPE *get_next(TYPE *element) { return NEXT(element); }
    inline TYPE *get_head()              { return head; }
    //for generic loop macro to work
    inline TYPE *for_ll_not_sentinel(TYPE *ptr) { return ptr; }
        
    
    
    TYPE *add_head(TYPE *element)
    {
        assert (NEXT(element) == null);
        NEXT(element) = head;
        head = element;
        count += 1;
        return head;
    }
    
    TYPE *get_tail_slowly()
    {
        for (TYPE *element = head; element; element = NEXT(element))
            if (!NEXT(element)) return element;
        return null;
    }
    
    TYPE *push_list_head_slowly(Singly_Linked_List<TYPE, NEXT_BYTE_OFFSET> list)
    {
        TYPE *tail = list.get_tail_slowly();
        if (tail)
        {
            NEXT(tail) = head;
            head = list.head;
            count += list.count;
        }
        return head;
    }
    
    TYPE *add_tail_slowly(TYPE *element)
    {
        assert (NEXT(element) == null);
        if (!head) head = element;
        else
        {
            TYPE *tail = head;
            while(NEXT(tail)) tail = NEXT(tail);
            NEXT(tail) = element;
        }
        count += 1;
        return element;
    }
    
    TYPE *pop_head()
    {
        TYPE *result = head;
        if (result)
        {
            head = NEXT(head);
            NEXT(result) = null;
        }
        
        if (count > 0) count -= 1;
        
        
        return result;
    }
    
    TYPE *remove(TYPE *prev, TYPE *elem)
    {
        assert (elem);
        if (prev)
        {
            assert (NEXT(prev) == elem);
            NEXT(prev) = NEXT(elem);
            NEXT(elem) = null;
            
            assert (count > 1);
            count -= 1;
            
            return elem;
        }
        else //elem must be head then
        {
            assert (elem == head);
            return pop_head();
        }
    }
};


template <typename TYPE, u32 PREV_BYTE_OFFSET, u32 NEXT_BYTE_OFFSET> 
struct Doubly_Linked_List {
    TYPE *head;
    TYPE *tail;
    s32 count;
    
    //NOTE just for dll_foreach macro
    inline TYPE *get_next(TYPE *element) { return NEXT(element); }
    inline TYPE *get_prev(TYPE *element) { return PREV(element); }
    
    inline TYPE *get_head()              { return head; }
    inline TYPE *get_tail()              { return tail; }
    //for generic loop macro to work
    inline TYPE *for_ll_not_sentinel(TYPE *ptr) { return ptr; }
    
    //if head and tail are null, we init them and return true
    //otherwise hand and tail are set to something (maybe same pointer), and we return true
    nil will_init_head_and_tail(TYPE *element) {
        assert (PREV(element) == null);
        assert (NEXT(element) == null);
        assert ((head && tail) || (!head && !tail));
        
        if (!head) {
            head = tail = element;
            assert (count == 0);
            count = 1;
            return true;
        }    
        else return false;
    }
    
    TYPE *add_head(TYPE *element) {       
        if (!will_init_head_and_tail(element)) {
            NEXT(element) = head;
            PREV(head)    = element;
            head = element;
            count += 1;
        }
        
        assert (NEXT(tail) == null);
        assert (PREV(head) == null);
        return element;
    }
    TYPE *add_tail(TYPE *element) {
        if (!will_init_head_and_tail(element)) {
            PREV(element) = tail;
            NEXT(tail)    = element;
            tail = element;
            count += 1;
        }
        
        assert (NEXT(tail) == null);
        assert (PREV(head) == null);
        return element;
    }
    
    TYPE *insert_after(TYPE *prev, TYPE *elem) {
        assert (PREV(elem) == null);
        assert (NEXT(elem) == null);
        
        if (prev) {
            PREV(elem) = prev;
            NEXT(elem) = NEXT(prev);
            
                        NEXT(prev) = elem;
            if (TYPE *next = NEXT(elem)) { //
                PREV(next) = elem;    
                        } else {
                                assert (prev == tail);
                                tail = elem;
                        }
            count += 1;
        } else {
            add_head(elem);
        }
        return elem;
    }
    
    TYPE *pop_head() { 
        if (head) {
            TYPE *result = head;
            assert (PREV(head) == null);
            if (NEXT(head)) {
                //if next(head) is tail then head will equal tail
                //and both will have their next, prev pointers equal to null
                head = NEXT(head);
                PREV(head) = null;
            } else {
                assert (head == tail);
                head = tail = null;
            }    
            
            NEXT(result) = null;
            assert (PREV(result) == null);
            
            assert (count > 0);
            count -= 1;
            return result;
        }
        else return null;
    }
    TYPE *pop_tail() {
        if (tail) {
            TYPE *result = tail;
            assert (NEXT(tail) == null);
            
            if (PREV(tail)) {
                tail = PREV(tail);
                NEXT(tail) = null;
            } else {
                assert (tail == head);
                tail = head = null;
            } 
            
            PREV(result) = null;
            assert (NEXT(result) == null);
            
            assert (count > 0);
            count -= 1;
            return result;
        }
        else return null;
    } 
    
    #if DEBUG_BUILD
    nil is_in_list(TYPE *element) {
        dll_foreach(node, *this) if (node == element) return true;
        return false;
    }
    #endif
    
    TYPE *remove(TYPE *element) {
        //assert (is_in_list(element)); 
        if      (element == head) return pop_head();
        else if (element == tail) return pop_tail();
        else {
            //if this fails then element doesn't have prev or next pointer
            PREV(NEXT(element)) = PREV(element);
            NEXT(PREV(element)) = NEXT(element);
            NEXT(element) = PREV(element) = null;
            
            assert (count > 0);
            count -= 1;
            return element;
        }
    }
    #undef NEXT
    #undef PREV
};


// NOTE this type of linked list, we can remove elements without having to reference the list object
// we just need the element pointer, and we patch up the next->prev && prev->next pointers
struct DLL_Nodes {
    DLL_Nodes *prev;
    DLL_Nodes *next;
    
    inline void 
    remove_from_list() {
        //assert (prev != next); //not correct as singly element list will have these point to sentinel.....
        prev->next = next;
        next->prev = prev;
        //NOTE do we want to disallow the sentinel element from basically de-initializing itself like this?...
        prev = next = null; //not necessary to do
    }
};

template <typename TYPE, s32 NODES_MEMBER_OFFSET = 0> 
struct DLL_Sentinel : DLL_Nodes { 

    inline void
    init() {
        prev = next = this;
    }
    
    inline DLL_Nodes *
    get_nodes(TYPE *ptr) {
        assert (ptr);
        DLL_Nodes *nodes = (DLL_Nodes *)((u8 *)ptr + NODES_MEMBER_OFFSET);
        assert (nodes != this); //NOTE this can be allowed if we ever want to allow an entire type struct to act as a sentinel
        return nodes;
    }
    
    inline TYPE *
    get_type_pointer(DLL_Nodes *nodes) {
        assert (nodes && nodes != this); //NOTE this can be allowed if we ever want to allow an entire type struct to act as a sentinel
        //assert (nodes->prev && nodes->next);
        return (TYPE *)((u8 *)nodes - NODES_MEMBER_OFFSET);
    }
    
    inline void
    is_sentinel(TYPE *ptr) {
        assert (ptr);
        return get_nodes(ptr) == this;
    }
    
    inline TYPE * //for loop macro. If it's a sentinel we return null. Otherwise we return the same pointer back
    for_ll_not_sentinel(TYPE *ptr) {
        return (get_nodes(ptr) == this) ? null : ptr;
    }
    
    inline void
    add_head(TYPE *ptr) {
        assert (next && prev); //otherwise init() never called
        DLL_Nodes *nodes = get_nodes(ptr);
        assert (nodes->next == null && nodes->prev == null);
        
        nodes->next = next;
        nodes->prev = this;
        
        nodes->next->prev = nodes;
        nodes->prev->next = nodes;
    }
    
    inline void
    add_tail(TYPE *ptr) {
        assert (next && prev); //otherwise init() never called
        DLL_Nodes *nodes = get_nodes(ptr);
        assert (nodes->next == null && nodes->prev == null);
        
        nodes->next = this;
        nodes->prev = prev;
        
        nodes->next->prev = nodes;
        nodes->prev->next = nodes;
    }
    
    inline TYPE *
    remove_head() {
        assert (next && prev); //otherwise init() never called
        if (next != this) {
            DLL_Nodes *head_nodes = next; 
            head_nodes->remove_from_list();
            return get_type_pointer(head_nodes);
        } else return null;
        
    }
    
    inline TYPE *
    remove_tail() {
        assert (next && prev); //otherwise init() never called
        if (prev != this) {
            DLL_Nodes *tail_nodes = prev;
            tail_nodes->remove_from_list();
            return get_type_pointer(tail_nodes);   
        } else return null;
    }
    
    inline TYPE *
    get_head() {
        assert (next && prev); //otherwise init() never called
        if (next != this) return get_type_pointer(next);
        else return null;
    }
    
    inline TYPE *
    get_tail() {
        assert (next && prev); //otherwise init() never called
        if (prev != this) return get_type_pointer(prev);
        else return null;
    }
    
    inline TYPE *
    get_next(TYPE *ptr) {
        assert (ptr);
        DLL_Nodes *nodes = get_nodes(ptr);
        if (nodes->next == this) return null; //ptr is the last element in the list
        else return get_type_pointer(nodes->next);
    }
    
    inline TYPE *
    get_prev(TYPE *ptr) {
        assert (ptr);
        DLL_Nodes *nodes = get_nodes(ptr);
        if (nodes->prev == this) return null; //ptr is the last element in the list
        else return get_type_pointer(nodes->prev);
    }
    
    inline TYPE * //if pass in null, we just repeat back at the beginning
    get_next_looped(TYPE *ptr) { 
        if (!ptr) return get_head();
        else      return get_next(ptr);
    }
    
    inline TYPE * //if pass in null, we just repeat back at the beginning ... err uhm end
    get_prev_looped(TYPE *ptr) {
        if (!ptr) return get_tail();
        else      return get_prev(ptr);
    }
    
    //returns null if doesn't exist at index
    TYPE *get_at_index(s32 index) {
        TYPE *head = get_head();
        if (!head) return null;
        
        TYPE *result = head;
        while (index--) {
            result = get_next(result);
            if (!result) break;
        }
        return result;
    }
};

template <typename TYPE, s32 NODES_MEMBER_OFFSET>  inline void
init_sentinel(DLL_Sentinel<TYPE, NODES_MEMBER_OFFSET> *sentinel) {
    sentinel->init();
} 



template <typename TYPE>
inline TYPE *
linear_search(TYPE *array, s32 max_count, boolint (*is_match)(TYPE *e)) {
    for (s32 i = 0; i < max_count; i += 1) {
        if (is_match(array + i)) return array + i;
    }
    return null;
}

#define LINEAR_SEARCH(result, array, max_count, ...) \
        do for (s32 __i = 0; __i < (max_count); __i += 1) { \
        auto _ = (array) + __i; \
        if ( __VA_ARGS__ ) { \
                result = _; \
                break; \
        } \
        } while(0)

#define LINEAR_SEARCH_ARRAY(result, array, ...)   LINEAR_SEARCH(result, (array).e, (array).for_array_get_count(), __VA_ARGS__)
#define LINEAR_SEARCH_C_ARRAY(result, array, ...) LINEAR_SEARCH(result, (array), countof(array), __VA_ARGS__)

template <typename TYPE> 
struct Array_View {
    TYPE *e;
    s32 count;
    
    boolint does_contain(TYPE *elem) {
        return (elem >= e && elem < (e + count));
    }
    
    s32 get_index(TYPE *elem) {
        assert (does_contain(elem));
        return (s32)(elem - e);
    }
    
    inline s32 for_array_get_count() { //for for_array_forward() macro
        assert (count >= 0);
        if_assert((count>0), e);
        return count;
    }
    
    TYPE *get_first() {
        assert (count >= 0);
        return (count > 0) ? e : null;
    }
    
    TYPE *get_last() {
        assert (count >= 0);
        return (count > 0) ? (e + count - 1) : null;
    }
    
    
    
    TYPE *remove_unordered(TYPE *elem) {
        assert (elem && (count > 0));
        assert (elem >= e);
        assert (elem < (e + count));
         
        TYPE *last = e + --count;
        SWAP(*elem, *last);
        return last;
    }
    
    void insertion_sort(s32 (*compare)(TYPE *entry)) {
        insertion_sort1(e, count, compare);
    } 
    void insertion_sort(f32 (*compare)(TYPE *entry)) {
        insertion_sort1(e, count, compare);
    } 
    void insertion_sort(s32 (*compare)(TYPE *entry, void *userdata), void *userdata) {
        insertion_sort1(e, count, compare, userdata);
    } 
    void insertion_sort(f32 (*compare)(TYPE *entry, void *userdata), void *userdata) {
        insertion_sort1(e, count, compare, userdata);
    } 
    void insertion_sort(s32 (*compare)(TYPE *prev, TYPE *next)) {
        insertion_sort2(e, count, compare);
    } 
    void insertion_sort(f32 (*compare)(TYPE *prev, TYPE *next)) {
        insertion_sort2(e, count, compare);
    } 
    
    TYPE &operator[](s32 index) {   
        assert (count >= 0);
        assert (index >= 0 && index < count);    
        return e[index];    
    }
    
    TYPE *operator+(s32 offset) { 
        assert (count >= 0);
        assert (offset >= 0 && offset < count);    
        return e + offset;    
    }
};

template <typename TYPE, s32 SIZE> 
inline Array_View<TYPE>
to_view(TYPE (&array)[SIZE]) {
    Array_View<TYPE> view = {array, SIZE};
    return view;
}

template <typename TYPE> 
inline Array_View<TYPE>
to_view(TYPE *array, s32 size) {
    Array_View<TYPE> view = {array, size};
    return view;
}

template <typename TYPE> 
internal Array_View<TYPE>
make_array_view(Memory_Arena *arena, s32 count) {
    Array_View<TYPE> result = {};
    if (count > 0) {
        result.e = push_array(arena, TYPE, count);
        if (result.e) result.count = count;
    }
    return result;
}

template <typename TYPE> 
internal Array_View<TYPE>
make_array_view_memzero(Memory_Arena *arena, s32 count) {
    Array_View<TYPE> result = {};
    if (count > 0) {
        result.e = push_array_memzero(arena, TYPE, count);
        if (result.e) result.count = count;
    }
    return result;
}






template <typename TYPE, s32 MAX_COUNT> 
struct Static_Array {
    static constexpr s32 max_count = MAX_COUNT;
    TYPE e[max_count];
    
    
    boolint does_contain(TYPE *elem) {
        return (elem >= e && elem < (e + count));
    }
    
    s32 get_index(TYPE *elem) {
        assert (does_contain(elem));
        return (s32)(elem - e);
    }
    
    inline TYPE *for_array_assert_contains(TYPE *elem) {
        assert (does_contain(elem));
        return elem;
    }
    
    inline s32 for_array_get_count() {
        return MAX_COUNT;
    }
    
    TYPE &operator[](s32 index) {   
        assert (index >= 0 && index < max_count);    
        return e[index];    
    }
    
    TYPE *operator+(s32 offset) {   
        assert (offset >= 0 && offset < max_count);    
        return e + offset;    
    }
};


template <typename TYPE, s32 MAX_COUNT>
struct Stack {
    s32 count; 
    static constexpr s32 max_count = MAX_COUNT;
    TYPE e[max_count]; 
    
    boolint does_contain(TYPE *elem) {
        assert (count >= 0 && count <= max_count);
        return (elem >= e && elem < (e + count));
    }
    
    boolint does_contain(s32 index) {
        assert (count >= 0 && count <= max_count);
        return (index >= 0 && index < count);
    }
    
    inline TYPE *for_array_assert_contains(TYPE *elem) {
        assert (does_contain(elem));
        return elem;
    }
    
    inline s32 for_array_get_count() {
        return count;
    }
    
    s32 get_index(TYPE *elem) {
        assert (does_contain(elem));
        return (s32)(elem - e);
    }
    
    s32 get_count_remaining() {  //TODO I don't like this name 
        assert (max_count >= 0);
        assert (count >= 0 && count <= (max_count));    
        return (max_count) - count;    
    }   
    
    uintptr get_current_size() {
        assert (count >= 0 && count <= (max_count)); 
        return sizeof(TYPE) * count;
    }
    
    uintptr get_reserved_size() {
        assert (count >= 0 && count <= (max_count)); 
        return sizeof(e);
    }
    
    TYPE *add() {  
        assert (count >= 0 && count <= (max_count));    
        return (count < max_count) ? (e + count++) : 0;    
    } 
    TYPE *add(TYPE *var) {
        TYPE *result = add();
        memcopy(result, var, sizeof(TYPE));
        return result;   
    } 
    TYPE *add(TYPE var) {
        TYPE *result = add();
        memcopy(result, &var, sizeof(TYPE));
        return result;  
    } 
    
    TYPE *add_multiple(s32 amount_to_add) {
        assert (count >= 0 && count <= (max_count)); 
        TYPE *result = null;
        if ((count + amount_to_add) <= max_count) {
            result = e + count;
            count += amount_to_add;
        }
        return result;
    }
    
    TYPE *insert_ordered(s32 index) { //maintains order by shifting all the elements up
        assert (count >= 0 && count <= max_count);
        assert (index >= 0 && index <= count); //<= because we may want to insert at new last element
        if (count < max_count) {
            count += 1;
            //TODO can we use a memcpy here or will rep movsb be a problem?...
            for(s32 at = count - 1; at > index; at -= 1)
                e[at] = e[at - 1];
            return e + index;    
        } else return null;
    }
    
    void insert_ordered(TYPE var, s32 index) {
        *insert_ordered(index) = var;
    }
    
    TYPE *insert_unordered(s32 index) { //doesn't maintain order by simply swapping with current last element
        assert (count >= 0 && count <= max_count);
        assert (index >= 0 && index <= count); //<= because we may want to insert at new last element
        if (count < max_count) {
            *(e + count++) = e[index];
            return e + index;    
        } else return null;
    }
    
    TYPE *remove_last() {     
        assert (count >= 0 && count <= (max_count));    
        return (count > 0) ? (e + --count) : 0;    
    } 
    
    TYPE *remove_unordered(s32 index) { /*NOTE this moves last element to index*/       
        assert (count >= 0 && count <= (max_count));    
        assert (index >= 0 && index < count);    
        SWAP(e[index], e[count - 1]);    
        return remove_last();
        }
    
    TYPE *remove_unordered(TYPE *element) { 
        return remove_unordered(get_index(element));
    }
    
    TYPE *remove_ordered(s32 index) {
        assert (count >= 0 && count <= (max_count));    
        assert (index >= 0 && index < count);    

        TYPE copy = e[index];
        for (s32 i = index; i < (count-1); i += 1) {
            memcopy(e + i, e + i+1, sizeof(TYPE));
        }
        memcopy(e + count-1, &copy, sizeof(TYPE));
        return e + --count;
    }
    
    inline TYPE *get_first() {
        assert (count >= 0 && count <= max_count);
        return (count > 0) ? e : null; 
    }
    
    inline TYPE *get_last() {
        assert (count >= 0 && count <= max_count);
        return (count > 0) ? (e + count-1) : null; 
    }
    
    void insertion_sort(s32 (*compare)(TYPE *entry)) {
        insertion_sort1(e, count, compare);
    } 
    void insertion_sort(u32 (*compare)(TYPE *entry)) {
        insertion_sort1(e, count, compare);
    } 
    void insertion_sort(f32 (*compare)(TYPE *entry)) {
        insertion_sort1(e, count, compare);
    } 
    void insertion_sort(s32 (*compare)(TYPE *entry, void *userdata), void *userdata) {
        insertion_sort1(e, count, compare, userdata);
    } 
    void insertion_sort(u32 (*compare)(TYPE *entry, void *userdata), void *userdata) {
        insertion_sort1(e, count, compare, userdata);
    } 
    void insertion_sort(f32 (*compare)(TYPE *entry, void *userdata), void *userdata) {
        insertion_sort1(e, count, compare, userdata);
    } 
    void insertion_sort(s32 (*compare)(TYPE *prev, TYPE *next)) {
        insertion_sort2(e, count, compare);
    } 
    void insertion_sort(f32 (*compare)(TYPE *prev, TYPE *next)) {
        insertion_sort2(e, count, compare);
    } 
    
    TYPE &operator[](s32 index) {
        assert (count >= 0 && count <= (max_count));    
        assert (index >= 0 && index < count);    
        return e[index];    
    }
    
    TYPE *operator+(s32 offset) {
        assert (count >= 0 && count <= (max_count));    
        assert (offset >= 0 && offset < count);    
        return e + offset;    
    }
};



template <typename TYPE, s32 SIZE> 
internal Stack<TYPE, SIZE>
make_stack_from_array(TYPE (&array)[SIZE]) {
    Stack<TYPE, SIZE> stack = {SIZE};
    cassert(sizeof(stack.e) == sizeof(array));
    memcopy(&stack.e, array, sizeof(array));
    return stack;
}


enum Allocator_Type : u32 {
    ALLOCATOR_NONE,
    ALLOCATOR_MEMORY_ARENA,
    ALLOCATOR_HEAP,
};

//NOTE sorts by ascending numbers
template <typename TYPE, typename GET_NUM>
internal void
insertion_sort1(TYPE *array, s32 array_count, GET_NUM get_sort_num) {
    for (s32 index = 0; index < array_count; index += 1) {
        TYPE *elem = array + index;
        TYPE *prev = elem - 1;
        if (prev < array) prev = 0;
        
        while (prev) {
            if (get_sort_num(prev) > get_sort_num(elem)) {
                SWAP(*prev, *elem);
                elem = prev;
                prev = elem - 1;
                if (prev < array)       prev = 0;
            } else break;
        }
    }
}

//NOTE sorts by ascending numbers
template <typename TYPE, typename GET_NUM>
internal void
insertion_sort1(TYPE *array, s32 array_count, GET_NUM get_sort_num, void *userdata) {
    for (s32 index = 0; index < array_count; index += 1) {
        TYPE *elem = array + index;
        TYPE *prev = elem - 1;
        if (prev < array) prev = 0;
        
        while (prev) {
            if (get_sort_num(prev, userdata) > get_sort_num(elem, userdata)) {
                SWAP(*prev, *elem);
                elem = prev;
                prev = elem - 1;
                if (prev < array)       prev = 0;
            } else break;
        }
    }
}


//NOTE COMPARE(prev, next) is fucking confusing but we do this to conform to c/c++ sorting standards
// return negative -> prev is BEFORE next
// return positive -> prev is AFTER next
template<typename TYPE, typename COMPARE>
internal void
insertion_sort2(TYPE *array, s32 array_count, COMPARE compare) {
    for (s32 index = 0; index < array_count; index += 1) {
        TYPE *elem = array + index;
        TYPE *prev = elem - 1;
        if (prev < array)       prev = 0;
        
        while (prev) {
            if (compare(prev, elem) > 0) {
                SWAP(*prev, *elem);
                elem = prev;
                prev = elem - 1;
                if (prev < array)       prev = 0;
            }
            else break;
        }
    }
}

//NOTE COMPARE(prev, next) is fucking confusing but we do this to conform to c/c++ sorting standards
// return negative -> prev is BEFORE next
// return positive -> prev is AFTER next
template<typename TYPE, typename COMPARE>
internal void 
insertion_sort2(TYPE *array, s32 array_count, COMPARE compare, void *userdata) {
    for (s32 index = 0; index < array_count; index += 1) {
        TYPE *elem = array + index;
        TYPE *prev = elem - 1;
        if (prev < array)       prev = 0;
        
        while (prev) {
            if (compare(prev, elem, userdata) > 0) {
                SWAP(*prev, *elem);
                elem = prev;
                prev = elem - 1;
                if (prev < array)       prev = 0;
            }
            else break;
        }
    }
}

template <typename TYPE> 
struct Array {
    s32 count;
    s32 max_count;
    TYPE *e;

    bool32 can_realloc; //can be flags if needed
    Allocator_Type allocator_type;
    void *allocator;

    void set_allocator(Memory_Arena *arena) {
        assert (arena);
        allocator = arena;
        allocator_type = ALLOCATOR_MEMORY_ARENA;
    }
    
    s32 get_count_remaining() {    
        assert (count >= 0 && count <= (max_count));    
        return max_count - count;    
    }    
    
    uintptr get_current_size() {
        assert (count >= 0 && count <= (max_count)); 
        return sizeof(TYPE) * count;
    }
    
    uintptr get_reserved_size() {
        assert (count >= 0 && count <= (max_count)); 
        return sizeof(TYPE) * max_count;
    }
    
    boolint does_contain(TYPE *elem) {
        assert (count >= 0 && count <= max_count);
        return (elem >= e && elem < (e + count));
    }
    
    //check if element is within the array and return it
    inline TYPE *for_array_assert_contains(TYPE *elem) {
        assert (does_contain(elem));
        return elem;
    }
    
    inline s32 for_array_get_count() {
        //assert (e);
        return count;
    }
    
    s32 get_index(TYPE *elem) {
        assert (does_contain(elem));
        return (s32)(elem - e);
    }

    boolint reserve(s32 reserve_count); //returns true if successfuly reserved at least reserve_count

    TYPE *add() {
        assert (count >= 0 && count <= max_count);
        if (count < max_count) {
            assert (e);
            return e + count++;   
        } else {
            if (max_count > 0) {
                assert ((2*max_count) > max_count); //overflow check
                if (reserve(2*max_count)) {
                    assert (count < max_count);
                    return e + count++; 
                } else return null;   
            } else { //initialize with default size
                if (reserve(32)) {
                    assert (count < max_count);
                    return e + count++; 
                } else return null; 
            }
        }
    }
    
    TYPE *add(TYPE *var) {    
        TYPE *result = add();
        if (result) memcopy(result, var, sizeof(*var));
        return result;
    }
    
    TYPE *add(TYPE var) {  
        return add(&var);
    }
    
    TYPE *add_array(s32 request_count) {
        TYPE *result = null;
        if ((count + request_count) <= max_count) {
            result = e + count;
            count += request_count;
        } else {
            s32 reserve_amt = max2(32, max_count);
            while ((reserve_amt-count) < request_count) {
                s32 next_reserve_amt = 2*reserve_amt;
                assert (next_reserve_amt > reserve_amt); //overflow check
                reserve_amt = next_reserve_amt;
            }
            
            if (reserve(reserve_amt)) {
                assert ((max_count-count) >= request_count);
                result = e + count;
                count += request_count;
            }
        } 
        
        return result;
    }
    
    Array_View<TYPE> to_view() {
        Array_View<TYPE> view = {};
        view.e = e;
        view.count = count;
        return view;
    }
    
    #if 0 //TODO
    TYPE *add_multiple(s32 count_to_push) {
        assert (count >= 0 && count <= (max_count)); 
        if ((count + count_to_push) <= max_count) {
            TYPE *result = e + count;
            count += amount_to_push;
            return result;
        } else return null;
    }
    #endif
    
    TYPE *insert_ordered(s32 index) { //maintains order by shifting all the elements up
        assert (count >= 0 && count <= max_count);
        assert (index >= 0 && index <= count); //<= because we may want to insert at new last element
        if (count < max_count) {
            count += 1;
            //TODO can we use a memcpy here or will rep movsb be a problem because of overlapped regions?...
            for (s32 at = count - 1; at > index; at -= 1) {
                e[at] = e[at - 1];
            }
            return e + index;    
        } else return null;
    }
    
    TYPE *insert_ordered(s32 index, TYPE var) { //maintains order by shifting all the elements up
        TYPE *result = insert_ordered(index);
        if (result) {
            memcopy(result, &var, sizeof(var)); 
        }
        return result;
    }
    
    TYPE *insert_unordered(s32 index) { //doesn't maintain order by simply swapping with current last element
        assert (count >= 0 && count <= max_count);
        assert (index >= 0 && index <= count); //<= because we may want to insert at new last element
        if (count < max_count) {
            *(e + count++) = e[index];
            return e + index;    
        } else return null;
    }
    
    TYPE *get_last() {
        assert (count >= 0 && count <= (max_count));
        return (count > 0) ? (e + count-1) : 0; 
    }
    
    TYPE *remove_last() {     
        assert (count >= 0 && count <= (max_count));    
        return (count > 0) ? (e + --count) : 0;    
    }
    
    TYPE *remove_unordered(s32 index) {/*NOTE this moves last element to index*/        
        assert (count >= 0 && count <= (max_count));    
        assert (index >= 0 && index < count);    
        SWAP(e[index], e[count - 1]);    
        return remove_last();    
    }
    
    TYPE *remove_unordered(TYPE *element) { 
        return remove_unordered(get_index(element));
    }
    
    TYPE *remove_ordered(s32 index) {
        assert (count >= 0 && count <= (max_count));    
        assert (index >= 0 && index < count);    
        s32 amount_to_shift = count - index - 1;
        if (amount_to_shift > 0) {
            TYPE copy = e[index];
            #if 0
            // TODO I don't know whether rep movsb is safe here since they're overlapping but not in a case where data will be overwritten to lead to corruption
            // yet if the pointers are so close, how much faster could this really be?...
            memcopy(e + index, e + index + 1, sizeof(TYPE)*amount_to_shift);
            #else
            for (s32 i = index; index < (count-1); index += 1) {
                e[i] = e[i+1];
            }
            #endif
            
            e[count-1] = copy;
            return e + --count;
        } else {
            SWAP(e[index], e[count - 1]);        
            return remove_last();       
        }
    }
    

    void insertion_sort(s32 (*compare)(TYPE *entry)) {
        insertion_sort1(e, count, compare);
    } 
    void insertion_sort(f32 (*compare)(TYPE *entry)) {
        insertion_sort1(e, count, compare);
    } 
    void insertion_sort(s32 (*compare)(TYPE *entry, void *userdata), void *userdata) {
        insertion_sort1(e, count, compare, userdata);
    } 
    void insertion_sort(f32 (*compare)(TYPE *entry, void *userdata), void *userdata) {
        insertion_sort1(e, count, compare, userdata);
    } 
    void insertion_sort(s32 (*compare)(TYPE *prev, TYPE *next)) {
        insertion_sort2(e, count, compare);
    } 
    void insertion_sort(f32 (*compare)(TYPE *prev, TYPE *next)) {
        insertion_sort2(e, count, compare);
    } 
    
    TYPE &operator[](s32 index) {
        assert (count >= 0 && count <= (max_count));    
        assert (index >= 0 && index < count);    
        return e[index];    
    }
    
    TYPE *operator+(s32 offset) {
        assert (count >= 0 && count <= (max_count));    
        assert (offset >= 0 && offset < count);    
        return e + offset;    
    }
}; 

template <typename TYPE> 
internal Array_View<TYPE>
make_array_view(Array<TYPE> array) {
    Array_View<TYPE> view;
    view.e = array.e;
    view.count = array.count;
    if_assert(view.count > 0, view.e);
    return view;
}



template <typename TYPE, u32 SIZE>
internal Array<TYPE>
make_array_from_buffer(TYPE (&buffer)[SIZE]) {
    Array<TYPE> slice = {};
    slice.e = buffer;
    slice.max_count = SIZE;
    return slice;
}

template <typename TYPE>
internal Array<TYPE>
make_array(Memory_Arena *arena, u32 max_count = 0) {
    align_next_allocation(arena, sizeof(uintptr)); //TODO alignment should be passed into reserve or have a member set on the array...
    Array<TYPE> array = {};
    array.set_allocator(arena);
    array.reserve(max_count);
    return array;
}

template <typename TYPE>
internal Array<TYPE>
make_array_and_memzero(Memory_Arena *arena, u32 max_count = 0) {
    Array<TYPE> array = {};
    array.set_allocator(arena);
    if ((max_count > 0) && array.reserve(max_count)) {
        memzero(array.e, sizeof(TYPE)*max_count);
    }
    return array;
}



template <typename TYPE, s32 MAX_COUNT>
struct Stack_Front_Back 
{
    s32 front_count; 
    s32 back_count;
    TYPE e[MAX_COUNT]; 
    
    static constexpr s32 max_count = MAX_COUNT;
    
    s32 left() {
        s32 result = max_count - front_count - back_count;
        assert (result >= 0 && result <= max_count);
        return result;
    }
    
    TYPE *front_push()
    {
        TYPE *result = 0;
        if (left())
            result = e + front_count++;
        return result;
    }
    
    TYPE *front_pop()
    {
        TYPE *result = 0;
        if (front_count > 0)
            result = e + --front_count;
        return result;
    }
    
    TYPE *back_push() {
        TYPE *result = 0;
        if (left()) result = (e + max_count) - ++back_count;
        return result;
    }
    
    TYPE *back_push(TYPE *ptr) {
        TYPE *result = 0;
        if (left()) {
            result = (e + max_count) - ++back_count;
            *result = *ptr;
        }
        return result;
    }
    
    TYPE *back_push(TYPE value) {
        back_push(&value);
    }
    
    TYPE *back_pop()
    {
        TYPE *result = 0;
        if (back_count > 0)
            result = (e + max_count) - back_count++;
        return result;
    }
    
    //I doubt we'll ever need this one honestly...
    //TYPE *remove(s32 index)/*NOTE this moves last element to index*/   
        //{
        //if (index >= 0 && index < front_count)
        //{
            //SWAP(e[index], e[front_count - 1]);        
            //return front_pop();       
        //}
        //else
        //{
            //assert (index >= (max_count - back_count)  &&  index < max_count);
            //SWAP(e[index], e[max_count - back_count]);        
            //return back_pop();        
        //}
        //}
    
    TYPE *front_remove(s32 index)
    {
        assert (index >= 0 && index < front_count);
        SWAP(e[index], e[front_count - 1]);      
        return front_pop();     
    }
    
    TYPE *back_remove(s32 index)
    {
        assert (index >= (max_count - back_count)  &&  index < max_count);
        SWAP(e[index], e[max_count - back_count]);      
        return back_pop();              
    }
    
    
    TYPE &operator[](s32 index)
    {
        assert ((index >= 0 && index < front_count) ||
                (index >= (max_count - back_count) && index < max_count));
        return e[index];    
    }
    TYPE *operator+(s32 index)
    {
        assert ((index >= 0 && index < front_count) ||
                (index >= (max_count - back_count) && index < max_count));
        return e + index;    
    }
};

internal boolint
find_least_significant_bit_index(u32 bits, s32 *index) {
    if (_BitScanForward((unsigned long *)index, bits)) {
        return true;
    }    
    else return false;
}

internal boolint
find_least_significant_bit_index(u64 bits, s32 *index) {
    if (_BitScanForward64((unsigned long *)index, bits)) {
        return true;
    }    
    else return false;
}

internal s32
get_num_bits_set(u64 x) {
    #if 1
    s32 count = 0;
    for (s32 i = 0; i < 64; i += 1) {
        if (x & (1ull << i)) count += 1;
    }
    return count;
    #else
    //TODO we need to check cpu flags to see if we have this instruction
    unsigned __int64 result = __popcnt64(x);
    assert (result <= S32_MAX);
    return (s32) result;
    #endif
    
}

struct Bit_Array_View {
    u64 *bitchunk;
    s32 bitcount; //number of bits
    s32 chunk_count; //number of u64 chunks we have
    
    struct Index_Pair {
        s32 chunk_index;
        s32 index;
    };
    
    Index_Pair get_index_pair(s32 bit) {
        assert (bit >= 0 && bit < bitcount);
        Index_Pair pair;
        pair.chunk_index = bit / 64;
        pair.index = bit - (pair.chunk_index*64);
        assert (pair.chunk_index < chunk_count);
        return pair;
    }
    
    boolint is_set(s32 bit) {
        Index_Pair p = get_index_pair(bit);
        //nil result = (bitchunk[p.chunk_index] & p.index) != 0; //fuck how long have I had this bug for?....
        boolint result = (bitchunk[p.chunk_index] & ((u64)1 << p.index));
        return result;
    }
};

//TODO make a SIMD-wide version of this
//you can also specify the size of the bitchunk and do 8*sizeof() to get bit counts 
template <s32 BITCOUNT>
struct Bit_Array64 {
    u64 bitchunk[(BITCOUNT + 63) / 64];
    
    inline Bit_Array_View
    to_view() {
        Bit_Array_View view = {};
        view.bitchunk = bitchunk;
        view.bitcount = BITCOUNT;
        view.chunk_count = (BITCOUNT + 63) / 64;
        return view;
    }
    
    struct Index_Pair {
        s32 chunk_index;
        s32 index;
    };
    
    Index_Pair get_index_pair(s32 bit) {
        assert (bit >= 0 && bit < BITCOUNT);
        Index_Pair pair;
        pair.chunk_index = bit / 64;
        pair.index = bit - (pair.chunk_index*64);
        return pair;
    }
    
    s32 get_num_bits_set() {
        cassert (BITCOUNT <= S32_MAX); //otherwise return an s64
        s32 count = 0;
        for (s32 i = 0; i < countof(bitchunk); i += 1) {
            count += get_num_bits_set(bitchunk[i]);
        }
        return count;
    }
    
    void set_all_bits() {
        set_memory(bitchunk, 0xff, sizeof bitchunk);
    } 
    
    void clear_all_bits() {
        memzero(bitchunk, sizeof bitchunk);
    } 
    
    void clear_bit(s32 bit) {
        Index_Pair p = get_index_pair(bit);
        bitchunk[p.chunk_index] &= ~((u64)1 << p.index);
    }
    
    void set_bit(s32 bit) {
        Index_Pair p = get_index_pair(bit);
        bitchunk[p.chunk_index] |= ((u64)1 << p.index);
    }
    
    void toggle_bit(s32 bit) {
        Index_Pair p = get_index_pair(bit);
        bitchunk[p.chunk_index] ^= ((u64)1 << p.index);
    }
    
    bool is_set(s32 bit) {
        Index_Pair p = get_index_pair(bit);
        //nil result = (bitchunk[p.chunk_index] & p.index) != 0; //fuck how long have I had this bug for?....
        nil result = (bitchunk[p.chunk_index] & ((u64)1 << p.index)) != 0;
        return result;
    }
        
    
    boolint find_least_significant_bit(s32 *bit_index) {
        #if (ARCH == ARCH_X64) || (ARCH == ARCH_ARM64)
        constexpr s32 bitchunk_count = sizeof(bitchunk) / sizeof(bitchunk[0]);
        //constexpr s32 bitchunk_count = countof(bitchunk); //not sure why doesn't like this?...
        for (s32 i = 0; i < bitchunk_count; i += 1) {
            unsigned long found_bit_index;
            if (_BitScanForward64(&found_bit_index, bitchunk[i])) {
                found_bit_index += i*64;
                if (found_bit_index < BITCOUNT) {
                    *bit_index = (s32)found_bit_index;
                    return true;
                }
                else break; //no use to keep searching
            }
        }
        #else
        #error "Gotta you a smaller chunk size or implement this yourself"
        #endif
        return false;
    }
};

template <typename TYPE, u32 MAX_COUNT>
struct Stack_With_Free_List {
    s32 count;
    s32 high_water_mark;
    TYPE *freelist;
    TYPE e[MAX_COUNT];
    static constexpr s32 max_count = MAX_COUNT;
    
    nil does_contain(TYPE *elem){
        return (elem >= e) && (elem < (e + MAX_COUNT));
    }
    
    TYPE *push() {
        TYPE *result = null;
        
        if (freelist) {
            result   = freelist;
            freelist = freelist->next_in_freelist;
            count += 1;
        } else if (count < MAX_COUNT) {
            result = e + count++;
        }
        
        high_water_mark = max2(high_water_mark, count);
        return result;
    }
    
    void *free(TYPE *elem) { //we could check if it's last element in array to avoid adding to freelist but why?...
        assert (does_contain(elem));
        elem->next_in_freelist = freelist;
        freelist = elem;
        count -= 1;
    }
};

#define GENERIC_PUSH_STRUCT(alloc_type, ptr, type)       (type *)generic_push_size(alloc_type, ptr, sizeof(type))
#define GENERIC_PUSH_ARRAY(alloc_type, ptr, type, count) (type *)generic_push_size(alloc_type, ptr, sizeof(type)*count)

internal void *
generic_push_size(Allocator_Type alloc_type, void *allocator, u32 size) {
    switch (alloc_type) {
    case ALLOCATOR_MEMORY_ARENA: {
        return push_size((Memory_Arena *)allocator, size);
    } break;
    
    default: assert (0); return null;
    }
}

//TODO eventually allow user to pass in a free_list type thing and just have a common interface to use these things
#if 1
template <typename TYPE, s32 BUCKET_SIZE>
struct Bucket_Free_List {
    TYPE *head;
    
    //NOTE this call is slowww
    inline boolint is_in_freelist(TYPE *array, TYPE *e) {
        for (TYPE *at = head; at; at = at->next_in_free_list) {
            if (at == e) return true;
        }
        return false;
    }
    
    inline TYPE *add_to_freelist(TYPE *array, TYPE *e) {
        assert (array);
        assert ((e >= array) && (e < (array+BUCKET_SIZE)));
        assert (e->next_in_free_list == null);
        e->next_in_free_list = head;
        head = e;
        return e;
    }
    
    inline TYPE *get_next_free(TYPE *array) {
        TYPE *result = null;
        if (head) {
            result = head;
            head = head->next_in_free_list;
            assert ((result >= array) && (result < (array+BUCKET_SIZE))); //ensure belogns to our array
        }
        return result;
    }
};

template <typename TYPE, s32 BUCKET_SIZE>
struct Bucket_Free_Indices {
    Bit_Array64<BUCKET_SIZE> free_indices;
    
    inline boolint is_in_freelist(TYPE *array, TYPE *e) {
        assert ((e >= array) && (e < (array+BUCKET_SIZE))); //ensure belogns to our array
        s32 e_index = (s32)(e - array);
        return free_indices.is_set(e_index);
    }
    
    inline TYPE *add_to_freelist(TYPE *array, TYPE *e) {
        assert (!is_in_freelist(array, e));
        assert ((e >= array) && (e < (array+BUCKET_SIZE))); //ensure belogns to our array
        
        s32 e_index = (s32)(e - array);
        assert (!free_indices.is_set(e_index));
        free_indices.set_bit(e_index);
        return e;
    }
    
    inline TYPE *get_next_free(TYPE *array) {
        s32 free_index;
        if (free_indices.find_least_significant_bit(&free_index)) {
            free_indices.clear_bit(free_index);
            return array + free_index;
        } else return null;
    }
};

template <typename TYPE, s32 BUCKET_SIZE>
struct Bucket_Free_Nothing {
    inline boolint is_in_freelist(TYPE *array, TYPE *e) {
        return false;
    }
    
    inline TYPE *add_to_freelist(TYPE *array, TYPE *e) {
        return null;
    }
    
    inline TYPE *get_next_free(TYPE *array, TYPE *e) {
        return null;
    }
};


//bucket is just an array with an optinal freeing mechanism
//user is expected to mark elements as free/active
template <typename TYPE, s32 BUCKET_SIZE, 
/**/      template<typename TYPE, s32 BUCKET_SIZE> typename Free_Struct = Bucket_Free_Indices>
struct Bucket {
    s32 count;
    s32 active_count;
    TYPE *e;
    
    Free_Struct<TYPE, BUCKET_SIZE> free;
    static constexpr s32 max_count = BUCKET_SIZE;
    cassert(BUCKET_SIZE > 0);
    
    boolint contains(TYPE *elem) {
        return (elem >= e) && (elem < (e + count));
    }
    
    s32 get_index(TYPE *elem) {
        assert (count >= 0);
        assert (e);
        assert ((elem >= e) && (elem < (e + count)));
        s32 index = (s32)(elem - e);
        return index;
    }
    
    TYPE *add() {
        TYPE *result = free.get_next_free(e);
        if (!result && (count < max_count)) {
            assert (e);
            result = e + count++;
        }
        
        active_count += result ? 1 : 0;
        return result;
    }
    
    void remove_unordered(TYPE *elem) {
        assert (count >= 0);
        assert (contains(elem));
        SWAP(e[count-1], *elem);
        count -= 1;
        
        assert (active_count > 0);
        active_count -= 1;
    }
    
    void remove_ordered(TYPE *elem) {
        free.add_to_freelist(e, elem);
        //NOTE I'm not sure if we should decrement this if we don't specify a freeing mechanism
        //but I think we should because the user could just be using an array+holes technique in which
        //case knowing how many holes (count - active_count) may be useful...
        assert (active_count > 0);
        active_count -= 1;
    }
};

template <typename TYPE, s32 BUCKET_SIZE, s32 MAX_BUCKET_COUNT,
/**/      template<typename TYPE, s32 BUCKET_SIZE> typename Free_Struct = Bucket_Free_Indices>
struct Bucket_Array {
    typedef Bucket<TYPE, BUCKET_SIZE, Free_Struct> BUCKET;
    Array<BUCKET> buckets;
    
    void set_allocator(Memory_Arena *arena) {
        buckets.set_allocator(arena);
    }
    
    boolint reserve(s32 max_elem_count) {
        s32 target_max_bucket_count = (max_elem_count + BUCKET_SIZE-1) / BUCKET_SIZE;
        s32 num_buckets_to_allocate = target_max_bucket_count - buckets.max_count; 
        if (buckets.reserve(target_max_bucket_count)) {
            assert (num_buckets_to_allocate > 0);
            
            TYPE *new_elements = GENERIC_PUSH_ARRAY(buckets.allocator_type, buckets.allocator, TYPE, num_buckets_to_allocate * BUCKET_SIZE);
            if (new_elements) {
                s32 at = 0;
                for_index_inc (s32, i, num_buckets_to_allocate) {
                    BUCKET *bucket = buckets.add(); 
                    memzero(bucket, sizeof *bucket);
                    bucket->e = new_elements + at;
                    at += BUCKET_SIZE;
                }    
                return true;
            } else {
                return false;
            }
        } else {
            return false;   
        }
    }
    
    TYPE *add() {
        for_array_forward (bucket, buckets) {
            elem = bucket->add();
            if (elem) return elem;
        }
        
        BUCKET *new_bucket = buckets.add();
        if (new_bucket) {
            memzero(new_bucket, sizeof *new_bucket);
            TYPE *new_elements = GENERIC_PUSH_ARRAY(buckets.allocator_type, buckets.allocator, TYPE, BUCKET_SIZE);
            if (new_elements) {
                new_bucket->e = new_elements;
                return new_bucket->add();
            }
        }
        
        return null;
    }
    
    inline TYPE *add(TYPE *ptr) {
        TYPE *slot = add();
        memcopy(slot, ptr, sizeof *ptr);
    }
    
    inline TYPE *add(TYPE value) {
        return add(&value);
    }
    
    void remove_unordered(TYPE *elem) {
        boolint did_remove = false;
        for_array_forward (bucket, buckets) {
            assert (bucket->e);
            if (bucket->contains(elem)) {
                bucket->remove_unordered(elem);
                did_remove = true;
            }
        }
        assert (did_remove);
    }
    
    void remove_to_freelist(TYPE *elem) {
        boolint did_remove = false;
        for_array_forward (bucket, buckets) {
            assert (bucket->e);
            if (bucket->contains(elem)) {
                bucket->remove_to_freelist(elem);
                did_remove = true;
            }
        }
        assert (did_remove);
    }
    
};
#endif



#if 0
template <typename TYPE, s32 BUCKET_SIZE>
struct Bucket {
    s32 count;
    Bit_Array64<BUCKET_SIZE> free_indices;
    TYPE e[BUCKET_SIZE];
    Bucket<TYPE, BUCKET_SIZE> *next_bucket;
};

template <typename TYPE, s32 BUCKET_SIZE, s32 MAX_BUCKET_COUNT>
struct Bucket_Array { //aka unordered_linked_list
    ::Bucket<TYPE, BUCKET_SIZE>       *freelist;
    Stack <::Bucket<TYPE, BUCKET_SIZE>, buckets;
    
    TYPE *add(Bucket **first_bucket) {
        Bucket *bucket = null;
        for (Bucket *bkt = *first_bucket; bkt; bkt = bkt->next_bucket) {
            if (bkt->free_indices) {
                bucket = bkt;
                break;
            }
        }
        
        if (!bucket) {
            if (freelist) {
                bucket = freelist;
                freelist = freelist->next_bucket;
            }
            else {
                bucket = buckets.push();
            }
            
            if (bucket) {
                bucket->next_bucket = *first_bucket;
                *first_bucket = bucket;
            }
        }
        
        TYPE *result = null;
        if (bucket) {
            s32 index_avail = bucket->free_indices.find_least_significant_bit();
            assert (index_avail >= 0);
            result = bucket->e + index_avail;
            bucket->count += 1;
            bucket->free_indices.clear_bit(index_avail); 
        }
        return result;
    }
    
    TYPE *remove() {
        
    }
};
#endif 

//never tested this yet
template<typename TYPE, typename KEYTYPE = u32>
internal TYPE *
radix_sort32(TYPE *elements, s32 array_count, TYPE *buffer)
{
    TYPE *src = elements;
    TYPE *dst = buffer;
    
    for (s32 byte_index = 0; byte_index < sizeof(KEYTYPE); byte_index += 1)
    {
        u32 offsets[256] = {};
        //count how many their are per each byte value
        for (s32 i = 0; i < array_count; i += 1)
        {
            KEYTYPE key = get_radix_key(src + i);
            u32 index = (key >> (8*byte_index)) & 0xff;
            offsets[index] += 1;
        }
        
        //convert counts into offsets
        u32 running_count = 0;
        for (s32 i = 0; i < 256; i += 1)
        {
            u32 count = offsets[i];
            offsets[i] = running_count;
            running_count += count;
        }
        
        for (s32 i = 0; i < array_count; i += 1)
        {
            KEYTYPE key = get_radix_key(src + i);
            u32 index = (key >> (8*byte_index)) & 0xff;
            dst[offsets[index]++] = src[i];
        }
        
        SWAP(src, dst);
    }
    
    assert (dst == elements); //often times buffer is temporary so we need to ensure
    //dst is not that otherwise we will be freeing that...
    return dst;
}





internal void *
copy_memory(void *destination, void *source, uintptr size);
#if 0
{
    //TODO intrinsic for this?
    u8 *dst = (u8 *)destination;
    u8 *src = (u8 *)source;
    assert (!ptr_in_base_size(src,        dst, size)); //ensure memory regions do not overlap
    assert (!ptr_in_base_size(src+size-1, dst, size)); //call move_memory() if they do overlap....
    for(uintptr i = 0; i < size; i += 1) 
        dst[i] = src[i];
    return dst;
}
#endif

internal void *
memzero(void *data, uintptr size);

internal void *
zero_memory(void *data, uintptr size);
#if 0
{
    assert (data && size > 0);
    //TODO intrinsic for this?
    u8 *at = (u8 *)data;
    for(uintptr i = 0; i < size; i += 1) at[i] = 0;
    return data;
}
#endif

#if 0 //TODO we have to write a SIMD version of this
internal void
move_memory(void *destination, void *source, uintptr size)
{
    u8 *dst = (u8 *)destination;
    u8 *src = (u8 *)source;
    if (dst < src)
    {
        // Copy forwards
        for(uintptr i = 0; i < size; i += 1) 
            dst[i] = src[i];    
    }
    else if (dst > src)
    {
        // Copy backwards
        for(uintptr i = size; (i--) > 0;        )
            dst[i] = src[i];
    }
    //else no point in copying...
}

internal nil //TODO write SIMD version of this
is_memory_equal(void *ap, void *bp, uintptr size)
{
    u8 *a = (u8 *)ap;
    u8 *b = (u8 *)bp;
    for(uintptr i = 0; i < size; i += 1)
        if (a[i] != b[i])       return false;
    return true;
}
#endif





enum Month : u32 {
    INVALID_MONTH, JANUARY, FEBUARY, MARCH, APRIL, MAY, JUNE, JULY, AUGUST, SEPTEMBER, OCTOBER, NOVEMBER, DECEMBER,
};

struct Date_Time {
    Month month;
    s32 day;
    s32 hour;
    s32 min;
    s32 sec;
    s32 year;
};



//// memory arenas

/* Some notes on memory management in operating systems

        Every process has it's own virtual memory space that maps into RAM. 
        This means (void *)0x80000 actually refers to different RAM region on different processes. This allows
        -privacy since each process touches it's own memory
        -reliability since a process can't corrupt another process's memory

        Mapping vram to ram is done in 4KB chunks called *pages*
        AMD64 cpus use 48bit addressing meaning we can represent up to 256TB of virtual memory, which is a lot of pages!!!
        Since we need to store mappings, this is done in some sparse structure

        *Commiting* memory basically 'reserves' physical pages. It increases some counter of phsycial pages used and the commit will
        fail if there aren't any more pages...
        *Commiting* != *allocating* memory!!! It just means when we allocate later on, we are gurannted to get memory back 
        since the commit check passed before

        NOTE all physical pages returned from the operating system are required to be zeroed out!!! Super nice!!!
*/

#define USING_MEMORY_ARENA(name, arena) Memory_Arena *name = &(arena); u32 __arena_mark__##name = name->used
#define RESTORE_MEMORY_ARENA(name)      name->used = __arena_mark__##name

#define RESTORE_MEMORY_ARENA_ON_SCOPE_EXIT(arena) let CONCAT(_savepoint, __LINE__) = get_save_point(arena); defer { restore(arena, CONCAT(_savepoint, __LINE__)); };
            

//just a simple linear allocator that can push and pop. We don't record allocations... 
//calls to pop() are expected to pass correct prev alloc size
struct Memory_Arena {
    u8 *data;
    u32 size;
    u32 used;
};

struct Memory_Arena_Save_Point {
    u32 point;
    s32 index;
};

internal Memory_Arena_Save_Point
get_save_point(Memory_Arena *arena) {
    Memory_Arena_Save_Point point = {};
    point.point = arena->used;
    return point;
}

internal boolint
contains_ptr(Memory_Arena *arena, void *ptr) {
    return ((uintptr)ptr >= (uintptr)arena->data) &&
           ((uintptr)ptr <  (uintptr)(arena->data + arena->used));
}

internal u32
get_byte_offset(Memory_Arena *arena, void *ptr) {
    assert (contains_ptr(arena, ptr));
    return (u32)((uintptr)(ptr) - (uintptr)arena->data);
}

internal void
restore(Memory_Arena *arena, Memory_Arena_Save_Point point) {
    assert (point.point <= arena->used);
    arena->used = point.point;
}

internal u32
get_size_remaining(Memory_Arena *arena) {
    assert (arena->used <= arena->size);
    return arena->size - arena->used;
}

internal Memory_Arena 
make_memory_arena(void *data, u32 size) {
    assert (data && size);
    Memory_Arena arena = {};
    arena.data = (u8 *)data;
    arena.size = size;
    return arena;
}

template <typename TYPE, u32 SIZE>
internal Memory_Arena 
make_memory_arena_from_buffer(TYPE (&buffer)[SIZE]) {
    Memory_Arena arena = {};
    arena.data = buffer;
    arena.size = sizeof(TYPE) * SIZE;
    return arena;
}


// TODO if alignment changes and can_realloc is true, we should reallocate everything to new alignment
// this is a very rare edge case (why would you change alignment while adding elements?)
template <typename TYPE> //if Array already has elements, it tries to regrow the arena. If it can't
boolint Array<TYPE>::reserve(s32 target_max_count) {
    assert (max_count >= 0);
    assert (count <= max_count);
    
    if (max_count < target_max_count) switch (allocator_type) {
    case ALLOCATOR_NONE: break;
    case ALLOCATOR_HEAP: break;
    case ALLOCATOR_MEMORY_ARENA: {
        Memory_Arena *arena = (Memory_Arena *)allocator;
        if (max_count == 0) { //init array
            e = push_array(arena, TYPE, target_max_count);
            if (e) {
                assert (count == 0);
                max_count = target_max_count;
            }
        } else { //try to allocate more
            assert (e);
            u32 arena_size_remaining = get_size_remaining(arena);

            boolint could_regrow_in_place = false;
            uintptr byte_after_last_elem   = (uintptr)(e + max_count);
            uintptr arena_first_avail_byte = (uintptr)(arena->data + arena->used);
            if (byte_after_last_elem == arena_first_avail_byte) { //try to regrow in place
                u32 size_to_push = (target_max_count-max_count)*sizeof(TYPE);
                if (arena_size_remaining >= size_to_push) {
                    uintptr new_data = (uintptr)push_size(arena, size_to_push);
                    assert (new_data && (new_data == byte_after_last_elem)); // NOTE if this fails, then alignement changed (which is why the pointers aren't the same)
                    max_count = target_max_count;
                    could_regrow_in_place = true;
                }
            }

            if (!could_regrow_in_place && can_realloc) {
                TYPE *new_base = push_array(arena, TYPE, target_max_count);
                if (new_base) {
                    memcopy(new_base, e, sizeof(TYPE) * count);
                    e = new_base;
                    max_count = target_max_count;
                }
            }
        }
    } break;
    }

    return (max_count >= target_max_count);
}


constexpr internal u32
next_power_2(u32 x) {
    x -= 1;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x += 1;
    return x;
}



#define Align__(n)  ALIGN##n
#define Align(n)    Align__(n)

#define ALIGNED_POINTER_UP(ptr, n)    (((uintptr)(ptr) + ((n)-1)) & ~((n)-1)) //if not aligned, get next higher aligned address
#define ALIGNED_POINTER_DOWN(ptr, n)  (((uintptr)(ptr) +       0) & ~((n)-1)) //if not aligned, get prev lower  aligned address
//#define ALIGNED_POINTER_PREV(ptr, n)    (((uintptr)(ptr) - ((n)-1)) & ~((n)-1))

//#define ALIGNED_POINTER2(ptr) (((uintptr)(ptr) + 1) & ~1)
//#define ALIGNED_POINTER4(ptr) (((uintptr)(ptr) + 3) & ~3)
//#define ALIGNED_POINTER8(ptr) (((uintptr)(ptr) + 7) & ~7)

internal nil 
is_aligned(void *ptr, uintptr alignment)
{
    assert ((alignment & (alignment - 1)) == 0);
    return (ALIGNED_POINTER_UP(ptr, alignment) - (uintptr)ptr) == 0;
}

internal nil
is_next_allocation_aligned(Memory_Arena *arena, uintptr alignment)
{
    assert ((alignment & (alignment - 1)) == 0);
    uintptr ptr = (uintptr)(arena->data + arena->used);
    return (ALIGNED_POINTER_UP(ptr, alignment) - ptr) == 0;
}

internal u32
get_offset_for_alignment(Memory_Arena *arena, uintptr alignment) {
    assert (arena->data);
    assert (arena->size >= arena->used);
    assert (alignment > 0 && IS_POW2(alignment));

    uintptr ptr = (uintptr)(arena->data + arena->used);
    u32 offset = (u32)(ALIGNED_POINTER_UP(ptr, alignment) - ptr);
    return offset;
}

//aligns current arena pointer and if we have enough room for a single byte allocation we return pointer. Otherwise we return null
internal void * 
align_next_allocation(Memory_Arena *arena, uintptr alignment) {
    u32 offset = get_offset_for_alignment(arena, alignment);
    u32 size_left = arena->size - arena->used; 
    if (size_left > offset) { //we have enough space to nudge the pointer + 1 byte allocation
        arena->used += offset;
        return (void *)(uintptr)(arena->data + arena->used);    
    } else return null;
}

internal void * 
align_next_allocation_pointer_size(Memory_Arena *arena) {
    return align_next_allocation(arena, sizeof uintptr);
}

#if 0
internal nil
did_allocate_scratch_block(Memory_Arena *arena)
{ return arena->used_on_scratch_plus_one > 0; }
#endif

internal void *
push_size_internal(Memory_Arena *arena, u32 size, u32 default_alignment, Push_Size_Args args) {
    assert (arena->data); //never set this arena up, dingus
    //assert (!did_allocate_scratch_block(arena)); //can't do allocations if we have active scratch blocks
    assert (arena->size && (size <= arena->size));
    
    if (size == 0) return null;
    
    u32 alignment = args.alignment ? args.alignment : default_alignment;
    assert (alignment > 0 && IS_POW2(alignment));
    uintptr result = (uintptr)align_next_allocation(arena, alignment);
    
    const u32 size_left = arena->size - arena->used;
    if (result && (size_left >= size)) {
        arena->used += size;
        if (args.zero_out_memory) memzero((void *)result, size);
    } else {
        result = 0;
        assert (0); //TODO handle this
    }
    return (void *)result;
}



//NOTE if size=0, just eat up rest of arena
internal Memory_Arena
push_memory_arena(Memory_Arena *arena, u32 size, Push_Size_Args args = {}) {
    void *data = push_size_internal(arena, size, 1, args);
    Memory_Arena result = make_memory_arena(data, size);
    return result;
}


internal void
clear(Memory_Arena *arena) {
    assert (arena->size && arena->used < arena->size);
    if (arena->used) 
        zero_memory(arena->data, arena->used);
    arena->used = 0;
}


#define front_push_size(pushbuffer, size, ...)                    front_push_internal(pushbuffer, size,      __VA_ARGS__)
#define front_push_struct(pushbuffer,  type, ...)         (type *)front_push_size(pushbuffer, sizeof(type),       __VA_ARGS__)
#define front_push_array(pushbuffer, type, count, ...)    (type *)front_push_size(pushbuffer, sizeof(type)*count, __VA_ARGS__)

#define back_push_size(pushbuffer, size, ...)                    back_push_internal(pushbuffer, size,      __VA_ARGS__)
#define back_push_struct(pushbuffer,  type, ...)         (type *)back_push_size(pushbuffer, sizeof(type),       __VA_ARGS__)
#define back_push_array(pushbuffer, type, count, ...)    (type *)back_push_size(pushbuffer, sizeof(type)*count, __VA_ARGS__)

struct Front_Back_Push_Buffer
{
    u8 *data;
    u32 datasize;
    u32 front_used;
    u32 back_used;
};

internal u32
get_size_remaining(Front_Back_Push_Buffer *pushbuffer) {
    u32 size = pushbuffer->datasize;
    u32 used = pushbuffer->front_used + pushbuffer->back_used;
    assert (used <= size);
    return size - used;
}

internal void * 
front_align_next_allocation(Front_Back_Push_Buffer *pushbuffer, uintptr alignment) {
    assert (alignment > 0 && IS_POW2(alignment));
        
    uintptr ptr = (uintptr)(pushbuffer->data + pushbuffer->front_used);
    u32 offset = (u32)(ALIGNED_POINTER_UP(ptr, alignment) - ptr);
    
    const u32 size_left = get_size_remaining(pushbuffer);
    if (size_left > offset) {
        pushbuffer->front_used += offset;
        return (void *)(ptr + offset);    
    } else return null;
}

internal void *
front_push_size_internal(Front_Back_Push_Buffer *pushbuffer, u32 size, u32 default_alignment, Push_Size_Args args)
{
    assert (pushbuffer->data);
    u32 alignment = args.alignment ? args.alignment : default_alignment;
    uintptr result = (uintptr)front_align_next_allocation(pushbuffer, alignment);
    
    if (result && get_size_remaining(pushbuffer) >= size) {
        pushbuffer->front_used += size;    
        if (args.zero_out_memory) memzero((void *)result, size);
        return (void *)result;    
    } else assert (0); //TODO handle this
}


internal void * 
back_align_next_allocation(Front_Back_Push_Buffer *pushbuffer, uintptr alignment) {
    assert (alignment > 0 && IS_POW2(alignment));
        
    uintptr ptr = (uintptr)(pushbuffer->data + pushbuffer->datasize - pushbuffer->back_used - 1);
    u32 offset  = (u32)(ptr - ALIGNED_POINTER_DOWN(ptr, alignment));
    
    const u32 size_left = get_size_remaining(pushbuffer);
    if (size_left > offset) {
        pushbuffer->back_used += offset;
        return (void *)(ptr - offset); //TODO this used to be (ptr + offset) which I'm pretty sure was a bug but I haven't tested this yet   
    } else return null;
}

internal void *
back_push_internal(Front_Back_Push_Buffer *pushbuffer, u32 size, u32 default_alignment, Push_Size_Args args)
{
    assert (pushbuffer->data);
    u32 alignment = args.alignment ? args.alignment : default_alignment;
    uintptr result = (uintptr)back_align_next_allocation(pushbuffer, alignment);
       
    if (result && get_size_remaining(pushbuffer) >= size) {
        pushbuffer->back_used += size;    
        if (args.zero_out_memory) zero_memory((void *)result, size);
        return (void *)result;    
    } else assert (0);
}

#if 0
struct Scratch_Block 
{
    Memory_Arena *parent;
    Memory_Arena  arena;
    //TODO this wasts some memory due to alignment requests. 
    //We could just store old_used in parent_arena to reset it
    //if we only want one scratch block per arena. But we could also 
    //do the recursion thing like 
    
    Scratch_Block(Memory_Arena *parent_arena, u32 size = 0, u32 flags = 0)
    {
        parent = parent_arena;
        u32 parent_used = parent->used;
        
        arena  = push_memory_arena(parent_arena, size, flags); 
        parent->used_on_scratch_plus_one = parent_used + 1;
    }
    
    //NOTE destructors get called in reverse order as the objects were created
    //meaning there's on problem with making multiple scratch blocks with same arena
    //and there's no issue when making a scratch block from a scratch block, ...it's just an arena
    ~Scratch_Block()
    {
        //TODO do some more checking in here I suppose
        assert (parent->used_on_scratch_plus_one > 0);
        parent->used = parent->used_on_scratch_plus_one - 1;
        parent->used_on_scratch_plus_one = 0;
        
        
        //TODO get rid of this zero_memory because I want the user
        //to manually zero it out when memory is retrieved so there is never
        //any confusion ....
        //zero_memory(this->arena.data, this->arena.used);
    }
    
    operator Memory_Arena*() { return &this->arena; }
};
#endif


struct Vector2u
{
    u32 x, y;
};

union Vector2i { 
    //never rearrange these
    struct {
        s32 x, y;
    };
    struct {
        s32 w, h;
    };

    
};

internal Vector2i
v2i(s32 x, s32 y)
{
    Vector2i v;
    v.x = x;
    v.y = y;
    return v;
}



internal s32
dot2(Vector2i a, Vector2i b)
{ return a.x*b.x + a.y*b.y; }

internal s32
normsq(Vector2i a)
{ return dot2(a, a); }

internal f32 sqroot(f32 x);
//TODO maybe move this somewhere
internal f32
norm(Vector2i a)
{
    s32 length_squared = normsq(a);
    return sqroot((f32)length_squared);
}

constexpr Vector2i
v2i_constexpr(s32 x, s32 y)
{
    Vector2i v = {x, y};
    return v;
}


static Vector2i
operator+(Vector2i a, Vector2i b)   { return v2i(a.x + b.x, a.y + b.y); }

static void
operator+=(Vector2i &a, Vector2i b) { a = a + b; }

static Vector2i
operator-(Vector2i a, Vector2i b)   { return v2i(a.x - b.x, a.y - b.y); }

static void
operator-=(Vector2i &a, Vector2i b) { a = a - b; }



static Vector2i
operator*(Vector2i a, s32 scale)    { return v2i(a.x * scale, a.y * scale); }

static Vector2i
operator*(s32 scale, Vector2i a)    { return v2i(a.x * scale, a.y * scale); }

static Vector2i
operator*(Vector2i a, Vector2i b)   { return v2i(a.x * b.x, a.y * b.y); }

static void
operator*=(Vector2i &a, Vector2i b) { a = a * b; }

static bool
operator==(Vector2i a, Vector2i b)  { return (a.x == b.x) && (a.y == b.y); }

static bool
operator!=(Vector2i a, Vector2i b)  { return !(a == b); }

//never rearrange these members
struct Vector3i { 
    union {
        struct { s32 x, y, z; }; 
        struct { Vector2i xy; s32 z; }; 
    };
};

internal Vector3i
v3i(s32 x, s32 y, s32 z)
{
    Vector3i v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

internal Vector3i
v3i(Vector2i xy, s32 z)
{
    Vector3i v;
    v.x = xy.x;
    v.y = xy.y;
    v.z = z;
    return v;
}

static Vector3i
operator+(Vector3i a, Vector3i b)   { return v3i(a.x + b.x, a.y + b.y, a.z + b.z); }

static void
operator+=(Vector3i &a, Vector3i b) { a = a + b; }

static Vector3i
operator-(Vector3i a, Vector3i b)   { return v3i(a.x - b.x, a.y - b.y, a.z - b.z); }

static void
operator-=(Vector3i &a, Vector3i b) { a = a - b; }



static Vector3i
operator*(Vector3i a, s32 scale)    { return v3i(a.x * scale, a.y * scale, a.z * scale); }

static Vector3i
operator*(s32 scale, Vector3i a)    { return v3i(a.x * scale, a.y * scale, a.z * scale); }

static Vector3i
operator*(Vector3i a, Vector3i b)   { return v3i(a.x * b.x, a.y * b.y, a.z * b.z); }

static bool
operator==(Vector3i a, Vector3i b)  { return (a.x == b.x) && (a.y == b.y) && (a.z == b.z); }

static bool
operator!=(Vector3i a, Vector3i b)  { return !(a == b); }

struct Vector4i {
    s32 x, y, z, w;
};

//NOTE we serialize this directly so never rearrange/modify any of these values!!!!!!
//and this must remain a 32 bit integer
enum Direction8 : s32 {
    NORTH,     //0
    NORTHEAST, //1
    EAST,      //2
    SOUTHEAST, //3
    SOUTH,     //4
    SOUTHWEST, //5
    WEST,      //6
    NORTHWEST, //7
    NO_DIRECTION8,
    DIRECTION8_COUNT = NO_DIRECTION8, 
};

internal f32
fmod(f32 numerator, f32 denominator) {
    //NOTE!!! this uses truncation instead of floor_s32 to get num_times_divisible
    //this should be the same beheavior as regular C % modulo operator, although it's implemetation-defined
    //however, in math literature I see more using the floor_s32 variant()
    //honestly, just try to keep numerator and denominator positive and use sign bit logic...
    assert (denominator != 0);
    s32 num_times_divisible = (s32)(numerator / denominator);
    f32 result = numerator - num_times_divisible*denominator;
    return result;
}

//find better place for this guys
internal f32
wrap_angle(f32 angle, f32 a1, f32 a2) {
    f32 period = a2 - a1;
    f32 result = a1 + fmod(angle - a1, period);
    if (result < a1) result += period;
    if (result >= a2) result = a2 - 0.000001f; //TODO we need something like nextafter() here
    assert (result >= a1 && result < a2);
    return result;
}

internal f32
flip_rads(f32 angle) {
    f32 result = wrap_angle(angle + PI, 0, TAU); 
    return result;
}

internal void
flip_rads(f32 *angle) { 
        *angle = flip_rads(*angle); 
}

internal f32
flip_turns(f32 t) {
        f32 result = wrap_angle(t + 0.5f, 0, 1); 
        return result;
}

internal void
flip_turns(f32 *t) { 
        *t = flip_turns(*t); 
}

internal f32
to_rads(Direction8 dir) {
    switch(dir) {
    case EAST:  return TAU*0.00f;
    case NORTH: return TAU*0.25f;
    case WEST:  return PI;
    case SOUTH: return TAU*0.75f;
        default: assert(0);
    }
    return 0;
}

internal f32
to_turns(Direction8 dir) {
        switch(dir) {
        case EAST:  return 0.00f;
        case NORTH: return 0.25f;
        case WEST:  return 0.50f;
        case SOUTH: return 0.75f;
        default: assert (0);
        }
        return 0;
}



internal Direction8
flip(Direction8 dir) {
        switch(dir) {
    case NORTH: return SOUTH;
    case EAST:  return WEST;
    case SOUTH: return NORTH;
    case WEST:  return EAST;
    case NORTHWEST:  return SOUTHEAST;
    case NORTHEAST:  return SOUTHWEST;
    case SOUTHWEST:  return NORTHEAST;
    case SOUTHEAST:  return NORTHWEST;
    default:    assert (0); return NO_DIRECTION8;
    }
}

internal void
flip(Direction8 *dir)
{ *dir = flip(*dir); }

internal Direction8
rotate_90ccw(Direction8 dir)
{
        switch(dir)
        {
    case NORTH: return WEST;
    case EAST:  return NORTH;
    case SOUTH: return EAST;
    case WEST:  return SOUTH;
    case NORTHEAST:  return NORTHWEST;
    case NORTHWEST:  return SOUTHWEST;
    case SOUTHWEST:  return SOUTHEAST;
    case SOUTHEAST:  return NORTHEAST;
    default:    assert (0); return NO_DIRECTION8;
    }
}

internal Direction8
rotate_90cw(Direction8 dir) {
        switch(dir) {
    case NORTH: return EAST;
    case EAST:  return SOUTH;
    case SOUTH: return WEST;
    case WEST:  return NORTH;
    case NORTHEAST:  return SOUTHEAST;
    case SOUTHEAST:  return SOUTHWEST;
    case SOUTHWEST: return NORTHWEST;
    case NORTHWEST:  return NORTHEAST; 
    default:    assert (0); return NO_DIRECTION8;
    }
}

internal Direction8
mirror(Direction8 init_dir, Vector2i mir) {
    Direction8 dir = init_dir;
    if (mir.x < 0) {
        switch(dir) {
        case NORTH:     break;
        case NORTHEAST: dir = NORTHWEST; break;
        case EAST:      dir = WEST;      break;
        case SOUTHEAST: dir = SOUTHWEST; break;
        case SOUTH:     break;
        case SOUTHWEST: dir = SOUTHEAST; break;
        case WEST:      dir = EAST;      break;
        case NORTHWEST: dir = NORTHEAST; break;
        }
    }
    if (mir.y < 0) {
        switch(dir) {
        case NORTH:     dir = SOUTH;     break;
        case NORTHEAST: dir = SOUTHEAST; break;
        case EAST:      break;
        case SOUTHEAST: dir = NORTHEAST; break;
        case SOUTH:     dir = NORTH;     break;
        case SOUTHWEST: dir = NORTHWEST; break;
        case WEST:      break;
        case NORTHWEST: dir = SOUTHWEST; break;
        }
    }
    return dir;
}

internal void
mirror(Direction8 *dir, Vector2i m) {
    *dir = mirror(*dir, m);
}

internal void
rotate_90cw(Direction8 *dir) {
    *dir = rotate_90cw(*dir);
}

template <typename TYPE, u64 (*hash)(TYPE *entry)>
internal u32
hashtable_get_index(u32 max_count, TYPE *entry)
{
    assert (IS_POW2(max_count));
    u64 hash64 = MeowU64From(hash, 0);
    return (u32)(hash64 & (max_count - 1));
}

template <typename TYPE, u64 (*hash)(TYPE *entry)>
internal TYPE *
hashtable_add_entry(TYPE *table, u32 max_count, TYPE *entry)
{
    u32 index_start = hashtable_get_index(max_count, entry);
    u32 index = index_start;
    
    TYPE *slot = null;
    
    do 
    {
        if (is_hash_slot_empty(e + index))
        {
            slot = e + index;
            *slot = *entry;
            count += 1;
            break;
        }
        else index = (index + 1) & (max_count - 1);
    } while(index != index_start);
    
    return slot;
}

template <typename TYPE, u32 (*get_hash_index)(TYPE *entry), nil (*is_hash_slot_empty)(TYPE *entry),
/**/      nil (*compare_and_remove_hash_slot)(TYPE *slot, TYPE *entry)>
internal nil 
hashtable_remove_entry(TYPE *table, u32 max_count, TYPE *entry)
{
    u32 index_start = hashtable_get_index(max_count, entry);
    u32 index = index_start;
    
    
    nil did_remove = false;
    do //do while is fine here since we only remove one entry and break out
    {
        if (is_hash_slot_empty(e + index)) break; //didn't find key to remove
        
            //if (will_remove_hash_slot(e + index, entrydata))
        if (compare_and_remove_hash_slot(e + index, entry))
        {
            did_remove = true;
            count -= 1;
            empty_hash_slot(e + index);
            
            u32 hole = index;
            u32 next = (hole + 1) & (max_count - 1);
            do //shift entries
            {
                if (is_hash_slot_empty(e + next)) break; // nothing to shift downards
                
                u32 hash_index_at_next = get_hash_index(e + next);
                    //I understand why we do the index < hash_index_at_next because if that's true, we can't move hash_index_at_next lower than it's natural position
                    //however I'm not sure why we do hash_index_at_next <= next ... I belive it handles some wrapping cases... dunno, this was taken from wikipedia, open addressing...
                if (!((hole<=next) ? ((hole < hash_index_at_next) && (hash_index_at_next <= next)) : 
                        /**/         ((hole < hash_index_at_next) || (hash_index_at_next <= next))))
                {
                    e[hole] = e[next];
                    empty_hash_slot(e + next);
                    hole = next; 
                }
                next = (next + 1) & (max_count - 1);
                
            } while (next != index); 
            break;
        }
        else index = (index + 1) & (max_count - 1); //continue to next index
        
    } while (index != index_start);
    
    return did_remove;
}



//c++ depresses the fuck out of me. All i want is to deduce return type of function in c++
#if 1
//for TYPE, need to specify..
// empty_hash_slot() -> void, called when clearing a hash slot. Set key to null
template <typename KEY, typename TYPE, u64   (*hash)(void *data, uintptr size),  
/**/      KEY * (*get_hash_key)(TYPE *entry), //NOTE we use this pointer both to compute hash index and also to set key value when adding entries
/**/      nil   (*do_hash_keys_match)(KEY *a, KEY *b),
/**/      nil   (*is_hash_slot_empty)(TYPE *entry), 
/**/      void  (*clear_hash_slot)(TYPE *slot)>

struct Hash_Table_Old {
    u32 count;
    u32 max_count;
    TYPE *e;
    
    
    s32 get_count_remaining() {
        assert (count >= 0 && count <= max_count);
        return max_count - count;
    }
    
    void init(TYPE *buffer, s32 buffer_size) {
        assert (IS_POW2(buffer_size));
        e = buffer;
        max_count = buffer_size;
        count = 0;
    }
    
    template <s32 BUFFER_SIZE>
    void init_from_buffer(TYPE (&buffer)[BUFFER_SIZE]) {
        init(buffer, BUFFER_SIZE);
    }
    
    s32 get_index(TYPE *entry) {
        assert (entry >= e);
        assert (entry < (e + max_count));
        return (s32)(entry - e);
    }
    
    u32 get_hash_index(KEY *key) {
        u64 hash64 = hash(key, sizeof(*key)); 
        return (u32)(hash64 & (max_count - 1)); 
    }
    
    u32 get_hash_index(TYPE *entry) {
        assert (max_count <= U32_MAX); 
        assert (IS_POW2(max_count));
        KEY *key = get_hash_key(entry); 
        return get_hash_index(key);
    }
    
    //NOTE this memcopies the key over
    TYPE *add_entry(KEY *key, nil zero_initialize_entry = false) {
        assert (count <= max_count);
        if (count == max_count) return null;
        
        //SET_HASH_INDEX(index_start, entrydata);
        u32 index_start = get_hash_index(key);
        u32 index = index_start;
        
        do {
            TYPE *slot = e + index;
            if (is_hash_slot_empty(slot)) {               
                if (zero_initialize_entry) zero_memory(slot, sizeof(*slot));
                memcopy(get_hash_key(slot), key, sizeof (*key));
                count += 1;
                return slot;
            }
            else index = (index + 1) & (max_count - 1);
        } while(index != index_start);
        
        return null;
    }    
    
    TYPE *add_entry(KEY key, nil zero_initialize_entry = false) { 
        return add_entry(&key, zero_initialize_entry); 
    }
    
    TYPE *find_entry(KEY *key) {
        if (count == 0) return null;
        
        u32 index_start = get_hash_index(key);
        u32 index = index_start;
        
        do 
        {
            if (is_hash_slot_empty(e + index)) return null; //didn't find key to remove
            if (do_hash_keys_match(key, get_hash_key(e + index)))
            {
                return e + index;
            }
            else index = (index + 1) & (max_count - 1); //continue to next index
        } while (index != index_start);
        
        return null;
    }
    
    TYPE *find_entry(KEY key) { 
        return find_entry(&key); 
    }
    
    boolint remove_entry(KEY *key) {
        assert (count > 0);
        
        //SET_HASH_INDEX(index_start);
        u32 index_start = get_hash_index(key);
        u32 index = index_start;
        
        nil did_remove = false;
        do //do while is fine here since we only remove one entry and break out
        {
            if (is_hash_slot_empty(e + index)) break; //didn't find key to remove
            
            if (do_hash_keys_match(key, get_hash_key(e + index)))
            {
                did_remove = true;
                count -= 1;
                clear_hash_slot(e + index); //we could avoid doing this clear if it will just get overwritten when shifting down
                
                u32 hole = index;
                u32 next = (hole + 1) & (max_count - 1);
                do //shift entries
                {
                    if (is_hash_slot_empty(e + next)) break; // nothing to shift downards
                    
                    u32 hash_index_at_next = get_hash_index(e + next);
                    //I understand why we do the index < hash_index_at_next because if that's true, we can't move hash_index_at_next lower than it's natural position
                    //however I'm not sure why we do hash_index_at_next <= next ... I belive it handles some wrapping cases... dunno, this was taken from wikipedia, open addressing...
                    nil can_fill_hole = false;
                    if (hole <= next)
                    {
                        //Keep in mind if we reached next, it's because it's PART OF THE CLUSTER
                        //so  if the natural position is before/at the hole. We can move it. (first check)
                        //and if the natural position is after next, that means inserting next wrapped us around and is still connected to the cluster
                        if ((hash_index_at_next <= hole) || (hash_index_at_next > next))
                        {
                            //NOTE it can't be hash_index_at_next >= next bc if 
                            //hash_index_at_next == next, that means next was the *very* first insertion. And we can't move that before it's natural position
                            can_fill_hole = true;
                        }
                        
                    }
                    else
                    {
                        //this one is even harder to think about for me.
                        // first case is same as before however we have this added condition that
                        //hash_index_at_next must have occured after next. A good way to think about this is how
                        //next came to be from it's natural position hash_index_at_next...
                        //if next wrapped around to the other side of the buffer, the natural poistion must have occured *after next* and wrapped around
                        //and we always check if it's <= hole.... it makes sense.
                        //if hash_index_at_next could be <= next, then no wrap around would have occured and we can't move the entry
                        if ((hash_index_at_next <= hole) && (hash_index_at_next > next))
                        {
                            can_fill_hole = true;
                        }
                    }
                    
                    if (can_fill_hole)
                    {
                        e[hole] = e[next];
                        clear_hash_slot(e + next);
                        hole = next; 
                    }
                    next = (next + 1) & (max_count - 1);
                    
                } while (next != index); 
                break;
            }
            else index = (index + 1) & (max_count - 1); //continue to next index
            
        } while (index != index_start);
        
        return did_remove;
    }
    
    nil remove_entry(KEY key) { 
        return remove_entry(&key); 
    }
};

//TODO find intrinsic for this
inline boolint
is_memory_equal(void *_a, void *_b, uintptr size) {
    u8 *a = (u8 *)_a;
    u8 *b = (u8 *)_b;
    while (size--) {
        if (*a++ != *b++) return false;
    }
    return true;
}


//for TYPE, need to specify..
// empty_hash_slot() -> void, called when clearing a hash slot. Set key to null
template <typename KEY, typename TYPE, s32 MAX_COUNT, 
/**/      u64   (*hash)(void *data, uintptr size),  
/**/      KEY * (*get_hash_key)(TYPE *entry)> //NOTE we use this pointer both to compute hash index and also to set key value when adding entries
struct Hash_Table {
    s32 count;
    static constexpr s32 max_count = MAX_COUNT;
    
    Bit_Array64<MAX_COUNT> occupied;
    cassert (IS_POW2(MAX_COUNT), "Hash Table must be a power of 2");
    
    TYPE e[MAX_COUNT];
    
    void reset() {
        count = 0;
        memzero(&occupied, sizeof(occupied));
    }
    
    s32 get_count_remaining() {
        assert (count >= 0 && count <= max_count);
        return max_count - count;
    }
    
    s32 get_index(TYPE *entry) {
        assert (entry >= e);
        assert (entry < (e + max_count));
        return (s32)(entry - e);
    }
    
    u32 get_hash_index(KEY *key) {
        u64 hash64 = hash(key, sizeof(*key)); 
        return (u32)(hash64 & (max_count - 1)); 
    }
    
    u32 get_hash_index(TYPE *entry) {
        KEY *key = get_hash_key(entry); 
        return get_hash_index(key);
    }
    
    //NOTE this memcopies the key over
    TYPE *add_entry(KEY *key, nil zero_initialize_entry = false) {
        assert (count <= max_count);
        if (count == max_count) return null;
        
        //SET_HASH_INDEX(index_start, entrydata);
        u32 index_start = get_hash_index(key);
        u32 index = index_start;
        
        do {
            TYPE *slot = e + index;
            if (!occupied.is_set(index)) {               
                if (zero_initialize_entry) zero_memory(slot, sizeof(*slot));
                memcopy(get_hash_key(slot), key, sizeof (*key));
                count += 1;
                occupied.set_bit(index);
                return slot;
            }
            else index = (index + 1) & (max_count - 1);
        } while(index != index_start);
        
        return null;
    }    
    
    TYPE *add_entry(KEY key, nil zero_initialize_entry = false) { 
        return add_entry(&key, zero_initialize_entry); 
    }
    
    TYPE *find_entry(KEY *key) {
        if (count == 0) return null;
        
        u32 index_start = get_hash_index(key);
        u32 index = index_start;
        
        do {
            if (!occupied.is_set(index)) return null; //didn't find key to remove
            if (is_memory_equal(key, get_hash_key(e + index), sizeof(*key))) {
                return e + index;
            }
            else index = (index + 1) & (max_count - 1); //continue to next index
        } while (index != index_start);
        
        return null;
    }
    
    TYPE *find_entry(KEY key) { 
        return find_entry(&key); 
    }
    
    boolint remove_entry(KEY *key) {
        assert (count > 0);
        
        //SET_HASH_INDEX(index_start);
        u32 index_start = get_hash_index(key);
        u32 index = index_start;
        
        nil did_remove = false;
        do { //do while is fine here since we only remove one entry and break out
            if (!occupied.is_set(index)) break; //didn't find key to remove
            
            if (is_memory_equal(key, get_hash_key(e + index), sizeof(*key))) {
                did_remove = true;
                count -= 1;
                occupied.clear_bit(index);
                
                u32 hole = index;
                u32 next = (hole + 1) & (max_count - 1);
                do { //shift entries
                    //if (is_hash_slot_empty(e + next)) break; // nothing to shift downards
                    if (!occupied.is_set(next)) break;
                    
                    u32 hash_index_at_next = get_hash_index(e + next);
                    //I understand why we do the index < hash_index_at_next because if that's true, we can't move hash_index_at_next lower than it's natural position
                    //however I'm not sure why we do hash_index_at_next <= next ... I belive it handles some wrapping cases... dunno, this was taken from wikipedia, open addressing...
                    nil can_fill_hole = false;
                    if (hole <= next) {
                        //Keep in mind if we reached next, it's because it's PART OF THE CLUSTER
                        //so  if the natural position is before/at the hole. We can move it. (first check)
                        //and if the natural position is after next, that means inserting next wrapped us around and is still connected to the cluster
                        if ((hash_index_at_next <= hole) || (hash_index_at_next > next)) {
                            //NOTE it can't be hash_index_at_next >= next bc if 
                            //hash_index_at_next == next, that means next was the *very* first insertion. And we can't move that before it's natural position
                            can_fill_hole = true;
                        }
                        
                    } else {
                        //this one is even harder to think about for me.
                        // first case is same as before however we have this added condition that
                        //hash_index_at_next must have occured after next. A good way to think about this is how
                        //next came to be from it's natural position hash_index_at_next...
                        //if next wrapped around to the other side of the buffer, the natural poistion must have occured *after next* and wrapped around
                        //and we always check if it's <= hole.... it makes sense.
                        //if hash_index_at_next could be <= next, then no wrap around would have occured and we can't move the entry
                        if ((hash_index_at_next <= hole) && (hash_index_at_next > next)) {
                            can_fill_hole = true;
                        }
                    }
                    
                    if (can_fill_hole) {
                        assert (hole != next);
                        memcopy(e + hole, e + next, sizeof(e[0]));
                        occupied.set_bit(hole);
                        occupied.clear_bit(next);
                        hole = next; 
                    }
                    next = (next + 1) & (max_count - 1);
                    
                } while (next != index); 
                break;
            }
            else index = (index + 1) & (max_count - 1); //continue to next index
            
        } while (index != index_start);
        
        return did_remove;
    }
    
    nil remove_entry(KEY key) { 
        return remove_entry(&key); 
    }
};
#endif


//taken from demetri spanos from handmade network discord
//has not been tested at all
internal u32 
murmur3_hash32(const void *key, int len, u32 h1) {
        
    let rotl32 = [](u32 x, s8 r) -> u32 { 
        return (x << r) | (x >> (32 - r)); 
    };
    
    let fmix = [](u32 h ) -> u32 {
        h ^= h >> 16; h *= 0x85ebca6b;
        h ^= h >> 13; h *= 0xc2b2ae35;
        return h ^= h >> 16;
    };
        
        
        
  //const u8 *tail = (const u8*)(key + (len/4)*4); // handle this separately
    assert (0); //validate this is correct
    const u8 *tail = (const u8*)key + (len/4)*4; // handle this separately
    
    u32 c1 = 0xcc9e2d51, c2 = 0x1b873593;
    
  // body (full 32-bit blocks) handled uniformly
    for (u32 *p = (u32*) key; p < (const u32*) tail; p++) {
        u32 k1 = *p; k1 *= c1; k1 = rotl32(k1,15); k1 *= c2; // MUR1
        h1 ^= k1; h1 = rotl32(h1,13); h1 = h1*5+0xe6546b64; // MUR2
    }
    
    u32 t = 0; // handle up to 3 tail bytes
    switch(len & 3) {
    case 3: t ^= tail[2] << 16;
    case 2: t ^= tail[1] << 8;
    case 1: {t ^= tail[0]; t *= c1; t = rotl32(t,15); t *= c2; h1 ^= t;};
    }
    return fmix(h1 ^ len);
}




#endif //UBIQUITOUS_H



