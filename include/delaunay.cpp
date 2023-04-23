#include "delaunay.h"

/************ Data Structure *************/
QuadEdge::QuadEdge() : alive(true), lenght(0.0) {

  // Set index
  e[0].index = 0; // Normal edge
  e[1].index = 1; // Face edge
  e[2].index = 2; // Normal edge
  e[3].index = 3; // Face edge

  // Normal edge pointing to themself
  e[0].next = &(e[0]);
  e[2].next = &(e[2]);

  // Face edge pointing eachother
  e[1].next = &(e[3]);
  e[3].next = &(e[1]);
}

void Edge::setEndPoints(const Node &ori, const Node &dest) {
  node = ori;
  Sym()->node = dest;

  // set lenght when setting edge origin/destination
  getQuadEdge()->lenght = lenghtSquared(ori.pos, dest.pos);
}

/*********************** Basic Topological Operators ************************/

int insideCircle(const Point2f &p, const Point2f &a, const Point2f &b,
                 const Point2f &c) {
  return (a.x * a.x + a.y * a.y) * computeArea(b, c, p) -
             (b.x * b.x + b.y * b.y) * computeArea(a, c, p) +
             (c.x * c.x + c.y * c.y) * computeArea(a, b, p) -
             (p.x * p.x + p.y * p.y) * computeArea(a, b, c) >
         0;
}

int ccw(const Point2f &a, const Point2f &b, const Point2f &c) {
  return (computeArea(a, b, c) > 0);
}

int rightOf(const Point2f &p, Edge *e) {
  return ccw(p, e->Dest2d(), e->Org2d());
}

int leftOf(const Point2f &p, Edge *e) {
  return ccw(p, e->Org2d(), e->Dest2d());
}

bool isValid(Edge *e, Edge *basel) { return rightOf(e->Dest2d(), basel); }

Edge *DivideConquer::makeEdge() {
  std::shared_ptr<QuadEdge> ql =
      std::make_shared<QuadEdge>(); // create 4 definition of edge
  m_quad_edges.push_back(ql);
  return ql->e;
}

Edge *DivideConquer::makeEdgeFrom(const Node &ori, const Node &de) {
  Edge *e = makeEdge();
  e->setEndPoints(ori, de);
  return e;
}

void Splice(Edge *a, Edge *b) {
  Edge *alpha = a->Onext()->Rot();
  Edge *beta = b->Onext()->Rot();

  Edge *t1 = b->Onext();
  Edge *t2 = a->Onext();
  Edge *t3 = beta->Onext();
  Edge *t4 = alpha->Onext();
  a->next = t1;
  b->next = t2;
  alpha->next = t3;
  beta->next = t4;
}
void DivideConquer::disconnectEdge(Edge *e) {
  Splice(e, e->Oprev());
  Splice(e->Sym(), e->Sym()->Oprev());

  // set alive false to no consider it nor remove it
  e->getQuadEdge()->alive = false;
  m_num_deleted_edges++;
  // delete e->Qedge();
}

Edge *DivideConquer::connect(Edge *a, Edge *b) {
  Edge *e = makeEdge();
  e->setEndPoints(a->Dest(), b->Org());

  Splice(e, a->Lnext());
  Splice(e->Sym(), b);
  e->getQuadEdge()->lenght = lenghtSquared(a->Dest().pos, b->Org().pos);

  return e;
}

/************************* Delaunay Triangulation Algorithm  ******************/

