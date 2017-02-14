/*
 * .h
 *
 *  Created on: 22. 3. 2016
 *      Author: Robert Penicka
 */

#ifndef SRC_VNSDOPN_VNSDOPN_H_
#define SRC_VNSDOPN_VNSDOPN_H_

#include <vector>
#include <list>
#include "logging.h"
#include "HeuristicTypes.h"
#include <cfloat>
#include <limits>
#include "CSort.h"
#include "DatasetLoader.h"
#include "coords.h"

#include <boost/foreach.hpp>
#include <imr_config.h>
#include <imr_algorithm.h>
#include <text_format_loader.h>
#include "random.h"
#include "timerN.h"
#include "perf_timer.h"
#include "logging.h"
#include "stringconversions.h"
#include "nodeloader.h"
#include "text_result_log.h"
#include "imr_file_utils.h"
#include "imr_assert.h"

#include "shapes.h"
#include "colormap.h"

#include "gui.h"
#include "canvasview_coords.h"
#include "MathCommon.h"
#include <unordered_set>
#include "dopn.h"

#include <limits>

#include <algorithm>    // std::random_shuffle

#define OUTPUT_DEFAULT "\033[0m"
#define OUTPUT_BLACK "\033[30m"
#define OUTPUT_RED "\033[31m"
#define OUTPUT_GREEN "\033[32m"
#define OUTPUT_YELLOW "\033[33m"
#define OUTPUT_BLUE "\033[34m"
#define OUTPUT_MAGENTA "\033[35m"
#define OUTPUT_CYAN "\033[36m"
#define OUTPUT_WHITE "\033[37m"

#define INFO_RED(x) INFO( OUTPUT_RED << x << OUTPUT_DEFAULT )
#define INFO_YELLOW(x) INFO( OUTPUT_YELLOW << x << OUTPUT_DEFAULT )
#define INFO_MAGENTA(x) INFO( OUTPUT_MAGENTA <<  x << OUTPUT_DEFAULT )
#define INFO_CYAN(x) INFO( OUTPUT_CYAN <<  x << OUTPUT_DEFAULT )
#define INFO_GREEN(x) INFO( OUTPUT_GREEN <<  x << OUTPUT_DEFAULT )
#define INFO_WHITE(x) INFO( OUTPUT_WHITE <<  x << OUTPUT_DEFAULT )
#define INFO_BLUE(x) INFO( OUTPUT_BLUE <<  x << OUTPUT_DEFAULT )
#define INFO_BLACK(x) INFO( OUTPUT_BLACK <<  x << OUTPUT_DEFAULT )

#define VARIABLE_STR(s) #s
#define STR(s) VARIABLE_STR(s)
#define ROVNASE1(X) X =
#define ROVNASE(X)  ROVNASE1(X)
#define INFO_VAR(x) INFO( STR(x) << " = " <<  x )
//"in function " << __PRETTY_FUNCTION__ << " var " <<
typedef struct NeighImrpovementValue {
	double distanceImprovement;
	std::vector<NeighAngValuesIds> originalNeighAngIds;
	std::vector<bool> improvedNode;
	std::vector<double> actualNeighAngles;
	std::vector<GraphNode> actualGraphNodes;
} NeighImrpovementValue;

namespace op {

class VNSDOPN: public imr::CAlgorithm {
	typedef imr::CAlgorithm Base;
	typedef std::vector<int> IntVector;

public:
	VNSDOPN(imr::CConfig& config, const std::string& problemFile);
	virtual ~VNSDOPN();
	//Tour find(DatasetOP dataset);
	//Tour find(std::vector<GraphNode> nodes, double budget, int startIndex_, int goalIndex_);

	static int greaterThanDistanceStartGoal(GraphNode gn1, GraphNode gn2, struct SortCompareVar<GraphNode> data);
	static int greaterThanGraphNodeReward(GraphNode gn1, GraphNode gn2, struct SortCompareVar<GraphNode> data);
	//CAlgorithm functions
	void solve(void);
	std::string getRevision(void);
	static imr::CConfig& getConfig(imr::CConfig& config);
	static std::string getName(void) {
		return "vns_dopn";
	}
	std::string getMethod(void) {
		return getName();
	}

protected:
	//CAlgorithm functions
	std::string getVersion(void);
	void load(void);
	void initialize(void);
	void after_init(void);
	void iterate(int step);
	void save(void);
	void release(void);
	void visualize(void);
	void defineResultLog(void);
	void fillResultRecord(int numIters, double length, int numItersLastImprovement, long timeLastImprovement);
	void drawPath(int usleepTime = 0, DOPN * toShow = NULL);
	void drawNeighborhoodPoints();
	const double BORDER;
	const bool SAVE_RESULTS;
	const bool SAVE_INFO;
	const bool SAVE_SETTINGS;
	const bool SAVE_SAMPLED_PATH;
	const bool SAVE_TARGETS;

private:
	std::vector<GraphNode> getReachableNodes();
	void checkConsistency(DOPN &actualDOP, std::vector<GraphNode> &actualVNS);
	void shake(DOPN &actualDOP, std::vector<GraphNode> &actualVNS, int k);
	bool localSearch(DOPN &actualDOP, std::vector<GraphNode> &actualVNS, int k);
	bool randomLocalSearch(DOPN &actualDOP, std::vector<GraphNode> &actualVNS, int k);

