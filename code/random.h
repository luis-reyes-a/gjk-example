#ifndef RANDOM_H //created on May 4th 2022 
#define RANDOM_H

//xorshift32 doesn't require math.h (and could belong in ubiquitous.h but it just feels appropriate here
//NOTE if you want something slightly better, look into PCG 
internal u32 
xorshift(u32 *seed) {
    assert (*seed);
    //Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" 
    u32 x = *seed;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *seed = x;
    return x;
}

internal u32
get_random_u32(u32 *seed) {
    u32 result = xorshift(seed);
    return result;
}


internal s32
get_random_index(u32 *seed, s32 size) {
    assert (size > 0);
    xorshift(seed);
    return *seed % size;
}

internal s32
get_random_index_pow2(u32 *seed, s32 size) {
    assert (size > 0);
    assert (IS_POW2(size));
    xorshift(seed);
    s32 result = *seed & (size-1);
    assert (result >= 0 && result < size);
    return result;
}

//TODO is this a correct way to have an event happen as in (3 / 64) -> do thing
//it seems not good because since we do < comparision the numbers are related sequentially (which isn't random?..) idk
internal nil
on_random_chance(u32 *seed, s32 num_chances, s32 total_num_chances)
{
    nil index = get_random_index(seed, total_num_chances);
    return index < num_chances;
}

//TODO is this a valid way to do random occurence
internal nil
pull_random_chance(u32 *seed, s32 num_chances, s32 *total_num_chances)
{
    assert (*total_num_chances > 0);
    assert (num_chances <= *total_num_chances);
    nil index = get_random_index(seed, *total_num_chances);
    *total_num_chances -= num_chances;
    return index < num_chances;
    
    //s32 num_chances = 36;
    //if      (pull_random_chance(seed, 1, &num_chances))  /* 1/36  chance*/;
    //else if (pull_random_chance(seed, 12, &num_chances)) /*12/35 chance*/;
    //else if (pull_random_chance(seed, 12, &num_chances)) /*12/23 chance*/;
    //else if (pull_random_chance(seed, 6, &num_chances))  /* 6/11 chance*/;
    //else if (pull_random_chance(seed, 5, &num_chances))  /* 5/ 5 chance*/;
}

internal f32
get_random_t(u32 *seed) {
    //TODO we gotta come up with a better way of doing this
    //we may get unevent distrubtion just because of the floating point precision loss
    f32 result = (f32)xorshift(seed) / (f32)U32_MAX;
    assert (result >= 0 && result <= 1.0f);
    return result;
}

internal f32 //min and max inclusive
get_random_in_range(u32 *seed, f32 min, f32 max) {
    assert (max > min);
    f32 t = get_random_t(seed);
    f32 result = lerp(min, max, t);
    return result;
}

internal f32 
get_random_angle(u32 *seed) {
    f32 t = get_random_t(seed);
    f32 result = TAU*t;
    return result;
}


internal s32 //min and max inclusive
get_random_in_range(u32 *seed, s32 min, s32 max) {
    if (min == max) return min;
    
    assert (max > min);
    s32 offset = get_random_index(seed, (max+1) - min);
    return min + offset;
}



internal s32 //min and max inclusive
get_random_in_range(u32 *seed, Range_s32 range) {
    return get_random_in_range(seed, range.min, range.max);
}

template<typename TYPE>
internal void
shuffle_array(u32 *seed, TYPE *array, s32 array_count) {
    #if 0 //naive way
    for (s32 i = 0; i < array_count; i += 1) {
        s32 random_index = i + get_random_index(seed, array_count - i);
        SWAP(array[i], array[random_index]);
    }
    #else 
    //NOTE I don't fully understand why this is better. I read that
    //this ensures an element value is updated only once... I don't really see that at all
    for (s32 i = array_count - 1; i > 0; i -= 1) {
        s32 random_index = get_random_index(seed, i+1); 
        SWAP(array[i], array[random_index]);
    }
    #endif
}

template<typename TYPE, s32 SIZE>
internal void
shuffle_array(u32 *seed, TYPE (&array)[SIZE]) {
    shuffle_array(seed, array, SIZE);
}


