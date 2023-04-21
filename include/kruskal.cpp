#include "kruskal.h"

int Kruskal::findSet(int i) {
  if (i == parent[i]) {
    return i;
  } else {
    return findSet(parent[i]);
  }
}

void Kruskal::computeSolution() {

  // sort increasing by lenght
  std::sort(edge_stack.begin(), edge_stack.end(), [](Edge *i, Edge *j) {
    return (i->Qedge()->lenght_sqrt < j->Qedge()->lenght_sqrt);
  });

  // loop all edges
  int node_orig_set = 0;
  int node_dest_set = 0;
  for (int i = 0; i < edge_stack.size(); i++) {

    if (!edge_stack[i]->Qedge()->alive) {
      continue;
    }

    //
    node_orig_set = findSet(edge_stack[i]->Org().id);
    node_dest_set = findSet(edge_stack[i]->Dest().id);

    // not in same set already
    if (node_orig_set != node_dest_set) {
      solution.push_back(edge_stack[i]); // add to tree
      // union two different set of points
      parent[node_orig_set] = parent[node_dest_set];

      // take minimym distance
      if (edge_stack[i]->Qedge()->lenght_sqrt > min_d) {
        min_d = edge_stack[i]->Qedge()->lenght_sqrt;
      }

      if (solution.size() == num_node - 1) {
        break;
      }
    }
  }

  // get real dist
  min_d = std::sqrt(min_d);
};
