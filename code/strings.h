#ifndef STRINGS_H //created on September 15th 2021 
#define STRINGS_H

//NOTE stb_snprintf.h includes this as well
//#include <stdarg.h> // for va_list()
//#include <stddef.h> // size_t, ptrdiff_t
//#include <stdlib.h> // for va_arg()
#define STB_SPRINTF_IMPLEMENTATION
#define STB_SPRINTF_STATIC
#include "..\libs\stb_sprintf.h"

#define litargs(lit)     lit, sizeof(lit)-1
#define strargs(string) (string).str, (string).length
#define strlit(lit)    {lit, sizeof(lit)-1, sizeof(lit)}

//#include "sal.h"
//#undef _USE_ATTRIBUTES_FOR_SAL
//#define _USE_ATTRIBUTES_FOR_SAL 1
//#define PARAM_FORMAT_STRING _Printf_format_string_

// NOTE all calls to duplicate null terminate always
// all strings *should* be null termianted. Most function calls here will fire assert if it's not the case

#define LOCAL_STRING(name, buffer_size) char buffer_for_ ## name[buffer_size]; String name = mkstr_from_buffer(buffer_for_ ## name)



struct String {
    //NOTE DO NOT REARRANGE THESE!!!!!
    char *str;
    s32 length; 
    s32 size;
    
    char &operator[](s32 index)
    {
        assert (index >= 0 && index < length);
        return str[index]; 
    }
};

template<typename T> inline nil 
is_ascii(T c)
{ 	return(c >= 0 && c <= 127);	}

template<typename T> inline nil 
is_digit(T c)
{ 	return c >= '0' && c <= '9';	}

template<typename T> inline nil
is_uppercase(T c)
{	return c >= 'A' && c <= 'Z';	}

template<typename T> inline nil
is_lowercase(T c)
{	return c >= 'a' && c <= 'z';	}

template<typename T> inline nil
is_alpha(T c)
{	return is_lowercase(c) || is_uppercase(c);	}

template<typename T> inline nil
is_alphanumeric(T c)
{	return is_alpha(c) || is_digit(c);	}

template<typename T> inline char
to_uppercase(T c) {	
    return is_lowercase(c) ? (c + ('A' - 'a')) : c;	
}

template<typename T> inline char
to_lowercase(T c) {	
    return is_uppercase(c) ? (c + ('a' - 'A')) : c;	
}

constexpr u32
constexpr_hash_string(char *str, s32 length) {
    u32 hash = 0;
    for_index_inc(s32, i, length)
        hash = hash*65599 + str[i];
    return hash;
}

#pragma warning(push)          
#pragma warning(disable: 4455) //this allows us to use 's' as postfix...std reserves that apparently...
constexpr String operator""s(const char *str, size_t length) {
    assert (length < S32_MAX);
    String string = {(char *)str, (s32)length, (s32)length + 1};
    return string;
}
#pragma warning(pop) 

//NOTE this doesn't check if string length is correct or not
internal nil
is_valid(String string)
{
    nil result = string.str != null;
    assert (result? (string.length >= 0 && string.size >= string.length) :true);
    return result;
}

//TODO intrinsic for this?
internal s32 
strlen(char *string)
{
    s32 length = 0;
    while(string[length])	length += 1;
    return length;
}

internal String
mkstr() {
    String str = {};
    return str;
}

inline String
mkstr(char *str, s32 length, s32 size) {
    assert (size >= length);
    if_assert ((size > 0), str != null);
    String string;
    string.str = str;
    string.length = length;
    string.size   = size;
    return string;
}

inline String
mkstr(char *str, s32 length) {
    return mkstr(str, length, length);
}

internal String 
mkstr(char *start, char *end_plus_one) {
    if (end_plus_one > start) {
        uintptr length_big = (uintptr)(end_plus_one - start);
        s32 length_small = to_s32(length_big);
        String result = mkstr(start, length_small);
        return result;
    } else return {};
}

internal String
mkstr(char *str)
{
    s32 length = strlen(str);
    return mkstr(str, length, length + 1);
}

template <u32 SIZE>
internal String
mkstr_from_buffer(char (&buffer)[SIZE], s32 length = 0) 
{
    String string = mkstr(buffer, length, SIZE);
    string.str[string.length] = 0;
    return string;
}

//NOTE this doesn't check if string length is correct or not
internal nil
is_null_terminated(String string)
{
    nil should_be_null_terminated = (string.size > string.length);
    assert (should_be_null_terminated? (string.str[string.length] == 0) :true);
    return should_be_null_terminated;
}


internal nil //NOTE better/colloquial name for this
are_strings_equal(char *a, char *b, s32 length) {
    for(s32 i = 0; i < length; i += 1) {
        assert (a[i]); //if either of these fail, it simply means 
        assert (b[i]); //that length is not correct min length for both strings
        if (a[i] != b[i]) return false;
    }
    return true;
}

