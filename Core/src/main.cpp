//Using following libraries: standard IO; IOstream; SDL
#include <stdio.h>
#include <iostream>
//#include <glad.h>
//#include <SDL.h>
//#include <global.h>
//#include <imgui.h>
//#include <imgui_impl_opengl3.h>
//#include <imgui_impl_sdl2.h>
//#include <render_internal.h>

int main(int argc, char* args[]) //the parameters are super important otherwise sdl_main.h missing
{
    //render_init();

    //ImGuiIO& io = ImGui::GetIO(); (void)io;

    ////Main loop now
    //bool done = false;
    //while (!done)
    //{
    //    //Poll events
    //    SDL_Event event;
    //    while (SDL_PollEvent(&event))
    //    {
    //        ImGui_ImplSDL2_ProcessEvent(&event);
    //        if (event.type == SDL_QUIT)
    //            done = true;
    //        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(global.render.window))
    //            done = true;
    //    }

    //    // Start the Dear ImGui frame
    //    ImGui_ImplOpenGL3_NewFrame();
    //    ImGui_ImplSDL2_NewFrame();
    //    ImGui::NewFrame();

    //    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    //    {
    //        static float f = 0.0f;
    //        static int counter = 0;

    //        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

    //        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
    //            counter++;
    //        ImGui::SameLine();
    //        ImGui::Text("counter = %d", counter);
    //        ImGui::End();
    //    }

    //    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    //    {
    //        static float f = 0.0f;
    //        static int counter = 0;

    //        ImGui::Begin("Bye world, world!");                          // Create a window called "Hello, world!" and append into it.

    //        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
    //            counter++;
    //        ImGui::SameLine();
    //        ImGui::Text("counter = %d", counter);
    //        ImGui::End();
    //    }

    //    //Rendering
    //    ImGui::Render();
    //    render_begin();
    //    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    //    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    //    render_quad(vec2{ global.render.width * 0.5f, global.render.height * 0.5f }, vec2{ 50, 50 }, vec4{ 0.5, 1.0, 0.7, 1 });
    //    // Update and Render additional Platform Windows
    //    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
    //    //  For this specific demo app we could also call SDL_GL_MakeCurrent(window, gl_context) directly)
    //    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    //    {
    //        SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
    //        SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
    //        ImGui::UpdatePlatformWindows();
    //        ImGui::RenderPlatformWindowsDefault();
    //        SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
    //    }

    //    render_end();
    //
    //}

    //render_quit_quad();

    //// Cleanup
    //ImGui_ImplOpenGL3_Shutdown();
    //ImGui_ImplSDL2_Shutdown();
    //ImGui::DestroyContext();

    ////SDL_GL_DeleteContext(gl_context);
    ////SDL_DestroyWindow(global.render.window);
    //SDL_Quit(); //As per docs, SDL_Quit() is a clean quit and should delete context and destroy window

    return 0;
}

