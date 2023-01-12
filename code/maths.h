#ifndef MATHS_H //Created on October 19th 2020 10:23 am
#define MATHS_H

//NOTE we use a right handed coordinate system 

#ifndef TIME_FUNCTION
#define TIME_FUNCTION(...)
#endif

internal f32
exp_slow(f32 x)
{
    f32 y = 1 + x + (x*x)/2.0f + (x*x*x)/6.0f + (x*x*x*x)/24.0f + (x*x*x*x*x)/120.0f;
    return y;
}

internal f32
exp_very_very_slow(f32 x, s32 num_iterations)
{
    f32 result    = 0;
    f32 power     = 1;
    f32 factorial = 1;
    for (s32 i = 0; i < num_iterations; i += 1)
    {
        f32 new_result = result + power/factorial;
        //TODO detect over/underflow
        result = new_result;
        
        f32 new_power = power *= x;
        //TODO detect over/underflow
        power = new_power;
        
        f32 new_factorial = factorial * (i+1);
        //TODO detect over/underflow
        factorial = new_factorial;
    }
    return result;
}



internal nil
fprox(f32 value, f32 target, f32 epsilon)
{
    f32 diff = absval(target - value);
    return diff <= epsilon;
}

//TODO intrinsic or better way to do this?...
//NOTE remainder and int_val will have the same sign
internal f32
modf(f32 value, s32 *int_val) {
    assert ((value >= S32_MIN) && (value <= S32_MAX)); //isn't s32 always larger?...
    *int_val = (s32)(value);
    f32 fractional = value - (f32)*int_val;
    return fractional;
}

//alternates between 0, 1
internal s32
square_wave(f32 rads) {
    f32 value  = sin(rads);
    s32 result = (value > 0) ? 1 : -1;
    return result;
}

//alternates between 0, 1
internal s32
square_wave01(f32 rads) {
    f32 value  = sin(rads);
    s32 result = (value > 0) ? 1 : 0;
    return result;
}

//alternates between -1, 1 every PI
internal s32
square_wave_t(f32 t) {
    f32 value  = sin_t(t);
    s32 result = (value > 0) ? 1 : -1;
    return result;
}

//alternates between 0, 1
internal s32
square_wave01_t(f32 t) {
    f32 value  = sin_t(t);
    s32 result = (value > 0) ? 1 : 0;
    return result;
}

internal f32
smoothstep(f32 min, f32 max, f32 x) {
    assert ((max-min) > 0.001f);
    f32 t = clamp((x - min) / (max - min), 0.0, 1.0);
    return t * t * (3.0f - 2.0f * t);    
}


union Vector2 {
    struct {
        f32 x, y;    
    };
    struct {
        f32 w, h;   
    };
};



constexpr internal Vector2
v2() {
    return {};
}


inline Vector2
v2(f32 x, f32 y) {	return {x, y};	}

inline Vector2
v2(s32 x, f32 y) {	return {(f32)x, y};	}

inline Vector2
v2(f32 x, s32 y) {	return {x, (f32)y};	}

inline Vector2
v2(s32 x, s32 y) {	return {(f32)x, (f32)y};	}


inline Vector2
v2(Vector2i v) {
	return v2(v.x, v.y);	
}

internal Vector2i
round_s32(Vector2 v) {
    Vector2i result;
    result.x = round_s32(v.x);
    result.y = round_s32(v.y);
    return result;
}

internal Vector2i
floor_s32(Vector2 v)
{
    Vector2i result;
    result.x = floor_s32(v.x);
    result.y = floor_s32(v.y);
    return result;
}

static Vector2
operator+(Vector2 a, Vector2 b) {	return v2(a.x + b.x, a.y + b.y);	}

static Vector2
operator+(Vector2 a, Vector2i b) {	return v2(a.x + b.x, a.y + b.y);	}

static Vector2
operator+(Vector2i a, Vector2 b) {	return v2(a.x + b.x, a.y + b.y);	}

static void
operator+=(Vector2 &a, Vector2 b) {	a = a + b;	}

static void
operator+=(Vector2 &a, Vector2i b) {	a = a + b;	}

static Vector2
operator-(Vector2 a) {	return v2(-a.x, -a.y);	} //unary negate sign

static Vector2
operator-(Vector2 a, Vector2 b) {	return v2(a.x - b.x, a.y - b.y);	}

static Vector2
operator-(Vector2 a, Vector2i b) {	return v2(a.x - b.x, a.y - b.y);	}

static Vector2
operator-(Vector2i a, Vector2 b) {	return v2(a.x - b.x, a.y - b.y);	}

static void
operator-=(Vector2 &a, Vector2 b) {	a = a - b;	}

static Vector2
operator*(Vector2 a, f32 scale) {	return v2(a.x * scale, a.y * scale);	}

static Vector2
operator*(f32 scale, Vector2 a) {	return v2(a.x * scale, a.y * scale);	}

static Vector2
operator*(Vector2 a, s32 scale) {	return v2(a.x * scale, a.y * scale);	}

static Vector2
operator*(s32 scale, Vector2 a) {	return v2(a.x * scale, a.y * scale);	}

static Vector2 //hadamard product
operator*(Vector2 a, Vector2 b) { return v2(a.x*b.x, a.y*b.y); }

static Vector2 //hadamard inverse product ?
operator/(Vector2 a, Vector2 b) {
    assert (b.x > 0.000001f);
    assert (b.y > 0.000001f);
    return v2(a.x/b.x, a.y/b.y); 
}

static Vector2
operator*(Vector2i a, f32 scale) { 
    return v2(a.x*scale, a.y*scale); 
}

static Vector2
operator*(f32 scale, Vector2i b) { 
    return v2(b.x*scale, b.y*scale); 
}

static Vector2
operator*(Vector2 a, Vector2i b) { return v2(a.x*b.x, a.y*b.y); }

static Vector2
operator*(Vector2i a, Vector2 b) { return v2(a.x*b.x, a.y*b.y); }

static void
operator*=(Vector2 &a, Vector2i b) { a = a * b; }

static void
operator*=(Vector2 &a, f32 scale) { a = a * scale; }

static void
operator*=(Vector2 &a, s32 scale) { a = a * scale; }

static Vector2
operator/(Vector2 a, f32 den) {	
    assert (den > 0); 
    return v2(a.x / den, a.y / den);	
}

static void
operator/=(Vector2 &a, f32 den) {	a = a / den;	}

