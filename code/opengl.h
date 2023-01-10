#ifndef OpenGL_H //Created on October 19th 2020 9:04 am
#define OpenGL_H
#include "gl/gl.h"
//NOTE instance rendering requires at least opengl 3.3

/* NOTE on SRGB color because it always confuses the fuck out of me
   linear RGB simply encodes how bright/dark colors are based on emitted intensity (how many photons are emitted)
   rgb(1,1,1) means full brightness
   rgb(.5, .5, .5) means half brightness
   rgb(0,0,0) means min  brightness

   However this looks odd to humans. rgb(.5, 5, .5) doesn't leook halfway bright. It looks brightere
   Humans *percieve light* exponentially/logmar... idk. But basically the monitor tries to correct this
   by raising colors by a gamma exponent (usually 2.2)

   Monitors take colors they are supposed to render and raise them to the 2.2 power, 
   making their valuse lower than they are. rgb(.5, .5, .5) becomes something like rgb(.21763764082, .21763764082, .21763764082)
   but that color appears halfway bright

   So how does this work with opengl...
  
*/

#define GL_NUM_EXTENSIONS                 0x821D
#define GL_FRAMEBUFFER_SRGB               0x8DB9
#define GL_SRGB8_ALPHA8                   0x8C43
#define GL_BGRA                           0x80E1
#define GL_R8                             0x8229

#define GL_MULTISAMPLE                    0x809D
#define GL_MAX_COLOR_TEXTURE_SAMPLES      0x910E
#define GL_MAX_DEPTH_TEXTURE_SAMPLES      0x910F
#define GL_SAMPLE_ALPHA_TO_COVERAGE       0x809E
#define GL_SAMPLE_ALPHA_TO_ONE            0x809F
#define GL_TEXTURE_2D_MULTISAMPLE         0x9100
#define GL_DEPTH_COMPONENT24              0x81A6
#define GL_DEPTH_COMPONENT32              0x81A7
#define GL_DEPTH_COMPONENT32F             0x8CAC

#define GL_CLAMP_TO_EDGE                  0x812F

#define GL_TEXTURE0                       0x84C0 //NOTE this goes up to GL_TEXTURE31, 32 total
#define GL_FRAMEBUFFER                    0x8D40
#define GL_READ_FRAMEBUFFER               0x8CA8
#define GL_DRAW_FRAMEBUFFER               0x8CA9
#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_COLOR_ATTACHMENT1              0x8CE1
#define GL_COLOR_ATTACHMENT2              0x8CE2
#define GL_COLOR_ATTACHMENT3              0x8CE3
#define GL_DEPTH_ATTACHMENT               0x8D00
#define GL_FRAMEBUFFER_COMPLETE           0x8CD5
#define GL_TEXTURE_SWIZZLE_RGBA           0x8E46

#define GL_ARRAY_BUFFER                   0x8892
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_UNIFORM_BUFFER                 0x8A11
#define GL_STREAM_DRAW                    0x88E0
#define GL_STREAM_READ                    0x88E1
#define GL_STREAM_COPY                    0x88E2
#define GL_STATIC_DRAW                    0x88E4
#define GL_STATIC_READ                    0x88E5
#define GL_STATIC_COPY                    0x88E6
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_DYNAMIC_READ                   0x88E9
#define GL_DYNAMIC_COPY                   0x88EA

#define GL_SHADING_LANGUAGE_VERSION       0x8B8C
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_VALIDATE_STATUS                0x8B83
#define GL_INVALID_INDEX                  0xFFFFFFFFu

#define GL_CLIP_DISTANCE0                 0x3000
#define GL_CLIP_DISTANCE1                 0x3001
#define GL_CLIP_DISTANCE2                 0x3002
#define GL_CLIP_DISTANCE3                 0x3003
#define GL_CLIP_DISTANCE4                 0x3004
#define GL_CLIP_DISTANCE5                 0x3005
#define GL_CLIP_DISTANCE6                 0x3006
#define GL_CLIP_DISTANCE7                 0x3007
#define GL_MAX_CLIP_DISTANCES             0x0D32


#define GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB   0x8242

// apparently we gl.h doesn't have these typedefs so we got these from corearb.h
typedef char          GLchar; 
typedef void          GLvoid;
typedef unsigned int  GLenum;
typedef int           GLint;
typedef int           GLsizei;
typedef unsigned int  GLbitfield;
typedef double        GLdouble;
typedef unsigned int  GLuint;
typedef unsigned char GLboolean;
typedef size_t        GLsizeiptr; //NOTE supposed to be khronos_sizett   but can't find it...  
typedef intptr_t      GLintptr;   //NOTE supposed to be khronos_intptr_t but can't find it...

// functions decls we need to get from opengl driver
#define X(ret, name, ...) typedef ret WINAPI name##_( __VA_ARGS__ );  global name##_* name

X(BOOL,  SwapIntervalEXT, int interval);

X(GLubyte*, glGetStringi,          GLenum name, GLuint index);
X(GLuint,   glCreateShader,        GLenum shader_type);
X(void,     glDeleteShader,        GLuint program);
X(void,     glShaderSource,        GLuint shader,  GLsizei count, GLchar **string, GLint *length);
X(void,     glAttachShader,        GLuint program, GLuint shader);
X(void,     glCompileShader,       GLuint shader);
X(GLuint,   glCreateProgram);
X(void,     glDeleteProgram,       GLuint program);
X(void,     glLinkProgram,         GLuint program);
X(void,     glValidateProgram,     GLuint program);

X(void,     glGetProgramiv,        GLuint program, GLenum pname, GLint *params);
X(void,     glGetProgramInfoLog,   GLuint program, GLsizei maxLength, GLsizei *length,         GLchar *infoLog);
X(void,     glGetShaderInfoLog,    GLuint shader,  GLsizei maxLength, GLsizei *length, GLchar *infoLog);

X(GLint,    glGetAttribLocation,   GLuint program, const GLchar *name);
X(void,     glBindAttribLocation,  GLuint program, GLuint index, const GLchar *name);
X(GLint,    glGetUniformLocation,  GLuint program, const GLchar *name);
X(GLuint,   glGetUniformBlockIndex,GLuint program, const GLchar *uniformBlockName);

X(void,     glBindBufferBase, 	 GLenum target,  GLuint index, GLuint buffer);
X(void,     glUniformBlockBinding, GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);

