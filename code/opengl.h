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

X(void,     glGetUniformfv, GLuint program, GLint location, GLfloat *params);
X(void,     glGetUniformiv, GLuint program, GLint location, GLint *params);

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

#if DEBUG_BUILD
inline void
flush_errors() {
    while(glGetError() != GL_NO_ERROR);  
}

inline void
check_for_errors() {  
    //1282 - INVALID_OPERATION, meaning invalid args passed in
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        breakpoint;
    } 
}
#else
#define flush_errors()
#define check_for_errors()
#endif

//typedef void (WINAPI *DEBUGPROC)(GLenum __source, GLenum type, GLuint id, GLenum severity,
//GLsizei length, const GLchar *message, void *userParam);

struct Opengl_Framebuffer  {
    GLuint id;
    GLuint color_attachment;
    GLuint depth_attachment;
    s32 width;
    s32 height;
    s32 multisample_count;
    //s32 is_srgb;
};

#define MAX_INSTANCE_BUFFER_SIZE (MAX_QUAD_COUNT * sizeof(Textured_Quad))

enum : u32 {
    OpenGL_SUPPORTS_SRGB_FRAMEBUFFER   = (1 << 0),
    OpenGL_SUPPORTS_SRGB_TEXTURE       = (1 << 1),
    OpenGL_SUPPORTS_FRAMEBUFFER_OBJECT = (1 << 2),
};


enum UBO_Binding_Point {
    UBO_CAMERA,
};


struct Vertex_Mesh {
    union {
        struct {
            GLuint vbo; //vertex pos+uv for quads that *NEVER* change
            GLuint ebo; //index array for quad triangle. *NEVER* changes
            //GLuint ibo; //per instance buffer object
            //GLuint ubo;        
        };
        GLuint buffers[2];
    };    
};

struct Vertex2 {
    Vector2 pos;
    Vector2 uv;
};

struct Simple_Shader_2D {
    GLuint pid;
	GLuint vert_id;
	GLuint frag_id;
    bool compiled;
    
    GLint vert_pos;
    GLint vert_uv;
    
    GLint u_viewport_dim;
    GLint u_quad_pos;
    GLint u_quad_dim;
    GLint u_quad_col;
};

internal void
enable_vertex_attributes(Simple_Shader_2D *shader) {
    flush_errors();
    
    glEnableVertexAttribArray(shader->vert_pos);
    glVertexAttribPointer(shader->vert_pos, 2, GL_FLOAT, GL_FALSE, 
                          sizeof(Vertex2), (void *)offsetof(Vertex2, pos));
    glEnableVertexAttribArray(shader->vert_uv);
    glVertexAttribPointer(shader->vert_uv, 2, GL_FLOAT, GL_FALSE, 
                          sizeof(Vertex2), (void *)offsetof(Vertex2, uv));
    check_for_errors();
}

internal void
disable_vertex_attributes(Simple_Shader_2D *shader) {
    flush_errors();
    glDisableVertexAttribArray(shader->vert_pos);
    glDisableVertexAttribArray(shader->vert_uv);
    check_for_errors();
}

struct Opengl {
    char *version;
    char *glsl_version;
    char *vendor;
    char *renderer;
    u32 flags;
    
    Simple_Shader_2D simple_shader_2d;
    Vertex_Mesh square_mesh;
    
    //GLuint big_circle_texture; //used by lightmap shader
    //GLuint white_texture;
    
    u32 max_texture_size;
    
    Opengl_Framebuffer framebuffer;
    //Opengl_Framebuffer lightmap;
    
    s32 multisample_count;
    s32 max_multisample_count;
};

static Opengl OpenGL;

internal Vector4
color_as_vector4(u32 color) {
    Vector4 result;
    result.a = ((color >> 24) & 0xff) / 255.0f;
    result.r = ((color >> 16) & 0xff) / 255.0f;
    result.g = ((color >>  8) & 0xff) / 255.0f;
    result.b = ((color >>  0) & 0xff) / 255.0f;
    return result;
}

internal Vector4
color_as_vector4(Color color) {
    Vector4 result;
    result.a = color.a / 255.0f;
    result.r = color.r / 255.0f;
    result.g = color.g / 255.0f;
    result.b = color.b / 255.0f;
    return result;
}

