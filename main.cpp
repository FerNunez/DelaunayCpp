
#include <chrono>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

#include "include/delaunay.h"
#include "include/kruskal.h"
#include "include/viewer.h"

#define Now() std::chrono::steady_clock::now()

#define WINDOW_WIDTH 1900
#define WINDOW_HEIGHT 1000

const int FRAME_RATE = 60;
const int FRAME_DELAY = 1000 / FRAME_RATE;

std::vector<Point2d> generateRandomPoints(int num_points, int radius_x,
                                          int radius_y, Point2d offset) {
  std::vector<Point2d> points;
  points.reserve(num_points);

  std::random_device rd;

  auto rndd = rd();
  std::cout << "rndd: " << rndd << std::endl;
  //  std::mt19937 gen(rndd);
  std::mt19937 gen(3489932919);
  std::uniform_real_distribution<float> dis_x(-radius_x, radius_x);
  std::uniform_real_distribution<float> dis_y(-radius_y, radius_y);

  for (size_t i = 0; i < num_points; ++i) {
    Point2d const pos(int(dis_x(gen)) + offset.x, int(dis_y(gen)) + offset.y);
    points.emplace_back(pos);
  }

  return points;
}

void printVectorEdgeLenght(std::vector<Edge *> v) {
  for (auto e : v) {
    std::cout << e->Qedge()->lenght_sqrt << ", ";
  }
  std::cout << " - " << std::endl;
}

int main() {

  // test
  const Point2d ta(0, 0);
  const Point2d tb(200, 200);
  const Point2d tc(100, 250);
  const Point2d td(150, 400);
  const Point2d te(300, 300);
  const Point2d tf(400, 410);

  std::vector<Point2d> test;
  test.push_back(ta);
  test.push_back(tb);
  test.push_back(tc);
  test.push_back(td);
  test.push_back(te);
  test.push_back(tf);

  std::vector<Point2d> rng =
      generateRandomPoints(50000, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2,
                           Point2d(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2));
  //  rng.push_back(rng.back());
  //  rng[rng.size() - 1] = rng[rng.size() - 2];

  DivideConquer DC(rng);
  Edge *oleft;
  Edge *oright;
  auto t = Now();
  DC.delaunay(oleft, oright, 0, rng.size() - 1);

  std::cout << "Time Delaunay: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(Now() - t)
                   .count()
            << "ms" << std::endl;
  Kruskal k(DC.edges_stack, DC.connected_nodes);

  Viewer viewer(WINDOW_WIDTH, WINDOW_HEIGHT);

  bool update = false;
  bool update_kruskal = false;
  bool draw_base = false;
  bool quit = false;

  while (!quit) {
    auto start_time_point = Now();

    // get events
    SDL_Event event;
    while (SDL_PollEvent(&event)) {

      switch (event.type) {
        // shutdown window key
      case SDL_QUIT:
      case SDLK_ESCAPE:
        quit = true;
        break;

      // a key pressed down
      case SDL_KEYDOWN:

        switch (event.key.keysym.sym) {

          // add point
        case SDLK_SPACE: {
          Point2d mouse_pos;
          SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
          //          S.InsertSite(mouse_pos);
          update = true;
          break;
        }
        case SDLK_RETURN: {
          update_kruskal = true;
          break;
        }
        case SDLK_BACKSPACE: {
          draw_base = !draw_base;
          break;
        }
        }
        break;
      }
    } // end of pull event while

    // clear window
    viewer.clear();

    //    // update and draw
    //    if (update) {
    //      update = false;
    //      std::cout << "Edge size: " << S.edges_stack.size() << std::endl;

    //      for (auto e : S.edges_stack) {
    //        std::cout << "Edge node orig:" << e->node->data << std::endl;
    //        std::cout << "Edge node orig ID:" << e->node->id << std::endl;

    //        std::cout << "Edge node dest:" << e->Sym()->node->data <<
    //        std::endl; std::cout << "Edge node dest ID:" << e->Sym()->node->id
    //        << std::endl; std::cout << "--" << std::endl;
    //      }
    //    }
    //    // update kruskal

    if (update_kruskal) {

      update_kruskal = false;
      auto t = Now();
      k.update();
      std::cout << "Time Kruskal: "
                << std::chrono::duration_cast<std::chrono::milliseconds>(Now() -
                                                                         t)
                       .count()
                << "ms" << std::endl;
    }

    // incremental
    //    viewer.drawAll(S.edges_stack, 0, 0, 255);
    //    viewer.drawAll(k.retrieveSol(), 255, 0, 0);

    //    Conquer divie
    if (draw_base) {
      viewer.drawAll(DC.edges_stack, 0, 0, 255);
    }

    viewer.drawAll(k.retrieveSol(), 255, 0, 0);

    viewer.drawVertex(rng, 0, 0x99, 0, 3);
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
