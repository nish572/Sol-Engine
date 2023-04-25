#ifndef RENDER_INTERNAL_H
#define RENDER_INTERNAL_H

#include <SDL.h>
#include <types.h>
#include <render.h>

typedef struct render_state_internal {
	u32 vao_quad;
	u32 vbo_quad;
	u32 ebo_quad;
	u32 default_shader_program;
	u32 texture_colour;
	mat4x4 projection;
} Render_State_Internal;

//used to initialise sdl window
SDL_Window *render_init_window(u32 width, u32 height);

//used to initialise the default vertex and fragment shaders
void shaders_init_default(Render_State_Internal *state);

//used to initialise quad for mapping texture to
void render_init_quad(u32 *vao, u32 *vbo, u32 *ebo);

#endif