X(void,     glUniform1ui,  GLint location, GLuint v0);
X(void,     glUniform1f,  GLint location, GLfloat v0);
X(void,     glUniform2f,  GLint location, GLfloat v0, GLfloat v1);
X(void,     glUniform3f,  GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
X(void,     glUniform4f,  GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
X(void,     glUniform1i,  GLint location, GLint v0);
X(void,     glUniform2i,  GLint location, GLint v0, GLint v1);
X(void,     glUniform4fv, GLint location, GLsizei count, const GLfloat *value);
X(void,     glUniformMatrix4fv, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
X(void,     glUniformMatrix3fv, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
X(void,     glUseProgram, GLuint program);

X(void,     glGenFramebuffers,    GLsizei n, GLuint *framebuffers);
X(void,     glBindFramebuffer,    GLenum target, GLuint framebuffer);
X(void,     glDeleteFramebuffers, GLsizei n, GLuint *framebuffers);

X(void,     glFramebufferTexture2D, GLenum target, GLenum, GLenum, GLuint, GLint);
X(GLenum,   glCheckFramebufferStatus, GLenum target);
X(void,     glBlitFramebuffer, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1,
  /**/                         GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);

X(void,     glActiveTexture, GLenum texture);

X(void,     glGenBuffers,    GLsizei n, GLuint *buffers);
X(void,     glBindBuffer,    GLenum target, GLuint buffer);
X(void,     glBufferData,    GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
X(void,     glBufferSubData, GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);

X(void,     glTexImage2DMultisample, GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);

X(void,     glEnableVertexAttribArray,  GLuint index);
X(void,     glDisableVertexAttribArray, GLuint index);
X(void,     glVertexAttribPointer,  GLuint index, GLint size, GLenum type, GLboolean normalized, 
  /**/                              GLsizei stride, const GLvoid *pointer);
X(void,     glVertexAttribIPointer, GLuint index, GLint size, GLenum type, 
  /**/                              GLsizei stride, const GLvoid *pointer);
X(void,     glVertexAttribDivisor,  GLuint index, GLuint divisor);

X(void,     glGenVertexArrays, GLsizei n, GLuint *arrays);
X(void,     glBindVertexArray, GLuint array);

X(void,     glDrawElementsInstanced, GLenum mode, GLsizei count, GLenum type, const void * indices, GLsizei primcount);
#undef X

//typedef void (WINAPI *DEBUGPROC)(GLenum __source, GLenum type, GLuint id, GLenum severity,
//GLsizei length, const GLchar *message, void *userParam);

#include "shaders/glsl_shader.h"
#include "shaders/glsl_lightmap.h"

struct Opengl_Framebuffer 
{
    GLuint id;
    GLuint color_attachment;
    GLuint depth_attachment;
    s32 width;
    s32 height;
    s32 multisample_count;
    //s32 is_srgb;
};

#define MAX_INSTANCE_BUFFER_SIZE (MAX_QUAD_COUNT * sizeof(Textured_Quad))

enum Opengl_Flags {
    OpenGL_SUPPORTS_SRGB_FRAMEBUFFER   = (1 << 0),
    OpenGL_SUPPORTS_SRGB_TEXTURE       = (1 << 1),
    OpenGL_SUPPORTS_FRAMEBUFFER_OBJECT = (1 << 2),
};


//NOTE this should match what's in the shaders
#pragma pack(push, 1)
struct UBO_Camera {
    struct Camera_Data { //size 32
        Vector2 pos;
        Vector2 dim;
        Rect2i  window_draw_region;
    };
    
	Camera_Data _cams[MAX_CAMERA_COUNT];
};
#pragma pack(pop)

enum UBO_Binding_Point {
    UBO_CAMERA,
};

struct Opengl {
    char *version;
    char *glsl_version;
    char *vendor;
    char *renderer;
    u32 flags;
    
    
    GLuint vbo; //vertex pos+uv for quads that *NEVER* change
    GLuint ebo; //index array for quad triangle. *NEVER* changes
    GLuint ibo; //per instance buffer object
    GLuint ubo;
    GLuint big_circle_texture; //used by lightmap shader
    GLuint white_texture;
    
    u32 max_texture_size;
    
    
    
    Opengl_Framebuffer framebuffer;
    Opengl_Framebuffer lightmap;
    
    s32 multisample_count;
    s32 max_multisample_count;
    
    GLSL_Shader::Shader   shader;
    GLSL_Lightmap::Shader lightmap_shader;
};
global Opengl OpenGL;

#if DEBUG_BUILD

internal void
flush_errors()
{   while(glGetError() != GL_NO_ERROR);  }

internal void
check_for_errors()
{  
    //1282 - INVALID_OPERATION, meaning invalid args passed in
    GLenum error = glGetError(); 
    assert (error == GL_NO_ERROR);
}
#else
#define flush_errors()
#define check_for_errors()
#endif

internal Vector4
color_as_vector4(u32 color)
{
    Vector4 result;
    result.a = ((color >> 24) & 0xff) / 255.0f;
    result.r = ((color >> 16) & 0xff) / 255.0f;
    result.g = ((color >>  8) & 0xff) / 255.0f;
    result.b = ((color >>  0) & 0xff) / 255.0f;
    return result;
}

internal Vector4
color_as_vector4(Color color)
{
    Vector4 result;
    result.a = color.a / 255.0f;
    result.r = color.r / 255.0f;
    result.g = color.g / 255.0f;
    result.b = color.b / 255.0f;
    return result;
}

struct Opengl_Texture_Parameters 
{
    enum Type
    {
        COLOR, //NOTE assumed to be in sRGB space
        DEPTH,
        SINGLE_CHANNEL,
    } type;
    u32 width;
    u32 height;
    s32 multisample_count;
    void *pixels;
};

internal Opengl_Texture_Parameters
default_texture_params(Opengl_Texture_Parameters::Type type, s32 width, s32 height)
{
    Opengl_Texture_Parameters params = {};
    params.type   = type;
    params.width  = width;
    params.height = height;
    return params;
}


//NOTE if MSAA data is num samples, otherwise it's a pointer to texure data...
internal GLuint
allocate_texture(Opengl_Texture_Parameters params)
{
    flush_errors();
    GLuint result;
    glGenTextures(1, &result); //TODO handle if this fails
    assert (result != 0); //NOTE can glGenTextures ever return 0 as a valid texture id?
    assert (params.width  <= OpenGL.max_texture_size);
    assert (params.height <= OpenGL.max_texture_size);
    
    GLenum filter_type = 0;
    GLenum wrap_type   = 0;
    GLenum target_type = (params.multisample_count > 0) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    glBindTexture(target_type, result);
    u32 memory_allocation_size_guess = 0;
    
    switch(params.type) {
    case Opengl_Texture_Parameters::COLOR:
    {
        //TODO why the fuck does this have to be inverted?... is this why we've been having to multiply colors with themselves for a long time?...
        GLuint texture_format = (OpenGL.flags & OpenGL_SUPPORTS_SRGB_TEXTURE) ? GL_SRGB8_ALPHA8 : GL_RGB8;
        
        if (params.multisample_count > 0)
        {
            CLAMP_HI(params.multisample_count, OpenGL.max_multisample_count);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, params.multisample_count,
                texture_format,
                params.width, params.height, 
                GL_FALSE);
            filter_type = GL_LINEAR;
            wrap_type   = GL_CLAMP_TO_EDGE;
        }
        else
        {
            //NOTE if data null, space is still allocated
            glTexImage2D(GL_TEXTURE_2D, 0, texture_format,
                params.width, params.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid *)params.pixels);
            filter_type = GL_NEAREST;
            wrap_type   = GL_REPEAT;
        }
    } break;
    
    case Opengl_Texture_Parameters::DEPTH:
    {
        if (params.multisample_count > 0)
        {
            CLAMP_HI(params.multisample_count, OpenGL.max_multisample_count);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, params.multisample_count,
                GL_DEPTH_COMPONENT32,
                params.width, params.height, 
                GL_FALSE);
            filter_type = GL_LINEAR;
            wrap_type   = GL_CLAMP_TO_EDGE;
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32,
                params.width, params.height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, (GLvoid *)params.pixels);
            filter_type = GL_NEAREST;
            wrap_type   = GL_REPEAT;
        }
    } break;
    
    case Opengl_Texture_Parameters::SINGLE_CHANNEL:
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, params.width, params.height, 0, 
            GL_RED, GL_UNSIGNED_BYTE, (GLvoid *)params.pixels);
        filter_type = GL_NEAREST;
        wrap_type   = GL_REPEAT;
        
        GLint swizzle[] = {GL_RED, GL_RED, GL_RED, GL_RED};
        glTexParameteriv(target_type, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
    } break;
    
    default: assert (0);
    }
    check_for_errors();
    
    flush_errors();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_type);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_type);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_type);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_type);
    glBindTexture(target_type, 0);
    check_for_errors();
    return result;
}


