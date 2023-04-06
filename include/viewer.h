#pragma once
#include <SDL2/SDL.h>

#include "delaunay.h"

class Viewer {
public:
  Viewer(int WindowWidth, int WindowHeigh) {

    // Init
    SDL_Init(SDL_INIT_VIDEO);
    window =
        SDL_CreateWindow("SDL2 line drawing", SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, WindowWidth, WindowHeigh, 0);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  };

  ~Viewer() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }

  void render() { SDL_RenderPresent(renderer); }
  void clear() {
    SDL_SetRenderDrawColor(renderer, 242, 242, 242, 255);
    SDL_RenderClear(renderer);
  }

  void drawEdge(const Edge &);
  void drawAll(const std::vector<Edge *> vector_e);

private:
  SDL_Window *window;
  SDL_Renderer *renderer;
};

void Viewer::drawEdge(const Edge &e) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

  auto &origin = e.Org2d();
  auto &dest = e.Dest2d();
  SDL_RenderDrawLine(renderer, origin.x, origin.y, dest.x, dest.y);
}

void Viewer::drawAll(const std::vector<Edge *> vector_e) {

  for (auto e : vector_e) {

    // std::cout << e->Qedge()->lenght_sqrt << std::endl;
    drawEdge(*e);
  }
}