struct Opengl_Texture_Parameters {
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
default_texture_params(Opengl_Texture_Parameters::Type type, s32 width, s32 height) {
    Opengl_Texture_Parameters params = {};
    params.type   = type;
    params.width  = width;
    params.height = height;
    return params;
}


//NOTE if MSAA data is num samples, otherwise it's a pointer to texure data...
internal GLuint
allocate_texture(Opengl_Texture_Parameters params) {
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
init_opengl() {
    //glEnable(GL_CULL_FACE);// for 2D we can just negate the basis to flip 
    //glCullFace(GL_BACK);   // the textures around and save on texture memory ...
    glDepthMask(GL_TRUE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glEnable(GL_FRAMEBUFFER_SRGB); //NOTE check to see if we can do this otherwise emulate in shader
    //also NOTE textures are in SRGB space, so we need to sample them and convert them into linear space before the end of the fragment shader
    
    glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_GREATER); //smaller values pass through and overwrite, 1.0f is the "farthest away" 
    glDepthFunc(GL_GEQUAL);
    
    if (SwapIntervalEXT) {
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
    
    
    {
        Vertex_Mesh *mesh = &OpenGL.square_mesh;
        glGenBuffers(countof(mesh->buffers), &mesh->buffers[0]);
        
        Vertex2 vertices[] = {
            {{-0.5f, -0.5f}, {0, 0}}, //0 bot left
            {{-0.5f, +0.5f}, {0, 1}}, //1 top left
            {{+0.5f, -0.5f}, {1, 0}}, //2 bot right
            {{+0.5f, +0.5f}, {1, 1}}, //3 top right
        };
        u32 indices[] = { //can we use a smaller size here?
            0, 2, 1,
            1, 2, 3,
        };
        glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 
        
        //glGenBuffers(1, &OpenGL.ibo);
        //glBindBuffer(GL_ARRAY_BUFFER, OpenGL.ibo);
        //glBufferData(GL_ARRAY_BUFFER, MAX_INSTANCE_BUFFER_SIZE, 0, GL_DYNAMIC_DRAW);
        
        //glGenBuffers(1, &OpenGL.ubo);
        //glBindBuffer(GL_UNIFORM_BUFFER, OpenGL.ubo);
        //glBufferData(GL_UNIFORM_BUFFER, sizeof(UBO_Camera), 0, GL_DYNAMIC_DRAW);
    }
    
    {
        Simple_Shader_2D *shader = &OpenGL.simple_shader_2d;
        flush_errors();
        defer { check_for_errors(); };
        
        shader->vert_id = glCreateShader(GL_VERTEX_SHADER);
        shader->frag_id = glCreateShader(GL_FRAGMENT_SHADER);
        shader->pid = glCreateProgram();
        if (!shader->vert_id || !shader->frag_id || !shader->pid) {
            assert (0); //ran out of vram somehow?...
            shader->vert_id = shader->frag_id = shader->pid = 0;
        }
        
        char* vert_src = ""
        "#version 330\n"
        "#line " STRINGIFY((__LINE__ + 1)) "\n"
        R"___(
         in vec2 vert_pos; //local pos offset to the vertex, in normalized space
         in vec2 vert_uv;

         uniform ivec2 u_viewport_dim;
         uniform vec2  u_quad_pos;
         uniform vec2  u_quad_dim;
         uniform vec4  u_quad_col;

         out V2F {
	         vec2 world_pos;
	         vec2 uv;
	         vec4 color;
         } v2f;
         
         void main () {
             v2f.world_pos = u_quad_pos + vert_pos*u_quad_dim; //screen pos of each vertex
		     vec2 pos = v2f.world_pos / u_viewport_dim; //makes pos [0, 1] in viewport
             pos = (2*pos) - vec2(1,1); //makes pos [-1, 1] in viewport
             gl_Position = vec4(pos, 0, 1);
             
             v2f.uv = vert_uv;
             v2f.color = u_quad_col;
             v2f.color.a *= u_quad_dim.x*v2f.uv.x;
         }
         )___";
        
        char* frag_src = ""
        "#version 330\n"
        "#line " STRINGIFY((__LINE__ + 1)) "\n"
        R"___(
         in V2F {
	         vec2 world_pos;
	         vec2 uv;
	         vec4 color;
         } v2f;
         
         out vec4 frag_color;
         void main () {
             frag_color = v2f.color;
         }
        )___";
        
        
        
        glShaderSource(shader->vert_id, 1, &vert_src, 0);
        glCompileShader(shader->vert_id);
        
        glShaderSource(shader->frag_id, 1, &frag_src, 0);
        glCompileShader(shader->frag_id);
        
        glAttachShader(shader->pid, shader->vert_id);
        glAttachShader(shader->pid, shader->frag_id);
        glLinkProgram(shader->pid);
        glValidateProgram(shader->pid);
        
        GLint linked_ok = false, compiled_ok = false;
        glGetProgramiv(shader->pid, GL_LINK_STATUS,     &linked_ok);
        glGetProgramiv(shader->pid, GL_VALIDATE_STATUS, &compiled_ok);
        shader->compiled = linked_ok && compiled_ok; 
        
        if (!shader->compiled) {
            char vertex_log  [256];
            char fragment_log[256];
            char program_log [256];
            glGetShaderInfoLog(shader->vert_id, sizeof(vertex_log),   NULL, vertex_log);
            glGetShaderInfoLog(shader->frag_id, sizeof(fragment_log), NULL, fragment_log);
            glGetProgramInfoLog(shader->pid,    sizeof(program_log),  NULL, program_log);
            breakpoint;
            glDeleteShader(shader->vert_id);
            glDeleteShader(shader->frag_id);
            glDeleteProgram(shader->pid);
            shader->vert_id = shader->frag_id = shader->pid = 0;
        } else {
            shader->vert_pos = glGetAttribLocation(shader->pid, "vert_pos");
            assert (shader->vert_pos != -1); //didn't find it, probably optimized out if never used in shader
            
            shader->vert_uv = glGetAttribLocation(shader->pid, "vert_uv");
            assert (shader->vert_uv != -1); //didn't find it, probably optimized out if never used in shader 
            
            shader->u_viewport_dim = glGetUniformLocation(shader->pid, "u_viewport_dim");
            assert(shader->u_viewport_dim != -1);
            
            shader->u_quad_pos = glGetUniformLocation(shader->pid, "u_quad_pos");
            assert(shader->u_quad_pos != -1);
            
            shader->u_quad_dim = glGetUniformLocation(shader->pid, "u_quad_dim");
            assert(shader->u_quad_dim  != -1);
            
            shader->u_quad_col = glGetUniformLocation(shader->pid, "u_quad_col");
            assert(shader->u_quad_col != -1);
        }
    }
    
    
    #if 0
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
    
    #if 0
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




static void
draw_2d_quads(Vector2i viewport_dim) {
    flush_errors();
    
    Simple_Shader_2D *shader = &OpenGL.simple_shader_2d;
    assert (shader->compiled);
    glUseProgram(shader->pid);
    enable_vertex_attributes(shader);
    
    Vertex_Mesh *mesh = &OpenGL.square_mesh;
    glBindBuffer(GL_ARRAY_BUFFER,         mesh->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo); //for glDrawElementsInstanced
    
    glUniform2i(shader->u_viewport_dim, viewport_dim.x, viewport_dim.y);
    
    struct Quad {
        Vector2 pos;
        f32 w, h;
        Vector4 color;
    };
    
    Quad quads[] = {
        {{400, 400}, 200, 200, {1, 0, 0, 1}},
        {{600, 600}, 150, 150, {0, 1, 0, 1}},
        {{600, 200}, 150, 250, {0, 1, 1, 1}},
        {{400, 500},  50, 400, {1, 1, 0, 1}},
    };
    
    for (s32 quad_index = 0; quad_index < countof(quads); quad_index += 1) {
        Quad *quad = quads + quad_index;
        glUniform2f(shader->u_quad_pos, quad->pos.x, quad->pos.y);
        glUniform2f(shader->u_quad_dim, quad->w, quad->h);
        glUniform4f(shader->u_quad_col, quad->color.x, quad->color.y, quad->color.z, quad->color.w);
        
        /*
        s32 a[2];
        glGetUniformiv(shader->pid, shader->u_viewport_dim, a);
        
        f32 b[2];
        glGetUniformfv(shader->pid, shader->u_quad_pos, b);
        f32 c[2];
        glGetUniformfv(shader->pid, shader->u_quad_dim, c);
        f32 d[4];
        glGetUniformfv(shader->pid, shader->u_quad_col, d);
        
        breakpoint;
        */
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    
    
    disable_vertex_attributes(shader);
    glUseProgram(0);
    
    check_for_errors();
}


static void
render_opengl(Platform *platform, Render_Context *rcx) {
    Vector2i viewport_dim = v2i(rcx->window_dim.x, rcx->window_dim.y); 
    if (viewport_dim.w == 0 || viewport_dim.h == 0) return; //nothing to render
    TIME_FUNCTION();
    
    
    
    constexpr bool draw_directly_into_backbuffer = false;
    if (draw_directly_into_backbuffer) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, viewport_dim.x, viewport_dim.h);    
    } else {
        bind_framebuffer_for_drawing(&OpenGL.framebuffer, rcx->window_dim.x, rcx->window_dim.y); //use framebuffer    
    }
    
    
	 
    glClearDepth(0.0f);
    glClearColor(SQUARED(0.5f), SQUARED(0.5f), SQUARED(0.5f), 1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    draw_2d_quads(viewport_dim);
    
    if (!draw_directly_into_backbuffer) {
        //we now have to blit into backbuffer to view 
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
    }
    check_for_errors();
}
#endif //OpenGL_H

