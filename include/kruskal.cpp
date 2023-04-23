#include "kruskal.h"

Kruskal::Kruskal(const std::vector<Edge *> &e_stack, int num_nodes) {
  // init
  min_d = 0;

  // generate unique sets for each node
  parent.resize(num_node);
  std::iota(std::begin(parent), std::end(parent), 0);

  // get vailable edge vector
  std::vector<Edge *> valid_edges;
  valid_edges.reserve(edge_stack.size());
  for (auto i : edge_stack) {
    if (i->Qedge()->alive) {
      valid_edges.push_back(i);
    }
  }

  std::sort(valid_edges.begin(), valid_edges.end(), [](Edge *i, Edge *j) {
    return (i->Qedge()->lenght < j->Qedge()->lenght);
  });

  // loop all edges
  int node_orig_set = 0;
  int node_dest_set = 0;
  for (int i = 0; i < valid_edges.size(); i++) {

    if (!valid_edges[i]->Qedge()->alive) {
      continue;
    }

    //
    node_orig_set = findSet(valid_edges[i]->Org().id);
    node_dest_set = findSet(valid_edges[i]->Dest().id);

    // not in same set already
    if (node_orig_set != node_dest_set) {
      solution.push_back(valid_edges[i]); // add to tree
      // union two different set of points
      parent[node_orig_set] = parent[node_dest_set];

      // take minimym distance
      if (edge_stack[i]->Qedge()->lenght > min_d) {
        min_d = valid_edges[i]->Qedge()->lenght;
      }

      if (solution.size() == num_node - 1) {
        break;
      }
    }
  }

  // get real dist
  min_d = std::sqrt(min_d);
};

int Kruskal::findSet(int i) {
  if (i == parent[i]) {
    return i;
  } else {
    return findSet(parent[i]);
  }
}

void Kruskal::computeSolution(){

};
