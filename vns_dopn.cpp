/*
 * VNSDOPN.cpp
 *
 *  Created on: 22. 3. 2016
 *      Author: Robert Penicka
 */

#include "vns_dopn.h"

using imr::logger;

using namespace op;
using namespace imr;
using namespace imr::gui;
using namespace opendubins;

#define foreach BOOST_FOREACH
#define MIN_CHANGE_EPS 0.00001
#define DD " , "
#define DEBUG_DOP_TRY_OPERATIONS false

VNSDOPN::VNSDOPN(imr::CConfig& config, const std::string& problemFile) :
		Base(config), SAVE_RESULTS(config.get<bool>("save-results")), SAVE_SETTINGS(config.get<bool>("save-settings")), BORDER(config.get<double>("canvas-border")), SAVE_INFO(
				config.get<bool>("save-info")), SAVE_TARGETS(config.get<bool>("save-targets")), SAVE_SAMPLED_PATH(config.get<bool>("save-sampled-path")) {

	DatasetOP loadedDataset = DatasetLoader::loadDataset(problemFile);
	INFO("testing dataset with " << loadedDataset.graph.size() << " nodes, " << loadedDataset.Tmax << " max time budget and " << loadedDataset.P << " paths to find");
	INFO("use budget " << loadedDataset.Tmax);
	INFO("use startIndex " << loadedDataset.startID);
	INFO("use goalIndex " << loadedDataset.goalID);
	this->name = config.get<std::string>("name");
	this->nodesAll = loadedDataset.graph;
	this->budget = loadedDataset.Tmax;
	this->startIndex = loadedDataset.startID;
	this->goalIndex = loadedDataset.goalID;
	this->goalNode = nodesAll[goalIndex];
	this->startNode = nodesAll[startIndex];
	this->resolution = config.get<int>("dop-initial-resolution");
	this->radius = config.get<double>("dop-radius");
	this->useRVNS = config.get<bool>("use-rvns");
	this->numIterations = config.get<int>("num-iterations");
	this->numIterationsUnimproved = config.get<int>("num-iterations-unimproved");
	this->neighborhood_radius = config.get<double>("neighborhood-radius");
	this->neighborhood_resolution = config.get<double>("neighborhood-resolution");
	this->null_start_goal_radius = config.get<bool>("dopn-null-start-goal-radius");
	this->draw_neighborhood_points = config.get<bool>("draw-neighborhood-points");
	this->heading_improvement_minimal_budget_ratio = config.get<double>("heading-improvement-minimal-budget-ratio");
	this->neighborhood_improvement_minimal_budget_ratio = config.get<double>("neighborhood-improvement-minimal-budget-ratio");
	this->maximal_calculation_time_min = config.get<double>("maximal-calculation-time-min");
	this->maximal_calculation_time_MS = maximal_calculation_time_min * 60.0 * 1000.0;

	this->heading_improvement_minimal_distance = heading_improvement_minimal_budget_ratio * budget;
	this->neighborhood_improvement_minimal_distance = neighborhood_improvement_minimal_budget_ratio * budget;

	this->improve_neigh_during_local_search = false;

	INFO_GREEN("heading_improvement_minimal_distance "<<heading_improvement_minimal_distance);
	INFO_GREEN("neighborhood_improvement_minimal_distance "<<neighborhood_improvement_minimal_distance);

	if (neighborhood_radius == 0) {
		INFO_GREEN("setting neighborhood_resolution to 1 with neighborhood_radius " << neighborhood_radius);
		neighborhood_resolution = 1;
	}

	if (radius == 0) {
		INFO_GREEN("setting resolution to 1 with radius " << radius);
		resolution = 1;
	}

	maximalRewardAll = 0;
	for (int var = 0; var < nodesAll.size(); ++var) {
		maximalRewardAll += nodesAll[var].reward;
	}
	INFO_GREEN("maximalRewardAll "<<maximalRewardAll);

	this->maximal_acceleration = 2.6;
	this->fly_speed = 4;
	//this->radius = (fly_speed * fly_speed) / maximal_acceleration;

	INFO("radius "<<radius);
	this->time_sample_rate = 0.2;
	maxOverGuessedTry = 0;
	avgsaved = NAN;
	numTestedAvg = 0;
}

VNSDOPN::~VNSDOPN() {

}

void VNSDOPN::solve() {
	Base::solve();
}

void VNSDOPN::exampleInsertRandom(DOPN &actualDOPN) {
	INFO("exampleInsertRandom");
	int targetIDFrom = 5;
	int targetIDTo = 2;

	DOPN copyAdd = actualDOPN;
	copyAdd.listIds();
	drawPath(1000000, &copyAdd);
	//double lengthBeforeAddCopy = copyAdd.getPathLength();
	GraphNode testingRelocate = copyAdd.getTarget(targetIDFrom);
	INFO("to relocate "<<testingRelocate.id)
	if (targetIDTo > targetIDFrom) {
		copyAdd.addPoint(testingRelocate, targetIDTo);
		if (targetIDFrom < copyAdd.getNumTargets()) {
			copyAdd.removePoint(targetIDFrom);
		}
	} else {
		if (targetIDFrom < copyAdd.getNumTargets()) {
			copyAdd.removePoint(targetIDFrom);
		}
		copyAdd.addPoint(testingRelocate, targetIDTo);
	}
	copyAdd.update();
	double lengthBeforeAddCopy = copyAdd.getPathLength();
	copyAdd.listIds();
	drawPath(1000000, &copyAdd);
	saveSampled("matlab/beforeInsertRandom.txt", copyAdd);
	//do it back
	targetIDFrom = 2;
	targetIDTo = 6; //old +1
	testingRelocate = copyAdd.getTarget(targetIDFrom);
	INFO("to relocate "<<testingRelocate.id)
	if (targetIDTo > targetIDFrom) {
		INFO("add to "<<targetIDTo);
		copyAdd.addPoint(testingRelocate, targetIDTo);
		if (targetIDFrom < copyAdd.getNumTargets()) {
			INFO("add "<<targetIDFrom);
			copyAdd.removePoint(targetIDFrom);
		}
	} else {
		if (targetIDFrom < copyAdd.getNumTargets()) {
			copyAdd.removePoint(targetIDFrom);
		}
		copyAdd.addPoint(testingRelocate, targetIDTo);
	}
	copyAdd.update();
	double lengthAfterAddCopy = copyAdd.getPathLength();
	copyAdd.listIds();
	drawPath(1000000, &copyAdd);
	saveSampled("matlab/afterInsertRandom.txt", copyAdd);

}

void VNSDOPN::exampleExchangeRandom(DOPN &actualDOPN) {
	INFO("exampleExchangeRandom");
	int targetIDFrom = 5;
	int targetIDTo = 2;

	DOPN copyAdd = actualDOPN;
	copyAdd.listIds();
	drawPath(1000000, &copyAdd);

	GraphNode testingRelocateFromCopy = copyAdd.getTarget(targetIDFrom);
	GraphNode testingRelocateToCopy = copyAdd.getTarget(targetIDTo);

	if (targetIDTo > targetIDFrom) {
		copyAdd.removePoint(targetIDTo);
		copyAdd.addPoint(testingRelocateFromCopy, targetIDTo);
		if (targetIDFrom < copyAdd.getNumTargets()) {
			copyAdd.removePoint(targetIDFrom);
			copyAdd.addPoint(testingRelocateToCopy, targetIDFrom);
		}
	} else {
		if (targetIDFrom < copyAdd.getNumTargets()) {
			copyAdd.removePoint(targetIDFrom);
			copyAdd.addPoint(testingRelocateToCopy, targetIDFrom);
		}
		copyAdd.removePoint(targetIDTo);
		copyAdd.addPoint(testingRelocateFromCopy, targetIDTo);
	}
	copyAdd.update();

	copyAdd.listIds();
	drawPath(1000000, &copyAdd);
	saveSampled("matlab/beforeExchangeRandom.txt", copyAdd);

	//do it back
	targetIDFrom = 2;
	targetIDTo = 5; //old +1

	testingRelocateFromCopy = copyAdd.getTarget(targetIDFrom);
	testingRelocateToCopy = copyAdd.getTarget(targetIDTo);

	if (targetIDTo > targetIDFrom) {
		copyAdd.removePoint(targetIDTo);
		copyAdd.addPoint(testingRelocateFromCopy, targetIDTo);
		if (targetIDFrom < copyAdd.getNumTargets()) {
			copyAdd.removePoint(targetIDFrom);
			copyAdd.addPoint(testingRelocateToCopy, targetIDFrom);
		}
	} else {
		if (targetIDFrom < copyAdd.getNumTargets()) {
			copyAdd.removePoint(targetIDFrom);
			copyAdd.addPoint(testingRelocateToCopy, targetIDFrom);
		}
		copyAdd.removePoint(targetIDTo);
		copyAdd.addPoint(testingRelocateFromCopy, targetIDTo);
	}
	copyAdd.update();

	copyAdd.listIds();
	drawPath(1000000, &copyAdd);
	saveSampled("matlab/afterExchangeRandom.txt", copyAdd);

}

void VNSDOPN::examplePathInsert(DOPN &actualDOPN) {
	INFO("examplePathInsert");
	int exchangeFromStart = 0;
	int exchangeFromEnd = 2;
	int insertTo = 5;

	DOPN copyAdd = actualDOPN;
	copyAdd.listIds();
	drawPath(1000000, &copyAdd);
	bool insertUp = true;
	std::vector<GraphNode> targets = copyAdd.getAllTargets();
	if (insertUp) {
		//insert up
		std::vector<GraphNode> newVec(targets.begin() + exchangeFromStart, targets.begin() + exchangeFromEnd + 1);
		if (insertTo < copyAdd.getNumTargets()) {
			copyAdd.addPoint(newVec, insertTo);
			copyAdd.listIds();
		}
		if (exchangeFromStart < copyAdd.getNumTargets()) {
			copyAdd.removePoint(exchangeFromStart, MIN(exchangeFromEnd, copyAdd.getNumTargets() - 1));
			copyAdd.listIds();
		}

	} else {
		//insert bellow
		std::vector<GraphNode> newVec(targets.begin() + exchangeFromStart, targets.begin() + exchangeFromEnd + 1);

		if (exchangeFromStart < copyAdd.getNumTargets()) {
			copyAdd.removePoint(exchangeFromStart, MIN(exchangeFromEnd, copyAdd.getNumTargets() - 1));
		}
		if (insertTo < copyAdd.getNumTargets()) {
			copyAdd.addPoint(newVec, insertTo);
		}
	}

	copyAdd.update();

	copyAdd.listIds();
	drawPath(1000000, &copyAdd);

	saveSampled("matlab/beforePathInsertRandom.txt", copyAdd);

	exchangeFromStart = 2;
	exchangeFromEnd = 4;
	insertTo = 0;

	insertUp = false;
	targets = copyAdd.getAllTargets();
	if (insertUp) {
		//insert up
		std::vector<GraphNode> newVec(targets.begin() + exchangeFromStart, targets.begin() + exchangeFromEnd + 1);
		if (insertTo < copyAdd.getNumTargets()) {
			copyAdd.addPoint(newVec, insertTo);
			copyAdd.listIds();
		}
		if (exchangeFromStart < copyAdd.getNumTargets()) {
			copyAdd.removePoint(exchangeFromStart, MIN(exchangeFromEnd, copyAdd.getNumTargets() - 1));
			copyAdd.listIds();
		}

	} else {
		//insert bellow
		std::vector<GraphNode> newVec(targets.begin() + exchangeFromStart, targets.begin() + exchangeFromEnd + 1);

		if (exchangeFromStart < copyAdd.getNumTargets()) {
			copyAdd.removePoint(exchangeFromStart, MIN(exchangeFromEnd, copyAdd.getNumTargets() - 1));
		}
		if (insertTo < copyAdd.getNumTargets()) {
			copyAdd.addPoint(newVec, insertTo);
		}
	}

	copyAdd.update();

	copyAdd.listIds();
	drawPath(1000000, &copyAdd);
	saveSampled("matlab/afterPathInsertRandom.txt", copyAdd);
}

void VNSDOPN::examplePathExchange(DOPN &actualDOPN) {
	INFO("examplePathExchange");
	int exchangeFromStart = 0;
	int exchangeFromEnd = 2;
	int exchangeToStart = 4;
	int exchangeToEnd = 5;

	DOPN copyAdd = actualDOPN;
	copyAdd.listIds();
	drawPath(1000000, &copyAdd);
	bool exchangeUp = true;
	std::vector<GraphNode> targets = copyAdd.getAllTargets();
	if (exchangeUp) {
		std::vector<GraphNode> newVecFrom(targets.begin() + exchangeFromStart, targets.begin() + exchangeFromEnd + 1);		//is below
		std::vector<GraphNode> newVecTo(targets.begin() + exchangeToStart, targets.begin() + exchangeToEnd + 1);		//is above

		if (exchangeToStart < copyAdd.getNumTargets()) {
			copyAdd.removePoint(exchangeToStart, MIN(exchangeToEnd, copyAdd.getNumTargets() - 1));
			copyAdd.addPoint(newVecFrom, exchangeToStart);
		}
		//INFO("add newVecTo");
		if (exchangeFromStart < copyAdd.getNumTargets()) {
			copyAdd.removePoint(exchangeFromStart, MIN(exchangeFromEnd, copyAdd.getNumTargets() - 1));
			copyAdd.addPoint(newVecTo, exchangeFromStart);
		}

	} else {
		std::vector<GraphNode> newVecFrom(targets.begin() + exchangeFromStart, targets.begin() + exchangeFromEnd + 1);
		std::vector<GraphNode> newVecTo(targets.begin() + exchangeToStart, targets.begin() + exchangeToEnd + 1);

		if (exchangeFromStart < copyAdd.getNumTargets()) {
			copyAdd.removePoint(exchangeFromStart, MIN(exchangeFromEnd, copyAdd.getNumTargets() - 1));
			copyAdd.addPoint(newVecTo, exchangeFromStart);
		}

		//INFO("add newVecFrom");
		if (exchangeToStart < copyAdd.getNumTargets()) {
			copyAdd.removePoint(exchangeToStart, MIN(exchangeToEnd, copyAdd.getNumTargets() - 1));
			copyAdd.addPoint(newVecFrom, exchangeToStart);
		}

	}

	copyAdd.update();

	copyAdd.listIds();
	drawPath(1000000, &copyAdd);

	saveSampled("matlab/beforePathExchangeRandom.txt", copyAdd);

	exchangeToStart = 0;
	exchangeToEnd = 1;
	exchangeFromStart = 3;
	exchangeFromEnd = 5;

	exchangeUp = false;
	targets = copyAdd.getAllTargets();
	if (exchangeUp) {
		std::vector<GraphNode> newVecFrom(targets.begin() + exchangeFromStart, targets.begin() + exchangeFromEnd + 1);		//is below
		std::vector<GraphNode> newVecTo(targets.begin() + exchangeToStart, targets.begin() + exchangeToEnd + 1);		//is above

		if (exchangeToStart < copyAdd.getNumTargets()) {
			copyAdd.removePoint(exchangeToStart, MIN(exchangeToEnd, copyAdd.getNumTargets() - 1));
			copyAdd.addPoint(newVecFrom, exchangeToStart);
		}
		//INFO("add newVecTo");
		if (exchangeFromStart < copyAdd.getNumTargets()) {
			copyAdd.removePoint(exchangeFromStart, MIN(exchangeFromEnd, copyAdd.getNumTargets() - 1));
			copyAdd.addPoint(newVecTo, exchangeFromStart);
		}

	} else {
		std::vector<GraphNode> newVecFrom(targets.begin() + exchangeFromStart, targets.begin() + exchangeFromEnd + 1);
		std::vector<GraphNode> newVecTo(targets.begin() + exchangeToStart, targets.begin() + exchangeToEnd + 1);

		if (exchangeFromStart < copyAdd.getNumTargets()) {
			copyAdd.removePoint(exchangeFromStart, MIN(exchangeFromEnd, copyAdd.getNumTargets() - 1));
			copyAdd.addPoint(newVecTo, exchangeFromStart);
		}

		//INFO("add newVecFrom");
		if (exchangeToStart < copyAdd.getNumTargets()) {
			copyAdd.removePoint(exchangeToStart, MIN(exchangeToEnd, copyAdd.getNumTargets() - 1));
			copyAdd.addPoint(newVecFrom, exchangeToStart);
		}

	}

	copyAdd.update();

	copyAdd.listIds();
	drawPath(1000000, &copyAdd);
	saveSampled("matlab/afterPathExchangeRandom.txt", copyAdd);
}