internal boolint //returns true if string starts with substr as a substring
starts_with(char *string, char *substr, s32 substr_length) {
    for (s32 i = 0; i < substr_length; i += 1) {
        if      (!substr[i])              return true;  // substr ends with no differences
        else if (!string[i])	          return false; // substr longer than string and therefore isn't a substring
        else if (string[i] != substr[i])  return false; // they differ
    }
    return true; //reached end of substring and it wasn't null terminated (but both equal each other up to substr_length)
}

internal boolint //returns true if string starts with substr as a substring
starts_with(String string, String substr) {
    if (string.length >= substr.length) return starts_with(string.str, substr.str, substr.length);
    else return false;
}

internal boolint //returns true if string starts with substr as a substring
starts_with_ignore_case(char *string, char *substr, s32 substr_length) {
    for (s32 i = 0; i < substr_length; i += 1) {
        if      (!substr[i])              return true;  // substr ends with no differences
        else if (!string[i])	          return false; // substr longer than string and therefore isn't a substring
        else if (to_lowercase(string[i]) != to_lowercase(substr[i]))  return false; // they differ
    }
    return true; //reached end of substring and it wasn't null terminated (but both equal each other up to substr_length)
}


internal boolint //returns true if string starts with substr as a substring
starts_with_ignore_case(String string, String substr) {
    if (string.length >= substr.length) return starts_with_ignore_case(string.str, substr.str, substr.length);
    else return false;
}

//NOTE this returns true if string points to the exact same substr_length
//chars found in substr, this does not care about main strings null character
internal nil
strsubmatch(char *string, char *substr, s32 substr_length)
{
    for(s32 i = 0; i < substr_length; i += 1)
    {
        if      (!substr[i])              return true;  // NOTE string2 has to be checked first in case 
        else if (!string[i])	          return false; // where they are both the same length...
        else if (string[i] != substr[i]) return false;
    }
    return true;
}

internal nil
strsubmatch(String str, String query)
{
    if (str.length >= query.length)
         return strsubmatch(str.str, query.str, query.length);
    else return false;
}

internal boolint
strmatch_ignore_case(char *string1, s32 length1, char *string2, s32 length2) {
    if (length1 != length2) return false;
    
    for (s32 i = 0; i < length1; i += 1) {
        char c1 = to_uppercase(string1[i]);
        char c2 = to_uppercase(string2[i]);
        assert (c1 && c2);
        if (c1 != c2) {
            return false;
        }
    }
    return true;
}

internal boolint
strmatch_ignore_case(String s1, String s2) {
    return strmatch_ignore_case(s1.str, s1.length, s2.str, s2.length);
}

internal nil
strmatch(char *string1, s32 length1, char *string2, s32 length2) {
    nil match = false;
    if (length1 == length2)
    {
        match = true; 
        for(s32 i = 0; i < length1; i += 1)
        {
            assert (string1[i]); //length1 incorrect
            assert (string2[i]); //length2 incorrect 
            if (string1[i] != string2[i])
            {
                match = false;
                break;
            }
        }
    }
    return match;
}

//TODO we can speed this up by not calling strlen and isntead just iterating
//both strings until hitting a terminator and checking other has terminator as well...
internal nil
strmatch(char *string1, char *string2)
{ return strmatch(string1, strlen(string1), string2, strlen(string2)); }

internal nil
strmatch(String a, String b)
{ return strmatch(a.str, a.length, b.str, b.length); }

template <s32 SIZE> 
internal nil strmatch(String str, const char (&buffer)[SIZE])
{ return strmatch(str.str, str.length, (char *)buffer, SIZE-1); }


internal String
advance(String string, s32 amount) {
    s32 length = string.length - amount;
    if (length <= 0) return {};
    else return mkstr(string.str + amount, length, string.size - amount);
}

internal void
advance(String *string, s32 amount) {
    *string = advance(*string, amount);	
}

internal void //advances prefix.length amount if string beginning matches that prefix
advance_prefix(String *string, String prefix) {
    if (starts_with(*string, prefix)) {
        advance(string, prefix.length);
    }
}



global String MONTH_STRINGS[13] = {"INVALID MONTH"s, "January"s, "Febuary"s, "March"s, "April"s, "May"s, "June"s, "July"s, "August"s, "Septemeber"s, "October"s, "November"s, "December"s};

internal String
get_month_string(s32 month) {
    if (month < 0)  month = 0;
    if (month > 12) month = 0;
    return MONTH_STRINGS[month];
}

internal s32 
compare_alphabetically(String a, String b)
{
    s32 length = min2(a.length, b.length);
    for_index_inc(s32, i, length)
    {
        char char_a = to_uppercase(a.str[i]);
        char char_b = to_uppercase(b.str[i]);
        if (char_a < char_b)      return -1;
        else if (char_a > char_b) return  1;
    }

    //if we have a perfect substring match, we do smaller < larger
    if      (a.length < b.length) return -1;
    else if (b.length < a.length) return 1;
    return 0;
}