internal Opengl_Framebuffer
make_framebuffer(s32 width, s32 height, s32 multisample_count)
{
    Opengl_Framebuffer framebuffer = {};
    glGenFramebuffers(1, &framebuffer.id);
    assert (framebuffer.id != 0);
    framebuffer.width  = width;
    framebuffer.height = height;
    framebuffer.multisample_count = multisample_count;
    //framebuffer.is_srgb = is_srgb;
    //if (is_srgb)	assert (hasfl(OpenGL, OpenGL_SUPPORTS_SRGB_TEXTURE));
    return framebuffer;
}

internal void
attach_color_texture(Opengl_Framebuffer *framebuffer)
{
    flush_errors();
    
    Opengl_Texture_Parameters params = default_texture_params(Opengl_Texture_Parameters::COLOR, framebuffer->width, framebuffer->height);
    //params.is_srgb = framebuffer->is_srgb;
    params.multisample_count = framebuffer->multisample_count;
    framebuffer->color_attachment = allocate_texture(params);
    
    
    assert (framebuffer->id);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
        ((framebuffer->multisample_count > 0) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D),
        framebuffer->color_attachment, 0);
    check_for_errors();
}

internal void
attach_depth_texture(Opengl_Framebuffer *framebuffer)
{
    Opengl_Texture_Parameters params = default_texture_params(Opengl_Texture_Parameters::DEPTH, framebuffer->width, framebuffer->height);
    params.multisample_count = framebuffer->multisample_count;
    framebuffer->depth_attachment = allocate_texture(params);
    
    GLenum target_type = 
        
        assert (framebuffer->id);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
        ((framebuffer->multisample_count > 0) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D), 
        framebuffer->depth_attachment, 0);
}

//TODO it might be a good idea to just allocate a big enough framebuffer to cover the max window monitor size
//and just do a subimage call equivalent with drawing, instead of calling glDeleteTextures
internal void
bind_framebuffer_for_drawing(Opengl_Framebuffer *framebuffer, s32 viewport_width, s32 viewport_height)
{
    assert (framebuffer->id);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id);
    #if DEBUG_BUILD
    GLenum framebuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    assert (framebuffer_status == GL_FRAMEBUFFER_COMPLETE);
    #endif
    if (framebuffer->width != viewport_width || framebuffer->height != viewport_height)
    {
        framebuffer->width  = viewport_width;
        framebuffer->height = viewport_height;
        
        //TODO we may not need to call glDeleteTextures as I think we can just
        //cal glTexImage2D() again with new dimensions...
        if (framebuffer->color_attachment)
        {
            glDeleteTextures(1, &framebuffer->color_attachment);
            attach_color_texture(framebuffer);
        }
        if (framebuffer->depth_attachment)
        {
            glDeleteTextures(1, &framebuffer->depth_attachment);
            attach_depth_texture(framebuffer);
        }   
    }
    glViewport(0, 0, viewport_width, viewport_height);
}

internal void
set_multisample_count(s32 new_multisample_count)
{
    CLAMP_HI(new_multisample_count, OpenGL.max_multisample_count);
    if (OpenGL.multisample_count != new_multisample_count)
    {
        Opengl_Framebuffer *framebuffer = &OpenGL.framebuffer;
        assert (framebuffer->multisample_count == OpenGL.multisample_count);
        framebuffer->multisample_count = new_multisample_count;
        if (framebuffer->color_attachment)
        {
            glDeleteTextures(1, &framebuffer->color_attachment);
            attach_color_texture(framebuffer);
        }
        if (framebuffer->depth_attachment)
        {
            glDeleteTextures(1, &framebuffer->depth_attachment);
            attach_depth_texture(framebuffer);
        } 
        OpenGL.multisample_count = new_multisample_count;
    }
}