void VNSDOPN::saveSampled(std::string filename, DOPN &actualDOPN) {
	std::string file = filename;
	std::ofstream ofs(filename.c_str());
	imr::assert_io(ofs.good(), "Cannot create path '" + filename + "'");
	ofs << std::setprecision(14);
	std::vector<State> samples = actualDOPN.getPathSampled(config.get<double>("sampled-path-distance"));
	for (int var = 0; var < samples.size(); ++var) {
		ofs << samples[var].point.x << DD << samples[var].point.y << DD << samples[var].ang << DD << std::endl;
	}
	imr::assert_io(ofs.good(), "Error occur during path saving");
	ofs.close();
}

void VNSDOPN::iterate(int iter) {

	INFO("start " << this->startNode.x << " " << this->startNode.y);
	INFO("goal " << this->goalNode.x << " " << this->goalNode.y);
	INFO("radius " << radius);
	INFO("resolution " << resolution);
	INFO("num-iterations " << numIterations);
	INFO("num-iterations-unimproved " << numIterationsUnimproved);
	INFO("neighborhood-radius " << neighborhood_radius);
	INFO("neighborhood-resolution " << neighborhood_resolution);

	this->nodesForInsertion = this->nodesAll;
	this->nodesForInsertion.erase(nodesForInsertion.begin() + MAX(startIndex, goalIndex));
	this->nodesForInsertion.erase(nodesForInsertion.begin() + MIN(startIndex, goalIndex));
	this->startGoalDistance = this->startNode.distanceTo(this->goalNode);
	//firstly select points in eclipse
	INFO("start timer")
	Timer testTouring;
	INFO("resetRealClock")
	testTouring.resetRealClock();
	INFO("calcAllDistances")
	DOPN::calcAllDistances(nodesAll, radius, resolution, neighborhood_radius, neighborhood_resolution, null_start_goal_radius, startIndex, goalIndex);
	//if (radius == 0) {
	//	DOPN::checkEucllideanDistances();
	//}
	INFO("drawNeighborhoodPoints")
	drawNeighborhoodPoints();
	this->reachableNodes = getReachableNodes();
	this->nodesInEllipseWithStartGoal = this->reachableNodes;
	this->nodesInEllipseWithStartGoal.push_back(startNode);
	this->nodesInEllipseWithStartGoal.push_back(goalNode);
	this->availableNodes = reachableNodes;
	INFO("we have " << reachableNodes.size() << " nodes in ellipse out of " << nodesForInsertion.size() << " add nodes");
	this->numAvailableNodes = availableNodes.size();
	INFO("initialize DOP");

	tourDOPN = DOPN(startNode, goalNode, radius, resolution, neighborhood_radius, neighborhood_resolution, null_start_goal_radius);
	INFO("tourDOPN created");
	drawPath(2000);

	generateInitialSolution(tourDOPN, vnsVector);
	//tourDOPN.evaluateUsage();

	//for example generation of neighborhood operations
	exampleInsertRandom(tourDOPN);
	exampleExchangeRandom(tourDOPN);
	examplePathInsert(tourDOPN);
	examplePathExchange(tourDOPN);
	exit(0);

	int numItersLastImprovement = 1;
	long timeLastImprovement = testTouring.getRTimeMS();
	int act_iter = 0;

	//NeighImrpovementValue imprValue = improveNeighLocations(tourDOPN, this->neighborhood_improvement_minimal_distance);
	//insertNeighborhoods(tourDOPN, imprValue.originalNeighAngIds, imprValue.improvedNode, imprValue.actualNeighAngles, imprValue.actualGraphNodes);
	std::vector<Dubins> dubinsPath = tourDOPN.getPath();

	INFO_GREEN("initial reward "<<tourDOPN.getReward());
	INFO_GREEN("initial length "<<tourDOPN.getPathLength());

	stop = false;
	int maximalNDepth = 2;
	while (!stop) {
		//INFO("act_iter "<<act_iter);
		act_iter++;
		//INFO("itteration "<<numIttertation);
		if (act_iter % 10 == 0) {
			INFO("itteration "<<act_iter <<" with best reward "<<tourDOPN.getReward()<<" at time "<<testTouring.getRTimeMS()<<" ms");
		}
		if (act_iter >= numIterations) {
			INFO("stop after maximal number of iterattion "<<numIterations);
			stop = true;
		}
		if (testTouring.getRTimeMS() >= maximal_calculation_time_MS) {
			INFO("stop at "<<testTouring.getRTimeMS()<<" after maximal number of misiliseconds "<<maximal_calculation_time_MS<< " obtained from "<<maximal_calculation_time_min<<" maximal minutes");
			stop = true;
		}
		if (act_iter - numItersLastImprovement >= numIterationsUnimproved) {
			INFO("stop after maximal number of iterattion without improvement "<<numIterationsUnimproved);
			stop = true;
		}
		int k = 1;
		//INFO("k "<<k);
		while (k <= maximalNDepth) {
			//INFO("k "<<k);
			DOPN actualDOPN = tourDOPN;

			std::vector<GraphNode> actualVNS = vnsVector;
			double rewardBefore = actualDOPN.getReward();
			double lengthBefore = actualDOPN.getPathLength();
			//INFO("randDoubleMinMax for shake");
			//srand(time(NULL));
			//INFO("after srand time null");
			//double randomForshake = randDoubleMinMax(0, 1.0);
			//if (randomForshake > 0.1) {
			shake(actualDOPN, actualVNS, k);
			//NeighImrpovementValue imprValue = improveNeighLocations(actualDOPN, this->neighborhood_improvement_minimal_distance);
			//insertNeighborhoods(actualDOPN, imprValue.originalNeighAngIds, imprValue.improvedNode, imprValue.actualNeighAngles, imprValue.actualGraphNodes);
			//}
			//
			//INFO("before local search");
			if (useRVNS) {
				randomLocalSearch(actualDOPN, actualVNS, k);
			} else {
				localSearch(actualDOPN, actualVNS, k);
			}
			//tourDOPN.evaluateUsage();
			//INFO("after local search");
			//checkConsistency(actualDOP, actualVNS);

			//twoopt(actualDOP, actualVNS);
			//INFO("after twoopt length " << actualDOPN.getPathLength());
			//exchange(actualDOP, actualVNS);
			//}

			//drawNeighborhoodPoints();
			//actualDOPN.update();
			double newReward = actualDOPN.getReward();
			double newLength = actualDOPN.getPathLength();
			//INFO_GREEN("actual best  "<<tourDOPN.getReward());
			if (newReward > rewardBefore) {

				//actualDOPN.evaluateUsage();
				//if (newReward > rewardBefore || (newReward == rewardBefore && (lengthBefore - newLength) > MIN_CHANGE_EPS)) {
				INFO_GREEN("improved to reward "<<newReward<<" with length "<<newLength<<" at time "<<testTouring.getRTimeMS()<<" ms");
				timeLastImprovement = testTouring.getRTimeMS();
				numItersLastImprovement = act_iter;
				tourDOPN = actualDOPN;
				vnsVector = actualVNS;
				drawPath(2000, &actualDOPN);
				saveCanvas();
				k = 1;
			} else {
				k++;
			}

		}
		//stop = true;
	}

	finalTourDOPN = tourDOPN;
	tSolve.stop();
	INFO("fillResultRecord");
	fillResultRecord(act_iter, tourDOPN.getPathLength(), numItersLastImprovement, timeLastImprovement);
	INFO("write result log");

	resultLog << imr::result::endrec;

	INFO("set finalTourDOP");
	INFO("found tour with reward "<<finalTourDOPN.getReward()<<" and length "<<finalTourDOPN.getPathLength()<<" out of "<<budget<<" budget");
}

void VNSDOPN::shake(DOPN &actualDOP, std::vector<GraphNode> &actualVNS, int k) {
	//INFO("shake "<<k);
	if (k % 2 == 1) {
		pathInsert(actualDOP, actualVNS, 1);
	} else {
		pathExchange(actualDOP, actualVNS, 1);
	}
	if (DEBUG_DOP_TRY_OPERATIONS) {
		INFO_VAR(DEBUG_DOP_TRY_OPERATIONS);
		checkConsistency(actualDOP, actualVNS);
	}
	//INFO("shake done "<<k);
}

bool VNSDOPN::localSearch(DOPN &actualDOPN, std::vector<GraphNode> &actualVNS, int k) {
	//INFO("localSearch "<<k);
	int numImprovements = 0;
	if (k == 1) {
		bool improved = true;
		while (improved) {
			improved = insertSystematic(actualDOPN, actualVNS, k);
			numImprovements++;
		}
	} else {
		bool improved = true;
		while (improved) {
			improved = exchangeSystematic(actualDOPN, actualVNS, k);
			numImprovements++;
		}
	}
	//INFO("localSearch done with improvements "<<numImprovements);
	if (DEBUG_DOP_TRY_OPERATIONS) {
		INFO_VAR(DEBUG_DOP_TRY_OPERATIONS);
		checkConsistency(actualDOPN, actualVNS);
	}
	//INFO("localSearch done "<<k);
	return (numImprovements > 1);
}

bool VNSDOPN::randomLocalSearch(DOPN &actualDOPN, std::vector<GraphNode> &actualVNS, int k) {
	//INFO("randomLocalSearch "<<k);

	int numIters = numAvailableNodes * numAvailableNodes;
	if (k == 1) {
		insertRandom(actualDOPN, actualVNS, numIters);
	} else if (k == 2) {
		exchangeRandom(actualDOPN, actualVNS, numIters);
	} else if (k == 3) {
		improveNeighLocations(actualDOPN, this->neighborhood_improvement_minimal_distance);
	} else {
		improveHeadings(actualDOPN, actualVNS, this->heading_improvement_minimal_distance);
	}
	//INFO("length "<<actualDOPN.getPathLength());
//INFO("localSearch done with improvements "<<numImprovements);
	if (DEBUG_DOP_TRY_OPERATIONS) {
		INFO_VAR(DEBUG_DOP_TRY_OPERATIONS);
		checkConsistency(actualDOPN, actualVNS);
	}
	//INFO("randomLocalSearch done "<<k);
	return true;
}

void VNSDOPN::fitDOPtoBudget(DOPN &actualDOPN, std::vector<GraphNode> &actualVNS) {
//remove nodes over budget
	while (actualDOPN.getPathLength() > budget) {
		actualDOPN.removePoint(actualDOPN.getNumTargets() - 1);
	}
	bool canAdd = true;

//add nodes to fill budget
	while (canAdd) {
		canAdd = false;
		int idToAdd = actualDOPN.getNumTargets();
		if (idToAdd < actualVNS.size()) {
			GraphNode testingNode = actualVNS[idToAdd];
			double testAdd = actualDOPN.tryToAdd(testingNode, idToAdd).actual_len;
			if (testAdd <= budget) {
				actualDOPN.addPoint(testingNode, idToAdd);
				canAdd = true;
			}
		}
	}

}

void VNSDOPN::onePointMove(DOPN &actualDOPN, std::vector<GraphNode> &actualVNS) {
	INFO("onePointMove() begin");

	int bestTargetIDFrom = -1;
	int bestTargetIDTo = -1;
	double actualPathLength = actualDOPN.getPathLength();
	double actualPathReward = actualDOPN.getReward();
	double minimalLength = actualPathLength;
	double maximalReward = actualPathReward;

	for (int targetIDFrom = actualDOPN.getNumTargets(); targetIDFrom < actualVNS.size(); ++targetIDFrom) {
		GraphNode testingRelocate = actualVNS[targetIDFrom];

		for (int targetIDTo = 0; targetIDTo <= actualDOPN.getNumTargets(); ++targetIDTo) {
			if (targetIDFrom != targetIDTo) {
				double addedLength = 0;
				double addedReward = 0;
				if (targetIDTo <= actualDOPN.getNumTargets()) {
					addedLength = actualDOPN.tryToAdd(testingRelocate, targetIDTo).actual_len;
					addedReward = testingRelocate.reward;
				}

				double rewardAfter = actualPathReward + addedReward;
				double lengthAfter = actualPathLength + addedLength;
				/*
				 if (lengthAfter <= budget) {
				 INFO("rewardChange "<<(addedReward - removedReward));
				 INFO("lengthChange "<<(addedLength - removedLength));
				 }
				 */
				if (lengthAfter <= budget && (rewardAfter > maximalReward || (rewardAfter == maximalReward && lengthAfter < minimalLength))) {
					bestTargetIDFrom = targetIDFrom;
					bestTargetIDTo = targetIDTo;
					maximalReward = rewardAfter;
					minimalLength = lengthAfter;
				}
			}
		}
	}

	if (bestTargetIDFrom != -1) {
		INFO("bestTargetIDFrom "<<bestTargetIDFrom);
		INFO("bestTargetIDTo "<<bestTargetIDTo);
		INFO("maximalReward "<<maximalReward);
		INFO("minimalLength "<<minimalLength);
		GraphNode testingRelocate = actualVNS[bestTargetIDFrom];
		if (bestTargetIDFrom > bestTargetIDTo) {
			if (bestTargetIDFrom < actualDOPN.getNumTargets()) {
				actualDOPN.removePoint(bestTargetIDFrom);
			}
			actualVNS.erase(actualVNS.begin() + bestTargetIDFrom);

			if (bestTargetIDTo <= actualDOPN.getNumTargets()) {
				actualDOPN.addPoint(testingRelocate, bestTargetIDTo);
			}
			actualVNS.insert(actualVNS.begin() + bestTargetIDTo, testingRelocate);
		} else {
			if (bestTargetIDTo <= actualDOPN.getNumTargets()) {
				actualDOPN.addPoint(testingRelocate, bestTargetIDTo);
			}
			actualVNS.insert(actualVNS.begin() + bestTargetIDTo, testingRelocate);

			if (bestTargetIDFrom < actualDOPN.getNumTargets()) {
				actualDOPN.removePoint(bestTargetIDFrom);
			}
			actualVNS.erase(actualVNS.begin() + bestTargetIDFrom);
		}
	}
	INFO("onePointMove() end");
}

