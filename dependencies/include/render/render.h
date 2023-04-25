#ifndef RENDER_H
#define RENDER_H

#include <SDL.h>
#include <linmath.h>
#include <types.h>

typedef struct render_state {
	SDL_Window *window;
	u32 width;
	u32 height;
} Render_State;

void render_init(void);
void render_begin(void);
void render_end(void);
void render_quad(vec2 pos, vec2 size, vec4 colour);
void render_quit_quad(void);

#endif