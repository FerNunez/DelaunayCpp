
#include <chrono>
#include <iostream>
#include <thread>

#include "include/delaunay.h"
#include "include/viewer.h"

#define Now() std::chrono::steady_clock::now()
const int FRAME_RATE = 60;
const int FRAME_DELAY = 1000 / FRAME_RATE;

void printVectorEdgeLenght(std::vector<Edge *> v) {
  for (auto e : v) {
    std::cout << e->Qedge()->lenght_sqrt << ", ";
  }
  std::cout << " - " << std::endl;
}

int main() {

  const Point2d a(0, 0);
  const Point2d b(640, 0);
  const Point2d c(320, 480);
  Subdivision S = Subdivision(a, b, c);
  // Init viewer
  Viewer viewer(640, 480);

  bool update = false;
  bool quit = false;
  while (!quit) {
    auto start_time_point = Now();

    // get events
    SDL_Event event;
    while (SDL_PollEvent(&event)) {

      switch (event.type) {
        // shutdown window key
      case SDL_QUIT:
        quit = true;
        break;

        // a key pressed down
      case SDL_KEYDOWN:

        switch (event.key.keysym.sym) {

          // add point
        case SDLK_SPACE:
          Point2d mouse_pos;
          SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
          S.InsertSite(mouse_pos);
          update = true;
          break;
        }
        break;
      }
    } // end of pull event while

    if (update) {
      update = false;
      std::cout << "before: ";
      printVectorEdgeLenght(S.edges_stack);
      std::cout << "after: ";
      printVectorEdgeLenght(S.edges_stack);
    }
    // clear and draw and sleep
    viewer.clear();

    viewer.drawAll(S.edges_stack);

    // present render
    viewer.render();

    auto elapsed_time_duration =
        std::chrono::steady_clock::now() - start_time_point;

    if (elapsed_time_duration < std::chrono::milliseconds(FRAME_DELAY)) {
      std::this_thread::sleep_for(std::chrono::milliseconds(FRAME_DELAY) -
                                  elapsed_time_duration);
    }
  }

  return 0;
}
