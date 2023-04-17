/*
 * dop.h
 *
 *  Created on: Apr 30, 2016
 *      Author: Robert Penicka, Petr Vana
 */

#pragma once

// using namespace crl;
#include "HeuristicTypes.h"
#include "MathCommon.h"
using namespace crl;
#include "crl/logging.h"
#include "opendubins/dubins.h"
#include <tuple>

#define OUTPUT_DEFAULT "\033[0m"
#define OUTPUT_BLACK "\033[30m"
#define OUTPUT_RED "\033[31m"
#define OUTPUT_GREEN "\033[32m"
#define OUTPUT_YELLOW "\033[33m"
#define OUTPUT_BLUE "\033[34m"
#define OUTPUT_MAGENTA "\033[35m"
#define OUTPUT_CYAN "\033[36m"
#define OUTPUT_WHITE "\033[37m"

#define INFO_RED(x) INFO(OUTPUT_RED << x << OUTPUT_DEFAULT)
#define INFO_YELLOW(x) INFO(OUTPUT_YELLOW << x << OUTPUT_DEFAULT)
#define INFO_MAGENTA(x) INFO(OUTPUT_MAGENTA << x << OUTPUT_DEFAULT)
#define INFO_CYAN(x) INFO(OUTPUT_CYAN << x << OUTPUT_DEFAULT)
#define INFO_GREEN(x) INFO(OUTPUT_GREEN << x << OUTPUT_DEFAULT)
#define INFO_WHITE(x) INFO(OUTPUT_WHITE << x << OUTPUT_DEFAULT)
#define INFO_BLUE(x) INFO(OUTPUT_BLUE << x << OUTPUT_DEFAULT)
#define INFO_BLACK(x) INFO(OUTPUT_BLACK << x << OUTPUT_DEFAULT)

#define VARIABLE_STR(s) #s
#define STR(s) VARIABLE_STR(s)
#define ROVNASE1(X) X =
#define ROVNASE(X) ROVNASE1(X)

#define INFO_VAR(x) INFO(STR(x) << " = " << x)
#define INFO_VAR_RED(x) INFO_RED(STR(x) << " = " << x)

typedef struct StartGoalNodes {
  GraphNode start;
  GraphNode goal;
} StartGoalNodes;

typedef struct MutliStartNodes {
  std::vector<GraphNode> start;
} MutliStartNodes;

typedef struct NodeIDWithRatio {
  double ratio;
  double nodeCost;
  int id;
} NodeIDWithRatio;

typedef struct NeighAngDist {
  NeighAngDist() {
    idNeigh = -1;
    idAng = -1;
    distance = 0;
  }
  NeighAngDist(int idNeigh_, int idAng_, double distance_) {
    idNeigh = idNeigh_;
    idAng = idAng_;
    distance = distance_;
  }
  int idNeigh;
  int idAng;
  double distance;
} NeighAngDist;

typedef struct DubinsWithIds {
  Dubins dubins;
  int idNeigh;
  int idAng;
} DubinsWithIds;

typedef struct NeighAngValuesIds {
  int idNeigh;
  int idAng;
  double ang;
  double neigh_ang;
  GraphNode node;
} NeighAngIds;

typedef struct NeighImprovement {
  double improvementLength;
  double neigh_ang;
  GraphNode nodeInNeigh;
} NeighImprovement;

typedef struct SimpleTryResult {
  double actual_len;
  double maximal_len;
} SimpleTryResult;

namespace opendubins {

// Class for the Dubins Orienteering Problem
// There is are two fixed positions - start and end
// Departure and termination headings are not given
// unlimited number of points can be inserted between start and end
class DOPN {
public:
  DOPN() {}

  static void calcAllDistances(std::vector<GraphNode> allGraphNodes,
                               double radius, int resolution,
                               double neighborhood_radius,
                               int neighborhood_resolution,
                               bool null_start_goal_radius, int startIndex,
                               int goalIndex);
  static std::vector<std::vector<std::vector<GraphNode_AngNeigh>>> *
  getAllSamples();

  std::vector<GraphNode> getAllTargets();
  static void checkDistancesAndSamples(double radius);
  static void checkEucllideanDistances();

