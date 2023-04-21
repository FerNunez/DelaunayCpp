#pragma once

#include "delaunay.h"
#include <algorithm>

#include <numeric> //std::iota
#include <vector>

class Kruskal {
public:
  Kruskal(const std::vector<Edge *> &e_stack, int num_nodes)
      : edge_stack(e_stack), num_node(num_nodes), min_d(){};

  void computeSolution();
  inline std::vector<Edge *> &retrieveSol() { return solution; }
  inline float retrieveMinD() { return min_d; }

  std::vector<Edge *> edge_stack;

private:
  // looks for set by checking parent
  int findSet(int i);

private:
  std::vector<Edge *> solution;
  std::vector<int> parent;
  int num_node;
  float min_d;
};
