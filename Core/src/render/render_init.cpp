//#include <stdio.h>
//#include <iostream>
//#include <glad.h>
//#include <SDL.h>
//#include <global.h>
//#include <render_internal.h>
//#include <imgui.h>
//#include <imgui_impl_opengl3.h>
//#include <imgui_impl_sdl2.h>
//#include <shader.h>
//
//SDL_Window* render_init_window(u32 width, u32 height) {
//    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
//    {
//        std::cerr << "Failed to initialize the SDL2 library\n" << SDL_GetError();
//        exit(-1);
//    }
//    else
//    {
//        //Set OpenGL version
//        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3); //set version to 3
//        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3); //specifically set to X.3
//        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); //chose core when getting glad rather than compatability for reliability
//        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); //believe defaults to 1 anyways, but explicitly writing for readability and assurance
//        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); //24 bit colour allows full colour range, even though 8 bits fine for human eye, more bits=higher quality
//        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
//        SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
//
//        //Create a window with specific dimensions and centering
//        SDL_Window* window = SDL_CreateWindow("SDL2 Window",
//            SDL_WINDOWPOS_CENTERED,
//            SDL_WINDOWPOS_CENTERED,
//            width, height,
//            window_flags);
//
//        if (!window)
//        {
//            std::cerr << "Window could not be created!" << SDL_GetError();
//            exit(-1);
//        }
//
//        //Declare sdl_glcontext
//        SDL_GLContext gl_context = SDL_GL_CreateContext(window);
//        SDL_GL_MakeCurrent(window, gl_context);
//        SDL_GL_SetSwapInterval(1); // Enable vsync
//
//        // Setup Dear ImGui context
//        IMGUI_CHECKVERSION();
//        ImGui::CreateContext();
//        ImGuiIO& io = ImGui::GetIO(); (void)io;
//        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
//        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
//        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
//        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
//
//        // Setup Dear ImGui style
//        ImGui::StyleColorsDark();
//        //ImGui::StyleColorsLight();
//
//        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
//        ImGuiStyle& style = ImGui::GetStyle();
//        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
//        {
//            style.WindowRounding = 0.0f;
//            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
//        }
//
//        //Setup function pointers
//        if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
//            std::cerr << "OpenGL context could not be created!" << SDL_GetError();
//            exit(-1);
//        }
//
//        // Setup Platform/Renderer backends
//        ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
//        ImGui_ImplOpenGL3_Init("#version 330");
//
//        return window;
//    }
//}
//
//void shaders_init_default(Render_State_Internal *state) {
//    Shader ourShader("C:\\shaders\\shader.vert", "C:\\shaders\\shader.frag");
//    state->default_shader_program = ourShader.getID();
//}
//
//void render_init_quad(u32* vao, u32* vbo, u32* ebo) {
//    //Two triangles will form a quad, but rather than having 6 vertices for two triangles we can use 4 and then use indices to draw two triangles
//    //x, y, z, u, v
//    f32 vertices[] = {
//         0.5,  0.5, 0, 0, 0,  // top right
//         0.5, -0.5, 0, 0, 1,  // bottom right
//        -0.5, -0.5, 0, 1, 1,  // bottom left
//        -0.5,  0.5, 0, 1, 0  // top left 
//    };
//    u32 indices[] = {  //Vertex indices
//        0, 1, 3,   // first triangle
//        1, 2, 3    // second triangle
//    };
//    //Generate VAO and then VBO and then EBO
//    glGenVertexArrays(1, vao);
//    glGenBuffers(1, vbo);
//    glGenBuffers(1, ebo);
//    //Bind VAO
//    glBindVertexArray(*vao);
//    //Copy vertices array into VBO
//    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//    //Copy index array into EBO
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//    //Configure vertex attributes xyz
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)0);
//    glEnableVertexAttribArray(0);
//    //Configure vertex attributes uv
//    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)(3 * sizeof(f32)));
//    glEnableVertexAttribArray(1);
//}