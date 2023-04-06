#pragma once

#include "delaunay.h"
#include <algorithm>

#include <numeric> //std::iota
#include <vector>

bool myfunction(Edge *i, Edge *j) {
  return (i->Qedge()->lenght_sqrt < j->Qedge()->lenght_sqrt);
}

class Kruskal {
public:
  Kruskal(Subdivision *S) {
    subdivision = S;
    // 0 is the starting number
  };
  ~Kruskal() {}

  void update() {
    solution.resize(subdivision->node_stack.size() - 1);

    int stacked_nodes = 0;
    // generate unique IDsfor
    std::vector<int> group_id;
    group_id.resize(subdivision->node_stack.size());
    std::iota(std::begin(group_id), std::end(group_id), 0);

    // sort stack
    std::sort(subdivision->edges_stack.begin(), subdivision->edges_stack.end(),
              myfunction);

    // loop all edges
    int sol_idx = 0;
    for (auto const e : subdivision->edges_stack) {
      if (stacked_nodes >= subdivision->node_stack.size()) {
        break;
      }

      // stack first edge and 2 nodes
      if (stacked_nodes == 0) {
        solution[sol_idx] = e;
        stacked_nodes = 2;
        sol_idx++;
      }
    }
  };

private:
  Subdivision *subdivision;
  std::vector<Edge *> solution;
};