//NOTE this was adapted from stb_perlin.h, Thanks Sean!
static constexpr s32 Perlin_Noise_Table[512] = {
    23, 125, 161, 52, 103, 117, 70, 37, 247, 101, 203, 169, 124, 126, 44, 123, 
    152, 238, 145, 45, 171, 114, 253, 10, 192, 136, 4, 157, 249, 30, 35, 72, 
    175, 63, 77, 90, 181, 16, 96, 111, 133, 104, 75, 162, 93, 56, 66, 240, 
    8, 50, 84, 229, 49, 210, 173, 239, 141, 1, 87, 18, 2, 198, 143, 57, 
    225, 160, 58, 217, 168, 206, 245, 204, 199, 6, 73, 60, 20, 230, 211, 233, 
    94, 200, 88, 9, 74, 155, 33, 15, 219, 130, 226, 202, 83, 236, 42, 172, 
    165, 218, 55, 222, 46, 107, 98, 154, 109, 67, 196, 178, 127, 158, 13, 243, 
    65, 79, 166, 248, 25, 224, 115, 80, 68, 51, 184, 128, 232, 208, 151, 122, 
    26, 212, 105, 43, 179, 213, 235, 148, 146, 89, 14, 195, 28, 78, 112, 76, 
    250, 47, 24, 251, 140, 108, 186, 190, 228, 170, 183, 139, 39, 188, 244, 246, 
    132, 48, 119, 144, 180, 138, 134, 193, 82, 182, 120, 121, 86, 220, 209, 3, 
    91, 241, 149, 85, 205, 150, 113, 216, 31, 100, 41, 164, 177, 214, 153, 231, 
    38, 71, 185, 174, 97, 201, 29, 95, 7, 92, 54, 254, 191, 118, 34, 221, 
    131, 11, 163, 99, 234, 81, 227, 147, 156, 176, 17, 142, 69, 12, 110, 62, 
    27, 255, 0, 194, 59, 116, 242, 252, 19, 21, 187, 53, 207, 129, 64, 135, 
    61, 40, 167, 237, 102, 223, 106, 159, 197, 189, 215, 137, 36, 32, 22, 5,  
    
    // and a second copy so we don't need an extra mask or static initializer
    23, 125, 161, 52, 103, 117, 70, 37, 247, 101, 203, 169, 124, 126, 44, 123, 
    152, 238, 145, 45, 171, 114, 253, 10, 192, 136, 4, 157, 249, 30, 35, 72, 
    175, 63, 77, 90, 181, 16, 96, 111, 133, 104, 75, 162, 93, 56, 66, 240, 
    8, 50, 84, 229, 49, 210, 173, 239, 141, 1, 87, 18, 2, 198, 143, 57, 
    225, 160, 58, 217, 168, 206, 245, 204, 199, 6, 73, 60, 20, 230, 211, 233, 
    94, 200, 88, 9, 74, 155, 33, 15, 219, 130, 226, 202, 83, 236, 42, 172, 
    165, 218, 55, 222, 46, 107, 98, 154, 109, 67, 196, 178, 127, 158, 13, 243, 
    65, 79, 166, 248, 25, 224, 115, 80, 68, 51, 184, 128, 232, 208, 151, 122, 
    26, 212, 105, 43, 179, 213, 235, 148, 146, 89, 14, 195, 28, 78, 112, 76, 
    250, 47, 24, 251, 140, 108, 186, 190, 228, 170, 183, 139, 39, 188, 244, 246, 
    132, 48, 119, 144, 180, 138, 134, 193, 82, 182, 120, 121, 86, 220, 209, 3, 
    91, 241, 149, 85, 205, 150, 113, 216, 31, 100, 41, 164, 177, 214, 153, 231, 
    38, 71, 185, 174, 97, 201, 29, 95, 7, 92, 54, 254, 191, 118, 34, 221, 
    131, 11, 163, 99, 234, 81, 227, 147, 156, 176, 17, 142, 69, 12, 110, 62, 
    27, 255, 0, 194, 59, 116, 242, 252, 19, 21, 187, 53, 207, 129, 64, 135, 
    61, 40, 167, 237, 102, 223, 106, 159, 197, 189, 215, 137, 36, 32, 22, 5,  
};