	//methods for local search
	void onePointMove(DOPN &actualDOP, std::vector<GraphNode> &actualVNS);
	void or_opt(DOPN &actualDOP, std::vector<GraphNode> &actualVNS, int num);

	void fitDOPtoBudget(DOPN &actualDOP, std::vector<GraphNode> &actualVNS);

	void generateInitialSolution(DOPN &actualDOP, std::vector<GraphNode> &actualVNS);

	void largestRewardInsertion(DOPN &actualDOP, std::vector<GraphNode> &actualVNS);

	bool insertRandom(DOPN &actualDOP, std::vector<GraphNode> &actualVNS, int num_changes);
	bool exchangeRandom(DOPN &actualDOP, std::vector<GraphNode> &actualVNS, int num_changes);
	NeighImrpovementValue improveNeighLocations(DOPN &actualDOPN, double minimal_improvement_distance);
	bool insertNeighborhoods(DOPN &actualDOPN, std::vector<NeighAngValuesIds> originalNeighAngIds, std::vector<bool> improvedNode,
			std::vector<double> actualNeighAngles, std::vector<GraphNode> actualGraphNodes);
	bool improveHeadings(DOPN &actualDOPN, std::vector<GraphNode> &actualVNS, double minimal_improvement_distance);

	bool insertSystematic(DOPN &actualDOP, std::vector<GraphNode> &actualVNS, int k);
	bool exchangeSystematic(DOPN &actualDOP, std::vector<GraphNode> &actualVNS, int k);
	void pathInsert(DOPN &actualDOP, std::vector<GraphNode> &actualVNS, int k);
	void pathExchange(DOPN &actualDOP, std::vector<GraphNode> &actualVNS, int k);

	void insertion(DOPN &actualDOP, std::vector<GraphNode> &actualVNS);

	//bool twoopt(DOPN &actualDOP, std::vector<GraphNode> &actualVNS);

	double getTourLength(std::vector<GraphNode> tour);
	double getTourReward(std::vector<GraphNode> tour);


	//just for generating examples
	void exampleInsertRandom(DOPN &actualDOPN);
	void exampleExchangeRandom(DOPN &actualDOPN);
	void examplePathInsert(DOPN &actualDOPN);
	void examplePathExchange(DOPN &actualDOPN);

	void saveSampled(std::string filename, DOPN &actualDOPN);

	std::vector<std::vector<GraphNode>> initialPaths;
	std::vector<std::vector<GraphNode>> nodesForInsertionInitialPaths;

	static int compareByDistanceFromStart();

	//default params
	std::vector<GraphNode> nodesAll;
	std::vector<GraphNode> nodesForInsertion;
	std::vector<GraphNode> availableNodes;
	int numAvailableNodes;
	double budget;
	int startIndex;
	int goalIndex;
	int K;
	int I;

	double maximalRewardAll;

	//specific for DOPN - resolution of angles and radius of turning
	int resolution;
	double radius;

	double neighborhood_radius;
	int neighborhood_resolution;
	bool null_start_goal_radius;
	bool draw_neighborhood_points;
	double fly_speed;
	double maximal_acceleration;
	double time_sample_rate;
	DOPN tourDOPN, finalTourDOPN;
	std::vector<GraphNode> vnsVector;

	double heading_improvement_minimal_budget_ratio;
	double neighborhood_improvement_minimal_budget_ratio;
	double maximal_calculation_time_min;
	long maximal_calculation_time_MS;

	double heading_improvement_minimal_distance;
	double neighborhood_improvement_minimal_distance;

	bool improve_neigh_during_local_search;

	bool useRVNS;
	int numIterations;
	int numIterationsUnimproved;

	//Tour path_op;
	//std::vector<Tour> path_nop;
	double record;
	double deviation;
	double p = 10;
	//Tour bestTour;

	double startGoalDistance;

	std::vector<GraphNode> reachableNodes;
	std::vector<GraphNode> nodesInEllipseWithStartGoal;

	std::vector<int> deletedIds;

	bool stop;

	GraphNode goalNode;
	GraphNode startNode;

	double maxOverGuessedTry;
	double avgsaved;
	double numTestedAvg;
};

}
#endif /* SRC__VNSDOP_H_ */
