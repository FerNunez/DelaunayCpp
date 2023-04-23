#pragma once
#include <algorithm>
#include <iostream>
#include <math.h>
#include <memory>
#include <numeric>
#include <tuple>
#include <vector>

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

// computes squared euclidean distance between two points
inline float lenghtSquared(const Point2f &a, const Point2f &b) {
  auto v = a - b;
  return v.x * v.x + v.y * v.y;
}
/************************** Helper end ************************/

/*!
 * \brief The Node class containing position and id
 */
struct Node {

  Node(){};
  Node(const Point2f &d, int idx) : pos(d), id(idx){};

  Point2f pos; // node's position
  int id;      // nodes id = used as index later
};

class QuadEdge;

class Edge {

public:
  int index;  // its index of edge inside a quad-edge
  Edge *next; // points to next edge
  Node node;  // node of origin for each edge

public:
  Edge() {}

  // Basic four operators
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
  // Return the ccw edge around the left face following the current edge.
  inline Edge *Lnext() { return invRot()->Onext()->Rot(); }
  // Return the edge around the right face ccw before the current edge.
  inline Edge *Rprev() { return Sym()->Onext(); }

  // return origin node
  inline Node Org() { return node; }
  // return destination node
  inline Node Dest() { return Sym()->node; }
  // return origin node position
  const Point2f &Org2d() const { return node.pos; }
  // return dest node position
  const Point2f &Dest2d() const {
    return (index < 2) ? ((this + 2)->node.pos) : ((this - 2)->node.pos);
  }

  /*!
   * \brief Sets dest and origin nodes of edge
   */
  void setEndPoints(const Node &ori, const Node &de);

  /*!
   * \brief Get pointer to father quadEdge
   * \return pointer to father quadEdge of edge
   */
  QuadEdge *getQuadEdge() { return (QuadEdge *)(this - index); }
};

/*!
 * \brief The QuadEdge class has 4 representation of an 'edge'
 */
class QuadEdge {
public:
  QuadEdge();

  Edge e[4];    // array containing 2 normal and 2 faces edges
  float lenght; // lenght squared
  bool alive;   // true if edge was 'removed'
};

// Returns twice the area of the oriented triangle (a, b, c)
inline float computeArea(const Point2f &a, const Point2f &b, const Point2f &c) {
  return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

/*********************** DivideConquer *************************************/

/*!
 * \brief The DivideConquer class applies Delaunay Triangulation Divide&Conquer
 */
class DivideConquer {
  // private:
public:
  DivideConquer(){};

  /*!
   * \brief Computes Divide&Conquer DelaunayTriang from 2d points
   * \param stars_system vector float of 2d points
   */
  void computeTriangulation(std::vector<Point2f> &a_stars_system);

  /*!
   * \brief Computes Kruskal on triangulation and outputs minimum d and graph
   * \param esmt_solution vector of edges for triangulation on Delaunay
   * \return
   */
  float computeKruskalMinD(std::vector<Edge *> &esmt_solution);

  /*!
   * \brief vector of edges for triangulation. Smaller & used more than quad
   */
  std::vector<Edge *> m_edges;

  /*!
   * \brief number of nodes added to Delaunay
   */
  int m_num_nodes = 0;

  /*!
   * \brief number of edges 'deleted'
   */
  int m_num_deleted_edges = 0;

private:
  // Computes recursive Delaunay Triangulation
  void recursiveDelaunay(Edge *&left, Edge *&right, int left_idx,
                         int right_idx);

  // creates an edge (and its quad edge)
  Edge *makeEdge();
  // creates an edge (and its quad edge) from node
  Edge *makeEdgeFrom(const Node &ori, const Node &de);
  // creates an edge connecting a and b
  Edge *connect(Edge *a, Edge *b);
  //
  void deleteEdge(Edge *e);

  // TODO: improve. used for smart ptr life in class
  std::vector<std::shared_ptr<QuadEdge>>
      m_quad_edges; // vector of all memory created quad edges
  std::vector<Point2f> m_ordered_points; // unique ordered points
};

/*********** Operators for Data Structure *************/
// Checks if
int insideCircle(const Point2f &p, const Point2f &a, const Point2f &b,
                 const Point2f &c);

// Returns TRUE if the points a, b, c are in a counterclockwise order
int ccw(const Point2f &a, const Point2f &b, const Point2f &c);

// Check if p is right or left of edge e
int rightOf(const Point2f &p, Edge *e);
int leftOf(const Point2f &p, Edge *e);

// Checks whether the edge e is above basel
bool isValid(Edge *e, Edge *basel);

// operator for split and connect edges
void Splice(Edge *a, Edge *b);

/*!
 * \brief Find cluster id (parent of all clusters)
 * \param index of a node
 * \param vector of ids of clusters
 * \return int id of cluster for node index
 */
int findCluster(int idx, const std::vector<int> &a_cluster);
