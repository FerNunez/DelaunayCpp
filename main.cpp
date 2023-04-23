
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

const int FRAME_RATE = 60;
const int FRAME_DELAY = 1000 / FRAME_RATE;

std::vector<Point2f> generateRandomPoints(int num_points, int radius_x,
                                          int radius_y, Point2f offset) {
  std::vector<Point2f> points;
  points.reserve(num_points);

  std::random_device rd;

  auto rndd = rd();
  std::cout << "rndd: " << rndd << std::endl;
  std::mt19937 gen(rndd);
  //  std::mt19937 gen(2731319382); // BUG CHECK!
  // rndd: 3369158952
  std::uniform_real_distribution<float> dis_x(-radius_x, radius_x);
  std::uniform_real_distribution<float> dis_y(-radius_y, radius_y);

  for (size_t i = 0; i < num_points; ++i) {
    Point2d const pos(int(dis_x(gen)) + offset.x, int(dis_y(gen)) + offset.y);
    points.emplace_back(pos);
  }

  return points;
}

// void printVectorEdgeLenght(std::vector<Edge *> v) {
//   for (auto e : v) {
//     std::cout << e->getQuadEdge()->lenght << ", ";
//   }
//   std::cout << " - " << std::endl;
// }

int main() {

  /********************* Generate Input ********************/
  std::vector<Point2f> rng =
      generateRandomPoints(50000, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2,
                           Point2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2));

  /******************  Delaunay   *************/
  // Compute Divide&Conquer and compute triangulation
  auto t = NOW();
  DivideConquer DC;
  DC.computeTriangulation(rng);

  std::cout << "Time Delaunay: "
            << ch::duration_cast<ch::milliseconds>(NOW() - t).count() << "ms"
            << std::endl;

  /******************  Kruskal   **************/
  // Compute Kruskal
  auto kt = ch::steady_clock::now();
  std::vector<Edge *> solution;
  float min_d = DC.computeKruskalMinD(solution);

  std::cout << "Time Kruskal: "
            << ch::duration_cast<ch::milliseconds>(NOW() - kt).count() << "ms"
            << std::endl;

  std::cout << "Time TOTAL: "
            << ch::duration_cast<ch::milliseconds>(NOW() - t).count() << "ms"
            << std::endl;

  std::cout << "min_d: " << min_d << std::endl;
  Viewer viewer(WINDOW_WIDTH, WINDOW_HEIGHT);

  /*************** Rendering cycle ***************/
  bool quit = false;
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
        }
        break;
      }
    } // end of pull event while

    // clear window
    viewer.clear();

    viewer.drawAll(solution, 255, 0, 0);

    viewer.drawVertex(rng, 0, 0x70, 0, 3);
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