void VNSDOPN::or_opt(DOPN &actualDOPN, std::vector<GraphNode> &actualVNS, int num) {
	INFO("or_opt"<<num<<"() begin");

	int bestTargetIDFrom = -1;
	int bestTargetIDTo = -1;
	double actualPathLength = actualDOPN.getPathLength();
	double actualPathReward = actualDOPN.getReward();
	double minimalLength = actualPathLength;
	double maximalReward = actualPathReward;

	for (int targetIDFrom = actualDOPN.getNumTargets(); targetIDFrom < actualVNS.size() - num; ++targetIDFrom) {
		//targetIDFrom num = 2 -> testingRelocate size = 2
		std::vector<GraphNode> testingRelocate = std::vector<GraphNode>(actualVNS.begin() + targetIDFrom, actualVNS.begin() + targetIDFrom + num);
		//INFO("testingRelocate.size()"<<testingRelocate.size());

		for (int targetIDTo = 0; targetIDTo <= actualDOPN.getNumTargets(); ++targetIDTo) {
			double addedLength = 0;
			double addedReward = 0;
			if (targetIDTo <= actualDOPN.getNumTargets()) {
				addedLength = actualDOPN.tryToAdd(testingRelocate, targetIDTo);
				for (int var = 0; var < testingRelocate.size(); ++var) {
					addedReward += testingRelocate[var].reward;
				}

			}

			double rewardAfter = actualPathReward + addedReward;
			double lengthAfter = actualPathLength + addedLength;

			if (lengthAfter <= budget && (rewardAfter > maximalReward || (rewardAfter == maximalReward && lengthAfter < minimalLength))) {
				bestTargetIDFrom = targetIDFrom;
				bestTargetIDTo = targetIDTo;
				maximalReward = rewardAfter;
				minimalLength = lengthAfter;
			}
		}
	}

	if (bestTargetIDFrom != -1) {
		INFO("bestTargetIDFrom "<<bestTargetIDFrom);
		INFO("bestTargetIDTo "<<bestTargetIDTo);
		INFO("maximalReward "<<maximalReward);
		INFO("minimalLength "<<minimalLength);
		std::vector<GraphNode> testingRelocate = std::vector<GraphNode>(actualVNS.begin() + bestTargetIDFrom, actualVNS.begin() + bestTargetIDFrom + num);
		if (bestTargetIDFrom > bestTargetIDTo) {
			if (bestTargetIDFrom < actualDOPN.getNumTargets()) {
				actualDOPN.removePoint(bestTargetIDFrom);
			}
			actualVNS.erase(actualVNS.begin() + bestTargetIDFrom);

			if (bestTargetIDTo <= actualDOPN.getNumTargets()) {
				actualDOPN.addPoint(testingRelocate, bestTargetIDTo);
			}
			actualVNS.insert(actualVNS.begin() + bestTargetIDTo, testingRelocate.begin(), testingRelocate.end());
		} else {
			if (bestTargetIDTo <= actualDOPN.getNumTargets()) {
				actualDOPN.addPoint(testingRelocate, bestTargetIDTo);
			}
			actualVNS.insert(actualVNS.begin() + bestTargetIDTo, testingRelocate.begin(), testingRelocate.end());

			if (bestTargetIDFrom < actualDOPN.getNumTargets()) {
				actualDOPN.removePoint(bestTargetIDFrom);
			}
			actualVNS.erase(actualVNS.begin() + bestTargetIDFrom);
		}
	}

	INFO("or_opt"<<num<<"() end");
}

void VNSDOPN::generateInitialSolution(DOPN &actualDOPN, std::vector<GraphNode> &actualVNS) {
	INFO("generateInitialSolution() begin");
	actualVNS = availableNodes;
	/*
	 largestRewardInsertion(actualDOP, actualVNS);
	 INFO("reward after largestRewardInsertion "<<actualDOPN.getReward());
	 for (int var = 0; var < actualDOPN.getNumTargets(); ++var) {
	 INFO("target "<<var<<" node "<<actualDOPN.getTarget(var).id);
	 }
	 //checkConsistency(actualDOP, actualVNS);
	 */
	insertion(actualDOPN, actualVNS);
	INFO("reward after insertion "<<actualDOPN.getReward());
	INFO("num targets " <<actualDOPN.getNumTargets());
	INFO("generateInitialSolution() end");
}

void VNSDOPN::largestRewardInsertion(DOPN &actualDOPN, std::vector<GraphNode> &actualVNS) {
	INFO("largestRewardInsertion() begin");
	struct SortCompareVar<GraphNode> scv;
	scv.greaterThan = &VNSDOPN::greaterThanGraphNodeReward;
	scv.data = NULL;

	INFO("actualVNS size "<<actualVNS.size())
	INFO("quicksort actualVNS");
	CSort<GraphNode>::quicksort(&actualVNS, 0, actualVNS.size() - 1, scv);
	INFO("quicksort actualVNS sorted");
	/*
	 for (int var = 0; var < actualVNS.size(); ++var) {
	 INFO("id"<<var<<" "<<actualVNS[var].id<<" "<<actualVNS[var].reward);
	 }
	 */
	for (int insertingID = 0; insertingID < actualVNS.size(); ++insertingID) {

		double minDistanceAfterAdd = DBL_MAX;
		int minIdTourNode = -1;
		int minInsertingID = -1;
		int startInsertingID = insertingID;
		do {

			for (int idTourNode = 0; idTourNode < actualDOPN.getSize() && actualVNS.size() > 0; ++idTourNode) {
				//INFO("try to add to position "<<idTourNode);
				double newDistance = actualDOPN.tryToAdd(actualVNS[insertingID], idTourNode).actual_len;
				if (newDistance < minDistanceAfterAdd) {

					minDistanceAfterAdd = newDistance;
					minIdTourNode = idTourNode;
					minInsertingID = insertingID;

				}
			}

			insertingID++;
		} while (insertingID < actualVNS.size() && actualVNS[insertingID - 1].reward == actualVNS[insertingID].reward);

		insertingID = startInsertingID;
		if (minDistanceAfterAdd <= budget) {

			GraphNode inserting = actualVNS[minInsertingID];
			actualDOPN.addPoint(inserting, minIdTourNode);

			actualVNS.erase(actualVNS.begin() + minInsertingID);
			actualVNS.insert(actualVNS.begin() + minIdTourNode, inserting);
			drawPath(2000, &actualDOPN);
		}

	}
	INFO("largestRewardInsertion() end");
}

bool VNSDOPN::insertRandom(DOPN &actualDOPN, std::vector<GraphNode> &actualVNS, int num_changes) {
	//INFO("insertRandom beg");
	const int sizeV = actualVNS.size() - 1;
	//double max_neigh_distance_between = neighborhood_radius * 2 + M_2PI * radius;
	//INFO_VAR(max_neigh_distance_between);
	//INFO("insert "<<k);
	double actualLength = actualDOPN.getPathLength();
	double maxOverbudget = 0;
	int num_improvable = 0;
	int num_test_improve = 0;
	int lastImprovementIndex = 0;
	//double minDifference = 99999999999999999999.0;
	//double maxDifference = -99999999999999999999.0;
	//double maxDiffTryToAdd = 0;
	//int numTestOverPossible = 0;
	//int numErrors = 0;
	if (actualVNS.size() >= 3) {
		for (int var = 0; var < num_changes; ++var) {
			int targetIDFrom = randIntMinMax(0, actualVNS.size() - 1);
			int targetIDTo = randIntMinMax(0, actualDOPN.getNumTargets());
			while (abs(targetIDFrom - targetIDTo) <= 1) {
				targetIDFrom = randIntMinMax(0, actualVNS.size() - 1);
				targetIDTo = randIntMinMax(0, actualDOPN.getNumTargets());
				//INFO("targetIDFrom "<<targetIDFrom);
				//INFO("targetIDTo "<<targetIDTo);
			}
			GraphNode testingRelocate = actualVNS[targetIDFrom];

			double lengthAfterRemove = actualLength;
			if (targetIDFrom < actualDOPN.getNumTargets()) {
				lengthAfterRemove = actualDOPN.tryToRemove(targetIDFrom);
			}
			double lengthRemoved = actualLength - lengthAfterRemove;
			double lengthAfterMove = 0;
			double addedReward = 0;
			if (targetIDFrom < actualDOPN.getNumTargets() && targetIDTo <= actualDOPN.getNumTargets()) {
				//INFO("both inside num targets "<<actualDOPN.getNumTargets());
				//INFO("cheeeeeeeeck try to move!!!!!!!!!!!!!!!!!!!!!!!");
				//double testTryToRemoveSaved = actualLength - actualDOPN.tryToRemove(targetIDFrom);
				//SimpleTryResult tryToAddResult = actualDOPN.tryToAdd(testingRelocate, targetIDTo);
				//INFO_VAR(tryToAddResult.maximal_len);
				//double diffTryAdd = tryToAddResult.maximal_len - tryToAddResult.actual_len;

				//double testTryToAddAdded = tryToAddResult.actual_len - actualLength;
				//double distanceAfterGuessed = actualLength - testTryToRemoveSaved + testTryToAddAdded;

				//if (diffTryAdd > maxDiffTryToAdd) {
				//	maxDiffTryToAdd = diffTryAdd;
				//}

				/*
				 if (distanceAfterGuessed > max_neigh_distance_between + budget) {
				 continue;
				 }
				 */

				lengthAfterMove = actualDOPN.tryToMove(targetIDFrom, targetIDTo);
				//INFO_VAR(diffTryAdd);
				//INFO_VAR((distanceAfterGuessed - lengthAfterMove));

				/*
				 double diff = distanceAfterGuessed - lengthAfterMove;
				 if (diff < minDifference) {
				 minDifference = diff;
				 }
				 if (diff > maxDifference) {
				 maxDifference = diff;
				 }
				 if (diff > maxOverGuessedTry) {
				 maxOverGuessedTry = diff;
				 }

				 if (distanceAfterGuessed > max_neigh_distance_between + budget) {
				 numTestOverPossible++;
				 if (lengthAfterMove <= budget) {
				 numErrors++;
				 INFO_RED("ERROR IN SKIPPING insertRandom")
				 INFO_VAR(diff);
				 INFO_VAR(max_neigh_distance_between);
				 //INFO_VAR(diffTryAdd);
				 exit(0);
				 }
				 }
				 */

				//check outcome
				if (DEBUG_DOP_TRY_OPERATIONS) {
					INFO_VAR(DEBUG_DOP_TRY_OPERATIONS);
					//DEBUG part
					//TODO erase debug
					DOPN copyAdd = actualDOPN;
					if (targetIDTo > targetIDFrom) {
						copyAdd.addPoint(testingRelocate, targetIDTo);
						if (targetIDFrom < copyAdd.getNumTargets()) {
							copyAdd.removePoint(targetIDFrom);
						}
					} else {
						if (targetIDFrom < copyAdd.getNumTargets()) {
							copyAdd.removePoint(targetIDFrom);
						}
						copyAdd.addPoint(testingRelocate, targetIDTo);
					}
					copyAdd.update();
					double lengthAfterAddCopy = copyAdd.getPathLength();
					if (fabs(lengthAfterAddCopy - lengthAfterMove) > 0.1) {
						ERROR("lengthAfterMove does not match lengthAfterAdd");
						ERROR("insertRandom from "<<targetIDFrom<<" to "<<targetIDTo);
						ERROR("id "<<testingRelocate.id);
						ERROR(lengthAfterAddCopy<< " and "<< lengthAfterMove);
						actualDOPN.listIds();
						exit(1);
					}
				}

			} else {
				//INFO("one outside inside");
				double lengthAfterAdd = actualDOPN.tryToAdd(testingRelocate, targetIDTo).actual_len;
				double lengthAdded = lengthAfterAdd - actualLength;
				addedReward = ((targetIDFrom >= actualDOPN.getNumTargets()) ? (testingRelocate.reward) : (0));
				lengthAfterMove = actualLength - lengthRemoved + lengthAdded;

			}

			if (lengthAfterMove <= budget && (addedReward > 0 || (fabs(addedReward) < MIN_CHANGE_EPS && (actualLength - lengthAfterMove) > MIN_CHANGE_EPS))) {
				lastImprovementIndex = var;
				GraphNode testingRelocate = actualVNS[targetIDFrom];

				if (targetIDTo > targetIDFrom) {
					//INFO("add point "<<targetIDTo);
					actualDOPN.addPoint(testingRelocate, targetIDTo);

					if (targetIDFrom < actualDOPN.getNumTargets()) {
						//INFO("remove point "<<targetIDFrom);
						actualDOPN.removePoint(targetIDFrom);
					}
					actualVNS.insert(actualVNS.begin() + targetIDTo, testingRelocate);
					actualVNS.erase(actualVNS.begin() + targetIDFrom);
				} else {

					if (targetIDFrom < actualDOPN.getNumTargets()) {
						//INFO("remove point "<<targetIDFrom);
						actualDOPN.removePoint(targetIDFrom);
					}
					//INFO("add point "<<targetIDTo);
					actualDOPN.addPoint(testingRelocate, targetIDTo);
					actualVNS.erase(actualVNS.begin() + targetIDFrom);
					actualVNS.insert(actualVNS.begin() + targetIDTo, testingRelocate);
				}

				//actualDOPN.evaluateUsage();

				//DEBUG TESTING
				actualLength = actualDOPN.getPathLength();
				if (actualLength - lengthAfterMove > 0.25) {
					INFO("wrongly calculated random insert");
					INFO_VAR(actualLength);
					INFO_VAR(lengthAfterMove);
					exit(1);
				}

			} else if (improve_neigh_during_local_search && addedReward > 0 && lengthAfterMove <= budget * (1.0 + actualVNS.size() * 0.0002)) {
				//INFO("length after move is "<<lengthAfterMove<<" the budget is "<<budget<<" allowing "<<(actualVNS.size() * 0.0001 ));
				GraphNode testingRelocate = actualVNS[targetIDFrom];
				DOPN dopnCopy = actualDOPN;
				if (targetIDTo > targetIDFrom) {
					//INFO("add point "<<targetIDTo);
					dopnCopy.addPoint(testingRelocate, targetIDTo);
					if (targetIDFrom < actualDOPN.getNumTargets()) {
						//INFO("remove point "<<targetIDFrom);
						dopnCopy.removePoint(targetIDFrom);
					}
				} else {
					if (targetIDFrom < actualDOPN.getNumTargets()) {
						dopnCopy.removePoint(targetIDFrom);
					}
					dopnCopy.addPoint(testingRelocate, targetIDTo);
				}
				NeighImrpovementValue improvedValue = this->improveNeighLocations(dopnCopy, 0);
				double lengthAfterImprovement = lengthAfterMove - improvedValue.distanceImprovement;
				num_test_improve++;
				if (lengthAfterImprovement <= budget) {
					num_improvable++;
					double overbudget = (lengthAfterMove - budget) / ((double) actualVNS.size()) / budget;
					if (overbudget > maxOverbudget) {
						maxOverbudget = overbudget;
					}
					//INFO_GREEN("the neighborhood improvement fixed overbudget from "<<lengthAfterMove<<" to "<<lengthAfterImprovement);
					lengthAfterMove = lengthAfterImprovement;
					insertNeighborhoods(dopnCopy, improvedValue.originalNeighAngIds, improvedValue.improvedNode, improvedValue.actualNeighAngles, improvedValue.actualGraphNodes);
					actualDOPN.update();
					//INFO_VAR(dopnCopy.getPathLength());
					//lengthAfterMove = lengthAfterImprovement;
					actualDOPN = dopnCopy;
					if (targetIDTo > targetIDFrom) {

						actualVNS.insert(actualVNS.begin() + targetIDTo, testingRelocate);
						actualVNS.erase(actualVNS.begin() + targetIDFrom);
					} else {

						actualVNS.erase(actualVNS.begin() + targetIDFrom);
						actualVNS.insert(actualVNS.begin() + targetIDTo, testingRelocate);
					}

				}
			}

		}
	}
	//INFO(lastImprovementIndex<<"/"<<num_changes);
	/*
	 INFO_VAR_RED(num_test_improve);
	 INFO_VAR_RED(num_improvable);
	 INFO_VAR_RED(num_changes);
	 INFO_VAR_RED(maxOverbudget);
	 drawNeighborhoodPoints();
	 */
	/*

	 INFO_VAR_RED(num_changes);
	 INFO_VAR_RED(numTestOverPossible);
	 INFO_VAR_RED(numErrors);

	 INFO_VAR_RED(canSave);
	 */
	//INFO_VAR_RED(minDifference);
	/*
	 INFO_VAR_RED(maxDifference);
	 double canSave = numTestOverPossible / (double) num_changes * 100.0;
	 INFO_VAR(canSave);
	 if (isnan(avgsaved)) {
	 avgsaved = canSave;
	 } else {
	 avgsaved -= avgsaved / numTestedAvg;
	 avgsaved += canSave / numTestedAvg;
	 numTestedAvg += 1.0;
	 }
	 INFO_VAR_RED(maxDiffTryToAdd);
	 */
	//INFO_CYAN("maxOverGuessedTry "<<(maxOverGuessedTry/(neighborhood_radius))<<" used max_neigh_distance_between "<<max_neigh_distance_between);
	//INFO_CYAN("avgsaved "<<avgsaved);
	//INFO("insertRandom end");
	return true;
}

