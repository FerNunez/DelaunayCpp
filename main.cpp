
#include <chrono>
#include <iostream>
#include <thread>

#include "include/delaunay.h"
#include "include/kruskal.h"
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

  DivideConquer DC(test);
  Edge *oleft;
  Edge *oright;
  DC.delaunay(oleft, oright, 0, test.size() - 1);

  Kruskal k(DC.edges_stack, DC.node_stack);

  std::cout << "oleft.node.data " << oleft->node->data << std::endl;

  std::cout << "oright.node.data " << oright->node->data << std::endl;

  const Point2d a(0, 0);
  const Point2d b(640, 0);
  const Point2d c(320, 480);
  Subdivision S = Subdivision(a, b, c);
  //  Kruskal k(&S);
  // Init viewer
  Viewer viewer(640, 480);

  bool update = false;
  bool update_kruskal = false;

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
        case SDLK_SPACE: {
          Point2d mouse_pos;
          SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
          S.InsertSite(mouse_pos);
          update = true;
          break;
        }
        case SDLK_RETURN: {
          update_kruskal = true;
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
      k.update();
      std::cout << "edge solution: " << k.retrieveSol().size() << std::endl;
    }

    // incremental
    //    viewer.drawAll(S.edges_stack, 0, 0, 255);
    //    viewer.drawAll(k.retrieveSol(), 255, 0, 0);

    // Conquer divie
    viewer.drawAll(DC.edges_stack, 0, 0, 255);
    viewer.drawAll(k.retrieveSol(), 255, 0, 0);

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