// different grad function from Perlin's, but easy to modify to match reference
// NOTE hash effectively decides what gradient vector to select from, 
// so it must be as unique per each [x, y, z] integer coordinate as possible
internal f32 
perlin_grad(s32 hash, f32 x, f32 y, f32 z)
{
    //NOTE x, y, z are the remaining points relative to the corner we are targeting, they may be negative
    assert (x >= -1 && x <= 1);
    assert (y >= -1 && y <= 1);
    assert (z >= -1 && z <= 1);
    //I believe the 4th coord was added just for alignment reasons
    static f32 basis[12][4] = {
        {  1, 1, 0 }, //extra weight
        { -1, 1, 0 }, //extra weight
        {  1,-1, 0 },
        { -1,-1, 0 },
        {  1, 0, 1 },
        { -1, 0, 1 },
        {  1, 0,-1 },
        { -1, 0,-1 },
        {  0, 1, 1 },
        {  0,-1, 1 }, //extra weight
        {  0, 1,-1 },
        {  0,-1,-1 }, //extra weight
    };
    
   // perlin's gradient has 12 cases so some get used 1/16th of the time
   // and some 2/16ths. We reduce bias by changing those fractions
   // to 5/16ths and 6/16ths, and the same 4 cases get the extra weight.
    static unsigned char indices[64] = {
        0,1,2,3,4,5,6,7,8,9,10,11,
        0,9,1,11,
        0,1,2,3,4,5,6,7,8,9,10,11,
        0,1,2,3,4,5,6,7,8,9,10,11,
        0,1,2,3,4,5,6,7,8,9,10,11,
        0,1,2,3,4,5,6,7,8,9,10,11,
    };
    
    // if you use reference permutation table, change 63 below to 15 to match reference
    f32 *grad = basis[indices[hash & 63]];
    //constexpr f32 one_over_sqrt2 = 1.0f / SQRT2;
    //constexpr f32 one_over_sqrt2 = 1.0F;
    //return one_over_sqrt2*(x*grad[0] + y*grad[1] + z*grad[2]);
    return x*grad[0] + y*grad[1] + z*grad[2];
}

internal s32 floor_s32(f32 value);