static bool
operator==(Vector2 a, Vector2 b) 
{	return (a.x == b.x) && (a.y == b.y);	}



internal f32
dot2(Vector2 a, Vector2 b) {
    f32 result = a.x*b.x + a.y*b.y; 
    return result;   
}

internal f32 //signed area of bivector 
wedge2(Vector2 a, Vector2 b) {
	return a.x*b.y - b.x*a.y;	
}

internal f32
normsq(Vector2 v)
{   return v.x*v.x + v.y*v.y;   }

internal f32
norm(Vector2 v)
{   return sqroot(v.x*v.x + v.y*v.y);   }

internal f32
norm_and_normalize(Vector2 *v) {   
    f32 length = norm(*v);
    if (length > 0) {
        *v /= length;    
    } else {
        *v = {};
    }
    return length;
}

internal f32
norm_and_noz(Vector2 *v)
{   
    f32 length_squared = dot2(*v, *v);
    if (length_squared > SQUARED(0.0001f))
    {
        f32 length = sqroot(length_squared);
        assert (length != 0);
        *v /= length;
        return length;
    }
    
    *v = {};
    return 0;
}

internal Vector2
normalize(Vector2 a)
{
    f32 length_squared = normsq(a);
    assert (length_squared > 0);
    return a / sqroot(length_squared);
}

internal void
normalize(Vector2 *a) { *a = normalize(*a); }

internal Vector2
noz(Vector2 a, f32 threshold = 0.0001f)
{
    assert (threshold > 0);
    f32 length_squared = normsq(a);
    if (length_squared > SQUARED(threshold))
        return a / sqroot(length_squared);
    else return {};
}

internal void
noz(Vector2 *a, f32 threshold = 0.0001f) { *a = noz(*a, threshold); }

internal boolint
fprox(Vector2 a, Vector2 b, f32 epsilon) {
    boolint result = normsq(a - b) < SQUARED(epsilon);
    return result;
}


internal void
clamp_length(Vector2 *v, f32 length)
{
    f32 sqrlen = normsq(*v);
    if ((sqrlen > 0) && (sqrlen > SQUARED(length)))
    {
        *v /= sqroot(sqrlen);
        *v *= length;
    } 
}

internal Vector2 
clamp(Vector2 v, f32 min, f32 max)
{
    Vector2 result;
    result.x = clamp(v.x, min, max);
    result.y = clamp(v.y, min, max);
    return result;
}

internal void
clamp(Vector2 *v, f32 min, f32 max)
{ *v = clamp(*v, min, max); }

internal Vector2
lerp(Vector2 a, Vector2 b, f32 t)
{   return a + (b - a)*t;   }


//NOTE this does not clamp to keep point within the line...
internal Vector2
project_point_onto_line(Vector2 l1, Vector2 l2, Vector2 point)
{
    Vector2 line = l2 - l1;
    line = normalize(line);
    Vector2 result = l1 + line*dot2(line, point - l1);
    return result;
}

//NOTE this is not the same as simply projecting v onto each basis with a dot product, which only works if the basis are orthonormal
//This returns a vector such that result.x*x_basis + result.y*y_basis = v
internal Vector2
decompose(Vector2 x_basis, Vector2 y_basis, Vector2 v)
{
    //the equivalent of this in linear algebra is Cramer's rule but it's easier to understand with bivectors :)
    Vector2 result;
    f32 x_wedge_y = wedge2(x_basis, y_basis);
    assert (x_wedge_y != 0); //otherwise x_basis && y_basis are parallel and you can't project...
    result.x = wedge2(v, y_basis) /  x_wedge_y;
    result.y = wedge2(v, x_basis) / -x_wedge_y; //y_wedge_x
    return result;
}

//NOTE this assumes normal is already normalized...
internal Vector2
project_onto_normal(Vector2 normal, Vector2 v)
{
    Vector2 result = normal * dot2(v, normal);
    return result;
}






//TODO we can speed this up. Instead of calling get_angle_0_to_tau
internal f32
get_smallest_difference(f32 a2, f32 a1)
{
    f32 diff = wrap_angle(absval(a2 - a1), 0, TAU);
    if (diff > PI) return diff - TAU;
    else return diff;
}

/*
internal f32
get_angle_difference(f32 a, f32 b)
{
    assert (0);
    return 0;
}

internal nil
is_angle_between(f32 angle, f32 a, f32 b)
{
    f32 a_to_b     = get_angle_difference(a, b);
    f32 a_to_angle = get_angle_difference(a, angle);
    if (a_to_b > 0)
        return (a_to_angle >= 0) && (a_to_angle <= a_to_b);
    else 
        return (a_to_angle <= 0) && (a_to_angle >= a_to_b);
} */

internal nil
is_vector_between(Vector2 t, Vector2 a, Vector2 b)
{
    f32 a_wedge_b = wedge2(a, b);
    assert (a_wedge_b != 0); //a and b are collinear, indetermine result
    if (a_wedge_b > 0) //shortest angle is counter-clockwise
    {
        f32 a_wedge_t = wedge2(a, t);
        f32 t_wedge_b = wedge2(t, b);
        return (a_wedge_t >= 0) && (t_wedge_b >= 0);
    }
    else
    {
        f32 a_wedge_t = wedge2(a, t);
        f32 t_wedge_b = wedge2(t, b);
        return (a_wedge_t <= 0) && (t_wedge_b <= 0);
    }
}

#if 0
internal nil
is_angle_between_greater(Vector2 a, Vector2 b, f32 angle)
{
    f32 cos_angle = dot2(normalize(a), normalize(b));
    return cos_angle > cos(angle);
}

//NOTE to reason to have this other than I like reading it
internal nil
is_angle_between_less(Vector2 a, Vector2 b, f32 angle)
{
    f32 cos_angle = dot2(normalize(a), normalize(b));
    return cos_angle < cos(angle);
}
#endif

//returns a rotated 90 degrees counter clockwise
internal Vector2
perp(Vector2 a)
{   return v2(-a.y, a.x);   }

//NOTE if b is very parallel to a, or b is very small (zero-vector), we return a zero vector
internal Vector2
perp(Vector2 a, Vector2 b)
{
    f32 signed_area = a.x*b.y - a.y*b.x;
    s32 sign = (signed_area < 0) ? -1 : 1;
    if ((signed_area*sign) < 0.00001f) sign = 0;
    
    Vector2 result = {-a.y, a.x};
    result *= sign;
    return result;
}