bool VNSDOPN::exchangeRandom(DOPN &actualDOPN, std::vector<GraphNode> &actualVNS, int num_changes) {
	//INFO("exchangeRandom beg");
	const int sizeV = actualVNS.size() - 1;
	double actualLength = actualDOPN.getPathLength();
	int num_test_improve = 0;
	//double maxOverbudget = 0;
	int num_improvable = 0;
	int lastImprovementIndex = 0;
	//double max_neigh_distance_between = neighborhood_radius * 2 + M_2PI * radius;
	//INFO_VAR(max_neigh_distance_between);
	//double minDifference = 99999999999999999999.0;
	//double maxDifference = -99999999999999999999.0;
	//int numTestOverPossible = 0;
	//int numErrors = 0;
	if (actualVNS.size() >= 4) {
		for (int var = 0; var < num_changes; ++var) {
			//INFO("var "<<var);

			double minLength = actualLength;
			double maxAddReward = 0;

			int targetIDFrom = randIntMinMax(0, actualVNS.size() - 1);
			int targetIDTo = randIntMinMax(0, actualDOPN.getNumTargets() - 1);
			//int whileIters = 0;
			while (abs(targetIDFrom - targetIDTo) <= 2) {
				targetIDFrom = randIntMinMax(0, actualVNS.size() - 1);
				targetIDTo = randIntMinMax(0, actualDOPN.getNumTargets() - 1);

				//INFO("targetIDFrom "<<targetIDFrom);
				//INFO("targetIDTo "<<targetIDTo);
			}
			GraphNode testingRelocateFrom = actualVNS[targetIDFrom];

			if (abs(targetIDFrom - targetIDTo) > 2) {
				GraphNode testingRelocateTo = actualVNS[targetIDTo];
				double addedReward = 0;
				double lengthAfterMove = 0;
				if (targetIDFrom < actualDOPN.getNumTargets() && targetIDTo < actualDOPN.getNumTargets()) {
					//SimpleTryResult tryReplace1 = actualDOPN.tryToReplace(testingRelocateTo, targetIDFrom);
					//double saveReplace1 = actualLength - tryReplace1.actual_len;
					//double saveReplace2 = actualLength - actualDOPN.tryToReplace(testingRelocateFrom, targetIDTo).actual_len;
					//double distanceAfterGuessed = actualLength - saveReplace1 - saveReplace2;

					//double diffTryReplace = tryReplace1.maximal_len - tryReplace1.actual_len;
					//INFO_VAR(diffTryReplace);
					/*
					 if (distanceAfterGuessed > diffTryReplace + budget) {
					 continue;
					 }
					 */

					lengthAfterMove = actualDOPN.tryToExchange(targetIDFrom, targetIDTo);
					/*
					 double diff = distanceAfterGuessed - lengthAfterMove;
					 if (diff < minDifference) {
					 minDifference = diff;
					 }
					 if (diff > maxDifference) {
					 maxDifference = diff;
					 }
					 if (diff > maxOverGuessedTry) {
					 maxOverGuessedTry = diff;
					 }

					 if (distanceAfterGuessed > max_neigh_distance_between + budget) {
					 numTestOverPossible++;
					 if (lengthAfterMove <= budget) {
					 numErrors++;
					 INFO_RED("ERROR IN SKIPPING exchangeRandom MOVE")
					 INFO_VAR(diff);
					 INFO_VAR(max_neigh_distance_between);
					 //INFO_VAR(diffTryReplace);
					 exit(0);
					 }
					 }
					 */

					if (DEBUG_DOP_TRY_OPERATIONS) {
						DOPN copyAdd = actualDOPN;
						GraphNode testingRelocateFromCopy = actualVNS[targetIDFrom];
						GraphNode testingRelocateToCopy = actualVNS[targetIDTo];

						if (targetIDTo > targetIDFrom) {
							copyAdd.removePoint(targetIDTo);
							copyAdd.addPoint(testingRelocateFrom, targetIDTo);
							if (targetIDFrom < copyAdd.getNumTargets()) {
								copyAdd.removePoint(targetIDFrom);
								copyAdd.addPoint(testingRelocateTo, targetIDFrom);
							}
						} else {
							if (targetIDFrom < copyAdd.getNumTargets()) {
								copyAdd.removePoint(targetIDFrom);
								copyAdd.addPoint(testingRelocateTo, targetIDFrom);
							}
							copyAdd.removePoint(targetIDTo);
							copyAdd.addPoint(testingRelocateFrom, targetIDTo);
						}
						copyAdd.update();
						double lengthAfterAddCopy = copyAdd.getPathLength();
						if (fabs(lengthAfterAddCopy - lengthAfterMove) > 0.1) {
							ERROR("lengthAfterMove does not match lengthAfterAdd");
							ERROR("exchangeRandom from "<<targetIDFrom<<" to "<<targetIDTo);
							ERROR(lengthAfterAddCopy<< " and "<< lengthAfterMove);
							//actualDOPN.listIds();
							exit(1);
						}
					}

				} else {
					double lengthAfterReplaceTo = actualLength;
					double addedAfterReplaceTo = 0;
					if (targetIDTo < actualDOPN.getNumTargets()) {
						lengthAfterReplaceTo = actualDOPN.tryToReplace(testingRelocateFrom, targetIDTo).actual_len;
						addedAfterReplaceTo = lengthAfterReplaceTo - actualLength;
						addedReward += testingRelocateFrom.reward - testingRelocateTo.reward;
					}
					double lengthAfterReplaceFrom = actualLength;
					double addedAfterReplaceFrom = 0;
					if (targetIDFrom < actualDOPN.getNumTargets()) {
						lengthAfterReplaceFrom = actualDOPN.tryToReplace(testingRelocateTo, targetIDFrom).actual_len;
						addedAfterReplaceFrom = lengthAfterReplaceFrom - actualLength;
						addedReward += testingRelocateTo.reward - testingRelocateFrom.reward;
					}
					//INFO("lengthAfterReplaceTo "<<lengthAfterReplaceTo);
					//INFO("lengthAfterReplaceFrom "<<lengthAfterReplaceFrom);
					lengthAfterMove = actualLength + addedAfterReplaceTo + addedAfterReplaceFrom;
				}

				if (lengthAfterMove <= budget && (addedReward > maxAddReward || (fabs(addedReward - maxAddReward) < MIN_CHANGE_EPS && (minLength - lengthAfterMove) > MIN_CHANGE_EPS))) {
					lastImprovementIndex = var;
					GraphNode testingRelocateFrom = actualVNS[targetIDFrom];
					GraphNode testingRelocateTo = actualVNS[targetIDTo];

					if (targetIDTo > targetIDFrom) {
						actualDOPN.removePoint(targetIDTo);
						actualDOPN.addPoint(testingRelocateFrom, targetIDTo);
						if (targetIDFrom < actualDOPN.getNumTargets()) {
							actualDOPN.removePoint(targetIDFrom);
							actualDOPN.addPoint(testingRelocateTo, targetIDFrom);
						}
						GraphNode temp = actualVNS[targetIDTo];
						actualVNS[targetIDTo] = actualVNS[targetIDFrom];
						actualVNS[targetIDFrom] = temp;
					} else {
						if (targetIDFrom < actualDOPN.getNumTargets()) {
							actualDOPN.removePoint(targetIDFrom);
							actualDOPN.addPoint(testingRelocateTo, targetIDFrom);
						}
						actualDOPN.removePoint(targetIDTo);
						actualDOPN.addPoint(testingRelocateFrom, targetIDTo);

						GraphNode temp = actualVNS[targetIDTo];
						actualVNS[targetIDTo] = actualVNS[targetIDFrom];
						actualVNS[targetIDFrom] = temp;
					}

					//actualDOPN.evaluateUsage();

					actualLength = actualDOPN.getPathLength();
					if (actualLength - lengthAfterMove > 0.01) {
						INFO("wrongly calculated random exchange");
						INFO_VAR(actualLength);
						INFO_VAR(lengthAfterMove);
						exit(1);
					}
				} else if (improve_neigh_during_local_search && addedReward > 0 && lengthAfterMove <= budget * (1.0 + actualVNS.size() * 0.0002)) {
					//INFO("length after move is "<<lengthAfterMove<<" the budget is "<<budget<<" allowing "<<(actualVNS.size() * 0.0001 ));
					GraphNode testingRelocate = actualVNS[targetIDFrom];
					DOPN dopnCopy = actualDOPN;
					if (targetIDTo > targetIDFrom) {
						dopnCopy.removePoint(targetIDTo);
						dopnCopy.addPoint(testingRelocateFrom, targetIDTo);
						if (targetIDFrom < dopnCopy.getNumTargets()) {
							dopnCopy.removePoint(targetIDFrom);
							dopnCopy.addPoint(testingRelocateTo, targetIDFrom);
						}
					} else {
						if (targetIDFrom < dopnCopy.getNumTargets()) {
							dopnCopy.removePoint(targetIDFrom);
							dopnCopy.addPoint(testingRelocateTo, targetIDFrom);
						}
						dopnCopy.removePoint(targetIDTo);
						dopnCopy.addPoint(testingRelocateFrom, targetIDTo);
					}
					NeighImrpovementValue improvedValue = this->improveNeighLocations(dopnCopy, 0);
					double lengthAfterImprovement = lengthAfterMove - improvedValue.distanceImprovement;
					num_test_improve++;
					if (lengthAfterImprovement <= budget) {
						num_improvable++;
						double overbudget = (lengthAfterMove - budget) / ((double) actualVNS.size()) / budget;
						//if (overbudget > maxOverbudget) {
						//	maxOverbudget = overbudget;
						//}
						//INFO_GREEN("the neighborhood improvement fixed overbudget from "<<lengthAfterMove<<" to "<<lengthAfterImprovement);
						lengthAfterMove = lengthAfterImprovement;
						insertNeighborhoods(dopnCopy, improvedValue.originalNeighAngIds, improvedValue.improvedNode, improvedValue.actualNeighAngles, improvedValue.actualGraphNodes);
						actualDOPN.update();
						//INFO_VAR(dopnCopy.getPathLength());
						//lengthAfterMove = lengthAfterImprovement;
						actualDOPN = dopnCopy;
						if (targetIDTo > targetIDFrom) {
							GraphNode temp = actualVNS[targetIDTo];
							actualVNS[targetIDTo] = actualVNS[targetIDFrom];
							actualVNS[targetIDFrom] = temp;
						} else {
							GraphNode temp = actualVNS[targetIDTo];
							actualVNS[targetIDTo] = actualVNS[targetIDFrom];
							actualVNS[targetIDFrom] = temp;
						}
					}
				}
			}
		}
	}

	/*

	 INFO_VAR_RED(num_changes);
	 INFO_VAR_RED(numTestOverPossible);
	 INFO_VAR_RED(numErrors);
	 */
	//INFO_VAR_RED(minDifference);
	/*
	 INFO_VAR_RED(maxDifference);
	 double canSave = numTestOverPossible / (double) num_changes * 100.0;
	 INFO_VAR_RED(canSave);

	 if (isnan(avgsaved)) {
	 avgsaved = canSave;
	 } else {
	 avgsaved -= avgsaved / numTestedAvg;
	 avgsaved += canSave / numTestedAvg;
	 numTestedAvg += 1.0;
	 }
	 */
	//INFO_CYAN("maxOverGuessedTry "<<(maxOverGuessedTry/(neighborhood_radius))<<" used max_neigh_distance_between "<<max_neigh_distance_between);
	//INFO_CYAN("avgsaved "<<avgsaved);
	//INFO(lastImprovementIndex<<"/"<<num_changes);
	//INFO("exchangeRandom end");
	return true;
}