//NOTE getting this to tile can be kind of tricky first time
//perlin noise wraps around once we get to 255 (we may want to change this I suppose)
//if we want to create a seamless texture, we will only be seamless if the texture creates the entire perlin noise texture
//meaning you want min to sample at 0,0 and max to sample at 255, 255. If you don't do this, it won't be seamless ...
internal f32 //this returns values from -1, to 1
perlin(u32 seed, f32 x, f32 y, f32 z)
       //s32 wrap_x = 256, s32 wrap_y = 256, s32 wrap_z = 256)
{   
    //assert(IS_POW2(wrap_x) && wrap_x <= 256);
    //assert(IS_POW2(wrap_y) && wrap_y <= 256);
    //assert(IS_POW2(wrap_z) && wrap_z <= 256);
    
    constexpr u32 mask = 255; //must be power of 2 minus 1
    //NOTE I got rid of mask paramter because we create the same effect by simply rendering a smaller texture
    //and increasing the distance between sample points... only time this would come in handy is if you wanted 
    //to wrap by different amounts in each direction.
    //u32 xmask = wrap_x - 1;
    //u32 ymask = wrap_y - 1;
    //u32 zmask = wrap_z - 1;
    //cassert(2*(mask+1) == countof(Perlin_Noise_Table));
    
    s32 fx = floor_s32(x);
    s32 fy = floor_s32(y);
    s32 fz = floor_s32(z);
    
    s32 x0 = (fx+0) & mask;//xmask;
    s32 y0 = (fy+0) & mask;//ymask;
    s32 z0 = (fz+0) & mask;//zmask;
    s32 x1 = (fx+1) & mask;//xmask;
    s32 y1 = (fy+1) & mask;//ymask;
    s32 z1 = (fz+1) & mask;//zmask;
    
    u32 i0 = Perlin_Noise_Table[x0];
    u32 i1 = Perlin_Noise_Table[x1];
    
    u32 i00 = Perlin_Noise_Table[i0+y0];
    u32 i01 = Perlin_Noise_Table[i0+y1];
    u32 i10 = Perlin_Noise_Table[i1+y0];
    u32 i11 = Perlin_Noise_Table[i1+y1];
    
    //remainder
    f32 rx = x - fx;
    f32 ry = y - fy;
    f32 rz = z - fz;
    
    //NOTE(luis) why do we do -1 here instead of +1?...
    f32 n000 = perlin_grad(Perlin_Noise_Table[i00+z0], rx  , ry  , rz   );
    f32 n001 = perlin_grad(Perlin_Noise_Table[i00+z1], rx  , ry  , rz-1 );
    f32 n010 = perlin_grad(Perlin_Noise_Table[i01+z0], rx  , ry-1, rz   );
    f32 n011 = perlin_grad(Perlin_Noise_Table[i01+z1], rx  , ry-1, rz-1 );
    f32 n100 = perlin_grad(Perlin_Noise_Table[i10+z0], rx-1, ry  , rz   );
    f32 n101 = perlin_grad(Perlin_Noise_Table[i10+z1], rx-1, ry  , rz-1 );
    f32 n110 = perlin_grad(Perlin_Noise_Table[i11+z0], rx-1, ry-1, rz   );
    f32 n111 = perlin_grad(Perlin_Noise_Table[i11+z1], rx-1, ry-1, rz-1 );
    
    let ease = [](f32 t) -> f32 {
        #if 0 //3t^2 - 2t^3
        #else //6t^5 - 15t^4 + 10t^3
        return CUBED(t) * (t*(t*6-15) + 10);
        #endif
    };
    
    //apparently just lerping with rx, ry, rz, doesn't look as nice/contnuous as this
    f32 u = ease(rx);
    f32 v = ease(ry);
    f32 w = ease(rz);
    
    f32 n00 = lerp(n000,n001,w);
    f32 n01 = lerp(n010,n011,w);
    f32 n10 = lerp(n100,n101,w);
    f32 n11 = lerp(n110,n111,w);
    
    f32 n0 = lerp(n00,n01,v);
    f32 n1 = lerp(n10,n11,v);
    
    return lerp(n0,n1,u);
}

internal f32
perlin2d(u32 seed, f32 xpos, f32 ypos)
{
    s32 fx = floor_s32(xpos);
    s32 fy = floor_s32(ypos);
    
    s32 _x[2] = {fx & 255, (fx + 1) & 255};
    s32 _y[2] = {fy & 255, (fy + 1) & 255};
    
    union {
        f32 dot_products[2][2];
        f32 all[4];
    } gradients;
    
    Vector2 normalized_pos = v2(xpos - fx, ypos - fy);
    assert (normalized_pos.x >= 0 && normalized_pos.x <= 1);
    assert (normalized_pos.y >= 0 && normalized_pos.y <= 1);
    
    //unique hashcode per seed, per y, then per x
    u32 hashcode_for_seed = Perlin_Noise_Table[seed & 255];
    for (s32 iy = 0; iy <= 1; iy += 1)
    {
        s32 y = _y[iy];
        u32 hashcode_y = Perlin_Noise_Table[hashcode_for_seed + y];
        for (s32 ix = 0; ix <= 1; ix += 1)
        {
            s32 x = _x[ix];
            u32 hashcode = Perlin_Noise_Table[x + hashcode_y];
            
            Vector2 signed_pos = normalized_pos - v2(ix, iy); //positive if 0, negative if 1
            //TODO find a good set of basis vectors to select from (apparently there's bias somehow)
            #if 1
            constexpr int basis_mask = 7;
            
            constexpr Vector2 basis[basis_mask + 1] = {
                #if 1
                { 1, 0},
                {-1, 0},
                {0,  1},
                {0, -1},
                
                { 1,  1},
                {-1, -1},
                { 1,  1},
                {-1, -1},
                
                #elif 0 //this looks very labrinth like
                { 1, 0},
                {-1, 0},
                {0,  1},
                {0, -1},
                
                { 1, 0},
                {-1, 0},
                {0,  1},
                {0, -1},
                #else
                { 1,  1},
                {-1, -1},
                { 1,  1},
                {-1, -1},
                
                { 0.5f,  0.5f},
                {-0.5f, -0.5f},
                { 0.5f,  0.5f},
                {-0.5f, -0.5f},
                #endif
            };
            
            
            
            
            
            gradients.dot_products[iy][ix] = dot2(basis[hashcode & basis_mask], signed_pos);
            #else
            
            f32 random_angle = TAU*get_random_t(&hashcode);
            Vector2 gradient = eix2(random_angle);
            breakpoint;
            gradients.dot_products[iy][ix] = dot2(gradient, signed_pos);
            #endif
        }    
    }
    
    let ease = [](f32 t) -> f32 {
        return CUBED(t) * (t*(t*6-15) + 10);
    };
    
    //apparently just lerping with rx, ry, rz, doesn't look as nice/contnuous as this
    f32 u = ease(normalized_pos.x);
    f32 v = ease(normalized_pos.y);
    
    f32 n0 = lerp(gradients.dot_products[0][0], gradients.dot_products[1][0], v);
    f32 n1 = lerp(gradients.dot_products[0][1], gradients.dot_products[1][1], v);
    return lerp(n0, n1, u);
}