  DOPN(GraphNode start, GraphNode end, double radius, int resolution,
       double neighborhood_radius, int neighborhood_resolution,
       bool null_start_goal_radius_);

  virtual ~DOPN();

  // returns number of Dubins maneuvers (targets.size() + 1)
  int getSize() const;

  // returns all Dubins maneuver in the shortest found path
  std::vector<Dubins> getPath() const;

  std::vector<NeighAngValuesIds> getPathNeighAngIds() const;

  // returns only the distance of the shortest found path
  // it schould be slightly faster than using getPath() and sum the distance
  double getPathLength() const;

  // add new target to specific position
  // example: addPoint(newPoint, 0)
  // before:  start > t1 > end
  // after:   start > newPoint > t1 > end
  void addPoint(GraphNode p, int idx);

  // add new targets to specific position
  // example: addPoint(points, 0)
  // before:  start > t1 > end
  // after:   start > points[0] >  points[last] > t1 > end
  void addPoint(std::vector<GraphNode> points, int idx);

  // add point to the best position
  void addPoint(GraphNode p);

  // remove one target with specific index
  // idx is in intervals [0, getSize()-2] = [0, getNumTargets()-1] = [0,
  // targets.size()]
  void removePoint(int idx);

  // remove targets starting with idxStart and ending with idxStop
  void removePoint(int idxStart, int idxStop);

  // same as addPoint but it does not insert the point
  // idx is in intervals [0, getSize()] = [0, targets.size()+1]
  // return length of new path
  SimpleTryResult tryToAdd(GraphNode p, int idx) const;

  double tryToAdd(std::vector<GraphNode> p, int idx) const;

  // replaces single target at idx with GraphNode p
  SimpleTryResult tryToReplace(GraphNode p, int idx,
                               int numToReplace = 1) const;

  // replaces single target at idx with vector of targets p
  double tryToReplace(std::vector<GraphNode> p, int idx) const;

  // returns length of path with exchanged idx1 and idx2
  double tryToExchange(int idx1, int idx2);

  // returns length of path with moved idxWhatToMove to idxWhereToMove
  double tryToMove(int idxWhatToMove, int idxWhereToMove);

  double tryToMoveEuclidean(int idxWhatToMove, int idxWhereToMove);
  // same as removePoint but it does not remove the point
  // idx is in intervals [0, getSize()-2] = [0, getNumTargets()-1] = [0,
  // targets.size()] return length of new path
  double tryToRemove(int idx) const;

  double tryToRemove(int idxStart, int idxStop) const;

  // remove the worst point
  void removeOne();

  NeighImprovement optimizeNeighborhoodPosition(
      int targetId, std::vector<double> actualNeighAngles,
      std::vector<NeighAngValuesIds> originalNeighAngIds,
      double minimal_improvement_distance);

  double optimizeHeading(int targetId, double minimal_improvement_distance);

  void insertAngSample(int inSampleId, int neighId, int angIdActual,
                       int angIdWhereInsert, GraphNode_AngNeigh editing_pos);
  void insertNeighSample(int inSampleId, int neighIdActual,
                         int neighIdWhereInsert, double newAngle,
                         GraphNode node);

  inline double neighborhoodAngDistanceAfter(GraphNode centroid,
                                             State editingState, double new_ang,
                                             State previousState,
                                             State nextState);
  inline double angDistanceAfter(State editingState, double new_ang,
                                 State previousState, State nextState);

  // test 2 opt operation between idx1 and idx2
  // idxFrom and idxTo are in intervals [0, getNumTargets()-1] = [0,
  // targets.size()-1] the path idxFrom - idxTo is inserted in reverse mode
  // return length of new path
  // reverse path between idx1+1 and idx2
  double tryTwoOpt(int idxFrom, int idxTo);

  // performs the two opt
  // idxFrom and idxTo are in intervals [0, getNumTargets()-1] = [0,
  // targets.size()-1] the path idxFrom - idxTo is inserted in reverse mode
  void twoOpt(int idxFrom, int idxTo);

