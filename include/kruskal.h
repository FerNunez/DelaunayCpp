#pragma once

#include "delaunay.h"
#include <algorithm>

#include <numeric> //std::iota
#include <vector>

bool sortCriteria(Edge *i, Edge *j) {
  return (i->Qedge()->lenght_sqrt < j->Qedge()->lenght_sqrt);
}

class Kruskal {
public:
  Kruskal(const std::vector<Edge *> &e_stack, int num_nodes)
      : edge_stack(e_stack), num_node(num_nodes), min_d(){};

  void computeSolution();
  inline std::vector<Edge *> &retrieveSol() { return solution; }
  inline float retrieveMinD() { return min_d; }

  std::vector<Edge *> edge_stack;

private:
  int find_set(int i);
  std::vector<Edge *> solution;
  std::vector<int> parent;

  int num_node;
  float min_d;
};

int Kruskal::find_set(int i) {
  // If i is the parent of itself
  if (i == parent[i])
    return i;
  else
    // Else if i is not the parent of itself
    // Then i is not the representative of his set,
    // so we recursively call Find on its parent
    return find_set(parent[i]);
}

void Kruskal::computeSolution() {
  parent.resize(num_node);
  std::iota(std::begin(parent), std::end(parent),
            0); // 0 is the starting number

  int stacked_nodes = 0;
  // generate unique IDsfor
  std::vector<int> group_id;
  group_id.resize(num_node);
  std::iota(std::begin(group_id), std::end(group_id), 0);

  // sort stack
  std::sort(edge_stack.begin(), edge_stack.end(), sortCriteria);

  // loop all edges
  int uRep, vRep;
  for (int i = 0; i < edge_stack.size(); i++) {

    if (solution.size() == num_node - 1) {
      break;
    }
    if (!edge_stack[i]->Qedge()->alive) {
      continue;
    }
    uRep = find_set(edge_stack[i]->Org().id);
    vRep = find_set(edge_stack[i]->Dest().id);
    if (uRep != vRep) {
      solution.push_back(edge_stack[i]); // add to tree
      // union two different set of points
      parent[uRep] = parent[vRep];

      if (edge_stack[i]->Qedge()->lenght_sqrt > min_d) {
        min_d = edge_stack[i]->Qedge()->lenght_sqrt;
      }
    }
  }

  // get real dist
  min_d = std::sqrt(min_d);
};