internal void
init_opengl()
{
    //glEnable(GL_CULL_FACE);// for 2D we can just negate the basis to flip 
    //glCullFace(GL_BACK);   // the textures around and save on texture memory ...
    glDepthMask(GL_TRUE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glEnable(GL_FRAMEBUFFER_SRGB); //NOTE check to see if we can do this otherwise emulate in shader
    //also NOTE textures are in SRGB space, so we need to sample them and convert them into linear space before the end of the fragment shader
    
    glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_GREATER); //smaller values pass through and overwrite, 1.0f is the "farthest away" 
    glDepthFunc(GL_GEQUAL);
    
    if (SwapIntervalEXT)
    {
        //TODO how can I figure out how long the interval is...should I just measure it?
        SwapIntervalEXT(1);
    }
    
    //NOTE Don't know why we need a vao, but we do, bound here and never unbound again
    GLuint vao; 
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    #if 1
    //NOTE take a read at mjp article: https://mynameismjp.wordpress.com/2012/10/24/msaa-overview/
    //NOTE MSAA is an optimized version of supersampling...
    //fragement shader runs once per pixel if coverage mask is not zero,
    //SSAA runs fragment for every subsample of a pixel
    //however just like SSAA, render targets (depth and color) increase in size
    
    //TODO if multisample is not turned on ATC wont' work obviously 
    //this means for textures with alpha we'd need to discard pixels through a separate shader..
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &OpenGL.max_multisample_count);
    OpenGL.multisample_count = DEFAULT_MULTISAMPLE_COUNT;
    CLAMP_HI(OpenGL.multisample_count, OpenGL.max_multisample_count);
    #endif
    
    
    //TODO pass in initial window dimensions here...
    OpenGL.framebuffer = make_framebuffer(DEFAULT_FRAMEBUFFER_WIDTH, DEFAULT_FRAMEBUFFER_HEIGHT, OpenGL.multisample_count);
    attach_color_texture(&OpenGL.framebuffer);
    attach_depth_texture(&OpenGL.framebuffer);
    OpenGL.lightmap = make_framebuffer(DEFAULT_FRAMEBUFFER_WIDTH, DEFAULT_FRAMEBUFFER_HEIGHT, 0);
    attach_color_texture(&OpenGL.lightmap);
    
    glGenBuffers(1, &OpenGL.vbo);
    glGenBuffers(1, &OpenGL.ebo);
    {
        Vector4 vertices[] = 
        {
            //pos...       uvs...
            {-0.5f, -0.5f, 0, 0}, //0 bot left
            {-0.5f, +0.5f, 0, 1}, //1 top left
            {+0.5f, -0.5f, 1, 0}, //2 bot right
            {+0.5f, +0.5f, 1, 1}, //3 top right
        };
        //1 3
        //0 2
        //The winding is COUNTER_CLOCKWISE. 
        u32 indices[] = 
        {
            0, 2, 1, //bottom left triangle
            1, 2, 3, //top right triangle
        };
        
        //NOTE if we modify these repeatidly switch to GL_DYNAMIC_DRAW
        glBindBuffer(GL_ARRAY_BUFFER, OpenGL.vbo);
        glBufferData(GL_ARRAY_BUFFER,         sizeof(vertices), vertices, GL_STATIC_DRAW); 
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, OpenGL.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),  indices, GL_STATIC_DRAW); 
    }
    
    glGenBuffers(1, &OpenGL.ibo);
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL.ibo);
    glBufferData(GL_ARRAY_BUFFER, MAX_INSTANCE_BUFFER_SIZE, 0, GL_DYNAMIC_DRAW);
    
    glGenBuffers(1, &OpenGL.ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, OpenGL.ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(UBO_Camera), 0, GL_DYNAMIC_DRAW);
    
    {
        Memory_Arena *scratch = &Temporary_Memory_Arena;
        RESTORE_MEMORY_ARENA_ON_SCOPE_EXIT(scratch);
        let buffer = push_struct(scratch, String_Buffer<1024>); 
        
        appendf(buffer, "#define SIMULATE_SRGB_CONVERSION_ON_FINAL_WRITE    %d\n",  !(OpenGL.flags & OpenGL_SUPPORTS_SRGB_FRAMEBUFFER));
        appendf(buffer, "#define SIMULATE_SRGB_CONVERSION_ON_TEXTURE_SAMPLE %d\n",!(OpenGL.flags & OpenGL_SUPPORTS_SRGB_TEXTURE));
        appendf(buffer, "#define MAX_CAMERA_COUNT %d\n\n", MAX_CAMERA_COUNT);
        
        append(buffer,  ""
               "vec4 sample_srgb_texture(sampler2D texture_unit, vec2 uv) {\n"
               "    vec4 result = texture(texture_unit, uv);\n"
               "    #if SIMULATE_SRGB_CONVERSION_ON_TEXTURE_SAMPLE\n"
               "    #if 0\n"
               "    result.rgb *= result.rgb;\n"
               "    #else\n"
               "    float exponent = 2.2;\n"
               "    result.r = pow(result.r, exponent);\n"
               "    result.g = pow(result.g, exponent);\n"
               "    result.b = pow(result.b, exponent);\n"
               "    #endif\n"
               "    #endif\n"
               "    return result;\n"
               "}\n"s);
        
        append(buffer, ""
               "vec4 finalize_srgb_color(vec4 color) {\n"
               "    #if SIMULATE_SRGB_CONVERSION_ON_FINAL_WRITE\n"
               "    float exponent = 1.0 / 2.2;\n"
               "    color.r = pow(color.r, exponent);\n"
               "    color.g = pow(color.g, exponent);\n"
               "    color.b = pow(color.b, exponent);\n"
               "    #endif\n"
               "    return color;\n"
               "}\n"s);
        
        
        OpenGL.shader.load(buffer->str);    
    }
    
    
    OpenGL.lightmap_shader.load();
    #if 1
    {
        const s32 dim = 256;
        u8 pixels[dim*dim];  
        
        f32 radius = dim*0.5f;
        Vector2 center = v2(radius, radius);
        f32 radius_squared = SQUARED(radius);
        for(s32 y = 0; y < dim; y += 1)
        {
            for(s32 x = 0; x < dim; x += 1)
            {
                f32 length_squared = normsq(v2(x, y) - center);
                pixels[x + y*dim] = (length_squared <= radius_squared) ? 0xff : 0;
            }
        }   
        Opengl_Texture_Parameters params = default_texture_params(Opengl_Texture_Parameters::SINGLE_CHANNEL, dim, dim);
        params.pixels = pixels;
        OpenGL.big_circle_texture = allocate_texture(params);
    }
    #endif
    
    #if 1
    {
        #if 1 
        constexpr s32 dim = 4;
        u32 pixels[dim*dim]; 
        for_index_inc(s32, i, countof(pixels))
            pixels[i] = 0xffffffff;
        
        Opengl_Texture_Parameters params = default_texture_params(Opengl_Texture_Parameters::COLOR, dim, dim);
        params.pixels = pixels;
        //params.is_srgb = true;
        OpenGL.white_texture = allocate_texture(params);
        #else
        constexpr s32 dim = 4;
        u8 pixels[dim*dim]; 
        for_index_inc(s32, i, countof(pixels))
            pixels[i] = 0xff;
        
        Opengl_Texture_Parameters params = default_texture_params(dim, dim);
        params.type = Opengl_Texture_Parameters::SINGLE_CHANNEL;
        params.pixels = pixels;
        OpenGL.white_texture = allocate_texture(params);
        #endif
    }
    #endif
}

/*
internal void
debug_output_framebuffer(Opengl_Framebuffer *framebuffer, char *filepath)
{
   u32 *pixels = (u32 *)win32_allocate(framebuffer->width*2*framebuffer->height*sizeof(u32));
   flush_errors();
   glPixelStorei(GL_PACK_ALIGNMENT, 1); 
   glReadPixels(0, 0, framebuffer->width, framebuffer->height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
   glPixelStorei(GL_PACK_ALIGNMENT, 4);
   check_for_errors();
   //stbi_write_png(filepath, framebuffer->width, framebuffer->height, 4, pixels, framebuffer->width*sizeof(pixels[0]));
   ExitProcess(1);
}
*/



internal void
enable_vertex_attributes(GLSL_Lightmap::Shader *shader) {
    flush_errors();
    let attrib = &shader->attrib;
    
    glEnableVertexAttribArray(attrib->vert_pos);
    glVertexAttribPointer(attrib->vert_pos, 2, GL_FLOAT, GL_FALSE, 
                          sizeof(Vector4), (void *)offsetof(Vector4, xy));
    glEnableVertexAttribArray(attrib->vert_uv);
    glVertexAttribPointer(attrib->vert_uv, 2, GL_FLOAT, GL_FALSE, 
                          sizeof(Vector4), (void *)offsetof(Vector4, zw));
    
    
    glEnableVertexAttribArray(attrib->offset); 
    glVertexAttribPointer(attrib->offset, 2, GL_FLOAT, GL_FALSE, sizeof(Point_Light), (void *)(offsetof(Point_Light, pos)));
    glVertexAttribDivisor(attrib->offset, 1);
    
    glEnableVertexAttribArray(attrib->radius); 
    glVertexAttribPointer(attrib->radius, 1, GL_FLOAT, GL_FALSE, sizeof(Point_Light), (void *)(offsetof(Point_Light, radius)));
    glVertexAttribDivisor(attrib->radius, 1);
    check_for_errors();
}

internal void
disable_vertex_attributes(GLSL_Lightmap::Shader *shader) {
    flush_errors();
    let attrib = &shader->attrib;
    glDisableVertexAttribArray(attrib->vert_pos);
    glDisableVertexAttribArray(attrib->vert_uv);
    glDisableVertexAttribArray(attrib->offset); 
    glDisableVertexAttribArray(attrib->radius); 
    check_for_errors();
}