internal s32 
compare_alphabetically(char *a, char *b)
{
    String str_a = mkstr(a);
    String str_b = mkstr(b);
    return compare_alphabetically(str_a, str_b);
}

internal s32 //first version I wrote. I don't think it's that useful (if you want to binary search for example)
compare_alphabetically_only_letters(String a, String b)
{
    s32 length = min2(a.length, b.length);
    for_index_inc(s32, i, length)
    {
        char char_a = a.str[i];
        char char_b = b.str[i];
        if (is_alpha(char_a) && is_alpha(char_b))
        {
            char_a = to_lowercase(char_a);
            char_b = to_lowercase(char_b);
            if (char_a < char_b)      return -1;
            else if (char_a > char_b) return  1;
            //else                      return  0;
        }
    }
    return 0;
}

internal s32
compare_alphabetically_only_letters(char *a, char *b)
{
    String str_a = mkstr(a);
    String str_b = mkstr(b);
    return compare_alphabetically_only_letters(str_a, str_b);
}


internal String
make_uppercase(String string)
{
    for_index_inc(s32, i, string.length)
        string.str[i] = to_uppercase(string.str[i]);
    return string;
}


internal String
make_lowercase(String string) {
    for_index_inc(s32, i, string.length)
        string.str[i] = to_lowercase(string.str[i]);
    return string;
}

//TODO is there a simd instruction or a non looped version of this?
internal u32 
pow10_u32(s32 exponent)
{
    assert (exponent >= 0);
    u32 value = 1;
    while(exponent--)
    {
        u32 new_value = value * 10;
        assert ((new_value / 10) == value); //overflow check
        value = new_value;
    }
    assert (value != 0); //something really bad happened, (overflow but check failed...?)
    return value;
}

//TODO is there a simd instruction or a non looped version of this?
internal u64
pow10_u64(s32 exponent) {
    assert (exponent >= 0);
    u64 value = 1;
    while(exponent--)
    {
        u64 new_value = value * 10;
        assert ((new_value / 10) == value); //overflow check
        value = new_value;
    }
    assert (value != 0); //something really bad happened, (overflow but check failed...?)
    return value;
}

/*internal String
s32_to_string(char *buffer, s32 buffer_size, s32 value)
{
    
} */

internal boolint
try_parse_hexadecimal(char *str, s32 desired_length, u32 *output) {
    assert (desired_length >= 0);
    if_assert(desired_length > 0, str);
    
    s32 start_index = 0;
    if ((desired_length >= 2) && (str[0] == '0') && (to_lowercase(str[1]) == 'x')) {
        start_index = 2;
    }
    
    u32 result = 0;
    s32 length = min2(8, desired_length - start_index);
    s32 end_plus_one = start_index + length;
    for (s32 at = start_index; at < end_plus_one; at += 1) {
        result *= 16;
        char c = str[at];
        if (is_digit(c)) {
            result += (c - '0');
        } else {
            c = to_lowercase(c);
            if (c >= 'a' && c <= 'f') {
                result += (c - 'a') + 10;
            } else {
                //TODO return some error code or something
                //if we using a decent language we could just return an optinal return value
                return false;
            }
        }
    }
    
    *output = result;
    return true;
}

internal boolint
try_parse_hexadecimal(String str, u32 *output) {
    return try_parse_hexadecimal(str.str, str.length, output);
}

internal void
append(String *string, char c) {
    if (string->length < (string->size-1)) {
        string->str[string->length++] = c;
        string->str[string->length]   = 0;
    }
}
    
    
internal void
append(String *string, char *copy, s32 copy_length) {
    char *at = string->str + string->length; 
    assert (ptr_in_base_count(at, string->str, string->size));
    
    //-1 for null terminator
    s32 amt_left = (string->size-1) - string->length; 
    if (amt_left > 0) {
        s32 amt_to_copy = min2(amt_left, copy_length);
        copy_memory(at, copy, amt_to_copy);
        string->length += amt_to_copy;
        at[amt_to_copy] = 0;
    }
}

//NOTE copies as much as it can into string
internal void
append(String *string, String copy) {
    append(string, copy.str, copy.length);
}

internal void
append_valist(String *string, char *fmt, va_list list) {
    assert (string->str);
    char *at = string->str + string->length; 
    assert (ptr_in_base_count(at, string->str, string->size));
    
    s32 amt_left  = string->size - string->length; 
    assert (amt_left > 0);
    s32 amt_added = stbsp_vsnprintf(at, amt_left, fmt, list);
    string->length += amt_added;
}

internal void
appendf(String *string, char *fmt, ...) {   
    va_list list;
    va_start(list, fmt);
    append_valist(string, fmt, list);
    va_end(list);
}