internal f32
atan(Vector2 v) {
    f32 radians = atan2(v.y, v.x); //-pi, pi
    return radians;
}



internal Vector2 //euler
eix2(f32 rads_counter_clockwise) {   
    return v2(cos(rads_counter_clockwise), sin(rads_counter_clockwise));   
}

internal Vector2 //euler
eit2(f32 t) {
    Vector2 result = {cos_t(t), sin_t(t)};
    return result;
}

internal Vector2 
rotate(Vector2 a, f32 rads_counter_clockwise) {
    Vector2 b = eix2(rads_counter_clockwise);
    //just complex multiplication
    //TODO optimize this mult
    return v2(a.x*b.x - a.y*b.y, a.x*b.y + a.y*b.x);
}

internal void
rotate(Vector2 *a, f32 rads_counter_clockwise) { 
    *a = rotate(*a, rads_counter_clockwise); 
}

internal Vector2 
rotate_t(Vector2 a, f32 t) {
    Vector2 b = eit2(t);
    //just complex multiplication
    //TODO optimize this mult
    return v2(a.x*b.x - a.y*b.y, a.x*b.y + a.y*b.x);
}

internal void
rotate_t(Vector2 *a, f32 t) {
    *a = rotate_t(*a, t);
}


union Vector3 {
    struct { f32 x, y, z;       };
    struct { Vector2 xy; f32 z; };
    
};

internal Vector3
v3(f32 x, f32 y, f32 z) {	return {x, y, z};	}

internal Vector3
v3(Vector2 v, f32 z) {	return {v.x, v.y, z};	}

internal f32
dot3(Vector3 a, Vector3 b) 
{ return a.x*b.x + a.y*b.y + a.z*b.z; }

internal Vector3
cross(Vector3 a, Vector3 b) {
    Vector3 result;
    result.x = a.y*b.z - a.z*b.y;
    result.y = a.z*b.x - a.x*b.z;
    result.z = a.x*b.y - a.y*b.x;
    return result;
}

union Vector4 {
    struct{ f32 x, y, z, w; };
    struct{ f32 r, g, b, a; };
    struct{ Vector2 xy, zw; };
    struct{ Vector3 xyz; f32 w; };
};

internal Vector4
v4(f32 x, f32 y, f32 z, f32 w) {	return {x, y, z, w};	}

static Vector4
operator+(Vector4 a, Vector4 b) {	return v4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);	}

static Vector4
operator-(Vector4 a, Vector4 b) {	return v4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);	}

static Vector4
operator*(Vector4 a, f32 s) {	return v4(a.x*s, a.y*s, a.z*s, a.w*s);	}

static Vector4
operator*(f32 s, Vector4 a) {	return v4(a.x*s, a.y*s, a.z*s, a.w*s);	}

internal Vector4
lerp(Vector4 a, Vector4 b, f32 t)
{   return a + (b - a)*t;   } 

//NOTE column major
union Matrix3x3 {
    f32 e[9];
    struct {
        f32 e11, e21, e31;
        f32 e12, e22, e32;
        f32 e13, e23, e33;
    };
    struct {
        Vector3 x_basis;
        Vector3 y_basis;
        Vector3 z_basis;
    };
};

internal Matrix3x3
identity3x3() {
    Matrix3x3 result = {};
    result.e11 = result.e22 = result.e33 = 1;
    return result;
}

static void
transpose(Matrix3x3 *m) {
    SWAP(m->e21, m->e12);
    SWAP(m->e31, m->e13);
    SWAP(m->e32, m->e23);
}

internal Matrix3x3
scale3x3(f32 x, f32 y, f32 z)
{
    Matrix3x3 mat;
    mat.x_basis = {x, 0, 0};
    mat.y_basis = {0, y, 0};
    mat.z_basis = {0, 0, z};
    return mat;
}

internal Vector3
multiply(Matrix3x3 *m, Vector3 v)
{
    TIME_FUNCTION();
    #if 1 //NOTE seems to be faster at the moment
    Vector3 result;
    result.x = m->e11*v.x + m->e12*v.y + m->e13*v.z;
    result.y = m->e21*v.x + m->e22*v.y + m->e23*v.z;
    result.z = m->e31*v.x + m->e32*v.y + m->e33*v.z;
    return result;
    
    
    #else //simd version, doesn't seem to be all that much faster?...
    
    union {
        __m128  m;
        Vector3 v;
    } sum;
    __m128 col;
    __m128 elm;
    
    col   = _mm_setr_ps(m->x_basis.x, m->x_basis.y, m->x_basis.z, 0);
    elm   = _mm_set_ps1(v.x);
    sum.m = _mm_mul_ps(col, elm);
    
    col = _mm_setr_ps(m->y_basis.x, m->y_basis.y, m->y_basis.z, 0);
    elm = _mm_set_ps1(v.y);
    sum.m = _mm_add_ps(sum.m, _mm_mul_ps(col, elm));
    
    col = _mm_setr_ps(m->z_basis.x, m->z_basis.y, m->z_basis.z, 0);
    elm = _mm_set_ps1(v.z);
    sum.m = _mm_add_ps(sum.m, _mm_mul_ps(col, elm));
    
    
    return sum.v;
    #endif
}

internal Matrix3x3
multiply(Matrix3x3 *m, Matrix3x3 *columns) {
    Matrix3x3 result;
    result.x_basis = multiply(m, columns->x_basis);
    result.y_basis = multiply(m, columns->y_basis);
    result.z_basis = multiply(m, columns->z_basis);
    return result;
}

union Matrix4x4 { //column major matrix
    f32 e[16];
    struct {
        f32 e11, e21, e31, e41;
        f32 e12, e22, e32, e42;
        f32 e13, e23, e33, e43;
        f32 e14, e24, e34, e44;
    };
    struct {
        Vector4 x_basis;
        Vector4 y_basis;
        Vector4 z_basis;
        Vector4 w_basis;
    };
};

static Matrix4x4 identity4x4() {
    Matrix4x4 m = {};
    m.e11 = m.e22 = m.e33 = m.e44 = 1;
    return m;
}

static void transpose(Matrix4x4 *m) {
    SWAP(m->e12, m->e21);
    SWAP(m->e13, m->e31);
    SWAP(m->e14, m->e41);
    SWAP(m->e23, m->e32);
    SWAP(m->e24, m->e42);
    SWAP(m->e34, m->e43);
}

