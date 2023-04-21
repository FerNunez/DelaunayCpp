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

inline Point2f operator-(const Point2f &L, const Point2f &R) {
  return Point2f(L) -= R;
}

inline float lenghtSquared(const Point2f &a, const Point2f &b) {
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

/*************** Geometric Predicates for Delaunay Diagrams *****************/
inline float computeArea(const Point2f &a, const Point2f &b, const Point2f &c)
// Returns twice the area of the oriented triangle (a, b, c), i.e., the
// area is positive if the triangle is oriented counterclockwise.
{
  return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

/*********************** DivideConquer *************************************/

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