NeighImrpovementValue VNSDOPN::improveNeighLocations(DOPN &actualDOPN, double minimal_improvement_distance) {
	NeighImrpovementValue neighImprovement;
	neighImprovement.distanceImprovement = 0;
	if (neighborhood_radius > 0.0) {
		//INFO("improveNeighLocations begin");
		int targetSize = actualDOPN.getNumTargets();
		double lengthBefore = actualDOPN.getPathLength();
		double improvedAllBy = 0;
		//INFO("getPathNeighAngIds");
		std::vector<NeighAngValuesIds> originalNeighAngIds = actualDOPN.getPathNeighAngIds();
		std::vector<GraphNode> originalGraphNodes;
		//INFO_VAR(originalNeighAngIds.size());
		//INFO_VAR(targetSize);

		std::vector<double> actualNeighAngles(targetSize);
		std::vector<GraphNode> actualGraphNodes(targetSize);
		std::vector<bool> improvedNode(targetSize);
		for (int var = 0; var < targetSize; ++var) {
			actualNeighAngles[var] = originalNeighAngIds[var + 1].neigh_ang;
			actualGraphNodes[var] = originalNeighAngIds[var + 1].node;
			improvedNode[var] = false;
		}
		originalGraphNodes = actualGraphNodes;

		bool someImprovement = true;
		int impriter = 0;
		while (someImprovement) {
			someImprovement = false;
			double oldImprovedAllBy = improvedAllBy;

			std::vector<int> random_order_of_targets(targetSize);
			for (int var = 0; var < targetSize; ++var) {
				random_order_of_targets[var] = var;
			}
			std::random_shuffle(random_order_of_targets.begin(), random_order_of_targets.end());

			for (int targetID = 0; targetID < targetSize; ++targetID) {
				int shuffled_target_id = random_order_of_targets[targetID];
				//int actualNeightAng =

				GraphNode testingTarget = actualDOPN.getTarget(shuffled_target_id);
				//INFO("optimize targetID:"<<shuffled_target_id<<"out of "<<targetSize<<" targets");
				//INFO("target position "<<testingTarget.x<<" "<<testingTarget.y);

				NeighImprovement neighImprovement = actualDOPN.optimizeNeighborhoodPosition(shuffled_target_id, actualNeighAngles, originalNeighAngIds, minimal_improvement_distance);
				//INFO("target "<<shuffled_target_id<<" improved by "<<neighImprovement.improvementLength<<" to ang "<<neighImprovement.neigh_ang<<" from ang "<<actualNeighAngles[shuffled_target_id]);
				//INFO("actual node "<<actualGraphNodes[shuffled_target_id].x<<" "<<actualGraphNodes[shuffled_target_id].y <<" from original node " << originalGraphNodes[shuffled_target_id].x <<" "<<originalGraphNodes[shuffled_target_id].y);

				improvedAllBy += neighImprovement.improvementLength;
				actualNeighAngles[shuffled_target_id] = neighImprovement.neigh_ang;
				actualGraphNodes[shuffled_target_id] = neighImprovement.nodeInNeigh;
				if (neighImprovement.improvementLength > 0) {
					improvedNode[shuffled_target_id] = true;
					//actualDOPN.update();
					//tourDOPN.update();
				}
			}

			//INFO_GREEN("improvedAllBy "<<impriter<<":"<<improvedAllBy);
			//INFO_GREEN("improvedAllBy "<<impriter<<":"<<(100*improvedAllBy/lengthBefore)<<"%");
			impriter++;
			if (improvedAllBy > oldImprovedAllBy) {
				someImprovement = true;
			}
		}
		neighImprovement.actualGraphNodes = actualGraphNodes;
		neighImprovement.actualNeighAngles = actualNeighAngles;
		neighImprovement.improvedNode = improvedNode;
		neighImprovement.originalNeighAngIds = originalNeighAngIds;
		neighImprovement.distanceImprovement = improvedAllBy;
		//return neighImprovement;

		//INFO_VAR(improvedAllBy);

		//now insert the new neighborhood samples

		//INFO("improveNeighLocations end");
	}
	return neighImprovement;
}

bool VNSDOPN::insertNeighborhoods(DOPN &actualDOPN, std::vector<NeighAngValuesIds> originalNeighAngIds, std::vector<bool> improvedNode, std::vector<double> actualNeighAngles,
		std::vector<GraphNode> actualGraphNodes) {
	int targetSize = actualDOPN.getNumTargets();
	int numAddedNeighbours = 0;
	bool anythingImproved = false;
	for (int targetID = 0; targetID < targetSize; ++targetID) {
		if (improvedNode[targetID]) {
			//INFO("imrpove node "<<targetID);
			double neigh_ang_min = originalNeighAngIds[targetID].neigh_ang;
			//double neigh_ang_max = normalizeAngle(originalNeighAngIds[targetID + 2].neigh_ang, neigh_ang_min, neigh_ang_min + M_2PI);
			double actual_neigh_ang = normalizeAngle(originalNeighAngIds[targetID + 1].neigh_ang, neigh_ang_min, neigh_ang_min + M_2PI);

			if (actualNeighAngles[targetID] < actual_neigh_ang) {
				//insert before neighId for target in stample - at position inSampleId
				anythingImproved = true;
				numAddedNeighbours++;
				//INFO("insert for "<<actualGraphNodes[targetID].id);
				//INFO("inset sample "<<normalizeAngle(actualNeighAngles[targetID], 0, M_2PI)<<" imrpoved from "<<originalNeighAngIds[targetID + 1].neigh_ang);
				//INFO("actual node "<<actualGraphNodes[targetID].x<<" "<<actualGraphNodes[targetID].y <<" from original node " << originalGraphNodes[targetID].x <<" "<<originalGraphNodes[targetID].y);

				actualDOPN.insertNeighSample(targetID + 1, originalNeighAngIds[targetID + 1].idNeigh, originalNeighAngIds[targetID + 1].idNeigh, normalizeAngle(actualNeighAngles[targetID], 0, M_2PI),
						actualGraphNodes[targetID]);
			} else if (actualNeighAngles[targetID] > actual_neigh_ang) {
				//insert after neighId
				anythingImproved = true;
				numAddedNeighbours++;
				//INFO("insert for "<<actualGraphNodes[targetID].id);
				//INFO("inset sample "<<normalizeAngle(actualNeighAngles[targetID], 0, M_2PI)<<" improved from "<<originalNeighAngIds[targetID + 1].neigh_ang);
				//INFO("actual node "<<actualGraphNodes[targetID].x<<" "<<actualGraphNodes[targetID].y <<" from original node " << originalGraphNodes[targetID].x <<" "<<originalGraphNodes[targetID].y);
				actualDOPN.insertNeighSample(targetID + 1, originalNeighAngIds[targetID + 1].idNeigh, originalNeighAngIds[targetID + 1].idNeigh + 1,
						normalizeAngle(actualNeighAngles[targetID], 0, M_2PI), actualGraphNodes[targetID]);
			}
		}

	}
	if (anythingImproved) {
		//INFO_GREEN("imrpoved neighborhood positions");
		actualDOPN.update();
		tourDOPN.update();
		//INFO_VAR_RED(numAddedNeighbours);
		//drawNeighborhoodPoints();
		return true;
	}
	return false;
}

bool VNSDOPN::improveHeadings(DOPN &actualDOPN, std::vector<GraphNode> &actualVNS, double minimal_improvement_distance) {
	if (radius > 0.0) {
		//INFO("improveHeadings begin");
		int targetSize = actualDOPN.getNumTargets();
		double lengthBefore = actualDOPN.getPathLength();
		double improvedAllHeadingsBy = 0;

		std::vector<int> random_order_of_targets(targetSize);
		for (int var = 0; var < targetSize; ++var) {
			random_order_of_targets[var] = var;
		}
		std::random_shuffle(random_order_of_targets.begin(), random_order_of_targets.end());

		for (int targetID = 0; targetID < targetSize; ++targetID) {
			int shuffled_target_id = random_order_of_targets[targetID];
			//GraphNode testingTarget = actualDOPN.getTarget(shuffled_target_id);
			//INFO("optimize targetID:"<<targetID<<"out of "<<targetSize<<" targets");
			//INFO("target position "<<testingTarget.x<<" "<<testingTarget.y);
			improvedAllHeadingsBy += actualDOPN.optimizeHeading(shuffled_target_id, minimal_improvement_distance);
		}
		actualDOPN.update();
		tourDOPN.update();

		//INFO_GREEN("improvedAllHeadingsBy:"<<improvedAllHeadingsBy);
		//INFO_GREEN("improvedAllHeadingsBy:"<<(100*improvedAllHeadingsBy/lengthBefore)<<"%");
		//INFO("improveHeadings end");
	}
	return true;
}

bool VNSDOPN::insertSystematic(DOPN &actualDOPN, std::vector<GraphNode> &actualVNS, int k) {
	double actualLength = actualDOPN.getPathLength();
	double actualReward = actualDOPN.getReward();
	//INFO("insertSystematic begin length "<<actualLength);
	//INFO("insertSystematic begin reward "<<actualReward);
	//INFO_VAR(actualDOPN.getNumTargets())
	double minLength = actualLength;
	double maxAddReward = 0;
	int bestTargetIDFrom = -1;
	int bestTargetIDTo = -1;

	for (int targetIDFrom = 0; targetIDFrom < actualVNS.size(); ++targetIDFrom) {
		GraphNode testingRelocate = actualVNS[targetIDFrom];
		double lengthAfterRemove = actualLength;
		if (targetIDFrom < actualDOPN.getNumTargets()) {
			lengthAfterRemove = actualDOPN.tryToRemove(targetIDFrom);
		}
		double lengthRemoved = actualLength - lengthAfterRemove;

		for (int targetIDTo = 0; targetIDTo <= actualDOPN.getNumTargets(); ++targetIDTo) {
			if (abs(targetIDFrom - targetIDTo) > 1) {
				double lengthAfterMove = 0;
				double addedReward = 0;
				double lengthAfterAdd = 0;
				if (targetIDFrom < actualDOPN.getNumTargets() && targetIDTo <= actualDOPN.getNumTargets()) {
					//INFO("both inside num targets "<<actualDOPN.getNumTargets());
					lengthAfterMove = actualDOPN.tryToMove(targetIDFrom, targetIDTo);
				} else {
					//INFO("one outside inside");
					double lengthAfterAdd = actualDOPN.tryToAdd(testingRelocate, targetIDTo).actual_len;
					double lengthAdded = lengthAfterAdd - actualLength;
					addedReward = ((targetIDFrom >= actualDOPN.getNumTargets()) ? (testingRelocate.reward) : (0));

					lengthAfterMove = actualLength - lengthRemoved + lengthAdded;
				}
				if (lengthAfterMove <= budget && (addedReward > maxAddReward || (fabs(addedReward - maxAddReward) < MIN_CHANGE_EPS && (minLength - lengthAfterMove) > MIN_CHANGE_EPS))) {
					//INFO("improved lengthAfterMove "<< lengthAfterMove);
					//INFO("improved minLength "<< minLength);
					//INFO("imrpoved addedReward "<< addedReward);
					//INFO("diff lengthAfterMove-minLength "<< ( minLength - lengthAfterMove));

					maxAddReward = addedReward;
					minLength = lengthAfterMove;
					bestTargetIDFrom = targetIDFrom;
					bestTargetIDTo = targetIDTo;

					if (DEBUG_DOP_TRY_OPERATIONS) {
						INFO_VAR(DEBUG_DOP_TRY_OPERATIONS);
						//DEBUG part
						//TODO erase debug
						DOPN copyAdd = actualDOPN;
						if (bestTargetIDTo > bestTargetIDFrom) {
							copyAdd.addPoint(testingRelocate, bestTargetIDTo);
							if (bestTargetIDFrom < actualDOPN.getNumTargets()) {
								copyAdd.removePoint(bestTargetIDFrom);
							}
						} else {
							if (bestTargetIDFrom < actualDOPN.getNumTargets()) {
								copyAdd.removePoint(bestTargetIDFrom);
							}
							copyAdd.addPoint(testingRelocate, bestTargetIDTo);
						}
						double lengthAfterAddCopy = copyAdd.getPathLength();
						if (fabs(lengthAfterAddCopy - lengthAfterMove) > 0.1) {
							ERROR("lengthAfterAddCopy does not match lengthAfterAdd");
							ERROR("insertSystematic from "<<bestTargetIDFrom<<" to "<<bestTargetIDTo);
							ERROR("id "<<testingRelocate.id);
							ERROR(lengthAfterAddCopy<< " and "<< lengthAfterMove);
							copyAdd.listIds();
							exit(1);
						}
					}

				}

			}

		}
	}

	if (bestTargetIDFrom != -1) {

		GraphNode testingRelocate = actualVNS[bestTargetIDFrom];

		//INFO("imporved in insertSystematic");
		//INFO("relocate id "<<testingRelocate.id);
		//INFO("from "<<bestTargetIDFrom<<" to "<<bestTargetIDTo);

		if (bestTargetIDTo > bestTargetIDFrom) {
			//INFO("add point "<<bestTargetIDTo);
			actualDOPN.addPoint(testingRelocate, bestTargetIDTo);
			//INFO("remove point "<<bestTargetIDFrom);
			if (bestTargetIDFrom < actualDOPN.getNumTargets()) {
				actualDOPN.removePoint(bestTargetIDFrom);
			}
			actualVNS.insert(actualVNS.begin() + bestTargetIDTo, testingRelocate);
			actualVNS.erase(actualVNS.begin() + bestTargetIDFrom);
		} else {
			//INFO("remove point "<<bestTargetIDFrom);
			if (bestTargetIDFrom < actualDOPN.getNumTargets()) {
				actualDOPN.removePoint(bestTargetIDFrom);
			}
			//INFO("add point "<<bestTargetIDTo);
			actualDOPN.addPoint(testingRelocate, bestTargetIDTo);
			actualVNS.erase(actualVNS.begin() + bestTargetIDFrom);
			actualVNS.insert(actualVNS.begin() + bestTargetIDTo, testingRelocate);
		}
		double afterChangeLength = actualDOPN.getPathLength();
		if (fabs(afterChangeLength - minLength) > 0.1) {
			ERROR("diff in afterChangeLength and minLength");
			ERROR("insertSystematic from "<<bestTargetIDFrom<<" to "<<bestTargetIDTo);
			ERROR("id "<<testingRelocate.id);
			ERROR(afterChangeLength <<" and "<<minLength);
			exit(1);
		}
		if (fabs(actualReward + maxAddReward - actualDOPN.getReward()) > 0.1) {
			ERROR("reward does not match");
			ERROR("insertSystematic from "<<bestTargetIDFrom<<" to "<<bestTargetIDTo);
			ERROR("id "<<testingRelocate.id);
			ERROR((actualReward + maxAddReward)<< " and "<< actualDOPN.getReward());
			exit(1);
		}
		return true;
	}
	return false;
}

