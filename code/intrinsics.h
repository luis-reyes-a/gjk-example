#ifndef INTRINSICS_H
#define INTRINSICS_H
#include "intrin.h" 

#define SSE    true //steam says 100% have this
#define SSE2   true //steam says 100% have this
#define SSE3   true //steam says 100% have this
#define SVML   true //?
#define SSE4_1 true //steam says 99.04%% have this

typedef __m128 m128;
typedef __m256 m256;

internal void *
copy_memory(void *dst, void *src, uintptr size) {
    assert (dst && src);
    //apparently this instruction became very performant starting with ivy-bridge (2012)
    //and got faster for sizes smaller than 128 bytes long in ice-lake (2019) //very recent...
    __movsb((unsigned char*)dst, (unsigned char*)src, size);
    return dst;
}

internal void *
memcopy(void *dst, void *src, uintptr size) {
    assert (dst && src);
	//NOTE I'm pretty sure movsb is fine to use if size==0, but couldn't find anything saying so online...
    //apparently this instruction became very performant starting with ivy-bridge (2012)
    //and got faster for sizes smaller than 128 bytes long in ice-lake (2019) //very recent...
    __movsb((unsigned char*)dst, (unsigned char*)src, size);
    return dst;
}

internal void *
set_memory(void *dst, int value, uintptr size) {
    assert (dst);
    assert (value >= 0 && value <= 0xff);
    __stosb((unsigned char*)dst, (unsigned char)value, size);
    return dst;
}

internal void *
zero_memory(void *dst, uintptr size) {
    assert (dst);
    __stosb((unsigned char*)dst, 0, size);
    return dst;
}

internal void *
memzero(void *dst, uintptr size) {
    assert (dst);
    __stosb((unsigned char*)dst, 0, size);
    return dst;
}



#if SSE2
internal f32 //SSE1 
sqroot(f32 x)
{
    m128 lane = _mm_sqrt_ss(_mm_set_ss(x));
    f32 result = _mm_cvtss_f32(lane);
    return result;
}

//NOTE this was taken from martins. I'm not sure how well it works
internal f32 
floor_sse2(f32 x)
{
  m128 f = _mm_set_ss(x);
  m128 one = _mm_set_ss(1.0f);

  m128 t = _mm_cvtepi32_ps(_mm_cvttps_epi32(f));
  m128 r = _mm_sub_ps(t, _mm_and_ps(_mm_cmplt_ps(f, t), one));

  return _mm_cvtss_f32(r);
}

//NOTE can ceil_sse2 just be floor_sse2(x + 1) ??
#endif



#if SSE
//> rounding stuff
internal s32 //SSE
round_s32(f32 value)
{
    //TODO the default convert mode is to round, is there anyway to check to ensure it's correct?
    return _mm_cvtss_si32(_mm_set_ss(value));
}

internal u32 //SSE
round_u32(f32 value)
{
    //TODO the default convert mode is to round, is there anyway to check to ensure it's correct?
    assert (value >= 0);
    return (u32)(_mm_cvtss_si32(_mm_set_ss(value))); //TODO is there a convert to u32?
}

internal f32 //SSE
round(f32 value)
{
    //TODO the default convert mode is to round, is there anyway to check to ensure it's correct?
    //TODO is there a specific instruction for floatingpoint rounding?
    return (f32)round_s32(value);
}
#endif

#if SSE4_1
internal s32 //SSE4.1
floor_s32(f32 value)
{
    return _mm_cvtss_si32(_mm_floor_ss(_mm_setzero_ps(), _mm_set_ss(value)));
}

internal s32 //SSE4.1
ceil_s32(f32 value) {
   //memcpy(0, 0, 1024);
    return _mm_cvtss_si32(_mm_ceil_ss(_mm_setzero_ps(), _mm_set_ss(value)));
}

internal f32 //SSE4.1
floor(f32 value)
{
    return _mm_cvtss_f32(_mm_floor_ss(_mm_setzero_ps(), _mm_set_ss(value)));
}

internal f32 //SSE4.1
ceil(f32 value)
{
   //memcpy(0, 0, 1024);
    return _mm_cvtss_f32(_mm_ceil_ss(_mm_setzero_ps(), _mm_set_ss(value)));
}
#endif



#if SVML
internal f32 
sin(f32 radians)
{
    m128 lane = _mm_sin_ps(_mm_set_ss(radians));
    f32 result = _mm_cvtss_f32(lane);
    return result;
}

internal f32 
cos(f32 radians)
{
    m128 lane = _mm_cos_ps(_mm_set_ss(radians));
    f32 result = _mm_cvtss_f32(lane);
    return result;
}

internal f32 
tan(f32 radians)
{
    m128 lane = _mm_tan_ps(_mm_set_ss(radians));
    f32 result = _mm_cvtss_f32(lane);
    return result;
}

internal f32
sin_t(f32 t) {
    return sin(TAU*t);
}

internal f32
cos_t(f32 t) {
    return cos(TAU*t);
}

internal f32
tan_t(f32 t) {
    return tan(TAU*t);
}

internal f32
acos(f32 ratio)
{
    m128 lane = _mm_acos_ps(_mm_set_ss(ratio));
    f32 result = _mm_cvtss_f32(lane);
    return result;
}

internal f32
asin(f32 ratio) {
    m128 lane = _mm_asin_ps(_mm_set_ss(ratio));
    f32 result = _mm_cvtss_f32(lane);
    return result;
}

internal f32
atan(f32 ratio)
{
    m128 lane = _mm_atan_ps(_mm_set_ss(ratio));
    f32 result = _mm_cvtss_f32(lane);
    return result;
}

internal f32
atan2(f32 y, f32 x) {
    assert (!((x == 0) && (y == 0))); //TODO what do we return here/...
    m128 my = _mm_set_ss(y);
    m128 mx = _mm_set_ss(x);
    m128 lane = _mm_atan2_ps(my, mx);
    f32 result = _mm_cvtss_f32(lane);
    return result;
}

internal f32
exp2(f32 power)
{
    m128 lane = _mm_exp2_ps(_mm_set_ss(power));
    f32 result = _mm_cvtss_f32(lane);
    return result;
}

internal f32 //SVML TODO got to replace this one
cbroot(f32 x)
{
    m128 lane = _mm_cbrt_ps(_mm_set_ss(x));
    f32 result = _mm_cvtss_f32(lane);
    return result;
}
#endif


//TODO find an intrinsic for this (or clear the sign bit)
internal f32 
absval(f32 x) {
    return (x < 0) ? -x : x;
}

//TODO find an intrinsic for this (or clear the sign bit)
internal s32 
absval(s32 x) {
    assert (x != S32_MIN); //NOTE this number doesn't have a positive counterpart to it
    return (x < 0) ? -x : x;
}

#if 0
//this is in vcruntime_string.h but why can't I just stick it in here and let compiler patch it up?
//it says name contradicts previous declaration but if so why can't I just use memcpy?
void* __cdecl memcpy(
                     _Out_writes_bytes_all_(_Size) void* _Dst,
                     _In_reads_bytes_(_Size)       void const* _Src,
                     _In_                          size_t      _Size
                     );
#endif


#endif //INTRINSICS_H