void DivideConquer::recursiveDelaunay(Edge *&o_left, Edge *&o_right,
                                      int left_idx, int right_idx) {
  // starts calling
  // delaunay(o_left, o_right, 0, point_size-1)
  //
  auto numb_points = 1 + right_idx - left_idx;
  if (numb_points == 2) {
    // a,b be the two sites, in sorted order.
    Node a(m_ordered_points[left_idx], m_num_nodes++);
    Node b(m_ordered_points[right_idx], m_num_nodes++);

    // Create an edge e from a to b
    Edge *e = makeEdgeFrom(a, b);

    o_left = e;
    o_right = e->Sym();

    return;
  }
  // abc
  else if (numb_points == 3) {
    // a, b, c be the three sites, in sorted order.
    Node a(m_ordered_points[left_idx], m_num_nodes++);
    Node b(m_ordered_points[left_idx + 1], m_num_nodes++);
    Node c(m_ordered_points[right_idx], m_num_nodes++);

    // Create edges ab connecting a-b and bc connecting b-c
    Edge *ab = makeEdgeFrom(a, b);
    Edge *bc = makeEdgeFrom(b, c);
    Splice(ab->Sym(), bc);

    //  Now close triangle
    // c.y < b.y
    if (ccw(m_ordered_points[left_idx], m_ordered_points[left_idx + 1],
            m_ordered_points[right_idx])) {
      connect(bc, ab);
      o_left = ab;
      o_right = bc->Sym();

    } else if (ccw(m_ordered_points[left_idx], m_ordered_points[right_idx],
                   m_ordered_points[left_idx + 1])) {
      Edge *c = connect(bc, ab);
      o_left = c->Sym();
      o_right = c;

    }
    // Three points are colinear
    else {
      o_left = ab;
      o_right = bc->Sym();
    }

    return;
  }
  // more than 3 points => recursive
  else {

    int lenght_half = numb_points / 2;
    // Compute delaunay onto leght side
    Edge *ldo; // leghtleft
    Edge *ldi; // leftright
    recursiveDelaunay(ldo, ldi, left_idx, left_idx + lenght_half - 1);

    // Compute delaunay onto right side
    Edge *rdi; // rightleft
    Edge *rdo; // rightright
    recursiveDelaunay(rdi, rdo, left_idx + lenght_half, right_idx);

    // Compute the lower common tangent of Left side and Right
    do {
      if (leftOf(rdi->Org2d(), ldi)) {
        ldi = ldi->Lnext();
      } else if (rightOf(ldi->Org2d(), rdi)) {
        rdi = rdi->Rprev();
      } else {
        break;
      }
    } while (true);

    // Create a first cross edge base1 from rdi.Org to ldi.Org
    Edge *basel = connect(rdi->Sym(), ldi);
    if (ldi->Org2d() == ldo->Org2d()) {
      ldo = basel->Sym();
    }
    if (rdi->Org2d() == rdo->Org2d()) {
      rdo = basel;
    }

    // This is the merge loop.
    do {
      // Locate the first L point (lcand->Dest2d()) to be encountered by the
      // rising bubble, and delete L edges out of base1->Dest2d() that fail the
      // circle test.
      Edge *lcand = basel->Sym()->Onext();
      if (isValid(lcand, basel)) {
        while (insideCircle(lcand->Onext()->Dest2d(), basel->Dest2d(),
                            basel->Org2d(), lcand->Dest2d())) {
          Edge *t = lcand->Onext();
          disconnectEdge(lcand);
          lcand = t;
        }
      }

      // Symmetrically, locate the first R point to be hit, and delete R edges
      Edge *rcand = basel->Oprev();
      if (isValid(rcand, basel)) {
        while (insideCircle(rcand->Oprev()->Dest2d(), basel->Dest2d(),
                            basel->Org2d(), rcand->Dest2d())) {
          Edge *t = rcand->Oprev();
          disconnectEdge(rcand);
          rcand = t;
        }
      }

      // If both lcand and rcand are invalid, then basel is the upper common
      // tangent
      if (!isValid(lcand, basel) && !isValid(rcand, basel))
        break;

      // The next cross edge is to be connected to either lcand->Dest2d() or
      // rcand->Dest2d() If both are valid, then choose the appropriate one
      // using the InCircle test
      if (!isValid(lcand, basel) ||
          (isValid(rcand, basel) &&
           insideCircle(rcand->Dest2d(), lcand->Dest2d(), lcand->Org2d(),
                        rcand->Org2d()))) {
        // Add cross edge basel from rcand->Dest2d() to basel->Dest2d()
        basel = connect(rcand, basel->Sym());
      } else {
        // Add cross edge base1 from basel->Org() to lcand->->Dest2d()
        basel = connect(basel->Sym(), lcand->Sym());
      }

    } while (true);

    o_left = ldo;
    o_right = rdo;

    return;
  }
}

