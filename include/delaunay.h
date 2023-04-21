#pragma once
#include <algorithm>
#include <iostream>
#include <math.h>
#include <tuple>
#include <vector>

#define EPS 0.0001

/************************** Helper functions *******************/
struct Point2d {
  int x;
  int y;

  Point2d(int X, int Y) : x(X), y(Y) {}
  Point2d() : x(0), y(0) {}
};

struct Point2f {

  float x;
  float y;

  Point2f(float X, float Y) : x(X), y(Y) {}
  Point2f() : x(0), y(0) {}
  Point2f(Point2d a) : x(float(a.x)), y(float(a.y)) {}

  // operators
  friend std::ostream &operator<<(std::ostream &os, const Point2f &v) {
    os << "(" << v.x << ", " << v.y << ")";
    return os;
  }

  friend bool operator==(const Point2f &L, const Point2f &R) {
    return std::tie(L.x, L.y) == std::tie(R.x, R.y);
  }

  Point2f &operator-=(const Point2f &v) {
    this->x -= v.x;
    this->y -= v.y;
    return *this;
  }
};

Point2f operator-(const Point2f &L, const Point2f &R) {
  return Point2f(L) -= R;
}

float lenghtSquared(const Point2f &a, const Point2f &b) {
  auto v = a - b;
  return v.x * v.x + v.y * v.y;
}
/************************** Helper end ************************/

// ************************* Delaunay Triangulation ******************

struct Node {

  Node(){};
  Node(const Point2f &d) : data(d), id(-1){};
  Point2f data;
  int id;
};

class QuadEdge;

class Edge {

public:
  int index; // its index of edge in quad-edge
  Edge *next;
  Node node;

public:
  Edge() {}
  // Return the dual of the current edge, directed from its right to its left.
  inline Edge *Rot() { return (index < 3) ? this + 1 : this - 3; }
  // Return the dual of the current edge, directed from its left to its right.
  inline Edge *invRot() { return (index > 0) ? this - 1 : this + 3; }
  // Return the edge from the destination to the origin of the current edge.
  inline Edge *Sym() { return (index < 2) ? this + 2 : this - 2; }
  // Return the next ccw edge around (from) the origin of the current edge.
  inline Edge *Onext() { return next; }
  // Return the next cw edge around (from) the origin of the current edge.
  inline Edge *Oprev() { return Rot()->Onext()->Rot(); }
  // Return the next ccw edge around (into) the destination of the current edge.
  inline Edge *Dnext() { return Sym()->Onext()->Sym(); }
  // Return the next cw edge around (into) the destination of the current edge.
  inline Edge *Dprev() { return invRot()->Onext()->invRot(); }
  // Return the ccw edge around the left face following the current edge.
  inline Edge *Lnext() { return invRot()->Onext()->Rot(); }
  // Return the ccw edge around the left face before the current edge.
  inline Edge *Lprev() { return Onext()->Sym(); }
  // Return the edge around the right face ccw following the current edge.
  inline Edge *Rnext() { return Rot()->Onext()->invRot(); }
  // Return the edge around the right face ccw before the current edge.
  inline Edge *Rprev() { return Sym()->Onext(); }
  inline Node Org() { return node; }
  inline Node Dest() { return Sym()->node; }
  const Point2f &Org2d() const { return node.data; }
  const Point2f &Dest2d() const {
    return (index < 2) ? ((this + 2)->node.data) : ((this - 2)->node.data);
  }

  void EndPoints(Node ori, Node de);

  // to remove
  QuadEdge *Qedge() { return (QuadEdge *)(this - index); }
};

class QuadEdge {

public:
  QuadEdge();

  Edge e[4];
  float lenght_sqrt;
  bool alive;
};

inline QuadEdge::QuadEdge() : alive(true) {

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

inline void Edge::EndPoints(Node ori, Node de) {
  node = ori;
  Sym()->node = de;

  this->Qedge()->lenght_sqrt = lenghtSquared(ori.data, de.data);
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
  e->Qedge()->lenght_sqrt = lenghtSquared(a->Dest().data, b->Org().data);

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
/*************** Geometric Predicates for Delaunay Diagrams *****************/
inline float TriArea(const Point2f &a, const Point2f &b, const Point2f &c)
// Returns twice the area of the oriented triangle (a, b, c), i.e., the
// area is positive if the triangle is oriented counterclockwise.
{
  return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}
int InCircle(const Point2f &a, const Point2f &b, const Point2f &c,
             const Point2f &d)
// Returns TRUE if the point d is inside the circle defined by the
// points a, b, c. See Guibas and Stolfi (1985) p.107.
{
  return (a.x * a.x + a.y * a.y) * TriArea(b, c, d) -
             (b.x * b.x + b.y * b.y) * TriArea(a, c, d) +
             (c.x * c.x + c.y * c.y) * TriArea(a, b, d) -
             (d.x * d.x + d.y * d.y) * TriArea(a, b, c) >
         0;
}
int ccw(const Point2f &a, const Point2f &b, const Point2f &c)
// Returns TRUE if the points a, b, c are in a counterclockwise order
{
  return (TriArea(a, b, c) > 0);
}
int RightOf(const Point2f &x, Edge *e) {
  return ccw(x, e->Dest2d(), e->Org2d());
}
int LeftOf(const Point2f &x, Edge *e) {
  return ccw(x, e->Org2d(), e->Dest2d());
}

bool sortIncremXY(const Point2f &a, const Point2f &b) {
  if (a.x == b.x)
    return (a.y < b.y);
  return a.x < b.x;
}

/*****************************************************************/

class DivideConquer {
  // private:
public:
  DivideConquer(std::vector<Point2f> &);
  ~DivideConquer();

  void delaunay(Edge *&left, Edge *&right, int left_idx, int right_idx);

  std::vector<Edge *> edges_stack;

  int connected_nodes = 0;

private:
  std::vector<Point2f> input;
};

DivideConquer::DivideConquer(std::vector<Point2f> &v) {
  // Sort in X, then on Y only if X==Y
  std::sort(v.begin(), v.end(), sortIncremXY);

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
        while (InCircle(basel->Dest2d(), basel->Org2d(), lcand->Dest2d(),
                        lcand->Onext()->Dest2d())) {
          Edge *t = lcand->Onext();
          DeleteEdge(lcand);
          lcand = t;
        }
      }

      // Symmetrically, locate the first R point to be hit, and delete R edges
      Edge *rcand = basel->Oprev();
      if (RightOf(rcand->Dest2d(), basel)) {
        while (InCircle(basel->Dest2d(), basel->Org2d(), rcand->Dest2d(),
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
           InCircle(lcand->Dest2d(), lcand->Org2d(), rcand->Org2d(),
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
