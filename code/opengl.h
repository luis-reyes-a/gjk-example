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



struct Vertex {
    Vector3 pos;
    Vector2 uv;
};



struct Vertex_Mesh {
    s32 first_index;
    s32 index_count;
    s32 vertex_count;
};





#define ATLAS_SPRITE_MAX_COUNT 128

//NOTE this serves as the binding point
enum Uniform_Buffer_Object_ID : GLuint { 
    UBO_ATLAS_RECTS,
    UBO_COUNT,
};

struct Uniform_Buffer_Object {
    Uniform_Buffer_Object_ID binding_point;
    u32 max_buffer_size;
    GLuint buffer;
};

static Uniform_Buffer_Object g_ubo[UBO_COUNT] = {
    {UBO_ATLAS_RECTS, sizeof(Rect2i)*ATLAS_SPRITE_MAX_COUNT},
};

struct Circle_Shader_2D {
    GLuint pid;
	GLuint vs_id;
	GLuint fs_id;
    bool compiled;
    
    GLint u_viewport_dim;
    GLint u_quad_pos;
    GLint u_outer_radius;
    GLint u_inner_radius;
    GLint u_quad_col;
    
    static constexpr char *common_src = ""
    "#version 330\n"
    "#line " STRINGIFY((__LINE__ + 1)) "\n"
    "//TODO we need to pass these in per quad, not as uniforms\n"
    "uniform float u_rotation_t;\n"
    "uniform vec2  u_quad_pos;\n"
    "uniform float u_outer_radius;\n"
    "uniform float u_inner_radius;\n"
    "uniform vec4  u_quad_col;\n"
    "uniform ivec2 u_viewport_dim;\n"
    "#define PI  3.14159265358979323846\n"
    "#define TAU 6.28318530717958647692\n"
    "#define V2F_STRUCT struct { \\\n"
    "    vec2 world_pos;\\\n"
    "    vec2 uv;\\\n"
    "    vec4 color;\\\n"
    "}\n"
    "//COMMON_SRC END\n";
    
    static constexpr char *vert_src = ""
    "#line " STRINGIFY((__LINE__ + 1)) "\n"
    R"___(

     out V2F_STRUCT v2f;
     
     void main () {
         vec2 vert_pos_array[4] = vec2[4](vec2(-0.5f, -0.5f), vec2(0.5f, -0.5f), vec2(-0.5f, 0.5f), vec2(0.5f, 0.5f));
         vec2 vert_uv_array[4]  = vec2[4](vec2(0, 0),         vec2(1, 0),        vec2(0, 1),        vec2(1, 1));
         vec2 vert_pos = vert_pos_array[gl_VertexID];
         vec2 vert_uv  = vert_uv_array[gl_VertexID];

         v2f.world_pos = u_quad_pos + vert_pos*(2*u_outer_radius); 

         vec2 final_pos = v2f.world_pos / u_viewport_dim; //makes pos [0, 1] in viewport
         final_pos = (2*final_pos) - vec2(1,1); //makes pos [-1, 1] in viewport
         gl_Position = vec4(final_pos, -1, 1); //NOTE we set z=-1, to indicate it should be close to us
         
         v2f.uv = vert_uv;
         v2f.color = u_quad_col;
     }
     )___";
    
    static constexpr char *frag_src = ""
    "#line " STRINGIFY((__LINE__ + 1)) "\n"
    R"___(
     smooth in V2F_STRUCT v2f;

     out vec4 frag_color;
     void main () {
         vec2 delta = v2f.world_pos - u_quad_pos;
         float dist_from_center = length(delta);
         if ((dist_from_center > u_outer_radius) ||
             (dist_from_center < u_inner_radius)) {
             discard;
         }
         frag_color = v2f.color;
     }
    )___";
    
    void compile_shader() {
        flush_errors();
        defer { check_for_errors(); };
        
        vs_id = glCreateShader(GL_VERTEX_SHADER);
        fs_id = glCreateShader(GL_FRAGMENT_SHADER);
        pid = glCreateProgram();
        if (!vs_id || !fs_id || !pid) {
            assert (0); //ran out of vram somehow?...
            vs_id = fs_id = pid = 0;
        }
        
        GLchar *vs[] = {common_src, vert_src};
        glShaderSource(vs_id, countof(vs), &vs[0], 0);
        glCompileShader(vs_id);
        
        GLchar *fs[] = {common_src, frag_src};
        glShaderSource(fs_id, countof(fs), &fs[0], 0);
        glCompileShader(fs_id);
        
        glAttachShader(pid, vs_id);
        glAttachShader(pid, fs_id);
        glLinkProgram(pid);
        glValidateProgram(pid);
        
        GLint linked_ok = false, compiled_ok = false;
        glGetProgramiv(pid, GL_LINK_STATUS,     &linked_ok);
        glGetProgramiv(pid, GL_VALIDATE_STATUS, &compiled_ok);
        compiled = linked_ok && compiled_ok; 
        
        if (!compiled) {
            char vertex_log  [256];
            char fragment_log[256];
            char program_log [256];
            glGetShaderInfoLog(vs_id, sizeof(vertex_log),   NULL, vertex_log);
            glGetShaderInfoLog(fs_id, sizeof(fragment_log), NULL, fragment_log);
            glGetProgramInfoLog(pid,    sizeof(program_log),  NULL, program_log);
            breakpoint;
            glDeleteShader(vs_id);
            glDeleteShader(fs_id);
            glDeleteProgram(pid);
            vs_id = fs_id = pid = 0;
        } else {
            
            u_viewport_dim = glGetUniformLocation(pid, "u_viewport_dim");
            assert (u_viewport_dim != -1);
            
            u_quad_pos = glGetUniformLocation(pid, "u_quad_pos");
            assert (u_quad_pos != -1);
            
            u_outer_radius = glGetUniformLocation(pid, "u_outer_radius");
            assert (u_outer_radius  != -1);
            
            u_inner_radius = glGetUniformLocation(pid, "u_inner_radius");
            assert (u_inner_radius  != -1);
            
            u_quad_col = glGetUniformLocation(pid, "u_quad_col");
            assert (u_quad_col != -1);
        }
    }
};