internal void
enable_vertex_attributes(GLSL_Shader::Shader *shader) {
    let attrib = &shader->attrib;
    
    //per vertex attributes
    flush_errors(); 
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL.vbo);
    glEnableVertexAttribArray(attrib->vert_pos);
    glVertexAttribPointer(attrib->vert_pos, 2, GL_FLOAT, GL_FALSE, 
                          sizeof(Vector4), (void *)offsetof(Vector4, xy));
    
    glEnableVertexAttribArray(attrib->vert_uv);
    glVertexAttribPointer(attrib->vert_uv, 2, GL_FLOAT, GL_FALSE, 
                          sizeof(Vector4), (void *)offsetof(Vector4, zw));
    check_for_errors();
    
    // NOTE that glVertexAttribDivisor(attrib, 0) means that each data ptr is advanced per each vertex
    // but doing glVertexAttribDivisor(attrib, 1) means that each data ptr is advanced per instance/triangle
    
    //per instance attributes
    flush_errors();
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL.ibo);
    
    
    glEnableVertexAttribArray(attrib->world_center_pos); 
    glVertexAttribPointer(attrib->world_center_pos, 2, GL_FLOAT, GL_FALSE, sizeof(Textured_Quad), 
                          (void*)(offsetof(Textured_Quad, center_pos))); 
    glVertexAttribDivisor(attrib->world_center_pos, 1);
    
    glEnableVertexAttribArray(attrib->world_x_basis); 
    glVertexAttribPointer(attrib->world_x_basis, 2, GL_FLOAT, GL_FALSE, sizeof(Textured_Quad), 
                          (void*)(offsetof(Textured_Quad, x_basis))); 
    glVertexAttribDivisor(attrib->world_x_basis, 1);
    
    glEnableVertexAttribArray(attrib->world_y_basis); 
    glVertexAttribPointer(attrib->world_y_basis, 2, GL_FLOAT, GL_FALSE, sizeof(Textured_Quad), 
                          (void*)(offsetof(Textured_Quad, y_basis))); 
    glVertexAttribDivisor(attrib->world_y_basis, 1);
    
    
    glEnableVertexAttribArray(attrib->in_color); 
    glVertexAttribPointer(attrib->in_color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Textured_Quad), 
                          (void*)offsetof(Textured_Quad, color));
    glVertexAttribDivisor(attrib->in_color, 1);
    
    glEnableVertexAttribArray(attrib->z_order); 
    glVertexAttribPointer(attrib->z_order, 1, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(Textured_Quad), 
                          (void*)offsetof(Textured_Quad, z_order));
    glVertexAttribDivisor(attrib->z_order, 1);
    
    glEnableVertexAttribArray(attrib->camera_index); 
    glVertexAttribIPointer(attrib->camera_index, 1, GL_INT, sizeof(Textured_Quad), 
                          (void*)offsetof(Textured_Quad, camera_index));
    glVertexAttribDivisor(attrib->camera_index, 1);
    
    glEnableVertexAttribArray(attrib->in_texture_uv_min); 
    glVertexAttribPointer(attrib->in_texture_uv_min, 2, GL_FLOAT, GL_FALSE, sizeof(Textured_Quad), 
                          (void*)(offsetof(Textured_Quad, rect_in_atlas_normalized) + offsetof(Rect2f, min))); 
    glVertexAttribDivisor(attrib->in_texture_uv_min, 1);
    
    glEnableVertexAttribArray(attrib->in_texture_uv_max); 
    glVertexAttribPointer(attrib->in_texture_uv_max, 2, GL_FLOAT, GL_FALSE, sizeof(Textured_Quad),
                          (void*)(offsetof(Textured_Quad, rect_in_atlas_normalized) + offsetof(Rect2f, max))); 
    glVertexAttribDivisor(attrib->in_texture_uv_max, 1);
    
    glEnableVertexAttribArray(attrib->clip_rect_min); 
    glVertexAttribPointer(attrib->clip_rect_min, 2, GL_FLOAT, GL_FALSE, sizeof(Textured_Quad), 
                          (void*)(offsetof(Textured_Quad, clip_rect) + offsetof(Rect2f, min))); 
    glVertexAttribDivisor(attrib->clip_rect_min, 1);
    
    glEnableVertexAttribArray(attrib->clip_rect_max); 
    glVertexAttribPointer(attrib->clip_rect_max, 2, GL_FLOAT, GL_FALSE, sizeof(Textured_Quad), 
                          (void*)(offsetof(Textured_Quad, clip_rect) + offsetof(Rect2f, max))); 
    glVertexAttribDivisor(attrib->clip_rect_max, 1);
    
    check_for_errors();
}

internal void
disable_vertex_attributes(GLSL_Shader::Shader *shader) {
    flush_errors();
    let attrib = &shader->attrib;
    glDisableVertexAttribArray(attrib->vert_pos);
    glDisableVertexAttribArray(attrib->vert_uv);
    
    //glDisableVertexAttribArray(attrib->transform + 0);
    //glDisableVertexAttribArray(attrib->transform + 1);
    //glDisableVertexAttribArray(attrib->transform + 2);
    glDisableVertexAttribArray(attrib->world_center_pos);
    glDisableVertexAttribArray(attrib->world_x_basis);
    glDisableVertexAttribArray(attrib->world_y_basis);
    //glDisableVertexAttribArray(attrib->cam_pos);
    //glDisableVertexAttribArray(attrib->cam_dim);
    //glDisableVertexAttribArray(attrib->cam_normalized_draw_pos);
    
    glDisableVertexAttribArray(attrib->in_color);
    glDisableVertexAttribArray(attrib->z_order);
    glDisableVertexAttribArray(attrib->in_texture_uv_min);
    glDisableVertexAttribArray(attrib->in_texture_uv_max); 
    check_for_errors();
}

internal Textured_Quad *
sort_textured_quads(Render_Context *rcx)
{
    { //sort entries by atlas id
        u32 batch_quad_offsets[MAX_QUAD_BATCH_COUNT] = {};
        static_assert (countof(batch_quad_offsets) == countof(rcx->num_quads_per_batch), "");
        
        u32 running_count = 0;
        for_index_inc (s32, i, countof(batch_quad_offsets)) {
            batch_quad_offsets[i] = running_count;
            running_count += rcx->num_quads_per_batch[i];
        }
        
        for_index_inc (u32, i, rcx->total_quad_count) {
            Textured_Quad *quad = rcx->quads + i;
            u32 atlas_id = get_texture_atlas_id(quad->sprite_id);
            u32 sorted_quad_index = batch_quad_offsets[get_batch_index(atlas_id, quad->color)]++; 
            rcx->sorted_quads[sorted_quad_index] = *quad;
        }
    }
    
    //sort by z with radix sort
    u32 running_quad_count = 0;
    
    Textured_Quad *src_base = rcx->sorted_quads;
    Textured_Quad *dst_base = rcx->quads;
    //NOTE opaque batches get ordered front to back
    //transparent stuff gets ordered  back to front
    for_index_inc (s32, batch_index, MAX_QUAD_BATCH_COUNT) {
        u32 index_to_first_quad = running_quad_count;
        u32 batch_quad_count    = rcx->num_quads_per_batch[batch_index];
        running_quad_count += batch_quad_count; 
        if (batch_quad_count == 0) continue;
        
        nil is_transparent = (batch_index >= ATLAS_ID(COUNT));
        
        //using a 32 bit zorder for the moment
        //NOTE this max byte index has to change if we change size of z_order
        for (s32 byte_index = 0; byte_index < 2; byte_index += 1) {
            Textured_Quad *src = src_base + index_to_first_quad;
            Textured_Quad *dst = dst_base + index_to_first_quad;
            
            //get quad count for each possible 8 byte value
            u32 offsets[256] = {};
            for (u32 i = 0; i < batch_quad_count; i += 1) {
                u16 z_order = src[i].z_order; 
                //if (!is_transparent) z_order = U16_MAX - z_order; //order front to back
                u32 index = (z_order >> (8*byte_index)) & 0xff;
                if (!is_transparent) index = 255 - index; //this way doesn't rely on max z_order size
                offsets[index] += 1;
            }
            
            //turn counts into offsets
            u32 running_count = 0;
            for (s32 i = 0; i < 256; i += 1) {
                u32 count = offsets[i];
                offsets[i] = running_count;
                running_count += count;
            }
            
            //place quads based on their offsets
            for (u32 i = 0; i < batch_quad_count; i += 1) {
                u16 z_order = src[i].z_order;
                //if (!is_transparent) z_order = U16_MAX - z_order; //order front to back
                u32 index = (z_order >> (8*byte_index)) & 0xff;
                if (!is_transparent) index = 255 - index; //this way doesn't rely on max z_order size
                dst[offsets[index]++] = src[i];
            }
            
            SWAP(src_base, dst_base);
        }
        //assert (src_base == rcx->sorted_quads);
        //assert (dst_base == rcx->quads);
    }
    //assert (src_base == rcx->sorted_quads);
    //assert (dst_base == rcx->quads);
    return dst_base;
}