// NOTE we allow size to be passed in here in case we want to allocate with extra space for some reason
// it's kind of silly because I never do that...
internal String
duplicate(Memory_Arena *arena, char *str, s32 length)
{
    if (length == 0) return {}; //null string please
    assert (length > 0);
    
    String result = {};
    result.str = push_array(arena, char, length + 1);
    if (result.str)
    {
        result.length = length;
        result.size   = length + 1;
        
        for(s32 i = 0; i < result.length; i += 1)
        {
            assert (str[i]); //length of str was incorrect
            result.str[i] = str[i];
        }
        result.str[result.length] = 0; //null terminate
    }
    return result;
}

#if 1
internal String //NOTE this function bites me everytime, maybe we should always postfix this funcs with _cstring or something...
duplicate(Memory_Arena *arena, char *str) {
    return duplicate(arena, str, strlen(str));    
}
#endif


internal String
duplicate(Memory_Arena *arena, String string)
{	return duplicate(arena, string.str, string.length);	}


internal Array<String>
break_up_lines(Memory_Arena *arena, String str) {
    if (str.length <= 0) return {};
    
    Array<String> lines = make_array<String>(arena, 32);
    lines.can_realloc = true;
    
    char *at = str.str;
    char *end_plus_one = str.str + str.length;
    
    char *line_start = at;
    while (at < end_plus_one) {
        if (*at == '\n') {
            String line = mkstr(line_start, at);
            if (line.length > 0) {
                lines.add(line);    
            }
            line_start = at + 1;
        }
        at += 1;
    }
    
    return lines;
    
}

internal String
push_stringf_valist(Memory_Arena *arena, char *fmt, va_list list) {
    assert (arena->used < arena->size);
    
    char *buffer = (char *)(arena->data + arena->used);
    u32 buffer_size   = arena->size - arena->used;
    u32 result_length = stbsp_vsnprintf(buffer, buffer_size, fmt, list);
    
    String result = mkstr(buffer, result_length, result_length + 1);
    arena->used += result.size;
    
    assert (arena->used <= arena->size);
    return result;
}

//always null terminated
internal String
push_stringf(Memory_Arena *arena, char *fmt, ...) {
    va_list list;
    va_start(list, fmt);
    String result = push_stringf_valist(arena, fmt, list);
    va_end(list);
    return result;
}

internal String //NOTE size includes null terminator
push_string(Memory_Arena *arena, s32 size) {
    String result = {};
    if (size > 0) {
        result.str = push_array(arena, char, size);
        if (result.str) {
            result.size   = size;
            result.str[0] = 0;
        }
    }
    return result;
}

internal String //took this idea from Jon Blow, thanks!
tprintf(char *fmt, ...) {
    Memory_Arena *temp = get_temp_memory();
    assert (temp);

    va_list list;
    va_start(list, fmt);
    String result = push_stringf_valist(temp, fmt, list);
    va_end(list);

    return result;
}

