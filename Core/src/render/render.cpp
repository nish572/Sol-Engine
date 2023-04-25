//#include <glad.h>
//#include <global.h>
//#include <render_internal.h>
//#include <shader.h>
//
//static Render_State_Internal state = {0};
//
//void render_init(void) {
//	global.render.width = 1280;
//	global.render.height = 720;
//	global.render.window = render_init_window(global.render.width, global.render.height);
//	render_init_quad(&state.vao_quad, &state.vbo_quad, &state.ebo_quad);
//	shaders_init_default(&state);
//}
//void render_begin(void) {
//	glClearColor(0.7f, 0.3f, 1.0f, 0.0f);
//	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//}
//void render_end(void) {
//	SDL_GL_SwapWindow(global.render.window);
//}
//void render_quad(vec2 pos, vec2 size, vec4 colour) {
//	glUseProgram(state.default_shader_program);
//	glBindVertexArray(state.vao_quad); //Research more: 'seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized'
//	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//	//Below unbinds
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindVertexArray(0);
//}
//
//void render_quit_quad(void) {
//	//De-allocate all resources once finished with them (optional but good practice)
//	glDeleteVertexArrays(1, &state.vao_quad);
//	glDeleteBuffers(1, &state.vbo_quad);
//	glDeleteBuffers(1, &state.ebo_quad);
//}