struct Simple_Shader_2D {
    GLuint pid;
	GLuint vert_id;
	GLuint frag_id;
    bool compiled;
    
    GLint u_viewport_dim;
    GLint u_rotation_t;
    GLint u_quad_pos;
    GLint u_quad_dim;
    GLint u_quad_col;
    
    GLint u_atlas_rect_index;
    GLint u_texture1_size;
    GLint u_texture1;
    
    GLuint u_block_atlas_rects; //uniform block index
    Uniform_Buffer_Object_ID ubo_id; //binding point for our guy
    
    static constexpr char *common_src = ""
    "#version 330\n"
    "#line " STRINGIFY((__LINE__ + 1)) "\n"
    "//TODO we need to pass these in per quad, not as uniforms\n"
    "uniform float u_rotation_t;\n"
    "uniform vec2  u_quad_pos;\n"
    "uniform vec2  u_quad_dim;\n"
    "uniform vec4  u_quad_col;\n"
    "uniform int u_atlas_rect_index;\n"
    "\n"
    "uniform ivec2 u_viewport_dim;\n"
    "\n"
    "uniform ivec2 u_texture1_size;\n"
    "uniform sampler2D u_texture1;\n"
    "\n"
    "struct Atlas_Rect { \n"
    "    ivec2 min;\n"
    "    ivec2 max;\n"
    "};\n"
    "\n"
    "layout (std140) uniform u_block_atlas_rects { \n"
    "    Atlas_Rect atlas_rects[128];\n"
    "};\n"
    "\n"
    "#define PI  3.14159265358979323846\n"
    "#define TAU 6.28318530717958647692\n"
    "#define V2F_STRUCT struct { \\\n"
    "    vec2 world_pos;\\\n"
    "    vec2 uv;\\\n"
    "    vec4 color;\\\n"
    "}\n"
    "//COMMON_SRC END\n";
    
    static constexpr char *vert_src = ""
    "#line " STRINGIFY((__LINE__ + 1)) "\n"
    R"___(

     out V2F_STRUCT v2f;
     
