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
  Kruskal(const std::vector<Edge *> &e_stack, int num_nodes) {
    edge_stack = e_stack;
    num_node = num_nodes;
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

    //    for (int i(0); i < edge_stack.size(); i++) {
    //      std::cout << "i:" << i << std::endl;
    //      auto edge = edge_stack[i];
    //      std::cout << "edge->Org2d():" << edge->Org2d() << std::endl;
    //      std::cout << "edge->Dest2d(): " << edge->Dest2d() << std::endl;
    //      auto asd = edge->Qedge();
    //      std::cout << "print: " << asd->lenght_sqrt << std::endl;
    //    }
    std::sort(edge_stack.begin(), edge_stack.end(), myfunction);

    // loop all edges
    int uRep, vRep;
    int min_d = 0;
    for (int i = 0; i < edge_stack.size(); i++) {

      uRep = find_set(edge_stack[i]->Org()->id);
      vRep = find_set(edge_stack[i]->Dest()->id);
      if (uRep != vRep) {
        solution.push_back(edge_stack[i]); // add to tree
        // union two different set of points
        parent[uRep] = parent[vRep];

        if (edge_stack[i]->Qedge()->lenght_sqrt > min_d) {
          min_d = edge_stack[i]->Qedge()->lenght_sqrt;
          std::cout << "min_d: " << min_d << std::endl;
        }
        if (solution.size() == num_node - 1) {
          std::cout << "Im out?" << std::endl;
          break;
        }
      }
    }

    std::cout << "min_d: " << min_d << std::endl;

    min_d = std::sqrt(min_d);
    std::cout << "min_d: " << min_d << std::endl;
    std::cout << "std::sqrt(min_d): " << std::sqrt(min_d) << std::endl;
  };

  inline std::vector<Edge *> retrieveSol() { return solution; }

  std::vector<Edge *> edge_stack;
  int num_node;

private:
  Subdivision *subdivision;
  std::vector<Edge *> solution;
  std::vector<int> parent;
};
