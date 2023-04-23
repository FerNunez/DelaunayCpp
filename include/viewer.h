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

  void show(const std::vector<Edge *> &vector_e,
            const std::vector<Point2f> &vertex) {
    clear();
    drawAll(vector_e, 255, 0, 0);
    drawVertex(vertex, 0, 0x70, 0, 3);
    render();
  }

private:
  void render() { SDL_RenderPresent(renderer); }
  void clear() {
    SDL_SetRenderDrawColor(renderer, 242, 242, 242, 255);
    SDL_RenderClear(renderer);
  }

  void drawEdge(const Edge &, int, int, int);
  void drawAll(const std::vector<Edge *> &vector_e, int, int, int);
  void drawVertex(const std::vector<Point2f> &, int, int, int, int);

private:
  SDL_Window *window;
  SDL_Renderer *renderer;
};

void Viewer::drawEdge(const Edge &e, int r, int g, int b) {
  SDL_RenderSetScale(renderer, 2, 2);
  SDL_SetRenderDrawColor(renderer, r, g, b, 255);

  auto &origin = e.Org2d();
  auto &dest = e.Dest2d();
  SDL_RenderDrawLine(renderer, origin.x / 2, origin.y / 2, dest.x / 2,
                     dest.y / 2);
  SDL_RenderSetScale(renderer, 1, 1);
}

void Viewer::drawAll(const std::vector<Edge *> &vector_e, int r, int g, int b) {

  int i = 0;
  for (auto e : vector_e) {
    // std::cout << "i:" << i << std::endl;
    i++;
    // std::cout << e->Qedge()->lenght_sqrt << std::endl;
    drawEdge(*e, r, g, b);
  }
}

void Viewer::drawVertex(const std::vector<Point2f> &vertex, int r, int g, int b,
                        int scale) {

  SDL_RenderSetScale(renderer, scale, scale);
  SDL_SetRenderDrawColor(renderer, r, g, b, 255);
  for (auto v : vertex) {
    SDL_RenderDrawPoint(renderer, v.x / scale, v.y / scale);
  }
  SDL_RenderSetScale(renderer, 1, 1);
  return;
}
