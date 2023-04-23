
#include <chrono>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

#include "include/delaunay.h"
#include "include/viewer.h"

namespace ch = std::chrono;

#define NOW() ch::steady_clock::now()

#define WINDOW_WIDTH 1900
#define WINDOW_HEIGHT 1000

const int FRAME_RATE = 5;
const int FRAME_DELAY = 1000 / FRAME_RATE;

std::vector<float2> const generateRandomPoints(int num_points, int radius_x,
                                                int radius_y, float2 offset) {
  std::vector<float2> points;
  points.reserve(num_points);

  std::random_device rd;

  auto rndd = rd();
  std::cout << "rndd: " << rndd << std::endl;
  std::mt19937 gen(rndd);
  // std::mt19937 gen(2731319382); // BUG CHECK!
  // std::mt19937 gen(2201349777); // bug?
  //  rndd: 3369158952
  std::uniform_real_distribution<float> dis_x(-radius_x, radius_x);
  std::uniform_real_distribution<float> dis_y(-radius_y, radius_y);

  for (size_t i = 0; i < num_points; ++i) {
    int2 const pos(int(dis_x(gen)) + offset.x, int(dis_y(gen)) + offset.y);
    points.emplace_back(pos);
  }

  return points;
}

int main() {

  // Viewer
  Viewer viewer(WINDOW_WIDTH, WINDOW_HEIGHT);

  /*************** Rendering cycle ***************/
  bool quit = false;
  bool update = true;
  bool draw = true;
  while (!quit) {
    auto start_time_point = NOW();

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
        case SDLK_ESCAPE:
          quit = true;
          break;
        case SDLK_SPACE:
          update = true;
          break;
        case SDLK_RETURN:
          draw = !draw;
          break;
        }
        break;
      }
    } // end of pull event while

    if (update) {

      // Input

      // Output
      std::vector<Edge *> solution;

      update = false;

      // Delaunay
      DivideConquer DC;

      /********************* Generate Input ********************/
      std::vector<float2> const rng =
          generateRandomPoints(50000, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2,
                               float2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2));

      /******************  Delaunay   *************/
      // Compute Divide&Conquer and compute triangulation
      auto t = NOW();
      DC.computeTriangulation(rng);
      std::cout << "Time Delaunay: "
                << ch::duration_cast<ch::milliseconds>(NOW() - t).count()
                << "ms" << std::endl;

      /******************  Kruskal   **************/
      // Compute Kruskal
      auto kt = ch::steady_clock::now();
      float min_d = DC.computeKruskalMinD(solution);
      std::cout << "Time Kruskal: "
                << ch::duration_cast<ch::milliseconds>(NOW() - kt).count()
                << "ms" << std::endl;

      std::cout << "Time TOTAL: "
                << ch::duration_cast<ch::milliseconds>(NOW() - t).count()
                << "ms" << std::endl;

      std::cout << "min_d: " << min_d << std::endl;

      // show
      if (draw) {
        viewer.show(solution, rng);
      }
    }
  }

  return 0;
}