bool VNSDOPN::exchangeSystematic(DOPN &actualDOPN, std::vector<GraphNode> &actualVNS, int k) {
	double actualLength = actualDOPN.getPathLength();
	double actualReward = actualDOPN.getReward();
	//INFO("exchangeSystematic begin length "<<actualLength);
	//INFO("exchangeSystematic begin reward "<<actualReward);
	//INFO_VAR(actualDOPN.getNumTargets())
	double minLength = actualLength;
	double maxAddReward = 0;
	int bestTargetIDFrom = -1;
	int bestTargetIDTo = -1;

	for (int targetIDFrom = 0; targetIDFrom < actualVNS.size(); ++targetIDFrom) {
		GraphNode testingRelocateFrom = actualVNS[targetIDFrom];

		for (int targetIDTo = 0; targetIDTo < actualDOPN.getNumTargets(); ++targetIDTo) {
			if (abs(targetIDFrom - targetIDTo) > 2) {
				GraphNode testingRelocateTo = actualVNS[targetIDTo];
				double addedReward = 0;
				double lengthAfterMove = 0;
				if (targetIDFrom < actualDOPN.getNumTargets() && targetIDTo < actualDOPN.getNumTargets()) {
					lengthAfterMove = actualDOPN.tryToExchange(targetIDFrom, targetIDTo);
				} else {
					double lengthAfterReplaceTo = actualLength;
					double addedAfterReplaceTo = 0;
					if (targetIDTo < actualDOPN.getNumTargets()) {
						lengthAfterReplaceTo = actualDOPN.tryToReplace(testingRelocateFrom, targetIDTo).actual_len;
						addedAfterReplaceTo = lengthAfterReplaceTo - actualLength;
						addedReward += testingRelocateFrom.reward - testingRelocateTo.reward;
					}
					double lengthAfterReplaceFrom = actualLength;
					double addedAfterReplaceFrom = 0;
					if (targetIDFrom < actualDOPN.getNumTargets()) {
						lengthAfterReplaceFrom = actualDOPN.tryToReplace(testingRelocateTo, targetIDFrom).actual_len;
						addedAfterReplaceFrom = lengthAfterReplaceFrom - actualLength;
						addedReward += testingRelocateTo.reward - testingRelocateFrom.reward;
					}

					lengthAfterMove = actualLength + addedAfterReplaceTo + addedAfterReplaceFrom;
				}

				if (lengthAfterMove <= budget && (addedReward > maxAddReward || (fabs(addedReward - maxAddReward) < MIN_CHANGE_EPS && (minLength - lengthAfterMove) > MIN_CHANGE_EPS))) {

					maxAddReward = addedReward;
					minLength = lengthAfterMove;
					bestTargetIDFrom = targetIDFrom;
					bestTargetIDTo = targetIDTo;

					if (DEBUG_DOP_TRY_OPERATIONS) {
						INFO_VAR(DEBUG_DOP_TRY_OPERATIONS);
						//debug
						//TODO erase debug
						DOPN copyAdd = actualDOPN;
						//copyAdd.listIds();
						if (bestTargetIDTo > bestTargetIDFrom) {

							copyAdd.removePoint(bestTargetIDTo);
							copyAdd.addPoint(testingRelocateFrom, bestTargetIDTo);

							if (bestTargetIDFrom < actualDOPN.getNumTargets()) {
								copyAdd.removePoint(bestTargetIDFrom);
								copyAdd.addPoint(testingRelocateTo, bestTargetIDFrom);
								//INFO("lengthAfterReplaceFrom copy "<<copyAdd.getPathLength());
							}
						} else {
							if (bestTargetIDFrom < actualDOPN.getNumTargets()) {
								copyAdd.removePoint(bestTargetIDFrom);
								copyAdd.addPoint(testingRelocateTo, bestTargetIDFrom);

							}
							copyAdd.removePoint(bestTargetIDTo);
							copyAdd.addPoint(testingRelocateFrom, bestTargetIDTo);

						}
						double lengthAfterAddCopy = copyAdd.getPathLength();
						if (fabs(lengthAfterAddCopy - lengthAfterMove) > 0.1) {
							ERROR("lengthAfterAddCopy does not match lengthAfterAdd");
							ERROR("exchangeSystematic from "<<bestTargetIDFrom<<" to "<<bestTargetIDTo);
							ERROR("should be "<<lengthAfterAddCopy);
							ERROR(" is "<<lengthAfterMove);

							exit(1);
						}
						if (fabs(actualReward + maxAddReward - copyAdd.getReward()) > 0.1) {
							ERROR("reward does not match");
							ERROR("exchangeSystematic from "<<bestTargetIDFrom<<" to "<<bestTargetIDTo);
							ERROR((actualReward + maxAddReward)<< " and "<< copyAdd.getReward());
							exit(1);
						}
					}

				}

			}

		}
	}

	if (bestTargetIDFrom != -1) {

		GraphNode testingRelocateFrom = actualVNS[bestTargetIDFrom];
		GraphNode testingRelocateTo = actualVNS[bestTargetIDTo];

		if (bestTargetIDTo > bestTargetIDFrom) {
			//INFO("add point "<<bestTargetIDTo);

			actualDOPN.removePoint(bestTargetIDTo);
			actualDOPN.addPoint(testingRelocateFrom, bestTargetIDTo);

			if (bestTargetIDFrom < actualDOPN.getNumTargets()) {
				actualDOPN.removePoint(bestTargetIDFrom);
				actualDOPN.addPoint(testingRelocateTo, bestTargetIDFrom);
			}
			GraphNode temp = actualVNS[bestTargetIDTo];
			actualVNS[bestTargetIDTo] = actualVNS[bestTargetIDFrom];
			actualVNS[bestTargetIDFrom] = temp;

		} else {

			if (bestTargetIDFrom < actualDOPN.getNumTargets()) {
				actualDOPN.removePoint(bestTargetIDFrom);
				actualDOPN.addPoint(testingRelocateTo, bestTargetIDFrom);
			}

			actualDOPN.removePoint(bestTargetIDTo);
			actualDOPN.addPoint(testingRelocateFrom, bestTargetIDTo);

			GraphNode temp = actualVNS[bestTargetIDTo];
			actualVNS[bestTargetIDTo] = actualVNS[bestTargetIDFrom];
			actualVNS[bestTargetIDFrom] = temp;
		}

		double afterChangeLength = actualDOPN.getPathLength();

		if (fabs(afterChangeLength - minLength) > 0.1) {
			ERROR("diff in afterChangeLength and minLength");
			ERROR("exchangeSystematic from "<<bestTargetIDFrom<<" to "<<bestTargetIDTo);
			ERROR(afterChangeLength <<" and "<<minLength);
			exit(1);
		}
		if (fabs(actualReward + maxAddReward - actualDOPN.getReward())) {
			ERROR("reward does not match");
			ERROR("exchangeSystematic from "<<bestTargetIDFrom<<" to "<<bestTargetIDTo);
			ERROR((actualReward + maxAddReward)<< " and "<< actualDOPN.getReward());
			exit(1);
		}
		return true;
	}
	return false;
}

void VNSDOPN::pathInsert(DOPN &actualDOPN, std::vector<GraphNode> &actualVNS, int numExchanges) {

	const int sizeV = actualVNS.size() - 1;
	if (sizeV >= 2) {
		for (int var = 0; var < numExchanges; ++var) {
			std::unordered_set<int> randNumbersSet;
			while (randNumbersSet.size() < 3) {
				randNumbersSet.insert(randIntMinMax(0, sizeV));
			}
			std::vector<int> randNumbers;
			int i = 0;
			for (auto it = randNumbersSet.begin(); it != randNumbersSet.end(); ++it) {
				randNumbers.push_back(*it);
			}
			CSort<int>::quicksort(&randNumbers, 0, randNumbers.size() - 1);

			int exchangeFromStart = randNumbers[0];
			int exchangeFromEnd = randNumbers[1];
			int insertTo = randNumbers[2];
			bool insertUp = true;
			if (randIntMinMax(0, 1) == 1) {
				insertTo = randNumbers[0];
				exchangeFromStart = randNumbers[1];
				exchangeFromEnd = randNumbers[2];
				insertUp = false;
			}

			if (insertUp) {
				//insert up

				std::vector<GraphNode> newVec(actualVNS.begin() + exchangeFromStart, actualVNS.begin() + exchangeFromEnd + 1);

				actualVNS.insert(actualVNS.begin() + insertTo, newVec.begin(), newVec.end());
				actualVNS.erase(actualVNS.begin() + exchangeFromStart, actualVNS.begin() + exchangeFromEnd + 1);

				if (insertTo < actualDOPN.getNumTargets()) {
					actualDOPN.addPoint(newVec, insertTo);
				}
				if (exchangeFromStart < actualDOPN.getNumTargets()) {
					actualDOPN.removePoint(exchangeFromStart, MIN(exchangeFromEnd, actualDOPN.getNumTargets() - 1));
				}

			} else {
				//insert bellow

				std::vector<GraphNode> newVec(actualVNS.begin() + exchangeFromStart, actualVNS.begin() + exchangeFromEnd + 1);

				actualVNS.erase(actualVNS.begin() + exchangeFromStart, actualVNS.begin() + exchangeFromEnd + 1);
				actualVNS.insert(actualVNS.begin() + insertTo, newVec.begin(), newVec.end());

				if (exchangeFromStart < actualDOPN.getNumTargets()) {
					actualDOPN.removePoint(exchangeFromStart, MIN(exchangeFromEnd, actualDOPN.getNumTargets() - 1));
				}
				if (insertTo < actualDOPN.getNumTargets()) {
					actualDOPN.addPoint(newVec, insertTo);
				}

			}

			fitDOPtoBudget(actualDOPN, actualVNS);

			//actualDOPN.evaluateUsage();
		}
	}
//INFO("pathInsert "<<numExchanges<<" end");
}

void VNSDOPN::pathExchange(DOPN &actualDOPN, std::vector<GraphNode> &actualVNS, int numExchanges) {
//INFO("pathExchange "<<numExchanges<<" begin");
	const int sizeV = actualVNS.size() - 1;
	if (sizeV >= 3) {
		for (int var = 0; var < numExchanges; ++var) {
			std::unordered_set<int> randNumbersSet;
			while (randNumbersSet.size() < 4) {
				randNumbersSet.insert(randIntMinMax(0, sizeV));
			}
			std::vector<int> randNumbers;
			for (auto it = randNumbersSet.begin(); it != randNumbersSet.end(); ++it) {
				randNumbers.push_back(*it);
			}
			CSort<int>::quicksort(&randNumbers, 0, randNumbers.size() - 1);

			int exchangeFromStart = randNumbers[0];
			int exchangeFromEnd = randNumbers[1];
			int exchangeToStart = randNumbers[2];
			int exchangeToEnd = randNumbers[3];
			bool exchangeUp = true;
			if (randIntMinMax(0, 1) == 1) {
				exchangeFromStart = randNumbers[2];
				exchangeFromEnd = randNumbers[3];
				exchangeToStart = randNumbers[0];
				exchangeToEnd = randNumbers[1];
				exchangeUp = false;
			}

			if (exchangeUp) {
				std::vector<GraphNode> newVecFrom(actualVNS.begin() + exchangeFromStart, actualVNS.begin() + exchangeFromEnd + 1);		//is below
				std::vector<GraphNode> newVecTo(actualVNS.begin() + exchangeToStart, actualVNS.begin() + exchangeToEnd + 1);		//is above

				actualVNS.erase(actualVNS.begin() + exchangeToStart, actualVNS.begin() + exchangeToEnd + 1);
				actualVNS.insert(actualVNS.begin() + exchangeToStart, newVecFrom.begin(), newVecFrom.end());
				actualVNS.erase(actualVNS.begin() + exchangeFromStart, actualVNS.begin() + exchangeFromEnd + 1);
				actualVNS.insert(actualVNS.begin() + exchangeFromStart, newVecTo.begin(), newVecTo.end());

				//INFO("add newVecFrom");
				if (exchangeToStart < actualDOPN.getNumTargets()) {
					actualDOPN.removePoint(exchangeToStart, MIN(exchangeToEnd, actualDOPN.getNumTargets() - 1));
					actualDOPN.addPoint(newVecFrom, exchangeToStart);
				}
				//INFO("add newVecTo");
				if (exchangeFromStart < actualDOPN.getNumTargets()) {
					actualDOPN.removePoint(exchangeFromStart, MIN(exchangeFromEnd, actualDOPN.getNumTargets() - 1));
					actualDOPN.addPoint(newVecTo, exchangeFromStart);
				}

			} else {
				std::vector<GraphNode> newVecFrom(actualVNS.begin() + exchangeFromStart, actualVNS.begin() + exchangeFromEnd + 1);
				std::vector<GraphNode> newVecTo(actualVNS.begin() + exchangeToStart, actualVNS.begin() + exchangeToEnd + 1);

				actualVNS.erase(actualVNS.begin() + exchangeFromStart, actualVNS.begin() + exchangeFromEnd + 1);
				actualVNS.insert(actualVNS.begin() + exchangeFromStart, newVecTo.begin(), newVecTo.end());

				actualVNS.erase(actualVNS.begin() + exchangeToStart, actualVNS.begin() + exchangeToEnd + 1);
				actualVNS.insert(actualVNS.begin() + exchangeToStart, newVecFrom.begin(), newVecFrom.end());

				//INFO("size after "<<actualVNS.size());

				//INFO("add newVecTo");
				if (exchangeFromStart < actualDOPN.getNumTargets()) {
					actualDOPN.removePoint(exchangeFromStart, MIN(exchangeFromEnd, actualDOPN.getNumTargets() - 1));
					actualDOPN.addPoint(newVecTo, exchangeFromStart);
				}

				//INFO("add newVecFrom");
				if (exchangeToStart < actualDOPN.getNumTargets()) {
					actualDOPN.removePoint(exchangeToStart, MIN(exchangeToEnd, actualDOPN.getNumTargets() - 1));
					actualDOPN.addPoint(newVecFrom, exchangeToStart);
				}

			}

			fitDOPtoBudget(actualDOPN, actualVNS);

			//actualDOPN.evaluateUsage();
		}
	}
//INFO("pathExchange "<<numExchanges<<" end");
}

void VNSDOPN::insertion(DOPN &actualDOPN, std::vector<GraphNode> &actualVNS) {
	INFO("insertion begin");
	int waitTimeus = 1000;
	bool somethingAdded = true;

	while (somethingAdded) {
		//INFO("inserting");
		somethingAdded = false;
		double minimalAddDistPerReward = DBL_MAX;
		double actualLength = actualDOPN.getPathLength();
		double newLengthAfterAdd = actualLength;
		int idAvNodeMinimal = -1;
		int idTourNodeMinimal = -1;

		int dopSize = actualDOPN.getNumTargets();
		for (int idAvNode = dopSize; idAvNode < actualVNS.size(); ++idAvNode) {
			//try to add between start ang goal
			GraphNode testingNode = actualVNS[idAvNode];
			for (int idTourNode = 0; idTourNode < actualDOPN.getSize(); ++idTourNode) {
				//INFO("try to add idAvNode "<<idAvNode<<" to position "<<idTourNode);
				double newDistance = actualDOPN.tryToAdd(testingNode, idTourNode).actual_len;
				double additionalDistance = newDistance - actualLength;
				double additionalDistPerReward = additionalDistance / testingNode.reward;
				//INFO("distance "<<newDistance);
				if (additionalDistPerReward < minimalAddDistPerReward && newDistance <= budget) {
					//INFO("additionalDistPerReward bettew "<<additionalDistPerReward);
					minimalAddDistPerReward = additionalDistPerReward;
					newLengthAfterAdd = newDistance;
					idAvNodeMinimal = idAvNode;
					idTourNodeMinimal = idTourNode;
				}
			}
		}
		if (idAvNodeMinimal >= 0 && idTourNodeMinimal >= 0 && newLengthAfterAdd <= budget) {
			somethingAdded = true;
			//INFO("add idAvNode "<<idAvNodeMinimal<<" to position "<<idTourNodeMinimal);

			GraphNode inserting = actualVNS[idAvNodeMinimal];
			actualDOPN.addPoint(actualVNS[idAvNodeMinimal], idTourNodeMinimal);

			//INFO("improveNeighLocations after insert");
			//improveNeighLocations(actualDOPN, actualVNS, 1);
			//INFO("improveNeighLocations after insert end");

			if (idAvNodeMinimal > idTourNodeMinimal) {
				actualVNS.erase(actualVNS.begin() + idAvNodeMinimal);
				actualVNS.insert(actualVNS.begin() + idTourNodeMinimal, inserting);
			} else {
				actualVNS.insert(actualVNS.begin() + idTourNodeMinimal, inserting);
				actualVNS.erase(actualVNS.begin() + idAvNodeMinimal);
			}

			drawPath(waitTimeus);
		}

	}

	checkConsistency(actualDOPN, actualVNS);
	INFO("insertion end");
}

