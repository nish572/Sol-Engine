#ifndef GLOBAL_H
#define GLOBAL_H

#include <render.h>
#include <imgui.h>

//Defining global struct
typedef struct global {
	Render_State render;
} Global;

//Extern so that compiler can assign data later
extern Global global;

#endif