void DivideConquer::computeTriangulation(std::vector<Point2f> &a_stars_system) {

  // Sort points front left-to-right, then up-down only if X==Y
  std::sort(a_stars_system.begin(), a_stars_system.end(),
            [](const Point2f &a, const Point2f &b) {
              if (a.x == b.x)
                return (a.y > b.y);
              return a.x < b.x;
            });

  m_ordered_points.reserve(a_stars_system.size());
  // remove repeated:
  // worst: all equals? -> O(n), all diff -> O(2*n) = O(n)
  for (int i(0); i < a_stars_system.size(); i++) {

    const Point2f &p = a_stars_system[i];
    m_ordered_points.push_back(p);

    for (int j(i + 1); j < a_stars_system.size(); j++) {
      const Point2f &q = a_stars_system[j];
      // not same
      if (fabsf(q.x - p.x) != 0 || fabsf(q.y - p.y) != 0) {
        break;
      }
      // found similar: skip!
      i++;
    }
  }

  // Reserve the best case:
  // #edges = (3* #triangles+b)/2 = (6*n-3*b-6+b) /2
  // #edges = 3*n-b-3 => most edge = 3*n-3-3
  m_quad_edges.reserve(3 * m_ordered_points.size() - 6);
  // Divide and Conquer:
  // https://dl.acm.org/doi/pdf/10.1145/282918.282923
  Edge *oleft;
  Edge *oright;
  recursiveDelaunay(oleft, oright, 0, m_ordered_points.size() - 1);
  return;
}

/****************** Kruksal ******************/

int findCluster(int idx, const std::vector<int> &a_cluster) {
  if (idx == a_cluster[idx]) {
    return idx;
  } else {
    return findCluster(a_cluster[idx], a_cluster);
  }
}
float DivideConquer::computeKruskalMinD(std::vector<Edge *> &a_solution) {
  // init
  float min_d = 0;

  // generate unique ids for set for each node
  std::vector<int> cluster_id;
  cluster_id.resize(m_num_nodes);
  std::iota(std::begin(cluster_id), std::end(cluster_id), 0);

  // get only alive edges (edges are easier to sort)
  std::vector<Edge *> valid_edges;
  valid_edges.reserve(m_quad_edges.size() - m_num_deleted_edges);
  for (const auto &i : m_quad_edges) {
    if (i->alive) {
      valid_edges.push_back(i->e);
    }
  }

  // sort valid edges by lenght
  std::sort(valid_edges.begin(), valid_edges.end(), [](Edge *i, Edge *j) {
    return (i->getQuadEdge()->lenght < j->getQuadEdge()->lenght);
  });

  // loop all edges
  int node_orig_set = -1;
  int node_dest_set = -1;
  for (int i = 0; i < valid_edges.size(); i++) {

    // find id of node's set/cluster
    node_orig_set = findCluster(valid_edges[i]->Org().id, cluster_id);
    node_dest_set = findCluster(valid_edges[i]->Dest().id, cluster_id);

    // not in same set already => add
    if (node_orig_set != node_dest_set) {
      a_solution.push_back(valid_edges[i]);

      // union two diffeHelperrent set of points
      cluster_id[node_orig_set] = cluster_id[node_dest_set];

      // take minimum distance
      if (valid_edges[i]->getQuadEdge()->lenght > min_d) {
        min_d = valid_edges[i]->getQuadEdge()->lenght;
      }

      // already connected all edges between nodes
      if (a_solution.size() == m_num_nodes - 1) {
        break;
      }
    }
  }

  // return real dist
  return std::sqrt(min_d);
}