inline f32 dot4(Vector4 a, Vector4 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}


static Vector4 multiply(Matrix4x4 *m, Vector4 v) {
    Vector4 result = v.x*m->x_basis + v.y*m->y_basis + v.z*m->z_basis + v.w*m->w_basis;
    return result;
}

static Matrix4x4 multiply(Matrix4x4 *m, Matrix4x4 *columns) {
    Matrix4x4 result;
    result.x_basis = multiply(m, columns->x_basis);
    result.y_basis = multiply(m, columns->y_basis);
    result.z_basis = multiply(m, columns->z_basis);
    result.w_basis = multiply(m, columns->w_basis);
    return result;
}

//// rects
struct Rect2i
{
    Vector2i min;
    Vector2i max;
};

internal nil
is_valid(Rect2i rect)
{	return (rect.max.x > rect.min.x) && (rect.max.y > rect.min.y);	}

internal Vector2i
get_top_left(Rect2i rect) { 
    return {rect.min.x, rect.max.y}; 
} 

internal Vector2i
get_bot_right(Rect2i rect) { 
    return {rect.max.x, rect.min.y}; 
}

internal Range_s32
get_y_range(Rect2i rect) {
    Range_s32 range;
    range.min = rect.min.y;
    range.max = rect.max.y;
    assert (is_valid(range));
    return range;
}

internal Range_s32
get_x_range(Rect2i rect) {
    Range_s32 range;
    range.min = rect.min.x;
    range.max = rect.max.x;
    assert (is_valid(range));
    return range;
}


internal Rect2i
r2i(Vector2i min, Vector2i max)
{
    Rect2i rect;
    rect.min = min;
    rect.max = max;
    assert (is_valid(rect));
    return rect; 
}

internal Rect2i
r2i(Vector2i center, s32 width, s32 height) {
    Rect2i rect;
    rect.min.x = center.x - (width/2);
    rect.min.y = center.y - (height/2);
    rect.max.x = rect.min.x + width;
    rect.max.y = rect.min.y + height;
    assert (is_valid(rect));
    return rect;
}

internal Rect2i
r2i_xx_yy(s32 x1, s32 x2, s32 y1, s32 y2)
{
    Rect2i rect;
    if (x1 < x2)
    {
        rect.min.x = x1;
        rect.max.x = x2;
    }
    else
    {
        rect.min.x = x2;
        rect.max.x = x1;
    }
    if (y1 < y2)
    {
        rect.min.y = y1;
        rect.max.y = y2;
    }
    else
    {
        rect.min.y = y2;
        rect.max.y = y1;
    }
    return rect;
}


inline s32 
get_width(Rect2i rect) {
    assert (is_valid(rect));
    return rect.max.x - rect.min.x;
}

inline s32 
get_height(Rect2i rect) {
    assert (is_valid(rect));
    return rect.max.y - rect.min.y;
}

inline Vector2i
get_dimensions(Rect2i rect) {
    Vector2i dim;
    dim.x = rect.max.x - rect.min.x;
    dim.y = rect.max.y - rect.min.y;
    return dim;
}

inline Vector2
get_dimensions_v2(Rect2i rect) {
    Vector2 dim;
    dim.x = (f32)(rect.max.x - rect.min.x);
    dim.y = (f32)(rect.max.y - rect.min.y);
    return dim;
}

internal s32 
get_area(Rect2i rect)
{
    assert (is_valid(rect));
    s32 width  = rect.max.x - rect.min.x;
    s32 height = rect.max.y - rect.min.y;
    return (width*height);
}

internal Vector2i
get_center(Rect2i rect) {
    assert (is_valid(rect));
    Vector2i pos;
    pos.x = rect.min.x + (get_width(rect)  / 2);
    pos.y = rect.min.y + (get_height(rect) / 2);
    return pos;
}

internal Vector2i
get_center_v2(Rect2i rect) {
    assert (is_valid(rect));
    Vector2i pos;
    pos.x = rect.min.x + (get_width(rect)  / 2);
    pos.y = rect.min.y + (get_height(rect) / 2);
    return pos;
}

internal Rect2i
inverted_infinity_rect2i()
{
    Rect2i rect; //NOTE this one isn't valid :p
    rect.max.x = rect.max.y = S32_MIN;
    rect.min.x = rect.min.y = S32_MAX;
    return rect;
}

internal Vector2i
get_min(Vector2i a, Vector2i b)
{
    Vector2i result = v2i(min2(a.x, b.x), min2(a.y, b.y));
    return result;
}

internal Vector2i
get_max(Vector2i a, Vector2i b)
{
    Vector2i result = v2i(max2(a.x, b.x), max2(a.y, b.y));
    return result;
}




internal boolint
is_point_inside(Rect2i rect, s32 x, s32 y) {
    assert (is_valid(rect));
    return(x >= rect.min.x && x < rect.max.x && y >= rect.min.y && y < rect.max.y);
}

internal boolint
is_point_inside(Rect2i rect, f32 x, f32 y) {
    assert (is_valid(rect));
    return(x >= rect.min.x && x < rect.max.x && y >= rect.min.y && y < rect.max.y);
}

internal nil
is_point_inside(Rect2i rect, Vector2i v)
{ return is_point_inside(rect, v.x, v.y); }

internal nil
is_point_inside(Rect2i rect, Vector2 v)
{ return is_point_inside(rect, v.x, v.y); }

internal nil
do_overlap(Rect2i a, Rect2i b)
{
    assert (is_valid(a) && is_valid(b));
    nil overlap_x;
    if (b.min.x < a.min.x)
        overlap_x = (b.max.x-1) >= a.min.x;
    else
        overlap_x = b.min.x < a.max.x;
    nil overlap_y;
    if (b.min.y < a.min.y)
        overlap_y = (b.max.y-1) >= a.min.y;
    else
        overlap_y = b.min.y < a.max.y;
    return overlap_x && overlap_y;
}

internal Rect2i
expand(Rect2i rect, s32 width, s32 height)
{
    assert (IS_EVEN(width));
    assert (IS_EVEN(height));
    rect.min.x -= width/2;
    rect.max.x += width/2;
    rect.min.y -= height/2;
    rect.max.y += height/2;
    assert (is_valid(rect));
    return rect;
}

internal void
expand(Rect2i *rect, s32 width, s32 height)
{ *rect = expand(*rect, width, height); }

