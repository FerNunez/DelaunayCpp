#pragma once
#include <algorithm>
#include <cmath>
#include <memory>
#include <numeric>
#include <vector>
/************************** Common functions *******************/
struct int2 {
  int x;
  int y;

  int2(int X, int Y) : x(X), y(Y) {}
  int2() : x(0), y(0) {}
};

struct float2 {

  float x;
  float y;

  float2(float X, float Y) : x(X), y(Y) {}
  float2() : x(0), y(0) {}
  float2(int2 a) : x(float(a.x)), y(float(a.y)) {}

  // operators
  //  friend std::ostream &operator<<(std::ostream &os, const Point2f &v) {
  //    os << "(" << v.x << ", " << v.y << ")";
  //    return os;
  //  }

  friend bool operator==(const float2 &L, const float2 &R) {
    return std::tie(L.x, L.y) == std::tie(R.x, R.y);
  }

  float2 &operator-=(const float2 &v) {
    this->x -= v.x;
    this->y -= v.y;
    return *this;
  }
};

inline float2 operator-(const float2 &L, const float2 &R) {
  return float2(L) -= R;
}

// computes squared euclidean distance between two points
inline float lenghtSquared(const float2 &a, const float2 &b) {
  auto v = a - b;
  return v.x * v.x + v.y * v.y;
}
/************************** Common end ************************/

/*!
 * \brief The Node class containing position and id
 */
struct Node {

  Node(){};
  Node(const float2 &d, int idx) : pos(d), id(idx){};

  float2 pos; // node's position
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
  const float2 &Org2d() const { return node.pos; }
  // return dest node position
  const float2 &Dest2d() const {
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
  // Constructor will init all Edges
  QuadEdge();

  Edge e[4];    // array containing 2 normal and 2 faces edges
  float lenght; // lenght squared
  bool alive;   // true if edge was 'removed'
};

/*********************** DivideConquer *************************************/

/*!
 * \brief The DivideConquer class applies Delaunay Triangulation Divide&Conquer
 */
class DivideConquer {
public:
  DivideConquer(){};

  /*!
   * \brief Computes Divide&Conquer DelaunayTriang from 2d points
   * \param stars_system vector float of 2d points
   */
  void computeTriangulation(std::vector<float2> const &a_stars_system);

  /*!
   * \brief Computes Kruskal on triangulation and outputs minimum d and graph
   * \param esmt_solution vector of edges for triangulation on Delaunay
   * \return
   */
  float computeKruskalMinD(std::vector<Edge *> &esmt_solution);

private:
  /*!
   * \brief Computes recursive Delaunay Triangulation
   * \param left output pointer edge of most left edge of triangulation
   * \param right output pointer edge of most right edge of triangulation
   * \param left_idx input index limiting left side of vector to triangulate
   * \param right_idx input index limiting right side of vector to triang
   */
  void recursiveDelaunay(Edge *&left, Edge *&right, int left_idx,
                         int right_idx);

  // creates an edge (and its quad edge)
  Edge *makeEdge();
  // creates an edge (and its quad edge) from node
  Edge *makeEdgeFrom(const Node &ori, const Node &de);
  // creates an edge connecting a and b
  Edge *connect(Edge *a, Edge *b);
  // disconect edge from triangulation
  void disconnectEdge(Edge *e);
  // void deleteEdge(Edge *e);

private:
  // vector of all memory created quad edges
  std::vector<std::shared_ptr<QuadEdge>> m_quad_edges;
  // unique ordered points
  std::vector<float2> m_ordered_points;
  // number of nodes added to Delaunay
  int m_num_nodes = 0;
  // number of edges 'deleted'
  int m_num_deleted_edges = 0;
};

/*********** Operators for Data Structure *************/
// Returns twice the area of the oriented triangle (a, b, c)
inline float computeArea(const float2 &a, const float2 &b, const float2 &c) {
  return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}
// Return true if point P inside triangle abc
int insideCircle(const float2 &p, const float2 &a, const float2 &b,
                 const float2 &c);

// Returns true if the points a, b, c are in a counterclockwise order
int ccw(const float2 &a, const float2 &b, const float2 &c);

// True if p is right of edge e
int rightOf(const float2 &p, Edge *e);

// True if p is left of edge e
int leftOf(const float2 &p, Edge *e);

// Checks whether the edge e is above basel
bool isValid(Edge *e, Edge *basel);

// operator for split and connect edges
void Splice(Edge *a, Edge *b);

/*** for Kruskal ****/
/*!
 * \brief Find cluster id (parent of all clusters) recursively
 * \param index of a node
 * \param vector of ids of clusters
 * \return int id of cluster for node index
 */
int findCluster(int idx, const std::vector<int> &a_cluster);
