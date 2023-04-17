/*
 * HeuristicTypes.h
 *
 *  Created on: Feb 22, 2016
 *      Author: Robert Penicka
 */

#ifndef SRC_FOURPHASEHEURISTIC_HEURISTICTYPES_H_
#define SRC_FOURPHASEHEURISTIC_HEURISTICTYPES_H_

#include "crl/logging.h"
#include <cmath>
#include <iomanip>
#include <iostream> // std::cout
#include <limits>
#include <vector>

#include "opendubins/point.h"
#include "opendubins/state.h"

using namespace opendubins;

typedef struct GraphNode {

  GraphNode() {
    x = 0;
    y = 0;
    reward = 0;
    id = 0;
  }

  GraphNode(double x_, double y_, double price_, unsigned int id_) {
    x = x_;
    y = y_;
    reward = price_;
    id = id_;
  }
  std::vector<double> toVector() {
    std::vector<double> vector;
    vector.push_back(x);
    vector.push_back(y);
    vector.push_back(reward);
    return vector;
  }
  Point toPoint() { return Point(this->x, this->y); }
  double distanceTo(GraphNode gn1) {
    double diffx = gn1.x - this->x;
    double diffy = gn1.y - this->y;
    return sqrt(diffx * diffx + diffy * diffy);
  }
  double x;
  double y;
  double reward;
  unsigned int id;
} GraphNode;

typedef struct GraphNode_AngNeigh {
  GraphNode_AngNeigh() {
    node = GraphNode();
    ang = 0;
  }

  GraphNode_AngNeigh(GraphNode node_, double ang_, double neigh_ang_) {
    node = node_;
    ang = ang_;
    neigh_ang = neigh_ang_;
  }

  State toState() { return State(node.toPoint(), ang); }

  GraphNode node;
  double ang;
  double neigh_ang;
} GraphNode_Ang;

typedef struct Tour {
  Tour() {
    this->reward = 0;
    this->length = 0;
  }
  Tour(double reward_, double length_, std::vector<GraphNode> tour_) {
    this->reward = reward_;
    this->length = length_;
    this->tour = tour_;
  }
  double reward;
  double length;
  std::vector<GraphNode> tour;
  std::vector<Point> getPointTour() {
    std::vector<Point> pointTour;
    for (int var = 0; var < tour.size(); ++var) {
      pointTour.push_back(tour[var].toPoint());
    }
    return pointTour;
  }
} Tour;

typedef struct DatasetOP {
  std::vector<GraphNode> graph;
  double Tmax;    //= available time budget per path
  unsigned int P; //= number of paths (=1)
  unsigned int startID;
  unsigned int goalID;
} datasetOP;

typedef struct InsertionHistoryRecord {
  InsertionHistoryRecord(std::vector<GraphNode> graph_,
                         GraphNode insertionNode_) {
    this->graph = graph_;
    this->insertionNode = insertionNode_;
  }
  std::vector<GraphNode> graph;
  GraphNode insertionNode;
} InsertionHistoryRecord;

bool operator==(const GraphNode &lhs, const GraphNode &rhs);
std::ostream &operator<<(std::ostream &o, GraphNode &p);
bool operator>(GraphNode &dt1, GraphNode &dt2);
bool operator<(GraphNode &dt1, GraphNode &dt2);

class HeuristicTypes {
public:
  HeuristicTypes();
  virtual ~HeuristicTypes();
};

#endif /* SRC_FOURPHASEHEURISTIC_HEURISTICTYPES_H_ */