struct Ray_Intersection_Line
{
    Vector2 pos;
    Vector2 dir; //normalize
    f32 line_length;
};

struct Ray_Intersection_Result
{
    nil intersect;
    Vector2 x;
    f32 length_along_first;
    f32 length_along_second;
};

internal Ray_Intersection_Result
find_ray_intersection(Ray_Intersection_Line *p, Ray_Intersection_Line *l)
{
    Ray_Intersection_Result result = {};
    
    f32 l_wedge_p = wedge2(l->dir, p->dir);
    if (l_wedge_p)
    {
        result.intersect = true;
        f32 along_l = wedge2(p->pos, p->dir) /  l_wedge_p;
        f32 along_p = wedge2(l->pos, l->dir) / -l_wedge_p;
        
        result.x = along_p*p->dir + along_l*l->dir;
        
        Vector2 p_to_x = result.x - p->pos;
        result.length_along_first  = norm(p_to_x) * signof(dot2(p_to_x, p->dir));
        Vector2 l_to_x = result.x - l->pos;
        result.length_along_second = norm(l_to_x) * signof(dot2(l_to_x, l->dir));
    }
    return result;
}

internal nil
find_line_intersection_point(Vector2 p1, Vector2 p2, Vector2 l1, Vector2 l2, Vector2 *out_vector)
{
    //@NOTE this doesn't return true if lines are parallel or overlapping
    //just returns true in case where there is a single intersection point
    Ray_Intersection_Line ray_p;
    ray_p.pos = p1;
    ray_p.dir = p2 - p1;
    ray_p.line_length = norm(ray_p.dir);
    ray_p.dir = ray_p.dir / ray_p.line_length;
    
    Ray_Intersection_Line ray_l;
    ray_l.pos = l1;
    ray_l.dir = l2 - l1;
    ray_l.line_length = norm(ray_l.dir);
    ray_l.dir = ray_l.dir / ray_l.line_length;
    
    Ray_Intersection_Result result  = find_ray_intersection(&ray_p, &ray_l);
    nil intersect_as_lines = false;
    if (result.intersect &&
        result.length_along_first  >= 0 && result.length_along_first  <= ray_p.line_length &&        
        result.length_along_second >= 0 && result.length_along_second <= ray_l.line_length)
    {
        intersect_as_lines = true;
        *out_vector = result.x;
    }
    return intersect_as_lines;
}

//internal nil
//find_line_intersection(Vector2 p1, Vector2 p2, Vector2 l1, Vector2 l2, Vector2 *out)
//{
//LineIntersection x = find_line_intersection(p1,p2,l1,l2);
//nil intersects = x.type == LINE_INTERSECTION_SINGLE;
//if (intersects)
//{
//*out = x.x;
//}
//return intersects;
//}

#if 0
internal nil
find_line_intersection(Vector2 l1, Vector2 l2, Vector2 p1, Vector2 p2, Vector2 *result)
{
    Vector2 l = l2 - l1;
    f32 l_length = norm(l);
    assert (l_length);
    l = l / l_length; //normalize
    Vector2 p = p2 - p1;
    f32 p_length = norm(p);
    assert (p_length);
    p = p / p_length; //normalize
    f32 l_wedge_p = wedge2(l, p);
    nil intersect = false;
    if (l_wedge_p)
    {
        f32 along_l = wedge2(p1, p) /  l_wedge_p;
        f32 along_p = wedge2(l1, l) / -l_wedge_p;
        if (along_l >= 0 && along_l <= l_length &&
            along_p >= 0 && along_p <= p_length)
        {
            intersect = true;
            //*result = (l1 + l*along_l);// + (p1 + p*along_p);
            //*result = p1 +  p*along_p;
            f32 t_of_p = along_p / p_length;
            f32 t_of_l = along_l / l_length;
            *result = l*along_l = p*along_p;
        }
    }
    return intersect;
}
#endif

internal Vector2
expect_line_intersection(Vector2 l1, Vector2 l2, Vector2 p1, Vector2 p2)
{
    Vector2 result;
    nil found = find_line_intersection_point(l1, l2, p1, p2, &result);
    assert (found);
    return result;
}

internal Vector2
get_dimensions_for_aspect_ratio(f32 aspect_ratio, Vector2 dim)
{
    assert (dim.x > 0 && dim.y > 0 && aspect_ratio > 0);
    f32 original_aspect_ratio = dim.x / dim.y;
    if (aspect_ratio > original_aspect_ratio) //we want a rect wider than current, keep width, adjust height
        dim.y = (dim.x / aspect_ratio);
    else //we want a rect taller than current, keep height, adjust width
        dim.x = (dim.y * aspect_ratio);
    return dim;
}

internal Vector2i
get_dimensions_for_aspect_ratio(f32 aspect_ratio, Vector2i dim) {
    Vector2 result = get_dimensions_for_aspect_ratio(aspect_ratio, v2(dim));
    return round_s32(result);
}

struct Rect2f {
    Vector2 min, max;
};

internal Rect2f
operator+(Rect2f rect, Vector2 offset)
{
    Rect2f result;
    result.min = rect.min + offset;
    result.max = rect.max + offset;
    return result;
}

internal void
operator-=(Rect2f &lval, Vector2 offset)
{
    lval.min -= offset;
    lval.max -= offset;
}

internal void
operator-=(Rect2f &lval, Vector2i offset)
{
    lval.min.x -= offset.x;
    lval.min.y -= offset.y;
    lval.max.x -= offset.x;
    lval.max.y -= offset.y;
}

internal Vector2
get_top_left(Rect2f rect)
{ return v2(rect.min.x, rect.max.y); } 

internal Vector2
get_bot_right(Rect2f rect)
{ return v2(rect.max.x, rect.min.y); }


internal Rect2f
inverted_infinity_rect2f()
{
    Rect2f rect; //NOTE this one isn't valid :p
    rect.max.x = rect.max.y = -F32_MAX;
    rect.min.x = rect.min.y =  F32_MAX;
    return rect;
}

internal nil
is_valid(Rect2f rect) {
    return (rect.max.x > rect.min.x) && (rect.max.y > rect.min.y);
}

internal Vector2
lerp(Rect2f rect, Vector2 t)
{
    Vector2 result;
    result.x = rect.min.x + t.x*(rect.max.x - rect.min.x);
    result.y = rect.min.y + t.y*(rect.max.y - rect.min.y);
    return result;
}

