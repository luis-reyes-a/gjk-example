#ifndef DRAW_H //created on January 9th 2023
#define DRAW_H

struct Color {
    union {
        //NOTE the order of these members will have to be flipped on big-endian
        struct { u8 a, r, g, b; };
        u32 argb;
    };
};

struct Camera2D {
    Vector2 pos;
    Vector2 view_dim;
};

struct Render_Context {
    Vector2i window_dim; //does not include border and title stuff. Just dimensions of window we can render into
    Camera2D screen_cam;
    Vector3 cam_pos;
    f32 xfov;
};

internal void
render_context_begin_frame(Render_Context *rcx, s32 window_w, s32 window_h, Vector2i mouse_pos) {
    rcx->window_dim = v2i(window_w, window_h);
    
    Camera2D *screen_cam = &rcx->screen_cam;
    screen_cam->view_dim = v2(window_w, window_h);
    screen_cam->pos = 0.5f*screen_cam->view_dim;
}

#endif //DRAW_H
