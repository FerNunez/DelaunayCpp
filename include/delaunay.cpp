#include "delaunay.h"

/************ Data Structure *************/
QuadEdge::QuadEdge() : alive(true) {

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

void Edge::EndPoints(Node ori, Node de) {
  node = ori;
  Sym()->node = de;

  this->Qedge()->lenght = lenghtSquared(ori.pos, de.pos);
}

/*********************** Basic Topological Operators ************************/
Edge *MakeEdge() {
  QuadEdge *ql = new QuadEdge;
  return ql->e;
}

Edge *MakeEdgeFrom(Node ori, Node de) {
  Edge *e = MakeEdge();
  e->EndPoints(ori, de);
  return e;
}
void Splice(Edge *a, Edge *b)
// This operator affects the two edge rings around the origins of a and b,
// and, independently, the two edge rings around the left faces of a and b.
// In each case, (i) if the two rings are distinct, Splice will combine
// them into one; (ii) if the two are the same ring, Splice will break it
// into two separate pieces.
// Thus, Splice can be used both to attach the two edges together, and
// to break them apart. See Guibas and Stolfi (1985) p.96 for more details
// and illustrations.
{
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
void DeleteEdge(Edge *e) {
  e->Qedge()->alive = false;
  Splice(e, e->Oprev());
  Splice(e->Sym(), e->Sym()->Oprev());
  // delete e->Qedge();
}

Edge *Connect(Edge *a, Edge *b)
// Add a new edge e connecting the destination of a to the
// origin of b, in such a way that all three have the same
// left face after the connection is complet e.
// Additionally, the data pointers of the new edge are set.
{
  Edge *e = MakeEdge();
  Splice(e, a->Lnext());
  Splice(e->Sym(), b);
  e->EndPoints(a->Dest(), b->Org());
  e->Qedge()->lenght = lenghtSquared(a->Dest().pos, b->Org().pos);

  return e;
}
void Swap(Edge *e)
// Essentially turns edge e counterclockwise inside its enclosing
// quadrilateral. The data pointers are modified accordingly.
{
  Edge *a = e->Oprev();
  Edge *b = e->Sym()->Oprev();
  Splice(e, a);
  Splice(e->Sym(), b);
  Splice(e, a->Lnext());
  Splice(e->Sym(), b->Lnext());
  e->EndPoints(a->Dest(), b->Dest());
}

int insideCircle(const Point2f &a, const Point2f &b, const Point2f &c,
                 const Point2f &d)
// Returns TRUE if the point d is inside the circle defined by the
// points a, b, c. See Guibas and Stolfi (1985) p.107.
{
  return (a.x * a.x + a.y * a.y) * computeArea(b, c, d) -
             (b.x * b.x + b.y * b.y) * computeArea(a, c, d) +
             (c.x * c.x + c.y * c.y) * computeArea(a, b, d) -
             (d.x * d.x + d.y * d.y) * computeArea(a, b, c) >
         0;
}
int ccw(const Point2f &a, const Point2f &b, const Point2f &c)
// Returns TRUE if the points a, b, c are in a counterclockwise order
{
  return (computeArea(a, b, c) > 0);
}
int RightOf(const Point2f &x, Edge *e) {
  return ccw(x, e->Dest2d(), e->Org2d());
}
int LeftOf(const Point2f &x, Edge *e) {
  return ccw(x, e->Org2d(), e->Dest2d());
}

DivideConquer::DivideConquer(std::vector<Point2f> &v) {
  // Sort in X, then on Y only if X==Y
  std::sort(v.begin(), v.end(), [](const Point2f &a, const Point2f &b) {
    if (a.x == b.x)
      return (a.y < b.y);
    return a.x < b.x;
  });

  // remove repeated: all equals? -> O(n), all diff -> O(2*n) = O(n)
  for (int i(0); i < v.size(); i++) {

    const Point2f &p = v[i];
    input.push_back(p);

    for (int j(i + 1); j < v.size(); j++) {
      const Point2f &q = v[j];
      // not same
      if (abs(q.x - p.x) > EPS || abs(q.y - p.y) > EPS) {
        break;
      }
      // found similar: skip!
      i++;
    }
  }
  v.clear();
  v = input;
}

DivideConquer::~DivideConquer() {
  for (auto p : edges_stack) {
    delete p;
  }
  edges_stack.clear();
}
// left: most left edge
// right: most right edge
void DivideConquer::delaunay(Edge *&o_left, Edge *&o_right, int left_idx,
                             int right_idx) {
  // starts calling
  // delaunay(o_left, o_right, 0, point_size-1)
  //
  // only 2 points
  auto numb_points = 1 + right_idx - left_idx;
  if (numb_points == 2) {
    Node a(input[left_idx]);
    a.id = connected_nodes++;
    Node b(input[right_idx]);
    b.id = connected_nodes++;

    Edge *e = MakeEdgeFrom(a, b);

    o_left = e;
    o_right = e->Sym();

    edges_stack.push_back(e);
    return;
  }
  // abc
  else if (numb_points == 3) {

    Node a(input[left_idx]);
    a.id = connected_nodes++;
    Node b(input[left_idx + 1]);
    b.id = connected_nodes++;
    Node c(input[right_idx]);
    c.id = connected_nodes++;

    Edge *ab = MakeEdgeFrom(a, b);
    Edge *bc = MakeEdgeFrom(b, c);
    Splice(ab->Sym(), bc);

    // c.y < b.y
    if (ccw(input[left_idx], input[left_idx + 1], input[right_idx])) {
      Edge *c = Connect(bc, ab);
      o_left = ab;
      o_right = bc->Sym();

      edges_stack.push_back(c);

    } else if (ccw(input[left_idx], input[right_idx], input[left_idx + 1])) {
      Edge *c = Connect(bc, ab);
      o_left = c->Sym();
      o_right = c;

      edges_stack.push_back(c);

    }
    // colinear
    else {
      o_left = ab;
      o_right = bc->Sym();
    }

    edges_stack.push_back(ab);
    edges_stack.push_back(bc);
    return;
  }
  // more than 3 points => recursive
  else {
    int lenght_half = numb_points / 2;
    Edge *leftleft;
    Edge *leftright;
    delaunay(leftleft, leftright, left_idx, left_idx + lenght_half - 1);

    Edge *rightleft;
    Edge *rightright;
    delaunay(rightleft, rightright, left_idx + lenght_half, right_idx);

    // Lower common tangets
    do {
      if (LeftOf(rightleft->Org2d(), leftright)) {
        leftright = leftright->Lnext();
      } else if (RightOf(leftright->Org2d(), rightleft)) {
        rightleft = rightleft->Rprev();
      } else {
        break;
      }
    } while (true);

    Edge *basel = Connect(rightleft->Sym(), leftright);
    if (leftright->Org2d() == leftleft->Org2d()) {
      leftleft = basel->Sym();
    }
    if (rightleft->Org2d() == rightright->Org2d()) {
      rightright = basel;
    }
    edges_stack.push_back(basel);

    // merge
    do {
      // Locate the first L point (lcand->Dest2d()) to be encountered by the
      // rising bubble, and delete L edges out of base1->Dest2d() that fail the
      // circle test.
      Edge *lcand = basel->Sym()->Onext();
      if (RightOf(lcand->Dest2d(), basel)) {
        while (insideCircle(basel->Dest2d(), basel->Org2d(), lcand->Dest2d(),
                            lcand->Onext()->Dest2d())) {
          Edge *t = lcand->Onext();
          DeleteEdge(lcand);
          lcand = t;
        }
      }

      // Symmetrically, locate the first R point to be hit, and delete R edges
      Edge *rcand = basel->Oprev();
      if (RightOf(rcand->Dest2d(), basel)) {
        while (insideCircle(basel->Dest2d(), basel->Org2d(), rcand->Dest2d(),
                            rcand->Oprev()->Dest2d())) {
          Edge *t = rcand->Oprev();
          DeleteEdge(rcand);
          rcand = t;
        }
      }

      // If both lcand and rcand are invalid, then basel is the upper common
      // tangent
      if (!RightOf(lcand->Dest2d(), basel) && !RightOf(rcand->Dest2d(), basel))
        break;

      // The next cross edge is to be connected to either lcand->Dest2d() or
      // rcand->Dest2d() If both are valid, then choose the appropriate one
      // using the InCircle test
      if (!RightOf(lcand->Dest2d(), basel) ||
          (RightOf(rcand->Dest2d(), basel) &&
           insideCircle(lcand->Dest2d(), lcand->Org2d(), rcand->Org2d(),
                        rcand->Dest2d()))) {
        // Add cross edge basel from rcand->Dest2d() to basel->Dest2d()
        basel = Connect(rcand, basel->Sym());
      } else {
        // Add cross edge base1 from basel->Org() to lcand->->Dest2d()
        basel = Connect(basel->Sym(), lcand->Sym());
      }
      edges_stack.push_back(basel);

    } while (true);

    o_left = leftleft;
    o_right = rightright;

    return;
  }
}