internal f32
simplex2d(u32 seed, f32 xpos, f32 ypos, f32 rotation = 0.0f)
{
    constexpr f32 sqrt3 = 1.7320508075688772935274463415059f; //TODO
    constexpr f32 f2 = 0.5f*(sqrt3 - 1.0f);
    f32 s = (xpos + ypos)*f2; //"hairy" factor?
    s32 i = floor_s32(xpos + s);
    s32 j = floor_s32(ypos + s);
    
    constexpr f32 g2 = (3.0f - sqrt3)/6.0f;
    f32 t = (i + j)*g2;
    f32 fx0 = i - t; //unskew cell origin back to xy space
    f32 fy0 = j - t;
    f32 x0 = xpos - fx0; //relative distances
    f32 y0 = ypos - fy0;

    //upper triangle by default
    s32 i1 = 0;
    s32 j1 = 1;
    if (x0 > y0) SWAP(i1, j1); //else lower triangle
    
    f32 x1 = x0 - i1 + g2; //offset middle corner unskewed coords
    f32 y1 = y0 - j1 + g2;
    f32 x2 = x0 - 1 + 2*g2; //offset for last corner in unskewed coords
    f32 y2 = y0 - 1 + 2*g2;
    
    //hashed gradient indices
    s32 ii = i & 255;
    s32 jj = j & 255;
    s32 gi0 = Perlin_Noise_Table[ii+Perlin_Noise_Table[jj]] % 12;
    s32 gi1 = Perlin_Noise_Table[ii+i1+Perlin_Noise_Table[jj+j1]] % 12;
    s32 gi2 = Perlin_Noise_Table[ii+1+Perlin_Noise_Table[jj+1]] % 12;
    
    //gradients
    constexpr Vector3 grad3[12] = {
        {1,1,0}, {-1,1,0}, {1,-1,0}, {-1,-1,0},
        {1,0,1}, {-1,0,1}, {1,0,-1}, {-1,0,-1},
        {0,1,1}, {0,-1,1}, {0,1,-1}, {0,-1,-1}
    };
    
    //contribution from three corners
    
    f32 n0 = 0;
    f32 t0 = 0.5f - x0*x0-y0*y0;
    if (t0 >= 0)
    {
        t0 *= t0;
        Vector2 gradient = grad3[gi0].xy;
        rotate(&gradient, rotation);
        n0 = t0 * t0 * dot2(gradient, v2(x0, y0)); // (x,y) of grad3 used for 2D gradient
    }
    
    f32 n1 = 0;
    f32 t1 = 0.5f - x1*x1-y1*y1;
    if (t1 >= 0)
    {
        t1 *= t1;
        Vector2 gradient = grad3[gi1].xy;
        rotate(&gradient, rotation);
        n1 = t1 * t1 * dot2(gradient, v2(x1, y1));
    }
    
    f32 n2 = 0;
    f32 t2 = 0.5f - x2*x2-y2*y2;
    if (t2 >= 0)
    {
        t2 *= t2;
        Vector2 gradient = grad3[gi2].xy;
        rotate(&gradient, rotation);
        n2 = t2 * t2 * dot2(gradient, v2(x2, y2));
    }
    
    return 70.0f * (n0 + n1 + n2);
}