internal Rect2f
r2f_xx_yy(f32 x1, f32 x2, f32 y1, f32 y2) {
    Rect2f rect;
    if (x1 < x2)
    {
        rect.min.x = x1;
        rect.max.x = x2;
    }
    else
    {
        rect.min.x = x2;
        rect.max.x = x1;
    }
    if (y1 < y2)
    {
        rect.min.y = y1;
        rect.max.y = y2;
    }
    else
    {
        rect.min.y = y2;
        rect.max.y = y1;
    }
    return rect;
}

internal Rect2f
r2f_pp(Vector2 p1, Vector2 p2) {
    return r2f_xx_yy(p1.x, p2.x, p1.y, p2.y);
}

internal Rect2f
get_bounding_box_of_swept_rect(Rect2f rect, Vector2 offset)
{
    Rect2f result = rect;
    if (offset.x > 0) //max will change
        result.max.x += offset.x;
    else if(offset.x < 0)
        result.min.x += offset.x;
    
    if (offset.y > 0)
        result.max.y += offset.y;
    else if(offset.y < 0)
        result.min.y += offset.y;
    
    return result;
}

internal Rect2f //union of rect
make_enclosing_rect2f(Rect2f a, Rect2f b) {
    Rect2f result;
    result.min.x = min2(a.min.x, b.min.x);
    result.max.x = max2(a.max.x, b.max.x);
    result.min.y = min2(a.min.y, b.min.y);
    result.max.y = max2(a.max.y, b.max.y);
    return result;
}

internal Rect2f
make_enclosing_rect2f(Vector2 p1, Vector2 p2) {
    Rect2f rect = r2f_xx_yy(p1.x, p2.x, p1.y, p2.y);
    return rect;
}

internal Rect2f
make_intersection(Rect2f a, Rect2f b) {
    Rect2f rect;
    rect.min.x = max2(a.min.x, b.min.x);
    rect.max.x = min2(a.max.x, b.max.x);
    rect.min.y = max2(a.min.y, b.min.y);
    rect.max.y = min2(a.max.y, b.max.y);
    return rect;
}

internal Rect2f 
expand(Rect2f rect, f32 width, f32 height)
{
    Rect2f result;
    result.min.x = rect.min.x - width*0.5f;
    result.max.x = rect.max.x + width*0.5f;
    result.min.y = rect.min.y - height*0.5f;
    result.max.y = rect.max.y + height*0.5f;
    return result;
}

internal void
expand(Rect2f *rect, f32 width, f32 height)
{ *rect = expand(*rect, width, height); }

//internal Rect2f 
//expand_unsafe(Rect2f rect, f32 width, f32 height)
//{
    //Rect2f result;
    //result.min.x = rect.min.x - width*0.5f;
    //result.max.x = rect.max.x + width*0.5f;
    //result.min.y = rect.min.y - height*0.5f;
    //result.max.y = rect.max.y + height*0.5f;
    //return result;
//}

internal Rect2f 
expand(Rect2f rect, f32 dim)
{ return expand(rect, dim, dim); }

internal void
expand(Rect2f *rect, f32 dim)
{ *rect = expand(*rect, dim, dim); }

//internal Rect2f 
//expand_unsafe(Rect2f rect, f32 dim)
//{ return expand_unsafe(rect, dim, dim); }

/*
internal Rect2f 
expand(Vector2 center, f32 width, f32 height)
{
   assert (width > 0 && height > 0);
   Rect2f result;
   result.min.x = center.x - width*0.5f;
   result.max.x = center.x + width*0.5f;
   result.min.y = center.y - height*0.5f;
   result.max.y = center.y + height*0.5f;
   assert (is_valid(result));
   return result;
}

internal Rect2f 
expand(Vector2 center, f32 dim)
{ return expand(center, dim, dim); } */

internal Rect2f 
r2f(Vector2 min, Vector2 max) {
    Rect2f rect;
    rect.min = min;
    rect.max = max;
    return rect; 
}

internal Rect2f 
r2f(Rect2i irect) {
    Rect2f result;
    result.min = v2(irect.min);
    result.max = v2(irect.max);
    return result;
}

internal Rect2f //wraps the rect around the tile the min/max are centered around
enclose_like_tiles_r2f(Rect2i irect) {
    Rect2f rect;
    rect.min = v2(irect.min);
    rect.min.x -= 0.5f;
    rect.min.y -= 0.5f;
    
    rect.max = v2(irect.max);
    rect.max.x -= 0.5f;
    rect.max.y -= 0.5f;
    return rect;
}

internal Rect2f
r2f(Vector2 center, f32 width, f32 height) {
    Vector2 dim = v2(width, height);
    Rect2f rect;
    rect.min = center - 0.5f*dim;
    rect.max = rect.min + dim;
    return rect;
}

internal Rect2f
r2f(Vector2 center, f32 size) {
    return r2f(center, size, size);
}

internal Rect2f
r2f(Vector2i center, f32 width, f32 height) { 
    return r2f(v2(center), width, height); 
}



internal Vector2
get_center(Rect2f rect)
{ return lerp(rect.min, rect.max, 0.5f); }

inline f32
get_width(Rect2f rect) { 
    return rect.max.x - rect.min.x; 
}

inline f32
get_height(Rect2f rect) { 
    return rect.max.y - rect.min.y; 
}

inline Vector2
get_dimensions(Rect2f rect) {
    Vector2 dim = {rect.max.x - rect.min.x, rect.max.y - rect.min.y};
    return  dim;
}

internal void
get_center_dim(Rect2f rect, Vector2 *center, Vector2 *dim) {
    dim->x = get_width(rect);
    dim->y = get_height(rect);
    center->x = rect.min.x + 0.5f*dim->x;
    center->y = rect.min.y + 0.5f*dim->y;
}

internal nil
is_point_inside(Rect2f rect, f32 x, f32 y) {
    return (x >= rect.min.x) && (x < rect.max.x) && (y >= rect.min.y) && (y < rect.max.y);
}

internal nil
is_point_inside(Rect2f rect, Vector2 v) { 
    return is_point_inside(rect, v.x, v.y); 
}

internal nil
do_overlap(Rect2f a, Rect2f b)
{
    assert (is_valid(a) && is_valid(b));
    f32 b_width  = b.max.x - b.min.x;
    f32 b_height = b.max.y - b.min.y;
    Vector2 b_center = b.min + v2(b_width, b_height)*0.5f;
    a = expand(a, b_width, b_height);
    return is_point_inside(a, b_center);
}