     void main () {
         vec2 vert_pos_array[4] = vec2[4](vec2(-0.5f, -0.5f), vec2(0.5f, -0.5f), vec2(-0.5f, 0.5f), vec2(0.5f, 0.5f));
         vec2 vert_uv_array[4]  = vec2[4](vec2(0, 0),         vec2(1, 0),        vec2(0, 1),        vec2(1, 1));
         vec2 vert_pos = vert_pos_array[gl_VertexID];
         vec2 vert_uv  = vert_uv_array[gl_VertexID];

         vec2 x_basis = vec2(cos(u_rotation_t*TAU), sin(u_rotation_t*TAU));
         vec2 y_basis = vec2(-x_basis.y, x_basis.x);
         x_basis *= u_quad_dim.x;
         y_basis *= u_quad_dim.y;
         v2f.world_pos = u_quad_pos + x_basis*vert_pos.x + y_basis*vert_pos.y; //screen pos of each vertex

         vec2 pos = v2f.world_pos / u_viewport_dim; //makes pos [0, 1] in viewport
         pos = (2*pos) - vec2(1,1); //makes pos [-1, 1] in viewport
         gl_Position = vec4(pos, -1, 1); //NOTE we set z=-1, to indicate it should be close to us
         
         v2f.uv = vert_uv;
         v2f.color = u_quad_col;
     }
     )___";
    
    static constexpr char *frag_src = ""
    "#line " STRINGIFY((__LINE__ + 1)) "\n"
    R"___(
     in V2F_STRUCT v2f;

     out vec4 frag_color;
     void main () {
         Atlas_Rect rect = atlas_rects[u_atlas_rect_index];
         vec2 uv_min = vec2(float(rect.min.x) / float(u_texture1_size.x), float(rect.min.y) / float(u_texture1_size.y));
         vec2 uv_max = vec2(float(rect.max.x) / float(u_texture1_size.x), float(rect.max.y) / float(u_texture1_size.y));
         vec2 uv = mix(uv_min, uv_max, v2f.uv);
         vec4 tex1 = texture(u_texture1, uv);
         frag_color = tex1 * v2f.color;
     }
    )___";
    
    
    void compile_shader(Uniform_Buffer_Object_ID _ubo_id) {
        ubo_id = _ubo_id;
        
        flush_errors();
        defer { check_for_errors(); };
        
        vert_id = glCreateShader(GL_VERTEX_SHADER);
        frag_id = glCreateShader(GL_FRAGMENT_SHADER);
        pid = glCreateProgram();
        if (!vert_id || !frag_id || !pid) {
            assert (0); //ran out of vram somehow?...
            vert_id = frag_id = pid = 0;
        }
        
        GLchar *vs[] = {common_src, vert_src};
        glShaderSource(vert_id, countof(vs), &vs[0], 0);
        glCompileShader(vert_id);
        
        GLchar *fs[] = {common_src, frag_src};
        glShaderSource(frag_id, countof(fs), &fs[0], 0);
        glCompileShader(frag_id);
        
        glAttachShader(pid, vert_id);
        glAttachShader(pid, frag_id);
        glLinkProgram(pid);
        glValidateProgram(pid);
        
        GLint linked_ok = false, compiled_ok = false;
        glGetProgramiv(pid, GL_LINK_STATUS,     &linked_ok);
        glGetProgramiv(pid, GL_VALIDATE_STATUS, &compiled_ok);
        compiled = linked_ok && compiled_ok; 
        
        if (!compiled) {
            char vertex_log  [256];
            char fragment_log[256];
            char program_log [256];
            glGetShaderInfoLog(vert_id, sizeof(vertex_log),   NULL, vertex_log);
            glGetShaderInfoLog(frag_id, sizeof(fragment_log), NULL, fragment_log);
            glGetProgramInfoLog(pid,    sizeof(program_log),  NULL, program_log);
            breakpoint;
            glDeleteShader(vert_id);
            glDeleteShader(frag_id);
            glDeleteProgram(pid);
            vert_id = frag_id = pid = 0;
        } else {
            u_viewport_dim = glGetUniformLocation(pid, "u_viewport_dim");
            assert (u_viewport_dim != -1);
            
            u_quad_pos = glGetUniformLocation(pid, "u_quad_pos");
            assert (u_quad_pos != -1);
            
            u_rotation_t = glGetUniformLocation(pid, "u_rotation_t");
            assert (u_rotation_t != -1);
            
            
            
            u_quad_dim = glGetUniformLocation(pid, "u_quad_dim");
            assert (u_quad_dim  != -1);
            
            u_quad_col = glGetUniformLocation(pid, "u_quad_col");
            assert (u_quad_col != -1);
            
            u_atlas_rect_index = glGetUniformLocation(pid, "u_atlas_rect_index");
            assert (u_atlas_rect_index != -1);
            
            u_texture1_size = glGetUniformLocation(pid, "u_texture1_size");
            assert (u_texture1_size != -1);
            
            u_texture1 = glGetUniformLocation(pid, "u_texture1");
            assert (u_texture1 != -1);
            
            u_block_atlas_rects = glGetUniformBlockIndex(pid, "u_block_atlas_rects"); 
            assert (u_block_atlas_rects != GL_INVALID_INDEX);
            
            //NOTE seems like the binding stays active until the program is linked/relinked again
            //meaning we can just set this here and not worry about it?....
            glUniformBlockBinding(pid, u_block_atlas_rects, ubo_id); 
        }
    }
};

struct Simple_Shader_3D {
    GLuint pid;
	GLuint vert_id;
	GLuint frag_id;
    bool compiled;
    