internal void
render_opengl(Platform *platform, Render_Context *rcx) {
    if (rcx->window_dim.x == 0 || rcx->window_dim.y == 0) return;
    
    TIME_FUNCTION();
    
    //update lightmap
    Opengl_Framebuffer *lightmap = &OpenGL.lightmap;
    {
        //bind_framebuffer_for_drawing(lightmap, viewport_width, viewport_height);
        //TODO we should get the camera that is rendering the lightmap stuff and clear 
        //to it's output region... or should we always clear (on screen resize?)
        //bind_framebuffer_for_drawing(lightmap, viewport_width, viewport_height);
        bind_framebuffer_for_drawing(lightmap, rcx->window_dim.x, rcx->window_dim.y);
        
        //TODO we prob want to use this again at some point if some gpus don't make zero-set their textures...
        //glClearColor(0, 0, 0, 0);
        
        //glClearColor(rcx->lightmap_default_value, rcx->lightmap_default_value, rcx->lightmap_default_value, 0); //little ambient light
        //glClear(GL_COLOR_BUFFER_BIT);
        if (rcx->point_lights.count > 0)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            
            GLSL_Lightmap::Shader *shader = &OpenGL.lightmap_shader;
            glUseProgram(shader->program);
            //NOTE even if we forgot to comment these out, opengl ignores known invalid locations of -1 :)
            //glUniform1i(shader->uniform_SIMULATE_SRGB_CONVERSION_ON_FINAL_WRITE,    !OpenGL.supports_srgb_framebuffer);
            //glUniform1i(shader->uniform_SIMULATE_SRGB_CONVERSION_ON_TEXTURE_SAMPLE, !OpenGL.supports_srgb_texture);
            
            glBindBuffer(GL_ARRAY_BUFFER, OpenGL.vbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, OpenGL.ebo);
            
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, OpenGL.big_circle_texture);
            
            f32 cam_dim_y = rcx->lightmap_height_in_meters;
            f32 cam_dim_x = rcx->lightmap_width_in_meters;
            glUniform2f(shader->uniform.projection_scale_2d, 1.0f / (cam_dim_x*0.5f), 1.0f / (cam_dim_y*0.5f));
            
                       
            glBindBuffer(GL_ARRAY_BUFFER, OpenGL.ibo);
            //glBufferData(GL_ARRAY_BUFFER, sizeof(Point_Light)*TEXTURED_QUAD_BATCH_SIZE, batch->quads, GL_STREAM_DRAW);
            uintptr batch_size = sizeof(Point_Light)*rcx->point_lights.count;
            assert (batch_size <= MAX_INSTANCE_BUFFER_SIZE);
            glBufferSubData(GL_ARRAY_BUFFER, 0, batch_size, rcx->point_lights.e);
            
            enable_vertex_attributes(shader);
            glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, rcx->point_lights.count);
            disable_vertex_attributes(shader);
            
            
            glUseProgram(0);
            
            glDisable(GL_BLEND);
            rcx->point_lights.count = 0;
        }
    }
    
    
    #define DEBUG_RENDER_DIRECTLY_INTO_BACKBUFFER_NO_FRAMEBUFFER_BLIT false
	#if DEBUG_RENDER_DIRECTLY_INTO_BACKBUFFER_NO_FRAMEBUFFER_BLIT
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, viewport_width, viewport_height);
    #else
    //bind_framebuffer_for_drawing(&OpenGL.framebuffer, viewport_width, viewport_height);
    bind_framebuffer_for_drawing(&OpenGL.framebuffer, rcx->window_dim.x, rcx->window_dim.y);
    #endif  
    glClearDepth(0.0f);
    
    glClearColor(SQUARED(0.5f), SQUARED(0.5f), SQUARED(0.5f), 1);
    //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    
    //set my default basic 2d shader
    GLSL_Shader::Shader *shader = &OpenGL.shader;
    glUseProgram(shader->program);
    enable_vertex_attributes(shader);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, OpenGL.ebo); //for glDrawElementsInstanced
    
    { //set uniforms
        //GLint manually_decode_textures_from_srgb    = !(OpenGL.flags & OpenGL_SUPPORTS_SRGB_TEXTURE);
        //GLint manually_write_to_framebuffer_in_srgb = !(OpenGL.flags & OpenGL_SUPPORTS_SRGB_FRAMEBUFFER);
        let uni = &shader->uniform;
        //glUniform1i(uni->SIMULATE_SRGB_CONVERSION_ON_TEXTURE_SAMPLE, manually_decode_textures_from_srgb);
        //glUniform1i(uni->SIMULATE_SRGB_CONVERSION_ON_FINAL_WRITE,    manually_write_to_framebuffer_in_srgb);
    	//glUniform2f(uni->viewport_dim, (GLfloat)viewport_width, (GLfloat)viewport_height);
        
        glUniform2i(uni->viewport_dim, rcx->window_dim.x, rcx->window_dim.y);
        glUniform1i(uni->bitmap1,  0);
        glUniform1i(uni->lightmap, 1);     
    }
    
    //set uniform blocks
    { 
        GLuint block_index   = shader->uniform_block.UBO_Camera;
        GLuint binding_point = UBO_CAMERA;
        
        Memory_Arena *scratch = &Temporary_Memory_Arena;
        RESTORE_MEMORY_ARENA_ON_SCOPE_EXIT(scratch);
        UBO_Camera *data = push_struct(scratch, UBO_Camera);
        zero_memory(data, sizeof (*data));
        
        for_index_inc (s32, i, rcx->registered_cam_count) {
            Render_Camera *cam = rcx->registered_cams + i;
            data->_cams[i].pos = cam->pos;
            data->_cams[i].dim = v2(cam->view_width, cam->view_height);
            data->_cams[i].window_draw_region = cam->window_draw_region;
            
        }
        
        
        glUniformBlockBinding(shader->program, block_index, binding_point); //bind buffer block to point
        glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, OpenGL.ubo); //set data range for binding point
        
        glBindBuffer(GL_UNIFORM_BUFFER, OpenGL.ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(*data), data); 
    }
    
    Textured_Quad *sorted_quads = sort_textured_quads(rcx);
    
    u32 running_quad_count = 0;
    nil enabled_alpha_blending = false;
    for_index_inc(s32, batch_index, MAX_QUAD_BATCH_COUNT) {
        TIME_BLOCK("openl draw calls");
        
        Textured_Quad *batch_quads = sorted_quads + running_quad_count; 
        u32 batch_quad_count = rcx->num_quads_per_batch[batch_index];
        running_quad_count += batch_quad_count;
        if (batch_quad_count == 0) continue;
        
        nil is_transparent_batch = batch_index >= ATLAS_ID(COUNT); 
        if (is_transparent_batch && !enabled_alpha_blending) {
            enabled_alpha_blending = true;
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        
        s32 atlas_id = batch_index;
        if (is_transparent_batch) atlas_id -= ATLAS_ID(COUNT);
        
        assert (index_in_range(atlas_id, ATLAS_ID(COUNT)));
        Texture_Atlas *atlas = rcx->atlas[atlas_id];
        //debug_printf("Drawing %d, %d of %d\n [%llu]", range->first_index, range->sprite_count, atlas_id, FRAME_INDEX);
        
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, atlas->texture_id);
        
        glActiveTexture(GL_TEXTURE0 + 1); //for lightmap
        glBindTexture(GL_TEXTURE_2D, OpenGL.white_texture);
        
        flush_errors();
        glBindBuffer(GL_ARRAY_BUFFER, OpenGL.ibo);
        uintptr range_size = sizeof(Textured_Quad)*batch_quad_count;
        assert (range_size <= MAX_INSTANCE_BUFFER_SIZE);
        glBufferSubData(GL_ARRAY_BUFFER, 0, range_size, batch_quads);
        check_for_errors();
        
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, batch_quad_count);
    }
    if (enabled_alpha_blending) glDisable(GL_BLEND);
    
    //glEnable(GL_DEPTH_TEST);
    disable_vertex_attributes(shader);
    glUseProgram(0);
    
    flush_errors();
    #if DEBUG_RENDER_DIRECTLY_INTO_BACKBUFFER_NO_FRAMEBUFFER_BLIT
    //do nothing
    #else
    Opengl_Framebuffer *fb = &OpenGL.framebuffer;
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fb->id); // src FBO (multi-sample)
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    //NOTE if our framebuffer is MSAA and the backbuffer size does not match, this will fail
    //meaning glBlitFramebuffer cannot scale MSAA framebuffers when blitting, it wants an exact size match...
    //glBlitFramebuffer(0, 0, OpenGL.framebuffer.width, OpenGL.framebuffer.height,
    //0, 0, backbuffer_width, backbuffer_height,
    //GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT,
    //GL_NEAREST);
    glBlitFramebuffer(0, 0, fb->width, fb->height,
        //rcx->output_window_region.min.x, rcx->output_window_region.min.y,
        //rcx->output_window_region.max.x, rcx->output_window_region.max.y,
                      0, 0, rcx->window_dim.x, rcx->window_dim.y,
                      GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT,
                      GL_NEAREST);
    #endif
    check_for_errors();
}