internal nil
do_intersect(Rect2f a, Rect2f b)
{ 
    TIME_FUNCTION();
    return do_overlap(a, b); 
}

internal nil
is_point_inside_circle(Vector2 circle_center, f32 radius, Vector2 point)
{
    f32 distance_from_circle_squared = normsq(point - circle_center);
    return distance_from_circle_squared <= SQUARED(radius);
}


#if 0 //TODO this needs to be more robust, or add an epsilon value to it
internal nil
find_line_intersection_point(Rect2f rect, Vector2 p1, Vector2 p2, Vector2 *out_intersection_point)
{
    assert (is_valid(rect));
    
    let test_wall = [](f32 sx, f32 dx, f32 wallx, f32 sy, f32 dy, f32 ymin, f32 ymax, f32 *tmin) -> nil {
        if (dx < 0.001f) return false;
        
        f32 t = (wallx - sx) / dx;
        if (t >= 0 && t <= 1.0f)
        {
            f32 y = sy + t*dy;
            if (y >= ymin && y < ymax)
            {
                *tmin = t;
                return true;
            }
        }
        return false;
    };
    
    nil found_intersection = false;
    f32 intersection_t = -1;
    
    f32 dx = (p2.x - p1.x);
    f32 dy = (p2.y - p1.y);
    
    if      (test_wall(p1.x, dx, rect.min.x, p1.y, dy, rect.min.y, rect.max.y, &intersection_t)) //left wall
        found_Rreintersection = 1;
    else if (test_wall(p1.x, dx, rect.max.x, p1.y, dy, rect.min.y, rect.max.y, &intersection_t)) //right wall
        found_intersection = 2;
    else if (test_wall(p1.y, dy, rect.min.y, p1.x, dx, rect.min.x, rect.max.x, &intersection_t)) //bot wall
        found_intersection = 3;
    else if (test_wall(p1.y, dy, rect.max.y, p1.x, dx, rect.min.x, rect.max.x, &intersection_t)) //top wall
        found_intersection = 4;
    
    if (found_intersection)
    {
        *out_intersection_point = lerp(p1, p2, intersection_t);
    }
    return found_intersection;
}
#endif


internal s32
line_intersects_rect(Vector2 p1, Vector2 p2, Rect2f rect, f32 *tmin)
{
    //NOTE because of floating point imprecision we may fail on some cases if we are intersecting
    //right on one of the corners
    //we know do if y <= max.y because of this
    //even though my rects are traditionally inclusive, exclusive
    //TODO in the corner case, should we still say we intersected twice?
    //because the game may use that to mean we passed entirely through
    //in the future we will want to handle the corner cases more precisely
    
    assert (is_valid(rect));
    s32 num_intersection_points = 0;
    f32 t = *tmin = F32_MAX;
    
    //test vertical wall collide
    f32 dx = (p2.x - p1.x);
    if (dx)
    {
        t = (rect.min.x - p1.x) / dx; //left wall
        if (t >= 0 && t <= 1.0f)
        {
            f32 y = lerp(p1.y, p2.y, t);
            if (y >= rect.min.y && y <= rect.max.y)
            {
                *tmin = min2(*tmin, t);
                num_intersection_points += 1;
            }
        }
        
        t = (rect.max.x - p1.x) / dx; //right wall
        if (t >= 0 && t <= 1.0f)
        {
            f32 y = lerp(p1.y, p2.y, t);
            if (y >= rect.min.y && y <= rect.max.y)
            {
                *tmin = min2(*tmin, t);
                num_intersection_points += 1;
            }
        }
    }
    
    //test horizontal wall collide
    f32 dy = (p2.y - p1.y);
    if (dy)
    {
        t = (rect.min.y - p1.y) / dy; //bottom wall
        if (t >= 0 && t <= 1.0f)
        {
            f32 x = lerp(p1.x, p2.x, t);
            if (x >= rect.min.x && x <= rect.max.x)
            {
                *tmin = min2(*tmin, t);
                num_intersection_points += 1;
            }
        }
        
        t = (rect.max.y - p1.y) / dy; //top wall
        if (t >= 0 && t <= 1.0f)
        {
            f32 x = lerp(p1.x, p2.x, t);
            if (x >= rect.min.x && x <= rect.max.x) {
                *tmin = min2(*tmin, t);
                num_intersection_points += 1;
            }
        }
    }
    
    assert (num_intersection_points < 3);
    assert (num_intersection_points ? (*tmin >= 0 && *tmin <= 1.0f) : true);
    return num_intersection_points;
}

//TODO better place for this
internal Direction8 
get_cardinal_direction(f32 angle) {
    angle = wrap_angle(angle, 0, TAU);
    if     (angle > TAU*(1.0f/8) && angle <= TAU*(3.0f/8))  return NORTH;
    else if (angle > TAU*(3.0f/8) && angle <= TAU*(5.0f/8)) return WEST;
    else if (angle > TAU*(5.0f/8) && angle <= TAU*(7.0f/8)) return SOUTH;
    else                                                    return EAST;
}

internal Direction8
get_cardinal_direction_t(f32 t) {
	t = wrap_angle(t, 0, 1);
	if      (t > (1.0f/8) && t <= (3.0f/8)) return NORTH;
	else if (t > (3.0f/8) && t <= (5.0f/8)) return WEST;
	else if (t > (5.0f/8) && t <= (7.0f/8)) return SOUTH;
	else                                    return EAST;
}

internal Direction8
get_cardinal_direction(Vector2 dir) {
    if (normsq(dir) > 0)
    {
        f32 rads = atan(dir);
        return get_cardinal_direction(rads);
    }
    else return NO_DIRECTION8;
}

internal Vector2
get_direction_vector(Direction8 dir)
{
    constexpr f32 sqrt2_over_2 = 0.70710678118f;
    switch(dir)
    {
    case NORTH:   return v2( 0,  1);
    case EAST:    return v2( 1,  0);
    case SOUTH:   return v2( 0, -1); 
    case WEST:    return v2(-1,  0);
    
    case NORTHEAST:   return v2( sqrt2_over_2,  sqrt2_over_2);
    case SOUTHEAST:   return v2( sqrt2_over_2, -sqrt2_over_2);
    case SOUTHWEST:   return v2(-sqrt2_over_2, -sqrt2_over_2);
    case NORTHWEST:   return v2(-sqrt2_over_2,  sqrt2_over_2);
    
    case NO_DIRECTION8: return v2(0,0); 
    default:  assert (0); return {};
    }
}