  // return number of targets - number of nodes between start a and end
  int getNumTargets();

  // returns target at position idx - idx in intervals [0, targets.size-1]
  GraphNode getTarget(int idx);

  // return target subvector
  std::vector<GraphNode> getTargets(int idxFrom, int idxTo);

  // return samples
  std::vector<std::vector<std::vector<GraphNode_AngNeigh>>> getSamples();
  // return number of samples - number of nodes in path totally = with start and
  // end
  int getNumSamples();

  // prints out ids of nodes
  void listIds();

  std::vector<State> getPathSampled(double samplePath);
  /*
   std::vector<State> getPathSampled(double initialSpeed, double maximal_speed,
   double maximal_acceleration, double time_sample_rate, double finalSpeed);
   */
  // get begin and end of target vectors
  std::vector<GraphNode>::iterator targetsBegin();
  std::vector<GraphNode>::iterator targetsEnd();

  void update();

  double getReward();
  // void evaluateUsage();

  static std::vector<
      std::vector<std::vector<std::vector<std::vector<std::vector<double>>>>>>
      allDistances;
  // static
  // std::vector<std::vector<std::vector<std::vector<std::vector<std::vector<int>>>>>>
  // allDistancesUsage;
  static std::vector<std::vector<std::vector<GraphNode_AngNeigh>>> allSamples;

protected:
  // void initializeAllNodes();

  typedef std::vector<std::vector<std::vector<double>>> dst_type;
  typedef std::vector<std::vector<double>> dst_between_two_type;
  // typedef std::tuple<int, int, double> dst_triple;
  // typedef std::pair<int, double> dst_pair;
  typedef std::vector<std::vector<std::vector<NeighAngDist>>>
      dop_3d_matrix_triple;
  typedef std::vector<std::vector<NeighAngDist>> dop_2d_matrix_triple;
  typedef std::vector<NeighAngDist> dop_vector_triple;

  // int getInverseId(int id);

  static void
  generateSamples(std::vector<std::vector<GraphNode_AngNeigh>> &samples,
                  GraphNode p, double resolution, double neighborhood_radius,
                  int neighborhood_resolution, bool null_start_goal_radius,
                  int startIndex, int goalIndex);

  void updateAfterInsert(int idxStart, int idxEnd);
  void updateAfterRemove(int idxStart, int idxEnd);
  bool null_start_goal_radius;
  double radius;
  int resolution;
  double neighborhood_radius;
  int neighborhood_resolution;
  GraphNode start;
  GraphNode end;

  std::vector<GraphNode> targets;

  // all samples
  // [1st index] - choose point/target
  // [2nd index] - choose neighborhood point in target
  // [3nd index] - choose state (sample) for this neighborhood
  std::vector<std::vector<std::vector<GraphNode_AngNeigh>>> samples;

  // matrix
  //    1) from city (layer)
  //    2) from sample i
  //    3) to sample j (destination is evident - the next city in sequence)
  // size is N x k_i x k_j where k_i is number of intervals
  // dst_type distances;

  // find all shortest paths
  // matrix:
  //    1) to city (layer)
  //    2) to neighborhood (layer)
  //    3) to idx i
  dop_3d_matrix_triple shortest;

  // find all shortest paths - backward path
  // matrix:
  //    1) to city (layer)
  //    2) to neighborhood (layer)
  //    3) to idx i
  dop_3d_matrix_triple shortest_back;

  /*
   these variables are used to store the all nodes precomputed values
   */
  // std::vector<GraphNode> allNodes;
  // shortest matrix for all nodes - precompute if needed
  // layers:
  // 1) from city
  // 2) to city
  // 3) from idx
  // 4) to idx
  // std::vector<std::vector<std::vector<std::vector<double>>>> distancesAll;
  // indicate whether the shortest between
  // 1) from city
  // 2) to city
  // were calculated and stored to shortest_all_points
  // std::vector<std::vector<bool>> computedShortest;
  // all samples
  // [1st index] - choose point/target
  // [2nd index] - choose state (sample) for this point
  // std::vector<std::vector<GraphNode_AngNeigh>> samplesAll;
};

} // namespace opendubins