    GLint vert_pos;
    GLint vert_uv;
    
    GLint u_xform;
    GLint u_color;
    GLint u_near; //optinal
    GLint u_far;  //optinal
    
    static constexpr char* common_src = ""
    "#version 330\n"
    "#line " STRINGIFY((__LINE__ + 1)) "\n"
    "uniform mat4x4 u_xform;\n"
    "uniform float u_near;\n"
    "uniform float u_far;\n"
    "uniform vec4  u_color;\n"
    "\n"
    "#define V2F_STRUCT struct {\\\n"
    "    vec2 uv;\\\n"
    "    vec4 color;\\\n"
    "}\n"
    "//COMMON_SRC END\n\n";
    
    static constexpr char* vert_src = ""
    "#line " STRINGIFY((__LINE__ + 1)) "\n"
    R"___(
     in vec3 vert_pos; //local pos offset to the vertex, in normalized space
     in vec2 vert_uv;
         
     out V2F_STRUCT v2f;
     void main () { 
         gl_Position = u_xform*vec4(vert_pos, 1);
         //gl_Position = vec4(vert_pos, 1)*u_xform;
         v2f.uv = vert_uv;
         v2f.color = u_color;
     }
     )___";
    
    static constexpr char* frag_src = ""
    "#line " STRINGIFY((__LINE__ + 1)) "\n"
    R"___(
     smooth in V2F_STRUCT v2f;
     
     float linearize_depth(float d) {
         float z_n = 2.0 * d - 1.0;
         return 2.0 * u_near * u_far / (u_far + u_near - z_n * (u_far - u_near));
     }

     out vec4 frag_color;
     void main () { 
         //float tz = linearize_depth(gl_FragCoord.z);
         frag_color = v2f.color;
         //frag_color.xyz *= tz;
     }
    )___";
    
    bool compile_shader() {
        flush_errors();
        defer { check_for_errors(); };
        
        vert_id = glCreateShader(GL_VERTEX_SHADER);
        frag_id = glCreateShader(GL_FRAGMENT_SHADER);
        pid = glCreateProgram();
        if (!vert_id || !frag_id || !pid) {
            assert (0); //ran out of vram somehow?...
            vert_id = frag_id = pid = 0;
        }
        
        
        GLchar *vs[] = {common_src, vert_src};
        glShaderSource(vert_id, countof(vs), &vs[0], 0);
        glCompileShader(vert_id);
        
        GLchar *fs[] = {common_src, frag_src};
        glShaderSource(frag_id, countof(fs), &fs[0], 0);
        glCompileShader(frag_id);
        
        glAttachShader(pid, vert_id);
        glAttachShader(pid, frag_id);
        glLinkProgram(pid);
        glValidateProgram(pid);
        
        GLint linked_ok = false, compiled_ok = false;
        glGetProgramiv(pid, GL_LINK_STATUS,     &linked_ok);
        glGetProgramiv(pid, GL_VALIDATE_STATUS, &compiled_ok);
        compiled = linked_ok && compiled_ok; 
        
        if (!compiled) {
            char vertex_log  [256];
            char fragment_log[256];
            char program_log [256];
            glGetShaderInfoLog(vert_id, sizeof(vertex_log),   NULL, vertex_log);
            glGetShaderInfoLog(frag_id, sizeof(fragment_log), NULL, fragment_log);
            glGetProgramInfoLog(pid,    sizeof(program_log),  NULL, program_log);
            breakpoint;
            glDeleteShader(vert_id);
            glDeleteShader(frag_id);
            glDeleteProgram(pid);
            vert_id = frag_id = pid = 0;
            return false;
        } else {
            vert_pos = glGetAttribLocation(pid, "vert_pos");
            assert (vert_pos != -1); //didn't find it, probably optimized out if never used in shader
            
            vert_uv = glGetAttribLocation(pid, "vert_uv");
            //assert (vert_uv != -1); //didn't find it, probably optimized out if never used in shader 
            
            u_xform = glGetUniformLocation(pid, "u_xform");
            assert (u_xform != -1);
            
            u_color = glGetUniformLocation(pid, "u_color");
            assert (u_color != -1);
            
            u_near = glGetUniformLocation(pid, "u_near"); //NOTE this are optinal, we don't care if equal =1
            u_far  = glGetUniformLocation(pid, "u_far");  //NOTE this are optinal, we don't care if equal =1
            
            //u_cam_pos = glGetUniformLocation(pid, "u_cam_pos");
            //assert (u_cam_pos != -1);
            return true;
        }
    }
    
    void enable_vertex_attributes() {
        flush_errors();
        glEnableVertexAttribArray(vert_pos);
        glVertexAttribPointer(vert_pos, 3, GL_FLOAT, GL_FALSE, 
                              sizeof(Vertex), (void *)offsetof(Vertex, pos));
        
        if (vert_uv != -1) {
            glEnableVertexAttribArray(vert_uv);
            
            glVertexAttribPointer(vert_uv, 2, GL_FLOAT, GL_FALSE, 
                                  sizeof(Vertex), (void *)offsetof(Vertex, uv));    
        }
        check_for_errors(); //NOTE ERROR:1282 may happen if GL_ARRAY_BUFFER not bound
    }
    
    void disable_vertex_attributes() {
        flush_errors();
        glDisableVertexAttribArray(vert_pos);
        if (vert_uv != -1) glDisableVertexAttribArray(vert_uv);
        check_for_errors();
    }
};