internal Vector2i
get_manhattan_vector2i(Direction8 dir)
{
    switch(dir)
    {
    case NORTH:   return v2i( 0,  1);
    case EAST:    return v2i( 1,  0);
    case SOUTH:   return v2i( 0, -1); 
    case WEST:    return v2i(-1,  0);
    
    case NORTHEAST:   return v2i( 1,  1);
    case SOUTHEAST:   return v2i( 1, -1);
    case SOUTHWEST:   return v2i(-1, -1);
    case NORTHWEST:   return v2i(-1,  1);
    
    case NO_DIRECTION8: return v2i(0,0); 
    default:  assert (0); return {};
    }
}

internal Vector2
get_manhattan_vector(Direction8 dir)
{
    switch(dir)
    {
    case NORTH:   return v2( 0,  1);
    case EAST:    return v2( 1,  0);
    case SOUTH:   return v2( 0, -1); 
    case WEST:    return v2(-1,  0);
    
    case NORTHEAST:   return v2( 1,  1);
    case SOUTHEAST:   return v2( 1, -1);
    case SOUTHWEST:   return v2(-1, -1);
    case NORTHWEST:   return v2(-1,  1);
    
    case NO_DIRECTION8: return v2(0,0); 
    default:  assert (0); return {};
    }
}

internal Vector2i
get_direction_vector2i(Direction8 dir)
{
    switch(dir)
    {
    case NORTH: return v2i( 0,  1);
    case EAST:  return v2i( 1,  0);
    case SOUTH: return v2i( 0, -1); 
    case WEST:  return v2i(-1,  0);
    
    case NORTHEAST:   assert(0);
    case SOUTHEAST:   assert(0);
    case SOUTHWEST:   assert(0);
    case NORTHWEST:   assert(0);;
    default:    return v2i( 0,  0);
    }
}

struct Line2f
{
    Vector2 start;
    Vector2 delta;
};

internal nil
is_valid(Line2f line) { return normsq(line.delta) > 0; }

internal Line2f
l2f(Vector2 start, Vector2 delta)
{
    Line2f line = {start, delta};
    return line;
}

internal Line2f
l2f(Vector2i start, Vector2i delta)
{
    Line2f line = {v2(start), v2(delta)};
    return line;
}

internal Line2f
l2f_pos(Vector2 start, Vector2 end)
{
    Line2f line = {start, end - start};
    return line;
}



//TODO find a better place for this maybe
struct Pixel_Cell_Iterator
{
    Vector2i init_pos;
    Vector2i target_pos_plus_one; //runs up to target_pos
    Vector2i current_pos;
    
    Vector2 to_target;
    s32 to_target_sign_x;
    s32 to_target_sign_y;
    
    
    nil valid() { return current_pos != target_pos_plus_one; }
    nil get_next()
    {
        if (current_pos != target_pos_plus_one)
        {
            f32 tx = F32_MAX;
            if (to_target_sign_x)
            {
                f32 wall_x  = current_pos.x + to_target_sign_x*0.5f;
                tx = (wall_x - init_pos.x) / to_target.x;
            }
            
            f32 ty = F32_MAX;
            if (to_target_sign_y)
            {
                f32 wall_y  = current_pos.y + to_target_sign_y*0.5f;
                ty = (wall_y - init_pos.y) / to_target.y;
            }
            
            assert (!((tx == F32_MAX) && (ty == F32_MAX)));
            if (tx < ty)
                current_pos.x += to_target_sign_x;
            else
                current_pos.y += to_target_sign_y;
        }
        
        return valid();
    }
};

internal Pixel_Cell_Iterator
iterate_pixels_to_target(Vector2i init_pos, Vector2i target_pos)
{
    Pixel_Cell_Iterator it = {};
    it.init_pos    = init_pos;
    it.current_pos = init_pos;
    
    if (init_pos == target_pos) //pick random direction to make everything else work the same way
    {
        it.to_target = v2(0, 1);
        it.to_target_sign_x = 0;
        it.to_target_sign_y = 1;
        it.target_pos_plus_one = init_pos + v2i(0, 1);
    }
    else
    {
        it.to_target = v2(target_pos - init_pos);
        it.to_target_sign_x = signof(it.to_target.x);
        it.to_target_sign_y = signof(it.to_target.y); 
        
        f32 tx = F32_MAX;
        if (it.to_target_sign_x)
        {
            f32 wallx = target_pos.x + it.to_target_sign_x*0.5f;
            tx = (wallx - init_pos.x) / it.to_target.x;
        }
        
        f32 ty = F32_MAX;
        if (it.to_target_sign_y)
        {
            f32 wally = target_pos.y + it.to_target_sign_y*0.5f;
            ty = (wally - init_pos.y) / it.to_target.y;
        }
        
        assert (!((tx == F32_MAX) && (ty == F32_MAX)));
        it.target_pos_plus_one = target_pos;
        if (tx < ty)
            it.target_pos_plus_one.x += it.to_target_sign_x;
        else
            it.target_pos_plus_one.y += it.to_target_sign_y;
    }
    return it;
}


//simd stuff 
union Vector3_SIMD 
{
    __m128 m;
    f32 _f32[4];
    u32 _u32[4];
    Vector3 v3;
};

internal Vector3_SIMD
v3_simd(f32 x, f32 y, f32 z)
{
    Vector3_SIMD result;
    result.m = _mm_setr_ps(x, y, z, 1); //default to 1 incase we want to do divides
    return result;
}

internal Vector3_SIMD
v3_simd()
{
    Vector3_SIMD result;
    result.m = _mm_setzero_ps();
    return result;
}

inline Vector3_SIMD
operator*(Vector3_SIMD a, Vector3_SIMD b)
{
    Vector3_SIMD result;
    result.m = _mm_mul_ps(a.m, b.m);
    return result;
}

inline Vector3_SIMD
operator/(Vector3_SIMD a, Vector3_SIMD b)
{
    Vector3_SIMD result;
    result.m = _mm_div_ps(a.m, b.m);
    return result;
}

inline void
operator*=(Vector3_SIMD &a, Vector3_SIMD b)
{ a = a * b; }

inline void
operator/=(Vector3_SIMD &a, Vector3_SIMD b)
{ a = a / b; }


#endif //MATHS_H