struct Color_RGBA {
    u8 r, g, b, a;
};

internal f32
fbm(f32 (*noise)(u32 seed , f32 x, f32 y), 
    u32 seed, Vector2 pos, s32 num_octaves, f32 H)
{   
    assert (num_octaves < 32);
    f32 gain = exp2(-H); //persistence or how much the amplitude diminishes, usually 0.5f is useed
    //f32 G = pow(2, -H);
    f32 a = 1.0;
    f32 t = 0.0;
    for (s32 i = 0; i < num_octaves; i++)
    {
        f32 f = (f32)(1 << i);
        t += a*noise(seed, f*pos.x, f*pos.y);
        f *= 2.0;
        a *= gain;
    }
    return t;
}

internal f32
fbm_voronoi(u32 seed, Vector2 pos, s32 num_octaves, f32 H)
{
    assert (num_octaves < 32);
    f32 gain = exp2(-H); //persistence or how much the amplitude diminishes, usually 0.5f is useed
    //f32 G = pow(2, -H);
    f32 a = 1.0;
    f32 t = 0.0;
    for (s32 i = 0; i < num_octaves; i++)
    {
        f32 f = (f32)(1 << i);
        t += a*voronoi(seed, f*pos.x, f*pos.y);
        f *= 2.0;
        a *= gain;
    }
    return t;
}

internal f32
get_perlin_ringlet_test(u32 seed, f32 x, f32 y)
{
    let fbm = [seed](f32 x, f32 y) -> f32 {
        //assert (num_octaves < 32);
        //f32 gain = exp2(-H); //persistence or how much the amplitude diminishes, usually 0.5f is useed
        f32 gain = 0.25f; 
        f32 a = 1.0;
        f32 t = 0.0;
        f32 f = 1.0f;
        for (s32 i = 0; i < 1; i++)
        {
            t += a*perlin2d(seed, f*x, f*y);
            //f *= 2.0;
            f *= 1.f;
            a *= gain;
        }
        return t;    
    };
    
    
    f32 offset_x = 8*fbm(x + 0, y + 0);
    f32 offset_y = 8*fbm(x + 5.2f, y + 1.3f);
                            
    f32 t = fbm(x + offset_x, y + offset_y);
    clamp(t, -1.0f, 1.0f);
    return 0.5f*t + 0.5f;
}