struct Opengl {
    char *version;
    char *glsl_version;
    char *vendor;
    char *renderer;
    u32 flags;
    
    Circle_Shader_2D circle_shader_2d;
    Simple_Shader_2D simple_shader_2d;
    Simple_Shader_3D simple_shader_3d;
    
    GLuint mesh_vbo;
    GLuint mesh_ebo;
    Vertex_Mesh meshes[MESH_COUNT];
    
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
    enum Type {
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
internal bool
allocate_texture(Opengl_Texture_Parameters params, GLuint *result) {
    flush_errors();
    glGenTextures(1, result); //NOTE can this every fail?... docs seem to indicate that it can't...
    assert (params.width  <= OpenGL.max_texture_size);
    assert (params.height <= OpenGL.max_texture_size);
    
    GLenum filter_type = 0;
    GLenum wrap_type   = 0;
    GLenum target_type = (params.multisample_count > 0) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    glBindTexture(target_type, *result);
    u32 memory_allocation_size_guess = 0;
    
    switch(params.type) {
    case Opengl_Texture_Parameters::COLOR: {
        //TODO why the fuck does this have to be inverted?... is this why we've been having to multiply colors with themselves for a long time?...
        GLuint texture_format = (OpenGL.flags & OpenGL_SUPPORTS_SRGB_TEXTURE) ? GL_SRGB8_ALPHA8 : GL_RGB8;
        
        if (params.multisample_count > 0) {
            set_min2(params.multisample_count, OpenGL.max_multisample_count);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, params.multisample_count,
                                    texture_format, params.width, params.height, GL_FALSE);
            filter_type = GL_LINEAR;
            wrap_type   = GL_CLAMP_TO_EDGE;
        } else {
            //NOTE if data null, space is still allocated
            glTexImage2D(GL_TEXTURE_2D, 0, texture_format, params.width, params.height, 
                         0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid *)params.pixels);
            filter_type = GL_NEAREST;
            wrap_type   = GL_REPEAT;
        }
    } break;
    
