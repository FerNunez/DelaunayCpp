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
    edge_stack = S->edges_stack;
    num_node = S->node_stack.size();
  };
  Kruskal(const std::vector<Edge *> &e_stack,
          const std::vector<Point2d> &node_stack) {
    edge_stack = e_stack;
    num_node = node_stack.size();
  };

  ~Kruskal() {}

  int find_set(int i) {
    // If i is the parent of itself
    if (i == parent[i])
      return i;
    else
      // Else if i is not the parent of itself
      // Then i is not the representative of his set,
      // so we recursively call Find on its parent
      return find_set(parent[i]);
  }

  void update() {
    parent.resize(num_node);
    std::iota(std::begin(parent), std::end(parent),
              0); // 0 is the starting number

    int stacked_nodes = 0;
    // generate unique IDsfor
    std::vector<int> group_id;
    group_id.resize(num_node);
    std::iota(std::begin(group_id), std::end(group_id), 0);

    // sort stack
    std::sort(edge_stack.begin(), edge_stack.end(), myfunction);

    // loop all edges
    int uRep, vRep;
    for (int i = 0; i < edge_stack.size(); i++) {

      uRep = find_set(edge_stack[i]->Org()->id);
      vRep = find_set(edge_stack[i]->Dest()->id);
      if (uRep != vRep) {
        solution.push_back(edge_stack[i]); // add to tree
        // union two different set of points
        parent[uRep] = parent[vRep];

        if (solution.size() == num_node - 1) {
          std::cout << "Im out?" << std::endl;
          break;
        }
      }
    }
  };

  inline std::vector<Edge *> retrieveSol() { return solution; }

  std::vector<Edge *> edge_stack;
  int num_node;

private:
  Subdivision *subdivision;
  std::vector<Edge *> solution;
  std::vector<int> parent;
};