/*
 bool VNSDOPN::twoopt(DOPN &actualDOPN, std::vector<GraphNode> &actualVNS) {
 //INFO_GREEN("test 2opt");
 bool changePerformed = true;
 int numTwoOpts = 0;
 while (changePerformed) {
 changePerformed = false;
 int numTargets = actualDOPN.getNumTargets();
 double actualLength = actualDOPN.getPathLength();
 double minimalLength2Opt = actualLength;
 int minTwoOptI = -1;
 int minTwoOptJ = -1;
 //INFO("numTargets before 2opt "<<numTargets);
 for (int i = 0; i < numTargets - 2; i++) {
 for (int j = i + 2; j < numTargets; j++) {
 double optLength = actualDOPN.tryTwoOpt(i, j);
 if (optLength < minimalLength2Opt) {
 minimalLength2Opt = optLength;
 minTwoOptI = i;
 minTwoOptJ = j;
 }
 }
 }
 //INFO("actualLength "<<actualLength);
 //INFO("minimalLength2Opt "<<minimalLength2Opt);
 if (minimalLength2Opt < actualLength) {
 actualDOPN.twoOpt(minTwoOptI, minTwoOptJ);
 //INFO("lengthAfter TwoOpt "<<actualDOPN.getPathLength());
 std::vector<GraphNode> actualVNSCopy = actualVNS;
 actualVNS.clear();
 for (int i = 0; i <= minTwoOptI - 1; i++) {
 actualVNS.push_back(actualVNSCopy[i]);
 }
 for (int i = minTwoOptJ; i >= minTwoOptI; i--) {
 actualVNS.push_back(actualVNSCopy[i]);
 }
 for (int i = minTwoOptJ + 1; i < actualVNSCopy.size(); i++) {
 actualVNS.push_back(actualVNSCopy[i]);
 }
 numTwoOpts++;
 drawPath(500000, &actualDOPN);
 if (fabs(actualDOPN.getPathLength() - minimalLength2Opt) > MIN_CHANGE_EPS) {
 INFO_RED("error different twoOpt "<<actualDOPN.getPathLength()<<" and tryTwoOpt "<<minimalLength2Opt);

 exit(0);
 }
 changePerformed = true;
 }
 //drawPath(1000000);

 }

 INFO("twoopt done "<<numTwoOpts<<" times");

 return (numTwoOpts > 0);
 }
 */

std::vector<GraphNode> VNSDOPN::getReachableNodes() {
	INFO("getReachableNodes num nodes for insetion "<<nodesForInsertion.size());
	std::vector<GraphNode> reachableNodes_;

	DOPN sgDOPN = DOPN(startNode, goalNode, radius, resolution, neighborhood_radius, neighborhood_resolution, null_start_goal_radius);

	for (int var = 0; var < nodesForInsertion.size(); ++var) {
		//INFO("sgDOPN.tryToAdd");
		double distanceNode = sgDOPN.tryToAdd(nodesForInsertion[var], 0).actual_len;
		//INFO(var<<" distanceNode "<<distanceNode);
		if (distanceNode <= this->budget) {
			reachableNodes_.push_back(nodesForInsertion[var]);
		}
	}
	INFO("getReachableNodes end");
	return reachableNodes_;
}

int VNSDOPN::greaterThanDistanceStartGoal(GraphNode gn1, GraphNode gn2, struct SortCompareVar<GraphNode> data) {
	StartGoalNodes * sgNodes = (StartGoalNodes *) data.data;
	double distanceSG1 = sgNodes->start.distanceTo(gn1) + gn1.distanceTo(sgNodes->goal);
	double distanceSG2 = sgNodes->start.distanceTo(gn2) + gn2.distanceTo(sgNodes->goal);
	if (distanceSG1 > distanceSG2) {
		return 1;
	} else if (distanceSG1 < distanceSG2) {
		return -1;
	} else {
		return 0;
	}
}

double VNSDOPN::getTourLength(std::vector<GraphNode> tour) {
	double distance = 0;
	for (int var = 0; var < tour.size() - 1; ++var) {
		distance += tour[var].distanceTo(tour[var + 1]);
	}
	return distance;
}

double VNSDOPN::getTourReward(std::vector<GraphNode> tour) {
	double price = 0;
	for (int var = 0; var < tour.size(); ++var) {
		price += tour[var].reward;
	}
	return price;
}

void VNSDOPN::checkConsistency(DOPN &actualDOPN, std::vector<GraphNode> &actualVNS) {
	INFO("checkConsistency");

	if (actualDOPN.getPathLength() > budget) {
		ERROR("inconsistent actualDOPN is over budget "<<actualDOPN.getPathLength()<<" > "<<budget);
		exit(1);
	}

	for (int var1 = 0; var1 < actualVNS.size(); ++var1) {
		if (var1 < actualDOPN.getNumTargets()) {
			if (actualVNS[var1].id != actualDOPN.getTarget(var1).id) {
				ERROR("inconsistent actualVNS not same as actualDOPN at position"<<var1<<" ids are "<<actualVNS[var1].id<<" and "<<actualDOPN.getTarget(var1).id);
				exit(1);
			}
		}
		for (int var2 = var1 + 1; var2 < actualVNS.size(); ++var2) {

			if (actualVNS[var1].id == actualVNS[var2].id) {
				ERROR("inconsistent actualVNS repeat "<<var1<<" and "<<var2);
				for (int nodeID = 0; nodeID < actualVNS.size(); ++nodeID) {
					INFO(nodeID<<" id "<<actualVNS[nodeID].id);
				}

				exit(1);
			}
		}
	}

}

std::string VNSDOPN::getVersion(void) {
	return "OPFourPhaseHeuristic 0.1";
}

void VNSDOPN::load(void) {
// nothing to load, structures are passed to the constructor
	INFO("load");
	int n = nodesAll.size();
	if (canvas) {
		CoordsVector points;
		//INFO("BORDER " << BORDER);
		foreach(GraphNode station, nodesAll) {
			Coords coord_up(station.x + BORDER, station.y + BORDER);
			Coords coord_down(station.x - BORDER, station.y - BORDER);
			points.push_back(coord_up);
			points.push_back(coord_down);
		}
		*canvas << canvas::AREA;
		//INFO("draw points");
		foreach(Coords coords, points) {
			//INFO(coords.x<<" "<<coords.y);
			*canvas << coords;
		}
		INFO("set to canvas");
		*canvas << canvas::END;

		if (config.get<bool>("draw-stations")) {
			std::string pallete = config.get<std::string>("draw-targets-reward-palette");
			if (config.get<bool>("draw-targets-reward") and imr::isFile(pallete)) {
				imr::gui::CColorMap map;
				map.load(pallete);
				double minReward = DBL_MAX;
				double maxReward = -DBL_MAX;
				for (int var = 0; var < nodesAll.size(); ++var) {
					if (nodesAll[var].reward < minReward) {
						minReward = nodesAll[var].reward;
					}
					if (nodesAll[var].reward > maxReward) {
						maxReward = nodesAll[var].reward;
					}
				}
				if (minReward == maxReward) {
					minReward = 0.99 * maxReward;
				}
				map.setRange(minReward, maxReward);
				*canvas << "targets" << CShape(config.get<std::string>("draw-shape-targets")) << canvas::POINT;
				for (GraphNode station : nodesAll) {
					//DEBUG("reward: " << target->reward << " color: " << map.getColor((double)target->reward));
					Coords cord(station.x, station.y);
					*canvas << imr::gui::canvas::FILL_COLOR << map.getColor((double) station.reward) << cord;

				}
				double radius = config.get<double>("neighborhood-radius");
				if (radius > 0) {
					for (GraphNode station : nodesAll) {
						if (station.id != startIndex && station.id != goalIndex) {
							Point point(station.x, station.y);

							//INFO_VAR(20 * radius);
							*canvas << "neighbours";
							*canvas << imr::gui::Shape::GREEN_POINT << imr::gui::canvas::PEN_ALPHA << 0.2 << imr::gui::canvas::FILL_ALPHA << 0.2 << imr::gui::canvas::FILL_COLOR
									<< map.getColor((double) station.reward) << canvas::ARC << Fill(false) << station.x << station.y << radius << (double) 0 << (2 * M_PI);
						}
					}
				}
			} else {
				//INFO("draw stations");
				*canvas << "stations" << CShape(config.get<std::string>("draw-shape-stations")) << canvas::POINT;
				for (GraphNode station : nodesAll) {

					//INFO(cord.x<<" ; "<<cord.y);
					Coords cord(station.x, station.y);
					*canvas << cord << canvas::END;
				}
			}
		}

		canvas->redraw();
		//usleep(500000);
	} //end canvas
}

void VNSDOPN::drawNeighborhoodPoints() {
	if (canvas && draw_neighborhood_points) {
		//INFO("drawNeighborhoodPoints");
		usleep(2000);
		*canvas << canvas::CLEAR << "neighborhoodp" << "neighborhoodp" << canvas::POINT;
		CShape neighborhoodPoint("blue", "blue", 1, 1);
		for (int nodeID = 0; nodeID < DOPN::getAllSamples()->size(); ++nodeID) {
			//INFO_VAR(nodeID);
			std::vector<std::vector<GraphNode_AngNeigh>>& neighborhood = DOPN::getAllSamples()->at(nodeID);
			const int neighborhoodSize = neighborhood.size();
			//INFO_VAR(neighborhoodSize);
			for (int neighID = 0; neighID < neighborhoodSize; ++neighID) {
				//INFO(neighID<<" draw neighborhood pos "<<neighborhood[neighID][0].node.x<<" "<<neighborhood[neighID][0].node.y);
				Coords cord(neighborhood[neighID][0].node.x, neighborhood[neighID][0].node.y);
				*canvas << neighborhoodPoint << cord;
			}
		}
		*canvas << canvas::END;
		canvas->redraw();
		usleep(2000);
		//INFO("drawNeighborhoodPoints end");
	}
}

/// - private method -----------------------------------------------------------
void VNSDOPN::drawPath(int usleepTime, DOPN * toShow) {
	//INFO("drawPath begin");
//CoordsVector path;
//get_ring_path(step, path);
	if (canvas) {
		*canvas << canvas::CLEAR << "path" << "path";
		//INFO("path cleared");

		std::vector<Dubins> pathDubins = this->tourDOPN.getPath();
		if (toShow != NULL) {
			pathDubins = toShow->getPath();
		}

		//INFO("pathDubins size "<<pathDubins.size());
		CShape blackPoint("black", "black", 1, 4);
		CShape blackLine("black", "black", 1, 0);
		for (int var = 0; var < pathDubins.size(); ++var) {
			//INFO("draw point start "<<pathDubins[var].start.point);
			//INFO("draw point end "<<pathDubins[var].end.point);

			//*canvas << imr::gui::Shape::GREEN_POINT << pathDubins[var].start.point;
			//*canvas << imr::gui::Shape::GREEN_POINT << pathDubins[var].end.point;
			*canvas << blackLine << pathDubins[var];
		}

		*canvas << canvas::CLEAR << "pathpoint" << "pathpoint" << canvas::POINT;
		for (int var = 0; var < pathDubins.size(); ++var) {

			Coords cordstart(pathDubins[var].start.point.x, pathDubins[var].start.point.y);
			*canvas << blackPoint << cordstart;
			Coords cordend(pathDubins[var].end.point.x, pathDubins[var].end.point.y);
			*canvas << blackPoint << cordend;
		}
		*canvas << canvas::END;

		if (neighborhood_radius > 0 && pathDubins.size() > 0) {
			SColor color;
			color.set(1.0, 1.0, 0.0, 0.2);
			*canvas << canvas::CLEAR << "sensorradius" << "sensorradius" << imr::gui::canvas::PEN_ALPHA << 0.0 << imr::gui::canvas::FILL_ALPHA << 0.3 << imr::gui::canvas::FILL_COLOR << color
					<< canvas::ARC;
			*canvas << pathDubins[0].start.point.x << pathDubins[0].start.point.y << neighborhood_radius << (double) 0 << (2 * M_PI);
			for (int var = 0; var < pathDubins.size(); ++var) {
				Coords cordend(pathDubins[var].end.point.x, pathDubins[var].end.point.y);
				*canvas << pathDubins[var].end.point.x << pathDubins[var].end.point.y << neighborhood_radius << 0.0 << (2 * M_PI);
			}
			*canvas << canvas::END;
		}

		canvas->redraw();
		//INFO("draw wait "<<usleepTime);
		usleep(usleepTime);

		drawNeighborhoodPoints();

	}
	//INFO("drawPath end");
}

void VNSDOPN::initialize(void) {

}

std::string VNSDOPN::getRevision(void) {
	return "$Id$";
}

void VNSDOPN::after_init(void) {

}