    case Opengl_Texture_Parameters::DEPTH: {
        if (params.multisample_count > 0) {
            set_min2(params.multisample_count, OpenGL.max_multisample_count);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, params.multisample_count,
                                    GL_DEPTH_COMPONENT32, params.width, params.height, GL_FALSE);
            filter_type = GL_LINEAR;
            wrap_type   = GL_CLAMP_TO_EDGE;
        } else {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, params.width, params.height, 
                         0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, (GLvoid *)params.pixels);
            filter_type = GL_NEAREST;
            wrap_type   = GL_REPEAT;
        }
    } break;
    
    case Opengl_Texture_Parameters::SINGLE_CHANNEL: {
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
    return true;
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
    allocate_texture(params, &framebuffer->color_attachment);
    
    
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
    allocate_texture(params, &framebuffer->depth_attachment);
    
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

static void 
load_simple_meshes_vbo_and_ebo() {
    Memory_Arena *temp = get_temp_memory();
    RESTORE_MEMORY_ARENA_ON_SCOPE_EXIT(temp);
    
    Array<Vertex> vertex_array = make_array<Vertex>(temp, 512);
    Array<u32> index_array = make_array<u32>(temp, 1024);
    
    auto init_mesh = [&vertex_array, &index_array](Vertex_Mesh *mesh, Vertex *vertices, s32 vertex_count, u32 *indices, s32 index_count) {
        mesh->first_index = index_array.count; 
        mesh->index_count = index_count;
        u32 *dst_i = index_array.add_array(index_count);
        assert (dst_i);
        
        s32 first_vertex_offset = vertex_array.count;
        for (int i = 0; i < index_count; i += 1) {
            dst_i[i] = first_vertex_offset + indices[i];
        }
        
        mesh->vertex_count = vertex_count;
        Vertex *dst_v = vertex_array.add_array(vertex_count);
        assert (dst_v);
        memcopy(dst_v, vertices, sizeof(vertices[0])*vertex_count);
    };
    
    for (int i = 0; i < MESH_COUNT; i += 1) {
        Vertex_Mesh *mesh = OpenGL.meshes + i;
        switch (i) {
        case MESH_TRIANGLE: {
            //f32 height = 0.86602540378f;
            f32 half_h = 0.43301270189f;
            Vertex vertices[] {
                {{-0.5f, -half_h, 0.0f},  {0, 0}}, //0 front, bot left
                {{+0.5f, -half_h, 0.0f},  {1, 0}}, //1 front, top left
                {{+0.0f, +half_h, 0.0f}, {0.5f, 1}}, //2 front, bot right   
            };
            u32 indices[] = {
                0, 1, 2,
            };
            init_mesh(mesh, vertices, countof(vertices), indices, countof(indices));
        } break;
        case MESH_QUAD: {
            Vertex vertices[] {
                {{-0.5f, -0.5f, 0.0f}, {0, 0}}, //0 front, bot left
                {{-0.5f, +0.5f, 0.0f}, {0, 1}}, //1 front, top left
                {{+0.5f, -0.5f, 0.0f}, {1, 0}}, //2 front, bot right
                {{+0.5f, +0.5f, 0.0f}, {1, 1}}, //3 front, top right    
            };
            u32 indices[] = {
                0, 2, 1,
                1, 2, 3,
            };
            init_mesh(mesh, vertices, countof(vertices), indices, countof(indices));
        } break;
        case MESH_CUBE: {
            Vertex vertices[] = {
                {{-0.5f, -0.5f, +0.5f}, {0, 0}}, //0 front, bot left
                {{-0.5f, +0.5f, +0.5f}, {0, 1}}, //1 front, top left
                {{+0.5f, -0.5f, +0.5f}, {1, 0}}, //2 front, bot right
                {{+0.5f, +0.5f, +0.5f}, {1, 1}}, //3 front, top right
                
                {{-0.5f, -0.5f, -0.5f}, {0, 0}}, //4 back, bot left
                {{-0.5f, +0.5f, -0.5f}, {0, 1}}, //5 back, top left
                {{+0.5f, -0.5f, -0.5f}, {1, 0}}, //6 back, bot right
                {{+0.5f, +0.5f, -0.5f}, {1, 1}}, //7 back, top right
            };
            
            u32 indices[] = { //can we use a smaller size here?
                0, 2, 1, //front
                1, 2, 3,
                
                1, 3, 5, //top
                5, 3, 7,
                
                6, 4, 7, //back
                7, 4, 5,
                
                4, 6, 0, //bottom
                0, 6, 2,
                
                2, 6, 3, //right
                3, 6, 7,
                
                4, 0, 5, //left
                5, 0, 1,
            };
            init_mesh(mesh, vertices, countof(vertices), indices, countof(indices));
        } break;
        }
    }
    
    
    glGenBuffers(1, &OpenGL.mesh_vbo);
    glGenBuffers(1, &OpenGL.mesh_ebo);
    
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL.mesh_vbo);
    s32 vertices_size = sizeof(Vertex)*vertex_array.count;
    glBufferData(GL_ARRAY_BUFFER, vertices_size, vertex_array.e, GL_STATIC_DRAW); 
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, OpenGL.mesh_ebo);
    s32 indices_size = sizeof(u32)*index_array.count;
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, index_array.e, GL_STATIC_DRAW); 
    
    debug_printf("Mesh vbo is %d bytes (%d vertices)\n", vertices_size, vertex_array.count);
    debug_printf("Mesh ebo is %d bytes (%d indices)\n",  indices_size,  index_array.count);
}