internal f32
voronoi(u32 seed, f32 xpos, f32 ypos)
{
    //breakpoint;
    s32 fx = floor_s32(xpos);
    s32 fy = floor_s32(ypos);
    
    constexpr u32 mask = 255;
    s32 _x[3] = {(fx - 1) & mask, (fx) & mask, (fx + 1) & mask};
    s32 _y[3] = {(fy - 1) & mask, (fy) & mask, (fy + 1) & mask};
    
    struct Cell {
        Vector2 offset;
        f32 strength;
        u32 id;
    };
    
    union Cell_Positions {
        struct {
            Cell c00;
            Cell c01;
            Cell c02;
            Cell c10;
            Cell c11;
            Cell c12;
            Cell c20;
            Cell c21;
            Cell c22;
        };
        Cell c[3][3];
        Cell all_cells[9];
    };
    
    Cell_Positions cp;
    
    //NOTE if change size here, gotta update the masking down below (see & 15)
    constexpr Vector2 offset_table[16] {//keep this power of two for & modulo mask trick 
        {-3.f/6.f, -3.f/6.f},
        {-3.f/6.f, -1.f/6.f},
        {-3.f/6.f,  1.f/6.f},
        {-3.f/6.f,  3.f/6.f},
        
        {-1.f/6.f, -3.f/6.f},
            //{-1.f/6.f, -1.f/6.f},
            //{-1.f/6.f,  1.f/6.f},
        {-1.f/6.f,  3.f/6.f},
        
        { 1.f/6.f, -3.f/6.f},
            //{ 1.f/6.f, -1.f/6.f},
            //{ 1.f/6.f,  1.f/6.f},
        { 1.f/6.f,  3.f/6.f},
        
        { 3.f/6.f, -3.f/6.f},
        { 3.f/6.f, -1.f/6.f},
        { 3.f/6.f,  1.f/6.f},
        { 3.f/6.f,  3.f/6.f},
        
        
        {0, -1.f/6.f},
        { 1.f/6.f, 0},
        {0,  1.f/6.f},
        {-1.f/6.f, 0},
    };
    
    
    
    for (s32 ry = -1; ry <= 1; ry += 1)
    {
        s32 y = _y[ry+1];
        u32 hashcode_y = Perlin_Noise_Table[y];
        
        for (s32 rx = -1; rx <= 1; rx += 1)
        {
            s32 x = _x[rx+1];
            u32 hashcode = Perlin_Noise_Table[x + hashcode_y]; //unique hashcode per (x,y)
            
            Cell *cell = &cp.c[ry+1][rx+1];
            *cell = {};
            
            cell->offset = v2(rx, ry) + offset_table[hashcode & 15];
            
            constexpr f32 strengths[4] = {1.25f, 1.0f, 0.75f, 1.0f};
            cell->strength = strengths[hashcode & 3];
            
            
            assert (x <= 255);
            assert (y <= 255);
            cell->id = (y << 8) | (x << 0);
        }
    }

    Vector2 pos = v2(xpos - fx - 0.5f, ypos - fy - 0.5f); //-0.5f, 0.5f
    
    
    #if 1
    f32 min_dist_sq =  F32_MAX;
    for (s32 i = 0; i < countof(cp.all_cells); i += 1)
    {
        Cell *cell = cp.all_cells + i;
        f32 dist = normsq(cell->offset - pos);
        //f32 dist = normsq(cell->strength * (cell->offset - pos)); //get some artifacts ... not sure why though?....
        if (dist < min_dist_sq) min_dist_sq = dist;
    }
    assert (min_dist_sq >= 0);
    f32 result = sqroot(min_dist_sq);
    return min2(result, 1.0f);
    #else
    f32 min_dist  = 1.0f;
    f32 min_dist2 = 1.0f;
    for (s32 i = 0; i < countof(cp.all_cells); i += 1)
    {
        Cell *cell = cp.all_cells + i;
        //f32 dist = norm(cell->offset.x - pos.x) + norm(cell->offset.y - pos.y); //manhattan distance
        //f32 dist = max2(norm(cell->offset.x - pos.x), norm(cell->offset.y - pos.y)); //Chebyshev distance
        f32 dist = norm(cell->offset - pos);
        if (dist < min_dist)
        {
            min_dist2 = min_dist;
            min_dist = dist;
        }
        else if (dist < min_dist2)
            min_dist2 = dist;
    }
    //f32 result = min_dist2;
    f32 result = min_dist2 - min_dist;
    result = 1.0f - smoothstep(0.0f,0.05f,result);
    return clamp(min_dist, 0.0f, 1.0f);
    #endif
}

