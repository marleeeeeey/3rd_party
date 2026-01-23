// based on the original file:
// https://github.com/ocornut/imgui/blob/master/examples/example_sdl3_sdlrenderer3/main.cpp

#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>

#include <cmath>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "implot.h"

static constexpr int data_count = 100;

struct AppState {
  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;
  bool show_demo_window = true;
  bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  // Variables for ImPlot demonstration
  float x[data_count];
  float y[data_count];
};

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
  AppState* state = new AppState;
  *appstate = state;

  // Setup SDL
  // [If using SDL_MAIN_USE_CALLBACKS: all code below until the main loop starts would likely be your SDL_AppInit() function]
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
    printf("Error: SDL_Init(): %s\n", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  // Create window with SDL_Renderer graphics context
  float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
  SDL_WindowFlags window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
  state->window = SDL_CreateWindow("Dear ImGui SDL3+SDL_Renderer example",
                                   (int)(1280 * main_scale), (int)(800 * main_scale), window_flags);
  if (state->window == nullptr) {
    printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  state->renderer = SDL_CreateRenderer(state->window, nullptr);
  SDL_SetRenderVSync(state->renderer, 1);
  if (state->renderer == nullptr) {
    SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  SDL_SetWindowPosition(state->window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
  SDL_ShowWindow(state->window);

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();  // Initialize ImPlot context
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  // Setup scaling
  ImGuiStyle& style = ImGui::GetStyle();
  style.ScaleAllSizes(main_scale);  // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
  style.FontScaleDpi = main_scale;  // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

  // Setup Platform/Renderer backends
  ImGui_ImplSDL3_InitForSDLRenderer(state->window, state->renderer);
  ImGui_ImplSDLRenderer3_Init(state->renderer);


  // Function for ImPlot demonstration
  for (int i = 0; i < data_count; i++) {
    state->x[i] = i * 0.1f;
    state->y[i] = sinf(state->x[i]);
  }

  // Our state
  state->show_demo_window = true;
  state->show_another_window = false;
  state->clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
  ImGui_ImplSDL3_ProcessEvent(event);

  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS; /* end the program, reporting success to the OS. */
  }

  return SDL_APP_CONTINUE; /* carry on with the program! */
}

SDL_AppResult SDL_AppIterate(void* appstate) {
  auto* state = static_cast<AppState*>(appstate);

  // Start the Dear ImGui frame
  ImGui_ImplSDLRenderer3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();

  // Create a simple ImPlot plot
  ImGui::Begin("ImPlot Example");
  if (ImPlot::BeginPlot("Sine Wave")) {
    ImPlot::PlotLine("y = sin(x)", state->x, state->y, data_count);
    ImPlot::EndPlot();
  }
  ImGui::End();

  // Rendering
  ImGuiIO& io = ImGui::GetIO();
  ImGui::Render();
  SDL_SetRenderScale(state->renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
  SDL_SetRenderDrawColorFloat(state->renderer, state->clear_color.x, state->clear_color.y, state->clear_color.z, state->clear_color.w);
  SDL_RenderClear(state->renderer);
  ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), state->renderer);
  SDL_RenderPresent(state->renderer);

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
  ImGui_ImplSDLRenderer3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImPlot::DestroyContext();
  ImGui::DestroyContext();
}