internal void
init_opengl() {
    //glEnable(GL_CULL_FACE);// for 2D we can just negate the basis to flip 
    //glCullFace(GL_BACK);   // the textures around and save on texture memory ...
    glDepthMask(GL_TRUE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glEnable(GL_FRAMEBUFFER_SRGB); //NOTE check to see if we can do this otherwise emulate in shader
    //also NOTE textures are in SRGB space, so we need to sample them and convert them into linear space before the end of the fragment shader
    
    if (SwapIntervalEXT) {
        //TODO how can I figure out how long the interval is...should I just measure it?
        SwapIntervalEXT(1);
    }
    
    //NOTE Don't know why we need a vao, but we do, bound here and never unbound again
    GLuint vao; 
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    
    for_index_inc (s32, i, UBO_COUNT) {
        glGenBuffers(1, &g_ubo[i].buffer);
        glBindBuffer(GL_UNIFORM_BUFFER, g_ubo[i].buffer);
        assert (g_ubo[i].max_buffer_size > 0); // forgot to initalize?
        glBufferData(GL_UNIFORM_BUFFER, g_ubo[i].max_buffer_size, 0, GL_DYNAMIC_DRAW);    
    }
    
    
    
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
    set_min2(OpenGL.multisample_count, OpenGL.max_multisample_count);
    #endif
    
    
    //TODO pass in initial window dimensions here...
    OpenGL.framebuffer = make_framebuffer(DEFAULT_FRAMEBUFFER_WIDTH, DEFAULT_FRAMEBUFFER_HEIGHT, OpenGL.multisample_count);
    attach_color_texture(&OpenGL.framebuffer);
    attach_depth_texture(&OpenGL.framebuffer);
    
    
    #if 0
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
        
        mesh->vertex_count = countof(vertices);
        mesh->index_count  = countof(indices);
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
    #endif
    
    load_simple_meshes_vbo_and_ebo();
    
    
    //TODO log error output
    OpenGL.circle_shader_2d.compile_shader();
    OpenGL.simple_shader_2d.compile_shader(UBO_ATLAS_RECTS);
    OpenGL.simple_shader_3d.compile_shader();
    
    
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
draw_2d_circles(Render_Context *rcx, Vector2i viewport_dim) {
    if (rcx->circle_count <= 0) return; //nothing to do
    
    flush_errors();
    
    Circle_Shader_2D *shader = &OpenGL.circle_shader_2d;
    assert (shader->compiled);
    glUseProgram(shader->pid);
    glUniform2i(shader->u_viewport_dim, viewport_dim.x, viewport_dim.y);
    
    for (int i = 0; i < rcx->circle_count; i += 1) {
        Draw_Circle *cir = rcx->circles + i; 
        glUniform2f(shader->u_quad_pos, cir->pos.x, cir->pos.y);
        glUniform1f(shader->u_outer_radius, cir->outer_r);
        glUniform1f(shader->u_inner_radius, cir->inner_r);
        
        Vector4 col = unpack_v4(cir->color);
        glUniform4f(shader->u_quad_col, col.r, col.g, col.b, col.a);
        
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    
    glUseProgram(0);
    check_for_errors();
}

static void
draw_2d_quads(Render_Context *rcx, Vector2i viewport_dim) {
    if (rcx->quad_count <= 0) return; //nothing to do
    flush_errors();
    
    Simple_Shader_2D *shader = &OpenGL.simple_shader_2d;
    assert (shader->compiled);
    glUseProgram(shader->pid);
    
    glUniform2i(shader->u_viewport_dim, viewport_dim.x, viewport_dim.y);
    
    Texture_Atlas *atlas = rcx->atlas + TEXTURE_ATLAS_UI_FONT;
    assert (atlas->loaded_on_gpu);
    glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
    glBindTexture(GL_TEXTURE_2D, atlas->gpuid);
    glUniform1i(shader->u_texture1, 0); //make slot point to texture "0"
    glUniform2i(shader->u_texture1_size, (s32)atlas->width, (s32)atlas->height);
    
    Uniform_Buffer_Object *ubo_atlas_rects = &g_ubo[UBO_ATLAS_RECTS];
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_atlas_rects->buffer); 
    glBindBufferBase(GL_UNIFORM_BUFFER, ubo_atlas_rects->binding_point, ubo_atlas_rects->buffer);
    u32 atlas_rect_data_size = sizeof(atlas->rects[0]) * atlas->rect_count;
    assert (atlas_rect_data_size <= ubo_atlas_rects->max_buffer_size);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, atlas_rect_data_size, atlas->rects);
    
    for (s32 quad_index = 0; quad_index < rcx->quad_count; quad_index += 1) {
        Textured_Quad *quad = rcx->quads + quad_index;
        assert (quad->sprite_id.atlas_id == 0); //NOTE for the moment we only use the ui_font altas
        glUniform2f(shader->u_quad_pos, quad->pos.x, quad->pos.y);
        glUniform1f(shader->u_rotation_t, quad->rotation_t);
        glUniform2f(shader->u_quad_dim, quad->w, quad->h);
        
        Vector4 col = unpack_v4(quad->color);
        glUniform4f(shader->u_quad_col, col.r, col.g, col.b, col.a);
        
        glUniform1i(shader->u_atlas_rect_index, quad->sprite_id.offset);
    
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    
    
    glBindBuffer(GL_UNIFORM_BUFFER, 0); 
    glUseProgram(0);
    
    check_for_errors();
}


static void
draw_3d_cubes(Render_Context *rcx, Vector2i viewport_dim) {
    flush_errors();
    
    glEnable(GL_CULL_FACE);  
    glCullFace(GL_BACK);
    Simple_Shader_3D *shader = &OpenGL.simple_shader_3d;
    assert (shader->compiled);
    glUseProgram(shader->pid);
    
    glBindBuffer(GL_ARRAY_BUFFER,         OpenGL.mesh_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, OpenGL.mesh_ebo); //for glDrawElementsInstanced
    shader->enable_vertex_attributes(); //NOTE buffers must be bound before
    
    Vector3 cam_x, cam_y, cam_z;
    get_camera_local_axis(&cam_x, &cam_y, &cam_z);
    
    //debug_printf("Cam yaw: %.3f, pitch: %.3f\n", rcx->cam_yaw_t*360, rcx->cam_pitch_t*360);
    //debug_printf("Cam x: (%.3f, %.3f, %.3f)\n", cam_x.x, cam_x.y, cam_x.z);
    //debug_printf("Cam y: (%.3f, %.3f, %.3f)\n", cam_y.x, cam_y.y, cam_y.z);
    //debug_printf("Cam z: (%.3f, %.3f, %.3f)\n", cam_z.x, cam_z.y, cam_z.z);
    
    //NOTE each camera axis takes up a row, meaning it will be dotted against income vector to project it onto camera local axis
    //the 4th column is translation but predotted with camera axis to do translation in view space
    Matrix4x4 lookat = make4x4(cam_x.x, cam_x.y, cam_x.z, -dot3(cam_x, rcx->cam_pos),
                               cam_y.x, cam_y.y, cam_y.z, -dot3(cam_y, rcx->cam_pos),
                               cam_z.x, cam_z.y, cam_z.z, -dot3(cam_z, rcx->cam_pos),
                                     0,       0,       0,  1);
    
    f32 np =  -0.1f; //near plane
    f32 fp = -25.0f; //far plane
    
    f32 aspect_w_over_h = (f32)viewport_dim.x / (f32)viewport_dim.y;
    
    clamp(&rcx->xfov_t, 60.0f/360.0f, 120.0f/360.0f);
    f32 np_width = 2*np*tan_t(rcx->xfov_t/2);
    f32 np_height = np_width / aspect_w_over_h;
    
    
    Matrix4x4 proj = {}; //from view space to
    proj.e11 = (2*np) / np_width;
    proj.e22 = (2*np) / np_height;
    //NOTE for z going forward into screen (left handed), the denominator would be fp-np
    proj.e33 = (fp + np)  / (np - fp);     
    proj.e34 = (-2*fp*np) / (np - fp);
    //NOTE for z going forward into screen (left handed), this would be positive
    proj.e43 = -1; //NOTE this puts z in w of vertex for perspective divide
    
    Matrix4x4 world_to_project = multiply(&proj, &lookat);
    
    if (shader->u_near != -1) {
        glUniform1f(shader->u_near, np);
    }
    if (shader->u_far != -1) {
        glUniform1f(shader->u_far, fp);
    }
    
    for (int i = 0; i < rcx->model_count; i += 1) {
        Model *model = rcx->models + i;    
        Vertex_Mesh *mesh = OpenGL.meshes + model->mesh_type;
        assert (mesh->vertex_count > 0);
        
        Matrix4x4 xform = identity4x4();
        xform.x_basis.xyz = model->basis.x;
        xform.y_basis.xyz = model->basis.y;
        xform.z_basis.xyz = model->basis.z;
        
        //translation
        xform.e14 = model->pos.x;
        xform.e24 = model->pos.y;
        xform.e34 = model->pos.z;
        
        Matrix4x4 m = multiply(&world_to_project, &xform);
        glUniformMatrix4fv(shader->u_xform, 1, GL_FALSE, m.e);
        
        Vector4 color = unpack_v4(model->color);
        glUniform4f(shader->u_color, color.r, color.g, color.b, color.a); 
        
        u32 indices_byte_offset = mesh->first_index * sizeof(u32);
        glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, (GLvoid *)(u64)indices_byte_offset);
    }
    
    shader->disable_vertex_attributes();
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
    
    
    
    glClearDepth(1);
    glClearColor(SQUARED(0.5f), SQUARED(0.5f), SQUARED(0.5f), 1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    draw_3d_cubes(rcx, viewport_dim);
    //draw_2d_quads(rcx, viewport_dim);
    //draw_2d_circles(rcx, viewport_dim);
    
    
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