internal void
update_perlin_texture(GLsizei width, GLsizei height, Color_RGBA *pixels)
{ //actual draw perlin texture her
    //NOTE in order for this to be seamless scale needs to be a multiple of v2(width, height)
    //if it's less than once, then we never draw full perlin texture and it will not be seamless
    //Vector2 scale = 0.0625f*v2(width, height); //spaces out sample points further and further away, makes it less continuous
    Vector2 scale = 4*v2(1,1); //basically scale is num of sample points 
    static Vector2 offset = v2(0, 0);
    //s32 wrap = 16;
    //s32 wrap = 32; //smaller number makes more reptitive tiles
    offset.x += 0.001f;
    //offset.y -= 0.02f;
    
    static s32 rotation_sign = 1;
    static f32 rotation = 0;
    rotation += 0.02f*rotation_sign;
    
    if (rotation > PI || rotation < -PI)
        rotation_sign *= -1;
    
    u32 seed = 0xcafebabe;
    //u32 seed_from_pos = (((s32)offset.y & (0xffff-1)) << 8) | ((s32)offset.x & (0xffff-1));
    //if (seed_from_pos == 0) seed_from_pos = 1;
    
    for(s32 y = 0; y < height; y += 1)
    {
        for(s32 x = 0; x < width; x += 1)
        {
            Vector2 pos = scale * v2((f32)x / (f32)(width-1), (f32)y / (f32)(height-1));
            //pos += offset;
            //f32 t = 0.5f*perlin(0xcafebabe, pos.x, pos.y, 0.5f) + 0.5f;
            //f32 t = fbm(perlin2d, 0xcafebabe, pos, 5, 0.8f);
            
            //constexpr f32 hurst = 0.5f; //normal value
            //constexpr f32 hurst = 1.4f;
            //constexpr f32 hurst = 1.8f;
            //constexpr s32 num_octaves = 2;
            //Vector2 fbm_offset = v2(fbm(perlin2d, 0xcafebabe, pos + v2(0.0f,0.0f)*offset.x, num_octaves, hurst),
                                    //fbm(perlin2d, 0xcafebabe, pos + v2(5.2f,1.3f)*offset.x, num_octaves, hurst)); 
            //f32 t =  fbm(perlin2d, 0xcafebabe, pos + 4.0f*fbm_offset, num_octaves, hurst);
            
            f32 t = get_perlin_ringlet_test(seed, pos.x, pos.y);
            
            //f32 t = 0.5f*simplex2d(0xcafebabe, pos.x, pos.y, rotation) + 0.5f;
            //f32 t = voronoi(0xcafebabe, pos.x, pos.y);
            //f32 t = fbm_voronoi(0xcafebabe, pos, 2, 0.4f);
            //f32 t = voronoi_v2(0xcafebabe, pos.x, pos.y);
            
            //f32 t = get_random_t(&seed_from_pos);
            //clamp(&t, 0.0f, 1.0f);
            //if (t < 0.5f) t = 0;
            //else t = 1.0f;
            
            #if 1
                //pixels[x + y*dim] = rgba(t, t, t, 1.0f);
            Color_RGBA *c = pixels + x + y*width;
            c->r = (u8)(t*255.0f);
            c->g = (u8)(t*255.0f);
            c->b = (u8)(t*255.0f);
            c->a = 255;
            
            #if 0 //pixel grid
            constexpr f32 epsilon = 0.07f;
            if ((pos.x - floor(pos.x) < epsilon) || (pos.y - floor(pos.y) < epsilon))
            {
                *c = {};
                c->r = 255;
                c->a = 255;
            }
            #endif
            #else
            f32 r1 = 255.0f / 255.0f;
            f32 g1 = 72.0f / 255.0f;
            f32 b1 = 59.0f / 255.0f;
            
            f32 r2 = 59.0f  / 255.0f;
            f32 g2 = 242.0f / 255.0f;
            f32 b2 = 255.0f / 255.0f;
            
            pixels[x + y*dim] = rgba(lerp(r1, r2, t),
                                     lerp(g1, g2, t),
                                     lerp(b1, b2, t), 1.0f);
            #endif
        }
    }
    
    GLint xoffset = 0;
    GLint yoffset = 0;
    glTexSubImage2D(GL_TEXTURE_2D, 0, xoffset, yoffset, width, height, 
                    GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}


#if 0
internal void
opengl_perlin_test(Platform *platform, Render_Context *rcx)
{
    
    constexpr s32 dim = 256;
    static Color_RGBA *pixels = null;
    static GLuint perlin_texture = 0;
    if (!pixels)
    {
        pixels = (Color_RGBA *)win32_allocate(sizeof(Color_RGBA) * dim * dim);
        
        glGenTextures(1, &perlin_texture); //TODO handle if this fails
        glBindTexture(GL_TEXTURE_2D, perlin_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8,// : GL_RGB8;
                     dim, dim, 0, GL_RGBA, GL_UNSIGNED_BYTE, null);
        
        
        flush_errors();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        check_for_errors();
    }
    
    
    
    
    s32 viewport_width  = get_width(rcx->output_window_region);
    s32 viewport_height = get_height(rcx->output_window_region);
    bind_framebuffer_for_drawing(&OpenGL.framebuffer, viewport_width, viewport_height);
    glClearDepth(0.0f);
    glClearColor(SQUARED(0.5f), SQUARED(0.5f), SQUARED(0.5f), 1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    GLSL_Shader *shader = &OpenGL.shader;
    glUseProgram(shader->program);
    
    flush_errors();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, OpenGL.ebo);
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL.vbo);
    
    glEnableVertexAttribArray(attrib->vert_pos);
    glVertexAttribPointer(attrib->vert_pos, 2, GL_FLOAT, GL_FALSE, 
                          sizeof(Vector4), (void *)offsetof(Vector4, xy));
    
    glEnableVertexAttribArray(attrib->vert_uv);
    glVertexAttribPointer(attrib->vert_uv, 2, GL_FLOAT, GL_FALSE, 
                          sizeof(Vector4), (void *)offsetof(Vector4, zw));
    
    
    Textured_Quad quad = {};
    quad.matrix = scale3x3(2,2,1);
    //breakpoint;
    //f32 output_width  = (f32)get_width(rcx->output_window_region);
    //f32 output_height = (f32)get_height(rcx->output_window_region);
    
    f32 viewport_aspect_ratio = (f32)viewport_width / (f32)viewport_height;
    if (viewport_width > viewport_height)
    {
        quad.matrix.e11 *=  1.0f / viewport_aspect_ratio;
    }
    else if (viewport_height > viewport_width)
    {
        quad.matrix.e22 *= viewport_aspect_ratio / 1.0f;
    }
    
    quad.color = rgb(1,1,1);
    //quad.color = rgb(1,0,0);
    quad.z_order = 32;
    f32 uv_scale = 1;
    quad.rect_in_atlas_normalized = r2f(uv_scale*v2(0, 0), uv_scale*v2(1,1));
    
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL.ibo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad), &quad);
    check_for_errors();
    
    
    glUniform1i(shader->uniform_SIMULATE_SRGB_CONVERSION_ON_TEXTURE_SAMPLE, !hasfl(OpenGL, OpenGL_SUPPORTS_SRGB_TEXTURE));
    glUniform1i(shader->uniform_SIMULATE_SRGB_CONVERSION_ON_FINAL_WRITE,    !hasfl(OpenGL, OpenGL_SUPPORTS_SRGB_FRAMEBUFFER));
    glUniform2f(shader->uniform_viewport_dim, (GLfloat)viewport_width, (GLfloat)viewport_height);
    glUniform1i(shader->uniform_bitmap1,  0);
    glUniform1i(shader->uniform_lightmap, 1); 
    enable_vertex_attributes(shader);
    
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, perlin_texture);
    update_perlin_texture(dim, dim, pixels);
    
    
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, OpenGL.white_texture);
    
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 1);
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    disable_vertex_attributes(shader);
    
    
    Opengl_Framebuffer *fb = &OpenGL.framebuffer;
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fb->id); // src FBO (multi-sample)
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    //NOTE if our framebuffer is MSAA and the backbuffer size does not match, this will fail
    //meaning glBlitFramebuffer cannot scale MSAA framebuffers when blitting, it wants an exact size match...
    //glBlitFramebuffer(0, 0, OpenGL.framebuffer.width, OpenGL.framebuffer.height,
    //0, 0, backbuffer_width, backbuffer_height,
    //GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT,
    //GL_NEAREST);
    glBlitFramebuffer(0, 0, fb->width, fb->height,
                      rcx->output_window_region.min.x, rcx->output_window_region.min.y,
                      rcx->output_window_region.max.x, rcx->output_window_region.max.y,
                      GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT,
                      GL_NEAREST);
 
    
    rcx->total_quad_count = 0; //for game that still running adding quads
}
#endif

#endif //OpenGL_H