internal boolint
make_hexadecimal_string(String *string, u32 init_value) {
    if (string->size < (8+1)) return false;
    
    string->length = 0;
    if (string->size >= (10+1)) {
        append(string, "0x"s);
    }
    
    u32 value = init_value;
    s32 count = 0;
    char buffer[8] = {};
    constexpr char symbol[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    while (value) {
        u32 remainder = (value & 15);
        buffer[8-count-1] = symbol[remainder];
        count += 1;
        
        value -= remainder;
        value /= 16;
    }
    
    #if 1
    append(string, buffer, countof(buffer));
    #else //short version
    if (count > 0) {
        append(string, buffer + countof(buffer) - count, count);
    } else {
        append(string, "0"s);
    }
    #endif
    
    
    return true;
}

internal String 
make_hexadecimal_string(Memory_Arena *arena, u32 value) {
    String result = push_string(arena, 10+1);
    make_hexadecimal_string(&result, value);
    return result;
}

internal u32
string_to_u32(char *string, s32 length)
{
    assert (length > 0);
    u32 value = 0;
    for(s32 i = 0; i < length; i += 1)
    {
        assert (is_digit(string[i]));
        
        u32 new_value = value * 10;
        assert ((new_value / 10) == value); //overflow check
        assert ((U32_MAX - new_value) >= (u32)(string[i] - '0'));
        new_value += (string[i] - '0');
        
        value = new_value;
    }
    return value;
}

internal u32
string_to_u32(String string)
{ return string_to_u32(string.str, string.length); }

internal u64
string_to_u64(char *string, s32 length) {
    assert (length > 0);
    u64 value = 0;
    for(s32 i = 0; i < length; i += 1)
    {
        assert (is_digit(string[i]));
        
        u64 new_value = value * 10;
        assert ((new_value / 10) == value); //overflow check
        assert ((U64_MAX - new_value) >= (u64)(string[i] - '0'));
        new_value += (string[i] - '0');
        
        value = new_value;
    }
    return value;
}

internal u64
string_to_u64(String str) {
    return string_to_u64(str.str, str.length);
}

internal s32
string_to_s32(char *string, s32 length) {
    assert (length > 0);
    nil is_negative = false;
    if (string[0] == '-')
    {
        is_negative = true;
        string += 1;
        length -= 1;
    }
    
    s32 result;
    u32 u32_val = string_to_u32(string, length);
    if (is_negative)
    {
        assert (u32_val <= (u32)-S32_MIN);
        result = -(s32)u32_val;
    }
    else
    {
        assert (u32_val <= (u32)S32_MAX);
        result = (s32)u32_val;
    }
    return result;
}

internal s32
string_to_s32(String str) {
    return string_to_s32(str.str, str.length);
}

internal s64
string_to_s64(char *string, s32 length)
{
    assert (length > 0);
    nil is_negative = false;
    if (string[0] == '-')
    {
        is_negative = true;
        string += 1;
        length -= 1;
    }
    
    s64 result;
    u64 u64_val = string_to_u64(string, length);
    if (is_negative)
    {
        assert (u64_val <= (u64)-S64_MIN);
        result = -(s64)u64_val;
    }
    else
    {
        assert (u64_val <= (u64)S64_MAX);
        result = (s64)u64_val;
    }
    
    return result;
}

//TODO how do we want to guard against overflow errors when 
//user passes in a very large string like 3.1423444444444444444444444444444444444
//for the decimal part we could just truncate the length
//but what about for the whole value part? ....
internal f32
string_to_f32(char *string, s32 length) {
    assert (length > 0);
    f32 sign = 1.0f;
    if (string[0] == '-') {
        sign = -1.0f;
        string += 1;
        length -= 1;
    }
    
    s32 decimal_index = length;
    for(s32 i = 0; i < length; i += 1) {
        assert (is_digit(string[i]) || string[i] == '.');
        if (string[i] == '.') { 
            decimal_index = i;
            break;
        }
    }
    
    f32 result = (f32)string_to_u32(string, decimal_index);
    if (decimal_index < length) {
        s32 fractional_length = length - (decimal_index + 1);
        if (fractional_length > 0) {
            //TODO clamp fractional_length here to some reasonable number....
            f32 fractional_value = (f32)string_to_u64(string + decimal_index + 1, fractional_length);
            f32 fractional_power = (f32)pow10_u64(fractional_length);
            assert (fractional_power != 0);
            result += (fractional_value / fractional_power);
        }
    }
    return sign*result;
}

internal f64
string_to_f64(char *string, s32 length) {
    assert (length > 0);
    f64 sign = 1.0f;
    if (string[0] == '-') {
        sign = -1.0f;
        string += 1;
        length -= 1;
    }
    
    s32 decimal_index = length;
    for (s32 i = 0; i < length; i += 1) {
        assert (is_digit(string[i]) || string[i] == '.');
        if (string[i] == '.') { 
            decimal_index = i;
            break;
        }
    }
    
    f64 result = (f64)string_to_u64(string, decimal_index);
    if (decimal_index < length) {
        s32 fractional_length = length - (decimal_index + 1);
        if (fractional_length > 0) {
            //TODO clamp fractional_length here to some reasonable number....
            f64 fractional_value = (f64)string_to_u64(string + decimal_index + 1, fractional_length);
            f64 fractional_power = (f64)pow10_u64(fractional_length);
            assert (fractional_power != 0);
            result += (fractional_value / fractional_power);
        }
    }
    return sign*result;
}

internal s32
find_first_char_offset(String string, char c)
{
    for_index_inc(s32, i, string.length)
        if (string.str[i] == c)	return i;
    return -1;
}

internal s32
find_first_substring_offset(String string, String substring)
{
    for_index_inc(s32, i, string.length)
    {
        s32 length = string.length - i;
        if (length < substring.length) break;
        
        if (strmatch(string.str + i, substring.length, substring.str, substring.length))
            return i;
    }
    return -1;
}

 

internal void
null_terminate(String string)
{
    assert (string.str);
    assert (string.length < string.size);
    string.str[string.length] = 0;
}

// TODO make this work on other platforms. (Linux/macos for example exclusivly use forward slashes)
internal bool
is_absolute_filepath(String string) {
    
    let is_separator = [](char c) -> bool {
        return (c == '/') || (c == '\\');
    };
        
    
    if (string.length >= 1) {
        if (is_separator(string.str[0])) return true; //absolute path from root of "current drive" ... might be a Windows thing only
        else if (string.length >= 2) {
            if (is_alpha(string.str[0]) && string.str[1] == ':') return true;
        }
    }
    
    return false;
}

//TODO #define this to be different on linux/macos
internal bool
is_directory_separator(char c) {
    return (c == '/') || (c == '\\');
}

internal boolint //means it's just a simple filename.exe, no \\ or /
has_directories(String string) {
    for (char *at = string.str; at < (string.str + string.length); at += 1) {
        if (is_directory_separator(*at)) return true;
    }
    return false;
}


internal String //gets rid of C:/path/.../
remove_file_path(String string) {
    for_index_dec(s32, i, string.length) {
        if (string[i] == '/' || string[i] == '\\')
            return advance(string, i + 1);        
    } 
    return string;
}

internal String //C:/dir1/file.txt -> C:/dir1/ -> C:/ -> C:/
remove_current_file_directory(String string) {
    for (s32 i = string.length - 1; i >= 0; i -= 1) {
        if (string[i] == '/' || string[i] == '\\') {
            string.length = i + 1;
            break;
        }
    }
    return string;
}

internal void
remove_current_file_directory(String *string)
{ *string = remove_current_file_directory(*string); }



// NOTE in trying to be consistent with remove_file_extension() which removes '.ext'
// this also includes a starting period
internal String
get_file_extension(String string) {
    s32 first_whitespace = string.length;
    for_index_dec (s32, at, string.length) {
        if (string[at] == ' ') {
            first_whitespace = at;
        } else if (string[at] == '.') {
            //NOTE if we hit a ../ or ./ before a .ext, then the filepath is ill-formed
            if (at < (string.length-1)) {
                if (is_directory_separator(string[at+1]) || string[at+1] == '.') {
                    return {};
                }
            }
            
            s32 length = first_whitespace - at;
            if (length > 0) {
                String result = mkstr(string.str + at, length, string.size - at);
                return result;
            } else return {};
        }
    }
    return {};
}

internal String
remove_from_end(String string, String end) {
    assert (string.length > 0);
    if (end.length == 0) return string; //{} null string is considered a substring, so we allow this but not for string
    
    assert ((end.str >= string.str) && (end.str < (string.str + string.length)));
    
    String result = string;
    result.length = (s32)(end.str - string.str);
    result.size   = result.length;
    return result;
}


internal String //gets rid of .whatever
remove_file_extension(String string) { 
    for_index_dec (s32, at, string.length) {
        if (string[at] == '.') {
            
            //this is ../ or ./ filepath symbol, just ignore
            if (at < (string.length-1)) {
                if (is_directory_separator(string[at+1]) || string[at+1] == '.'){
                    continue;
                }
            }
            
            string.length = at;
            break;
        }
    }
    return string;
}

inline void //gets rid of .whatever
remove_file_extension(String *string) {
    *string = remove_file_extension(*string);
}

//no directory/path and no extension
internal String
get_file_base_name(String filepath)
{
    String result = remove_file_path(filepath);
    result = remove_file_extension(result);
    return result;
}

internal String
trim_whitespace(String string) {
    assert (string.length >= 0);
    if_assert(string.length > 0, string.str);

    char *start = string.str;
    char *end_plus_one = string.str + string.length;
    while (start < end_plus_one) {
        if (*start == ' '  || *start == '\t' || *start == '\n' || *start == '\r') {
            start += 1;
        } else break;
    }

    char *end = end_plus_one - 1;
    while (end >= start) {
        if (*end == ' '  || *end == '\t' || *end == '\n' || *end == '\r') {
            end -= 1;
        } else break;
    }

    if (end >= start) {
        String result;
        result.str    = start;
        result.length = (s32)(end - start)+1;
        result.size   = result.length;
        return result;
    } else return {};
}

internal void
trim_whitespace(String *string) {
    *string = trim_whitespace(*string);
}

internal String
trim_left_whitespace(String string) {
    assert (string.length >= 0);
    if_assert(string.length > 0, string.str);
    
    char *at = string.str;
    while (at < (string.str + string.length)) {
        if (*at == ' '  || *at == '\t' || *at == '\n' || *at == '\r') {
            at += 1;
        } else break;
    }
    
    s32 length = (s32)((string.str + string.length) - at);
    if (length > 0) {
        s32 size = (s32)((string.str + string.size) - at);
        String result = mkstr(at, length, size);
        return result;
    } else return {};
}

internal void
trim_left_whitespace(String *string) {
    *string = trim_left_whitespace(*string);
}

internal String
trim_right_whitespace(String string) {
    s32 amount_to_remove = 0;
    for (s32 at = string.length - 1; at >= 0; at -= 1) {
        if (string.str[at] == ' '  || string.str[at] == '\t' ||
            string.str[at] == '\n' || string.str[at] == '\r') {
            amount_to_remove += 1;
        } else break;
    }

    assert (string.length >= amount_to_remove);
    if (amount_to_remove == string.length) {
        return {};
    } else {
        string.length -= amount_to_remove;
        return string;
    }
}

internal void
trim_right_whitespace(String *string) {
    *string = trim_right_whitespace(*string);
}





#define LOCAL_STRING_BUILDER(name, buffer_size) char buffer_for_ ## name[buffer_size]; String_Builder name = make_string_builder(buffer_for_ ## name, (buffer_size))

//TODO make fixed size version of this
struct String_Builder {
    char *buffer;
    u32 size;
    u32 current_string_offset;
    u32 current_string_length;
};

internal String_Builder
make_string_builder(char *buffer, s32 buffer_size) {
    assert (buffer && buffer_size > 1);
    String_Builder builder = {};
    builder.buffer = buffer;
    builder.size = buffer_size;
    return builder;
}

template <s32 SIZE>
internal String_Builder
make_string_builder(char (&buffer)[SIZE]) {
    return make_string_builder(buffer, SIZE);
}

internal void
append(String_Builder *builder, char *str, s32 length) {
    char *at = builder->buffer + builder->current_string_offset + builder->current_string_length;
    assert (ptr_in_base_count(at, builder->buffer, builder->size));
    
    s32 amt_added = 0;
    s32 amt_left  = (s32)(builder->buffer + builder->size - at - 1); //-1 for null terminator
    assert (amt_left > 0);
    
    while(amt_added < amt_left && amt_added < length)
    {
        at[amt_added] = str[amt_added];
        assert (at[amt_added]); //length is incorrect
        amt_added += 1;
    }
    builder->current_string_length += amt_added;
}

internal void
append(String_Builder *builder, char *str) 
{   append(builder, str, strlen(str));   }

internal void
append(String_Builder *builder, String string) 
{   append(builder, string.str, string.length);   }

internal void
append_valist(String_Builder *builder, char *fmt, va_list list)
{
    char *at = builder->buffer + builder->current_string_offset + builder->current_string_length;
    assert (ptr_in_base_count(at, builder->buffer, builder->size));
    
    s32 amt_left  = (s32)((builder->buffer + builder->size) - at); //no -1, because  snprintf null terminates always 
    assert (amt_left > 0);
    s32 amt_added = stbsp_vsnprintf(at, amt_left, fmt, list); 
    builder->current_string_length += amt_added;
}



internal void
appendf(String_Builder *builder, char *fmt, ...)
{   
    va_list list;
    va_start(list, fmt);
    append_valist(builder, fmt, list);
    va_end(list);
}

internal void
remove_trailing_whitespace(String_Builder *builder) {
    assert (builder->current_string_length > 0);
    u32 index = builder->current_string_offset + builder->current_string_length - 1;
    for(;;)
    {
        if (builder->buffer[index] == ' '  || builder->buffer[index] == '\t' ||
            builder->buffer[index] == '\n' || builder->buffer[index] == '\r')
        {
            builder->buffer[index] = 0;
            assert (builder->current_string_length > 0);
            builder->current_string_length -= 1;
        }
        else break;
        
        if (index == builder->current_string_offset)	break;
    }
}

//NOTE final call to mkstr null terminates the current string and returns it
internal String
mkstr(String_Builder *builder)
{
    //ensure we have room to null terminate
    assert ((builder->current_string_offset + builder->current_string_length) < builder->size);
    String result = mkstr(builder->buffer + builder->current_string_offset,
        builder->current_string_length,
        builder->current_string_length + 1);
    result.str[result.length] = 0; 
    
    builder->current_string_offset += builder->current_string_length + 1;
    builder->current_string_length = 0;
    return result;
}

//this is basically appendf, mkstr(), we have this we can inline it as function argument...
internal String
push_stringf(String_Builder *builder, char *fmt, ...)
{
    assert (builder->current_string_length == 0);
    va_list list;
    va_start(list, fmt);
    append_valist(builder, fmt, list);
    va_end(list);
    return mkstr(builder);
}

internal String
duplicate(String_Builder *builder, String str)
{
    assert (builder->current_string_length == 0);
    append(builder, str);
    return mkstr(builder);
}

internal String
duplicate(String_Builder *builder, char *str, s32 length)
{
    assert (builder->current_string_length == 0);
    append(builder, str, length);
    return mkstr(builder);
}


//copies as much as src as possible, leaving space for null terminator
internal void
duplicate(String *str, String src)
{
    assert (str->size > 0);
    str->length = min2(str->size-1, src.length); //-1 allows space for null terminator
    copy_memory(str->str, src.str, str->length);
    str->str[str->length] = 0;  
}


internal String_Builder
push_string_builder(Memory_Arena *arena, s32 buffer_size)
{
    char *buffer = push_array(arena, char, buffer_size);
    return make_string_builder(buffer, buffer_size);
}






//TODO better place for this

//TODO manually create this
//global String DIR4_STRINGS[] = {
    //strlit("NORTH"), 
    //strlit("EAST"), 
    //strlit("SOUTH"), 
    //strlit("WEST"), 
    //strlit("NO_DIRECTION")
//};

//NOTE this must match the Direction8 enum
global String Direction8_Strings[] = {
    "NORTH"s,         //0
    "NORTHEAST"s,     //1
    "EAST"s,          //2
    "SOUTHEAST"s,     //3
    "SOUTH"s,         //4
    "SOUTHWEST"s,     //5
    "WEST"s,          //6
    "NORTHWEST"s,     //7
};

internal String
get_string(Direction8 dir) {
    switch(dir) {
    case NORTH: return "NORTH"s;
    case EAST:  return "EAST"s;
    case SOUTH: return "SOUTH"s;
    case WEST:  return "WEST"s;
    case NORTHEAST: return "NORTHEAST"s;
    case NORTHWEST: return "NORTHWEST"s;
    case SOUTHEAST: return "SOUTHEAST"s;
    case SOUTHWEST: return "SOUTHWEST"s;
    default:    return strlit("NO_DIRECTION8");
    };
}




//internal f32        get_angle_0_to_tau(f32 angle);
//internal Direction8 to_direction8(f32 angle);
//TODO get rid of this, currently metaprogram is calling get_string(e->facing_dir)
//internal String
//get_string(f32 angle)
//{
    //Direction8 dir = to_direction8(angle);
    //return get_string(dir);
//}

 

/*
internal String
get_direction_string(Direction8 dir)
{
    switch(dir)
    {
    case NORTH: return strlit("NORTH");
    case EAST:  return strlit("EAST");
    case SOUTH: return strlit("SOUTH");
    case WEST:  return strlit("WEST");
    case NO_DIRECTION: return strlit("NO DIRECTION");
    default: return strlit("INVALID Direction8 value");
    }
} */



template <s32 SIZE>
struct String_Buffer {
    s32 length;
    char str[SIZE];
    static constexpr s32 size = SIZE;
};

template <s32 SIZE>
internal void
clear(String_Buffer<SIZE> *buffer) {
    buffer->length = 0;
    buffer->str[0] = 0;
}

template <s32 SIZE>
internal String
mkstr(String_Buffer<SIZE> *buffer) {
    String string = mkstr(buffer->str, buffer->length, countof(buffer->str));
    return string;
}

template <s32 SIZE>
internal String
make_hexadecimal_string(String_Buffer<SIZE> *buffer) {
    cassert(SIZE > 8);
    buffer->length = 0; 
    String string = mkstr(buffer);
    make_hexadecimal_string(&buffer);
    buffer->length = string->length;
    return string;
    return string;
}

template <s32 SIZE>
internal void
duplicate(String_Buffer<SIZE> *buffer, String src) {
    buffer->length = min2(SIZE-1, src.length);
    if (buffer->length > 0) {
        copy_memory(buffer->str, src.str, buffer->length);
    }
    buffer->str[buffer->length] = 0; 
}

template <s32 SIZE>
internal void
append(String_Buffer<SIZE> *buffer, String string) {
    assert (buffer->length >= 0 && buffer->length < SIZE); //not equal to bc of null terminator
    
    s32 amt_left  = (SIZE-1) - buffer->length; //-1 for terminator
    s32 amt_to_copy = min2(amt_left, string.length);
    if (amt_to_copy > 0) {
        char *dst = buffer->str + buffer->length;
        copy_memory(dst, string.str, amt_to_copy);
        buffer->length += amt_to_copy; 
        assert (buffer->length < SIZE); //should never fire 
        buffer->str[buffer->length] = 0;
    }
}

template <s32 SIZE>
internal void
append(String_Buffer<SIZE> *buffer, char c) {
    String string = {&c, 1, 1};
    append(buffer, string);
}

template <s32 SIZE>
internal void
append_valist(String_Buffer<SIZE> *buffer, char *fmt, va_list list) {
    char *at = buffer->str + buffer->length; 
    assert (ptr_in_base_count(at, buffer->str, SIZE-1));
    
    //NOTE no -1 here since stbsp_vsnprintf always null terminates
    s32 amt_left  = SIZE - buffer->length; 
    s32 amt_added = stbsp_vsnprintf(at, amt_left, fmt, list); //always returns zero-terminated string
    buffer->length += amt_added;
}

template <s32 SIZE>
internal void
appendf(String_Buffer<SIZE> *buffer, char *fmt, ...) {   
    va_list list;
    va_start(list, fmt);
    append_valist(buffer, fmt, list);
    va_end(list);
}

template <s32 SIZE>
internal void
clear(String_Buffer<SIZE> *buffer, String string) {
    buffer->length = 0;
    buffer->str[0] = 0;
    append(buffer, string);
}

template <s32 SIZE>
internal void
clearf(String_Buffer<SIZE> *buffer, char *fmt, ...) {
    buffer->length = 0;
    buffer->str[0] = 0;
    
    va_list list;
    va_start(list, fmt);
    append_valist(buffer, fmt, list);
    va_end(list);
}

template <s32 SIZE>
internal bool
strmatch(String_Buffer<SIZE> *buffer, String string) {
    return strmatch(mkstr(buffer), string);
}

template <s32 SIZE>
internal bool
strmatch(String string, String_Buffer<SIZE> *buffer) {
    return strmatch(mkstr(buffer), string);
}

template <s32 SIZE1, s32 SIZE2>
internal bool
strmatch(String_Buffer<SIZE1> *buffer1, String_Buffer<SIZE2> *buffer2) {
    return strmatch(mkstr(buffer1), mkstr(buffer2));
}


#endif //STRINGS_H