/// - protected method ---------------------------------------------------------
void VNSDOPN::save(void) {
	std::string dir;
//updateResultRecordTimes(); //update timers as load and initilization is outside class
//DEBUG("LOAD_TIME_CPU: " << tLoad.cpuTime());
//DEBUG("INIT_TIME_CPU: " << tInit.cpuTime());
//DEBUG("SAVE_TIME_CPU: " << tSave.cpuTime());
	DEBUG("SOLVE_TIME_CPU: " << tSolve.cpuTime());
	if (SAVE_SETTINGS) {
		saveSettings(getOutputIterPath(config.get<std::string>("settings"), dir));
	}
	if (SAVE_INFO) {
		saveInfo(getOutputIterPath(config.get<std::string>("info"), dir));
	}
	if (SAVE_RESULTS) {
		std::string file = getOutputIterPath(config.get<std::string>("result-path"), dir);
		imr::assert_io(createDirectory(dir), "Can not create file in path'" + file + "'");
		std::ofstream ofs(file.c_str());
		imr::assert_io(ofs.good(), "Cannot create path '" + file + "'");
		ofs << std::setprecision(14);

		std::vector<Dubins> finalPath = finalTourDOPN.getPath();
		foreach(const Dubins &pt, finalPath) {
			ofs << pt << std::endl;
			//ofs << pt.getStart().point.x << " " << pt.getStart().point.y << std::endl;
			//ofs << pt.getEnd().point.x << " " << pt.getEnd().point.y << std::endl;
		}

		imr::assert_io(ofs.good(), "Error occur during path saving");
		ofs.close();
	}

	std::string file = getOutputIterPath(config.get<std::string>("sampled-path"), dir);
	imr::assert_io(createDirectory(dir), "Can not create file in path'" + file + "'");
	std::ofstream ofs(file.c_str());
	imr::assert_io(ofs.good(), "Cannot create path '" + file + "'");
	ofs << std::setprecision(14);

	std::string file2 = getOutputIterPath("dop.txt", dir);
	imr::assert_io(createDirectory(dir), "Can not create file in path'" + file2 + "'");
	std::ofstream ofs2(file2.c_str());
	imr::assert_io(ofs2.good(), "Cannot create path '" + file2 + "'");
	ofs2 << std::setprecision(14);

	std::vector<Dubins> finalPath = finalTourDOPN.getPath();
	std::vector<State> samples = finalTourDOPN.getPathSampled(config.get<double>("sampled-path-distance"));
	for (int var = 0; var < samples.size(); ++var) {
		ofs << samples[var].point.x << DD << samples[var].point.y << DD << samples[var].ang << DD << std::endl;
	}
	imr::assert_io(ofs.good(), "Error occur during path saving");
	ofs.close();

	for (int var = 0; var < samples.size(); ++var) {
		ofs2 << samples[var].point.x << " " << samples[var].point.y << " " << samples[var].ang << std::endl;
	}

	imr::assert_io(ofs2.good(), "Error occur during path saving");
	ofs2.close();

	/*
	 std::string udageFile = getOutputIterPath("usage.txt", dir);
	 std::ofstream ofsUsage(udageFile.c_str());
	 imr::assert_io(ofsUsage.good(), "Cannot create path '" + udageFile + "'");
	 ofsUsage << std::setprecision(14);

	 INFO("find min max");
	 double maxLength = -std::numeric_limits<double>::max();
	 double minLength = std::numeric_limits<double>::max();
	 int numCounts = 100;
	 std::vector<double> histcounts(numCounts);
	 histcounts.assign(numCounts, 0);

	 double nextPart = 0.1;
	 for (int gnid1 = 0; gnid1 < DOPN::allDistances.size(); ++gnid1) {
	 if (gnid1 / ((float) DOPN::allDistances.size()) > nextPart) {
	 INFO("calc distances "<<(100*nextPart)<<"%");
	 nextPart += 0.1;
	 }
	 int gn1Id = nodesAll[gnid1].id;
	 for (int gnid2 = 0; gnid2 < DOPN::allDistances[gnid1].size(); ++gnid2) {
	 //allDistances[gnid1][gnid2].resize(resolution);
	 int gn2Id = nodesAll[gnid2].id;
	 if (gn1Id != gn2Id && !(gn1Id == 0 && gn2Id == 1) && !(gn1Id == 1 && gn2Id == 0)) {
	 for (int neighID1 = 0; neighID1 < DOPN::allDistances[gnid1][gnid2].size(); ++neighID1) {
	 for (int neighID2 = 0; neighID2 < DOPN::allDistances[gnid1][gnid2][neighID1].size(); ++neighID2) {
	 for (int idx1 = 0; idx1 < DOPN::allDistances[gnid1][gnid2][neighID1][neighID2].size(); ++idx1) {
	 //allDistances[gnid1][gnid2][idx1].resize(resolution);
	 for (int idx2 = 0; idx2 < DOPN::allDistances[gnid1][gnid2][neighID1][neighID2][idx1].size(); ++idx2) {

	 if (DOPN::allDistances[gnid1][gnid2][neighID1][neighID2][idx1][idx2] > maxLength) {
	 maxLength = DOPN::allDistances[gnid1][gnid2][neighID1][neighID2][idx1][idx2];
	 }
	 if (DOPN::allDistances[gnid1][gnid2][neighID1][neighID2][idx1][idx2] < minLength) {
	 minLength = DOPN::allDistances[gnid1][gnid2][neighID1][neighID2][idx1][idx2];
	 }

	 }
	 }
	 }
	 }
	 }
	 }
	 }

	 INFO("create histogram");
	 nextPart = 0.1;
	 for (int gnid1 = 0; gnid1 < DOPN::allDistances.size(); ++gnid1) {
	 if (gnid1 / ((float) DOPN::allDistances.size()) > nextPart) {
	 INFO("calc distances "<<(100*nextPart)<<"%");
	 nextPart += 0.1;
	 }
	 int gn1Id = nodesAll[gnid1].id;
	 for (int gnid2 = 0; gnid2 < DOPN::allDistances[gnid1].size(); ++gnid2) {
	 //allDistances[gnid1][gnid2].resize(resolution);
	 int gn2Id = nodesAll[gnid2].id;
	 if (gn1Id != gn2Id && !(gn1Id == 0 && gn2Id == 1) && !(gn1Id == 1 && gn2Id == 0)) {
	 for (int neighID1 = 0; neighID1 < DOPN::allDistances[gnid1][gnid2].size(); ++neighID1) {
	 for (int neighID2 = 0; neighID2 < DOPN::allDistances[gnid1][gnid2][neighID1].size(); ++neighID2) {
	 for (int idx1 = 0; idx1 < DOPN::allDistances[gnid1][gnid2][neighID1][neighID2].size(); ++idx1) {
	 //allDistances[gnid1][gnid2][idx1].resize(resolution);
	 for (int idx2 = 0; idx2 < DOPN::allDistances[gnid1][gnid2][neighID1][neighID2][idx1].size(); ++idx2) {

	 for (int count = 1; count <= numCounts; ++count) {
	 double actualOver = minLength + ((double) (count - 1) / (double) numCounts) * (maxLength - minLength);
	 double actualBelow = minLength + ((double) (count) / (double) numCounts) * (maxLength - minLength);
	 if (DOPN::allDistances[gnid1][gnid2][neighID1][neighID2][idx1][idx2] > actualOver
	 && DOPN::allDistances[gnid1][gnid2][neighID1][neighID2][idx1][idx2] < actualBelow) {
	 histcounts[count - 1] += 1;
	 }
	 }

	 }
	 }
	 }
	 }
	 }
	 }
	 }

	 for (int var = 0; var < numCounts; ++var) {
	 ofsUsage << var << DD << histcounts[var] << std::endl;
	 //ofsUsage << gn1Id << DD << gn2Id << DD << neighID1 << DD << neighID2 << DD << idx1 << DD << idx2 << DD
	 //		<< DOPN::allDistances[gnid1][gnid2][neighID1][neighID2][idx1][idx2] << DD << DOPN::allDistancesUsage[gnid1][gnid2][neighID1][neighID2][idx1][idx2]
	 //		<< std::endl;
	 }


	 imr::assert_io(ofsUsage.good(), "Error occur during path saving");
	 ofsUsage.close();
	 */

	if (SAVE_TARGETS) {
		std::string file = getOutputIterPath(config.get<std::string>("targets"), dir);
		imr::assert_io(createDirectory(dir), "Can not create file in path'" + file + "'");
		std::ofstream ofs(file.c_str());
		imr::assert_io(ofs.good(), "Cannot create path '" + file + "'");
		ofs << std::setprecision(14);
		for (int var = 0; var < nodesAll.size(); ++var) {
			ofs << nodesAll[var].id << DD << nodesAll[var].x << DD << nodesAll[var].y << DD << nodesAll[var].reward << std::endl;
		}
		imr::assert_io(ofs.good(), "Error occur during path saving");
		ofs.close();
	}
	if (canvas) { // map must be set
		*canvas << canvas::CLEAR << "ring";
		if (config.get<bool>("draw-path")) {
			drawPath();
		}
		saveCanvas();
	}
}

void VNSDOPN::release(void) {

}

void VNSDOPN::visualize(void) {

}

void VNSDOPN::defineResultLog(void) {
	static bool resultLogInitialized = false;
	if (!resultLogInitialized) {
		resultLog << result::newcol << "NAME";
		resultLog << result::newcol << "METHOD";
		resultLog << result::newcol << "CTIME";
		resultLog << result::newcol << "NUM_ITERS";
		resultLog << result::newcol << "BUDGET";
		resultLog << result::newcol << "RADIUS";
		resultLog << result::newcol << "RESOLUTION";
		resultLog << result::newcol << "NEIGHBORHOOD_RADIUS";
		resultLog << result::newcol << "NEIGHBORHOOD_RESOLUTION";
		resultLog << result::newcol << "REWARDS";
		resultLog << result::newcol << "MAX_ACHIEVABLE_REWARDS";
		resultLog << result::newcol << "LENGTH";
		resultLog << result::newcol << "NUM_ITERS_LAST_IMPR";
		resultLog << result::newcol << "CTIME_LAST_IMPR";
		resultLog << result::newcol << "MAX_ALLOWED_CALC_TIME_MS";
		resultLog << result::newcol << "RESULT_TARGET_IDS";
		resultLogInitialized = true;
	}
}

void VNSDOPN::fillResultRecord(int numIters, double length, int numItersLastImprovement, long timeLastImprovement) {
	long t[3] = { 0, 0, 0 };

	tSolve.addTime(t);

	double final_reward = this->finalTourDOPN.getReward();

	std::vector<std::vector<std::vector<GraphNode_Ang>>>samples = this->finalTourDOPN.getSamples();
	std::stringstream tourNodes;
	for (int var = 0; var < samples.size(); ++var) {
		if (var != 0) {
			tourNodes << ",";
		}
		tourNodes << samples[var][0][0].node.id;
	}

	double final_length = this->finalTourDOPN.getPathLength();

	resultLog << result::newrec << name << getMethod() << t[0] << numIters << this->budget << radius << resolution << neighborhood_radius << neighborhood_resolution << final_reward << maximalRewardAll
			<< final_length << numItersLastImprovement << timeLastImprovement << maximal_calculation_time_MS << tourNodes.str();

}

imr::CConfig & VNSDOPN::getConfig(imr::CConfig & config) {
// basic config
	config.add<std::string>("output", "output directory to store particular results and outputs", "./");
	config.add<std::string>("results", "result log file, it will be placed in output directory", "results.log");
	config.add<std::string>("info", "information file, it will be placed in particular experiment directory", "info.txt");
	config.add<std::string>("settings", "store configurations in boost::program_options config file format ", "settings.txt");
	config.add<std::string>("result-path", "file name for the final found path (ring) as sequence of points", "path.txt");
	config.add<std::string>("result-canvas-output", "file name for final canvas output (eps,png,pdf,svg) are supported");
	config.add<std::string>("result-canvas-suffixes",
			"comman separated list of exptensis e.g. png,pdf.  if specified  several result images are created, with particular suffix to the resultCanvasOutput");
	config.add<std::string>("name", "name used in result log as user identification if not set a default values (cities) is used");
	config.add<int>("iteration", "set particular interation, otherwise all interations (batch) are performed", -1);
	config.add<int>("batch", "number of iterations from 0 to batch (not included) ", -1);
	config.add<bool>("continue", "in batch mode, partial results are loaded and checked, only missing iterations are computed ", false);
	config.add<bool>("save-results", "disable/enable save results,configs and so on", true);
	config.add<bool>("save-info", "disable/enable save info", true);
	config.add<bool>("save-settings", "disable/enable save settings", true);

	config.add<bool>("save-visual", "disable/enable save visualization results, canvas be", true);
	config.add<bool>("verbose-result-log", "disable/enable printing results log into logger", false);
// end basic config
	config.add<double>("learning-rate", "neuron adaptation parametr in activation function mi*exp(d)", 0.6); //0.6

	config.add<double>("number-neurons-multiplication", "multiplication parametr to get number of neurons as multiplication of number of cities", 2.5);
	config.add<double>("gain-decreasing-rate", "Decrasing rate, higher value means faster convergence", 1e-4);
	config.add<double>("neighborhood-factor",
			"factor to compute number of neighborhood neurons as division of number of neurons, neighborhood are considered on both sides (left, right) of winning neuron so d*2 neurons are moved ",
			5);
	config.add<int>("termination-max-steps", "Stop adaptation if number of steps is large than given", 180);
	config.add<double>("termination-minimal-learning-rate", "Stop adaptation if the current learning rate is below this threshold", 0.4);
	config.add<double>("termination-error", "Stop adaptation when the current error is less than this value", 0.001);

	config.add<bool>("best-path", "Enable/disable considering best path found during the evaluation", false);

	config.add<std::string>("pic-dir", "relative directory in result directory to store pictures from each iteration");
	config.add<std::string>("pic-ext", "extension of pic, eps, png, pdf, svg (supported by particular gui renderer rendered", "png");
	config.add<bool>("save-pic", "enable/disable saving pictures (after each refine)");

	config.add<bool>("draw-stations", "Enable/Disable drawing stations", true);
	config.add<bool>("draw-ring", "Enable/Disable drawing ring in the final shoot", true);
	config.add<bool>("draw-path", "Enable/Disable drawing ring in the final shoot", true);
	config.add<double>("canvas-border", "Free space around the canvas", 10);
	config.add<std::string>("draw-shape-stations", "Shape of the station", Shape::CITY());
	config.add<std::string>("draw-shape-neuron", "Shape of the neurons", Shape::NEURON());
	config.add<std::string>("draw-shape-winner", "Shape of the winners", Shape::NEURON());
	config.add<std::string>("draw-shape-depot", "Shape of the depot", Shape::DEPOT());
	config.add<std::string>("draw-shape-path", "Shape of the path", Shape::RED_LINE());
	config.add<std::string>("draw-shape-ring", "Shape of the ring", Shape::GREEN_BOLD_LINE());
	config.add<std::string>("draw-shape-tour-represented-by-ring", Shape::BLACK_BOLD_LINE());

	config.add<bool>("draw-neurons", "enable/disable drawing neurons", false);
	config.add<bool>("draw-winners", "enable/disable drawing winner using a different shape", false);
	config.add<bool>("draw-ring-iter", "enable/disable drawing ring at each iteration", false);
	config.add<bool>("draw-path-vertices", "enable/disable drawing path vertices(nodes)", true);
	config.add<bool>("draw-tour-represented-by-ring", "enable/disable drawing tour represented by ring", false);

	config.add<int>("dop-initial-resolution", "intial resolution of dop", 64);
	config.add<double>("dop-radius", "radius of dubins vehicle", 0.5);
	config.add<bool>("save-targets", "disable/enable save targets", true);
	config.add<bool>("save-sampled-path", "disable/enable save sampled path", true);
	config.add<std::string>("targets", "file to save targets", "targets.txt");
	config.add<std::string>("sampled-path", "file to save sampled path", "sampled-path.txt");
	config.add<double>("sampled-path-distance", "distance between samples of path", 0.05);
	config.add<int>("num-iterations", "number of iteration used in VNS", 1000);
	config.add<int>("num-iterations-unimproved", "number of iteration used in VNS", 3000);
	config.add<bool>("use-rvns", "disable/enable randomized VNS", false);
	config.add<double>("neighborhood-radius", "radius of the neighborhood disc", 0.0);
	config.add<double>("neighborhood-resolution", "radius of the neighborhood disc", 1);
	config.add<bool>("dopn-null-start-goal-radius", "disable/enable null radius for start and goal", true);
	config.add<bool>("draw-neighborhood-points", "disable/enable draw points in neighborhoods", false);
	config.add<double>("maximal-calculation-time-min", "maximal time for calculation", 100);

	config.add<double>("heading-improvement-minimal-budget-ratio", "ratio of the budget that is minal for apliing heading improvement", 0.001);
	config.add<double>("neighborhood-improvement-minimal-budget-ratio", "ratio of the budget that is minal for apliing neighborhood improvement", 0.001);
	config.add<bool>("improve-neigh-during-local-search", "wheter improve neigh position during local search", false);

	config.add<bool>("draw-targets-reward", "enable/disable drawing targets in different color using penalty", false);
	config.add<std::string>("draw-targets-reward-palette", "File name with colors for the reward palette", "");
	config.add<std::string>("draw-shape-targets", "Shape of the target", CShape("black", "green", 1, 4).getStringOptions());
	return config;
}

int VNSDOPN::greaterThanGraphNodeReward(GraphNode gn1, GraphNode gn2, struct SortCompareVar<GraphNode> data) {
//INFO("greaterThanGraphNodeReward");
	if (gn1.reward < gn2.reward) {
		return 1;
	} else if (gn1.reward > gn2.reward) {
		return -1;
	} else {
		return 0;
	}
}