//don't use this ever
internal f32
voronoi_v2(u32 seed, f32 xpos, f32 ypos)
{
    //breakpoint;
    s32 fx = floor_s32(xpos);
    s32 fy = floor_s32(ypos);
    
    constexpr u32 mask = 255;
    s32 _x[2] = {(fx) & mask, (fx + 1) & mask};
    s32 _y[2] = {(fy) & mask, (fy + 1) & mask};
    
    struct Cell {
        Vector2 offset;
        f32 strength;
        u32 id;
    };
    
    union Cell_Positions {
        Cell m[2][2];
        Cell all[4];
    } cells;
    
    //NOTE if change size here, gotta update the masking down below (see & 15)
    constexpr Vector2 offset_table[16] {//keep this power of two for & modulo mask trick 
        {-3.f/6.f, -3.f/6.f},
        {-3.f/6.f, -1.f/6.f},
        {-3.f/6.f,  1.f/6.f},
        {-3.f/6.f,  3.f/6.f},
        
        {-1.f/6.f, -3.f/6.f},
            //{-1.f/6.f, -1.f/6.f},
            //{-1.f/6.f,  1.f/6.f},
        {-1.f/6.f,  3.f/6.f},
        
        { 1.f/6.f, -3.f/6.f},
            //{ 1.f/6.f, -1.f/6.f},
            //{ 1.f/6.f,  1.f/6.f},
        { 1.f/6.f,  3.f/6.f},
        
        { 3.f/6.f, -3.f/6.f},
        { 3.f/6.f, -1.f/6.f},
        { 3.f/6.f,  1.f/6.f},
        { 3.f/6.f,  3.f/6.f},
        
        
        {0, -1.f/6.f},
        { 1.f/6.f, 0},
        {0,  1.f/6.f},
        {-1.f/6.f, 0},
    };
    
    
    
    for (s32 iy = 0; iy <= 1; iy += 1)
    {
        s32 y = _y[iy];
        u32 hashcode_y = Perlin_Noise_Table[y];
        
        for (s32 ix = 0; ix <= 1; ix += 1)
        {
            s32 x = _x[ix];
            u32 hashcode = Perlin_Noise_Table[x + hashcode_y]; //unique hashcode per (x,y)
            
            Cell *cell = &cells.m[iy][ix];
            *cell = {};
            
            cell->offset = v2(-0.5f + 1*ix, -0.5f + 1*iy) + offset_table[hashcode & 15];
            
            constexpr f32 strengths[4] = {1.25f, 1.0f, 0.75f, 1.0f};
            cell->strength = strengths[hashcode & 3];
            
            
            assert (x <= 255);
            assert (y <= 255);
            cell->id = (y << 8) | (x << 0);
        }
    }

    Vector2 pos = v2(xpos - (fx & 255) - 0.5f, 
                     ypos - (fy & 255) - 0.5f); //-0.5f, 0.5f
    f32 min_dist_sq =  F32_MAX;
    
    for (s32 i = 0; i < countof(cells.all); i += 1)
    {
        Cell *cell = cells.all + i;
        f32 dist = normsq(cell->offset - pos);
        //f32 dist = normsq(cell->strength * (cell->offset - pos)); //get some artifacts ... not sure why though?....
        if (dist < min_dist_sq) min_dist_sq = dist;
    }
    assert (min_dist_sq >= 0);
    f32 result = sqroot(min_dist_sq);
    return min2(result, 1.0f);
}


#endif //RANDOM_H
