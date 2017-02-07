/*
 * dtsp.cpp
 *
 *  Created on: Sep 25, 2016
 *      Author: Robert Penicka, Petr Vana
 */

#include "dopn.h"
#define MEMORIZE_ALL_DISTANCES
using imr::logger;

#define M (numeric_limits<double>::max())

using namespace std;

namespace opendubins {

std::vector<std::vector<std::vector<GraphNode_AngNeigh>>>DOPN::allSamples;
std::vector<std::vector<std::vector<std::vector<std::vector<std::vector<double>>>>> >DOPN::allDistances;

DOPN::DOPN(GraphNode start, GraphNode end, double radius, int resolution, double neighborhood_radius, int neighborhood_resolution, bool null_start_goal_radius_) :
		start(start), end(end), radius(radius), resolution(resolution), neighborhood_radius(neighborhood_radius), neighborhood_resolution(
				neighborhood_resolution), null_start_goal_radius(null_start_goal_radius_) {
	INFO("DOPN constructor");
	update();
}

DOPN::~DOPN() {

}

void DOPN::calcAllDistances(std::vector<GraphNode> allGraphNodes, double radius, int resolution, double neighborhood_radius, int neighborhood_resolution,
		bool null_start_goal_radius, int startIndex, int goalIndex) {
	INFO("calcAllDistances begin");
	const int S = allGraphNodes.size();
	allSamples.resize(S);
	for (int var = 0; var < S; ++var) {
		generateSamples(allSamples[var], allGraphNodes[var], resolution, neighborhood_radius, neighborhood_resolution, null_start_goal_radius, startIndex,
				goalIndex);
	}
	INFO("samples generated");
	allDistances.resize(S);
	//allDubins.resize(S);
	for (int gnid1 = 0; gnid1 < allGraphNodes.size(); ++gnid1) {
		allDistances[gnid1].resize(S);
		//allDubins[gnid1].resize(S);
		for (int gnid2 = 0; gnid2 < allGraphNodes.size(); ++gnid2) {
			//here between two targets
			int maxneighID1 = neighborhood_resolution;
			if (null_start_goal_radius && (gnid1 == startIndex || gnid1 == goalIndex)) {
				maxneighID1 = 1;
			}
			allDistances[gnid1][gnid2].resize(maxneighID1);
			//allDubins[gnid1][gnid2].resize(neighborhood_resolution);
			for (int neighID1 = 0; neighID1 < maxneighID1; ++neighID1) {
				int maxneighID2 = neighborhood_resolution;
				if (null_start_goal_radius && (gnid2 == startIndex || gnid2 == goalIndex)) {
					maxneighID2 = 1;
				}
				allDistances[gnid1][gnid2][neighID1].resize(maxneighID2);
				//allDubins[gnid1][gnid2][neighID1].resize(neighborhood_resolution);
				for (int neighID2 = 0; neighID2 < maxneighID2; ++neighID2) {
					//here between two points in 2D = points around target
					allDistances[gnid1][gnid2][neighID1][neighID2].resize(resolution);
					//allDubins[gnid1][gnid2][neighID1][neighID2].resize(resolution);
					for (int idx1 = 0; idx1 < resolution; ++idx1) {
						allDistances[gnid1][gnid2][neighID1][neighID2][idx1].resize(resolution);
						//allDubins[gnid1][gnid2][neighID1][neighID2][idx1].resize(resolution);
					}
				}
			}

		}
	}
	INFO("distance array resized");
	float nextPart = 0.1;
	int numNodes = 0;
	for (int gnid1 = 0; gnid1 < allGraphNodes.size(); ++gnid1) {
		if (gnid1 / ((float) allGraphNodes.size()) > nextPart) {
			INFO("calc distances "<<(100*nextPart)<<"%");
			nextPart += 0.1;
		}
		//allDistances[gnid1].resize(S);
		int gn1Id = allGraphNodes[gnid1].id;
		for (int gnid2 = 0; gnid2 < allGraphNodes.size(); ++gnid2) {
			//allDistances[gnid1][gnid2].resize(resolution);
			int gn2Id = allGraphNodes[gnid2].id;
			for (int neighID1 = 0; neighID1 < allSamples[gn1Id].size(); ++neighID1) {
				for (int neighID2 = 0; neighID2 < allSamples[gn2Id].size(); ++neighID2) {
					for (int idx1 = 0; idx1 < allSamples[gn1Id][neighID1].size(); ++idx1) {
						//allDistances[gnid1][gnid2][idx1].resize(resolution);
						for (int idx2 = 0; idx2 < allSamples[gn2Id][neighID2].size(); ++idx2) {
							GraphNode_AngNeigh & from = allSamples[gn1Id][neighID1][idx1];
							GraphNode_AngNeigh & to = allSamples[gn2Id][neighID2][idx2];
							Dubins dub(from.toState(), to.toState(), radius);
							allDistances[gn1Id][gn2Id][neighID1][neighID2][idx1][idx2] = dub.length;
							numNodes++;
							//allDubins[gn1.id][gn2.id][neighID1][neighID2][idx1][idx2] = dub;
						}
					}
				}
			}
		}
	}
	INFO("calc distances "<<(100)<<"%");
	INFO("totally we have " << numNodes << " number of nodes");
	//INFO("filled");
	INFO("calcAllDistances end");
}

std::vector<std::vector<std::vector<GraphNode_AngNeigh>>>* DOPN::getAllSamples() {
	return &allSamples;
}

void DOPN::checkEucllideanDistances() {
	for (int gnid1 = 0; gnid1 < allSamples.size(); ++gnid1) {
		//allDistances[gnid1].resize(S);
		int gn1Id = allSamples[gnid1][0][0].node.id;
		for (int gnid2 = 0; gnid2 < allSamples.size(); ++gnid2) {
			//allDistances[gnid1][gnid2].resize(resolution);
			int gn2Id = allSamples[gnid2][0][0].node.id;
			for (int neighID1 = 0; neighID1 < allSamples[gn1Id].size(); ++neighID1) {
				for (int neighID2 = 0; neighID2 < allSamples[gn2Id].size(); ++neighID2) {
					for (int idx1 = 0; idx1 < allSamples[gn1Id][neighID1].size(); ++idx1) {
						//allDistances[gnid1][gnid2][idx1].resize(resolution);
						for (int idx2 = 0; idx2 < allSamples[gn2Id][neighID2].size(); ++idx2) {
							GraphNode_AngNeigh & from = allSamples[gn1Id][neighID1][idx1];
							GraphNode_AngNeigh & to = allSamples[gn2Id][neighID2][idx2];
							double euclideanDistance = from.node.distanceTo(to.node);
							if (allDistances[gn1Id][gn2Id][neighID1][neighID2][idx1][idx2] != euclideanDistance) {
								INFO("different dubins and euclidean distance for zero radius");
								INFO_VAR(euclideanDistance);
								INFO_VAR(allDistances[gn1Id][gn2Id][neighID1][neighID2][idx1][idx2]);
								exit(1);
							}

						}
					}
				}
			}
		}
	}
}

void DOPN::checkDistancesAndSamples(double radius) {
	for (int gn1Id = 0; gn1Id < allSamples.size(); ++gn1Id) {
		for (int gn2Id = 0; gn2Id < allSamples.size(); ++gn2Id) {
			for (int neighID1 = 0; neighID1 < allSamples[gn1Id].size(); ++neighID1) {
				for (int neighID2 = 0; neighID2 < allSamples[gn2Id].size(); ++neighID2) {
					for (int idx1 = 0; idx1 < allSamples[gn1Id][neighID1].size(); ++idx1) {
						for (int idx2 = 0; idx2 < allSamples[gn2Id][neighID2].size(); ++idx2) {
							if (allSamples[gn1Id].size() != allDistances[gn1Id][gn2Id].size()) {
								ERROR("sizes does not match - 1");
								exit(1);
							}
							if (allSamples[gn1Id][neighID1].size() != allDistances[gn1Id][gn2Id][neighID1][neighID2].size()) {
								ERROR("sizes does not match - 2");
								exit(1);
							}

							GraphNode_AngNeigh & from = allSamples[gn1Id][neighID1][idx1];
							GraphNode_AngNeigh & to = allSamples[gn2Id][neighID2][idx2];
							Dubins dub(from.toState(), to.toState(), radius);
							if (gn1Id >= allDistances.size() || gn2Id >= allDistances[gn1Id].size() || neighID1 >= allDistances[gn1Id][gn2Id].size()
									|| neighID2 >= allDistances[gn1Id][gn2Id][neighID1].size() || idx1 >= allDistances[gn1Id][gn2Id][neighID1][neighID2].size()
									|| idx2 >= allDistances[gn1Id][gn2Id][neighID1][neighID2][idx1].size()
									|| fabs(allDistances[gn1Id][gn2Id][neighID1][neighID2][idx1][idx2] - dub.length) > 0.01) {
								ERROR("bad distance at position ");
								INFO_VAR(gn1Id);
								INFO_VAR(gn2Id);
								INFO_VAR(neighID1);
								INFO_VAR(neighID1);
								INFO_VAR(idx1);
								INFO_VAR(idx2);
								INFO_VAR(dub.length);
								INFO_VAR(allDistances[gn1Id][gn2Id][neighID1][neighID2][idx1][idx2]);
								exit(1);
							}

						}
					}
				}
			}
		}
	}
}

void DOPN::generateSamples(std::vector<std::vector<GraphNode_AngNeigh>> &samples, GraphNode p, double resolution, double neighborhood_radius,
		int neighborhood_resolution, bool null_start_goal_radius, int startIndex, int goalIndex) {
//INFO("generateSamples begin");
	double usedRadius = neighborhood_radius;
	if (null_start_goal_radius && (p.id == startIndex || p.id == goalIndex)) {
		samples.resize(1);
		usedRadius = 0;
	} else {
		samples.resize(neighborhood_resolution);
	}
	for (int neighID = 0; neighID < samples.size(); neighID++) {
		samples[neighID].resize(resolution);
		double neigh_ang = (2 * M_PI * neighID) / neighborhood_resolution;
		GraphNode p_neigh = p;
		p_neigh.x += usedRadius * cos(neigh_ang);
		p_neigh.y += usedRadius * sin(neigh_ang);
		for (int ang_i = 0; ang_i < resolution; ang_i++) {
			double ang = (2 * M_PI * ang_i) / resolution;
			samples[neighID][ang_i] = GraphNode_AngNeigh(p_neigh, ang, neigh_ang);
		}
	}
//INFO("generateSamples end");
}

int DOPN::getSize() const {
	return targets.size() + 1;
}

void DOPN::addPoint(GraphNode p, int idx) {
//INFO("addPoint single begin");
	if (idx > targets.size() || idx < 0) {
		INFO_RED("addPoint idx "<<idx<<" exceeds maximum "<<(targets.size()-1));
		exit(1);
	}

	//INFO("add point "<<p.id<<" between "<<samples[idx][0][0].node.id<<" and "<<samples[idx+1][0][0].node.id);
	//double lengthBef = getPathLength();
	targets.insert(targets.begin() + idx, p);
//INFO("inserted point "<<p.id <<" into "<<idx<<" position , target size "<<targets.size())
	updateAfterInsert(idx, idx);

	/*
	 double length = getPathLength();
	 update();
	 double length2 = getPathLength();
	 if (fabs(length - length2) > 0.001) {
	 ERROR("error in updateAfterInsert");
	 ERROR("lengthBef "<<lengthBef);
	 ERROR("counted -"<<length);
	 ERROR("real -"<<length2);
	 INFO("inserted point "<<p.id <<" into "<<idx<<" position , target size "<<targets.size());
	 exit(1);
	 }
	 */
//INFO("addPoint single end");
}

void DOPN::addPoint(std::vector<GraphNode> points, int idx) {
//INFO("addPoint multi begin");
	if (idx > targets.size() || idx < 0) {
		INFO_RED("addPoint idx "<<idx<<" exceeds maximum "<<(targets.size()-1));
		exit(1);
	}
	//double lengthBef = getPathLength();
	targets.insert(targets.begin() + idx, points.begin(), points.end());
//update(); //min len 68.7228      31
//INFO("addPoint multi end");
	updateAfterInsert(idx, idx + points.size() - 1);	//min len 68.7228      17

	/*
	 double length = getPathLength();
	 update();
	 double length2 = getPathLength();
	 if (fabs(length - length2) > 0.001) {
	 ERROR("error in updateAfterInsert");
	 ERROR("lengthBef "<<lengthBef);
	 ERROR("counted -"<<length);
	 ERROR("real -"<<length2);
	 exit(1);
	 }
	 */
}

void DOPN::addPoint(GraphNode p) {

	int idx = 0;
	double minLen = numeric_limits<double>::max();
	const int S = getSize();

	for (int i = 0; i < S; i++) {
		double newLen = tryToAdd(p, i).actual_len;
		if (newLen < minLen) {
			minLen = newLen;
			idx = i;
		}
	}
	addPoint(p, idx);
}

void DOPN::removePoint(int idx) {
//INFO("removePoint single begin");

	//INFO("removePoint id "<<samples[idx+1][0][0].node.id<<" that is between "<<samples[idx][0][0].node.id<<" and "<<samples[idx+2][0][0].node.id);
	if (idx >= targets.size() || idx < 0) {
		INFO_RED("getTarget idx "<<idx<<" exceeds maximum "<<(targets.size()-1));
		exit(1);
	}
	double lengthBef = getPathLength();
	targets.erase(targets.begin() + idx);

//INFO("removePoint single begin");
	updateAfterRemove(idx, idx);

	/*
	 double length = getPathLength();
	 update();
	 double length2 = getPathLength();
	 if (fabs(length - length2) > 0.001) {
	 ERROR("error in updateAfterInsert");
	 ERROR("lengthBef "<<lengthBef);
	 ERROR("counted -"<<length);
	 ERROR("real -"<<length2);
	 exit(1);
	 }
	 */
}

void DOPN::removePoint(int idxStart, int idxStop) {
//INFO("removePoint multi begin");
//INFO("removePoint("<<idxStart<<","<<idxStop<<")");

	//double lengthBef = getPathLength();
	targets.erase(targets.begin() + idxStart, targets.begin() + idxStop + 1);
//INFO("targets.size()"<<targets.size());
//update();
//INFO("removePoint multi end");
	updateAfterRemove(idxStart, idxStop); // - min len 68.7228      18

	/*
	 double length = getPathLength();
	 update();
	 double length2 = getPathLength();
	 if (fabs(length - length2) > 0.001) {
	 ERROR("error in updateAfterInsert");
	 ERROR("lengthBef "<<lengthBef);
	 ERROR("counted -"<<length);
	 ERROR("real -"<<length2);
	 exit(1);
	 }
	 */
}

double DOPN::optimizeHeading(int targetId, double minimal_improvement_distance) {
	int inSampleId = targetId + 1;
	std::vector<std::vector<GraphNode_AngNeigh>>& sample = this->samples.at(inSampleId);
	std::vector<NeighAngValuesIds> pathNeightIds = this->getPathNeighAngIds();

	int neighId = pathNeightIds[inSampleId].idNeigh;
	int angId = pathNeightIds[inSampleId].idAng;

	int neighIdPrev = pathNeightIds[inSampleId - 1].idNeigh;
	int angIdPrev = pathNeightIds[inSampleId - 1].idAng;
	GraphNode_AngNeigh angNeighPrev = samples[inSampleId - 1][neighIdPrev][angIdPrev];

	int neighIdNext = pathNeightIds[inSampleId + 1].idNeigh;
	int angIdNext = pathNeightIds[inSampleId + 1].idAng;
	GraphNode_AngNeigh angNeighNext = samples[inSampleId + 1][neighIdNext][angIdNext];

	GraphNode_AngNeigh editing_pos = sample[neighId][angId];
	State editingPosState = editing_pos.toState();

	double ang_min = sample[neighId][(angId - 1) % sample[neighId].size()].ang;
	double actual_ang = normalizeAngle(sample[neighId][angId].ang, ang_min, ang_min + M_2PI);
	double ang_max = normalizeAngle(sample[neighId][(angId + 1) % sample[neighId].size()].ang, ang_min, ang_min + M_2PI);

	//INFO_VAR(ang_min);
	//INFO_VAR(actual_ang);
	//INFO_VAR(ang_max);

	State previousState = angNeighPrev.toState();
	State nextState = angNeighNext.toState();
	Dubins dubPrev(previousState, editingPosState, radius);
	Dubins dubNext(editingPosState, nextState, radius);
	double actualDistance = dubPrev.length + dubNext.length;

	double angleChange = (ang_max - ang_min) / 2.0;
	double iterativeDistance = actualDistance;
	double iterativeActualAngle = actual_ang;
	while (angleChange > 0.01) {
		//INFO("angleChange="<<angleChange);
		double ang_to_set_1 = iterativeActualAngle + angleChange;
		double ang_to_set_2 = iterativeActualAngle - angleChange;
		double distanceAfter_1 = angDistanceAfter(editingPosState, ang_to_set_1, previousState, nextState);
		double distanceAfter_2 = angDistanceAfter(editingPosState, ang_to_set_2, previousState, nextState);
		//angles2.push_back(ang_to_set_1);
		//distances2.push_back(distanceAfter_1);
		//angles2.push_back(ang_to_set_2);
		//distances2.push_back(distanceAfter_2);
		if (distanceAfter_1 < iterativeDistance || distanceAfter_2 < iterativeDistance) {

			angleChange *= 2.0;
			if (distanceAfter_1 < distanceAfter_2) {
				iterativeActualAngle = ang_to_set_1;
				iterativeDistance = distanceAfter_1;
			} else {
				iterativeActualAngle = ang_to_set_2;
				iterativeDistance = distanceAfter_2;
			}
			//INFO("iterative improved to "<<iterativeDistance);
		} else {
			angleChange /= 2.0;
		}
	}

	double distanceImprovement = 0;
	if (fabs(actual_ang - iterativeActualAngle) > 0.05 && actualDistance - iterativeDistance > 0.01) {
		//if (actualDistance - iterativeDistance > minimal_improvement_distance) {
		editing_pos.ang = iterativeActualAngle;
		State editingPosStateAfter = editing_pos.toState();
		Dubins dubPrev(previousState, editingPosStateAfter, radius);
		Dubins dubNext(editingPosStateAfter, nextState, radius);
		double actualDistanceAfter = dubPrev.length + dubNext.length;
		if (fabs(actualDistanceAfter - iterativeDistance) > 0.01) {
			INFO("bad actualDistanceAfter-iterativeDistance ");
			INFO_VAR(actualDistanceAfter);
			INFO_VAR(iterativeDistance);
			exit(1);
		}
		//INFO_VAR(editing_pos.node.x);
		//INFO_VAR(editing_pos.node.y);
		if (iterativeActualAngle < actual_ang) {
			//insert before neighId for target in stample - at position inSampleId
			insertAngSample(inSampleId, neighId, angId, angId, editing_pos);
		} else {
			//insert after neighId
			insertAngSample(inSampleId, neighId, angId, angId + 1, editing_pos);
		}
		distanceImprovement = actualDistance - iterativeDistance;
	}

	return distanceImprovement;
}

void DOPN::insertAngSample(int inSampleId, int neighId, int angIdActual, int angIdWhereInsert, GraphNode_AngNeigh editing_pos) {
	int nodeId = editing_pos.node.id;
	//INFO("insert angle "<<editing_pos.ang);
	GraphNode_AngNeigh angSampleOld = allSamples[nodeId][neighId][angIdActual];

	//add olf ang to the new position and change ang
	allSamples[nodeId][neighId].insert(allSamples[nodeId][neighId].begin() + angIdWhereInsert, angSampleOld);
	allSamples[nodeId][neighId][angIdWhereInsert].ang = editing_pos.ang;

	//copy distances -- insert from old
	for (int otherNodeId = 0; otherNodeId < allDistances[nodeId].size(); ++otherNodeId) {
		for (int neighID2 = 0; neighID2 < allDistances[nodeId][otherNodeId][neighId].size(); ++neighID2) {
			std::vector<double> distancesAngles = allDistances[nodeId][otherNodeId][neighId][neighID2][angIdActual];
			allDistances[nodeId][otherNodeId][neighId][neighID2].insert(allDistances[nodeId][otherNodeId][neighId][neighID2].begin() + angIdWhereInsert,
					distancesAngles);
		}

		for (int otherNeighId = 0; otherNeighId < allDistances[otherNodeId][nodeId].size(); ++otherNeighId) {
			for (int otherAngId = 0; otherAngId < allDistances[otherNodeId][nodeId][otherNeighId][neighId].size(); ++otherAngId) {
				double distancesAngle = allDistances[otherNodeId][nodeId][otherNeighId][neighId][otherAngId][angIdActual];
				allDistances[otherNodeId][nodeId][otherNeighId][neighId][otherAngId].insert(
						allDistances[otherNodeId][nodeId][otherNeighId][neighId][otherAngId].begin() + angIdWhereInsert, distancesAngle);
			}
		}
	}

	//fill new distances
	for (int otherNodeId = 0; otherNodeId < allDistances[nodeId].size(); ++otherNodeId) {
		//calc distances inside allDistances[nodeId][otherNodeId][neighIdWhereInsert] that was inserted before
		for (int neighID2 = 0; neighID2 < allDistances[nodeId][otherNodeId][neighId].size(); ++neighID2) {
			for (int idx2 = 0; idx2 < allDistances[nodeId][otherNodeId][neighId][neighID2][angIdWhereInsert].size(); ++idx2) {
				GraphNode_AngNeigh & from = allSamples[nodeId][neighId][angIdWhereInsert];
				GraphNode_AngNeigh & to = allSamples[otherNodeId][neighID2][idx2];
				Dubins dub(from.toState(), to.toState(), radius);
				allDistances[nodeId][otherNodeId][neighId][neighID2][angIdWhereInsert][idx2] = dub.length;
			}

		}

		for (int otherNeighId = 0; otherNeighId < allDistances[otherNodeId][nodeId].size(); ++otherNeighId) {
			//calc distances inside allDistances[otherNodeId][nodeId][otherNeighId][neighIdWhereInsert] that was inserted before
			for (int idx1 = 0; idx1 < allDistances[otherNodeId][nodeId][otherNeighId][neighId].size(); ++idx1) {
				GraphNode_AngNeigh & from = allSamples[otherNodeId][otherNeighId][idx1];
				GraphNode_AngNeigh & to = allSamples[nodeId][neighId][angIdWhereInsert];
				Dubins dub(from.toState(), to.toState(), radius);
				allDistances[otherNodeId][nodeId][otherNeighId][neighId][idx1][angIdWhereInsert] = dub.length;
			}
		}
	}

	this->update();
}

NeighImprovement DOPN::optimizeNeighborhoodPosition(int targetId, std::vector<double> actualNeighAngles, std::vector<NeighAngValuesIds> originalNeighAngIds,
		double minimal_improvement_distance) {
//INFO("optimizeNeighborhoodPosition: targetId="<<targetId);

	int inSampleId = targetId + 1;

	std::vector<std::vector<GraphNode_AngNeigh>>& sample = this->samples.at(inSampleId);

	//std::vector<NeighAngValuesIds> pathNeightIds = this->getPathNeighAngIds();

	int neighId = originalNeighAngIds[inSampleId].idNeigh;
	int angId = originalNeighAngIds[inSampleId].idAng;

	int neighIdPrev = originalNeighAngIds[inSampleId - 1].idNeigh;
	int angIdPrev = originalNeighAngIds[inSampleId - 1].idAng;
	GraphNode_AngNeigh angNeighPrev = samples[inSampleId - 1][neighIdPrev][angIdPrev];

	int neighIdNext = originalNeighAngIds[inSampleId + 1].idNeigh;
	int angIdNext = originalNeighAngIds[inSampleId + 1].idAng;

	GraphNode_AngNeigh angNeighNext = samples[inSampleId + 1][neighIdNext][angIdNext];

	//this defines the neighborhood
	GraphNode previousTargetCenterNode;
	State previousState;
	previousState.ang = angNeighPrev.ang;
	if (targetId - 1 >= 0) {
		previousTargetCenterNode = targets[targetId - 1];
		previousState.point.x = previousTargetCenterNode.x + this->neighborhood_radius * cos(actualNeighAngles[targetId - 1]);
		previousState.point.y = previousTargetCenterNode.y + this->neighborhood_radius * sin(actualNeighAngles[targetId - 1]);
	} else {
		previousTargetCenterNode = start;
		previousState.point.x = previousTargetCenterNode.x;
		previousState.point.y = previousTargetCenterNode.y;
	}

	GraphNode nextTargetCenterNode;
	State nextState;
	nextState.ang = angNeighNext.ang;
	if (targetId + 1 < targets.size()) {
		nextTargetCenterNode = targets[targetId + 1];
		nextState.point.x = nextTargetCenterNode.x + this->neighborhood_radius * cos(actualNeighAngles[targetId + 1]);
		nextState.point.y = nextTargetCenterNode.y + this->neighborhood_radius * sin(actualNeighAngles[targetId + 1]);
	} else {
		nextTargetCenterNode = end;
		nextState.point.x = nextTargetCenterNode.x;
		nextState.point.y = nextTargetCenterNode.y;
	}

	GraphNode targetCenterNode = targets[targetId];

	GraphNode_AngNeigh editing_pos = sample[neighId][angId];
	editing_pos.neigh_ang = actualNeighAngles[targetId];
	editing_pos.node.x = targetCenterNode.x + this->neighborhood_radius * cos(actualNeighAngles[targetId]);
	editing_pos.node.y = targetCenterNode.y + this->neighborhood_radius * sin(actualNeighAngles[targetId]);
	//dubins heading angles are same
	//positions are set according to actualNeighAngles

	State editingState;
	editingState.ang = editing_pos.ang;
	editingState.point.x = editing_pos.node.x;
	editingState.point.y = editing_pos.node.y;
	//INFO("editing state "<<editingState.point.x<<" "<<editingState.point.y);

	double neigh_ang_min = sample[(neighId - 1) % sample.size()][0].neigh_ang;
	double neigh_ang_max = normalizeAngle(sample[(neighId + 1) % sample.size()][0].neigh_ang, neigh_ang_min, neigh_ang_min + M_2PI);
	double actual_neigh_ang = normalizeAngle(actualNeighAngles[targetId], neigh_ang_min, neigh_ang_min + M_2PI);

	Dubins dubPrev(previousState, editingState, radius);
	Dubins dubNext(editingState, nextState, radius);
	double actualDistance = dubPrev.length + dubNext.length;

	double angleChange = (neigh_ang_max - neigh_ang_min) / 2.0;
	double iterativeDistance = actualDistance;
	double lastIterativeDistance = 2.0 * iterativeDistance;
	double iterativeActualAngle = actual_neigh_ang;
	while (angleChange > 0.02) {	// && lastIterativeDistance - iterativeDistance > 0.005) {

		double ang_to_set_1 = iterativeActualAngle + angleChange;
		double ang_to_set_2 = iterativeActualAngle - angleChange;
		double distanceAfter_1 = neighborhoodAngDistanceAfter(targetCenterNode, editingState, ang_to_set_1, previousState, nextState);
		double distanceAfter_2 = neighborhoodAngDistanceAfter(targetCenterNode, editingState, ang_to_set_2, previousState, nextState);

		if (distanceAfter_1 < iterativeDistance || distanceAfter_2 < iterativeDistance) {
			lastIterativeDistance = iterativeDistance;
			angleChange *= 2.0;
			if (distanceAfter_1 < distanceAfter_2) {
				iterativeActualAngle = ang_to_set_1;
				iterativeDistance = distanceAfter_1;
			} else {
				iterativeActualAngle = ang_to_set_2;
				iterativeDistance = distanceAfter_2;
			}

		} else {
			angleChange /= 2.0;
		}
	}

	double distanceImprovement = 0;
	//if (actualDistance - iterativeDistance > 0.005) {
	if (fabs(actual_neigh_ang - iterativeActualAngle) > 0.2 && actualDistance - iterativeDistance > 0.005) {
		//if ( actualDistance - iterativeDistance > minimal_improvement_distance) {
		//if (iterativeDistance != actualDistance) {
		editing_pos.neigh_ang = normalizeAngle(iterativeActualAngle, 0, M_2PI);
		editing_pos.node.x = targetCenterNode.x + this->neighborhood_radius * cos(editing_pos.neigh_ang);
		editing_pos.node.y = targetCenterNode.y + this->neighborhood_radius * sin(editing_pos.neigh_ang);
		State editingPosStateAfter = editing_pos.toState();
		Dubins dubPrev(previousState, editingPosStateAfter, radius);
		Dubins dubNext(editingPosStateAfter, nextState, radius);
		double actualDistanceAfter = dubPrev.length + dubNext.length;
		if (fabs(actualDistanceAfter - iterativeDistance) > 0.01) {
			INFO("bad actualDistanceAfter-iterativeDistance ");
			INFO_VAR(actualDistanceAfter);
			INFO_VAR(iterativeDistance);
			exit(1);
		}
		//INFO_VAR(editing_pos.node.x);
		//INFO_VAR(editing_pos.node.y);
		//INFO_VAR(editing_pos.neigh_ang);
		/*
		 if (iterativeActualAngle < actual_neigh_ang) {
		 //insert before neighId for target in stample - at position inSampleId
		 insertNeighSample(inSampleId, neighId, neighId, editing_pos.neigh_ang, editing_pos.node);
		 } else {
		 //insert after neighId
		 insertNeighSample(inSampleId, neighId, neighId + 1, editing_pos.neigh_ang, editing_pos.node);
		 }
		 */
		//INFO_CYAN("improved");
		distanceImprovement = actualDistance - iterativeDistance;
	}

	//update();

	NeighImprovement toReturn;
	toReturn.improvementLength = distanceImprovement;
	toReturn.neigh_ang = iterativeActualAngle;
	toReturn.nodeInNeigh = editing_pos.node;
	//INFO("editing state after "<<editing_pos.node.x<<" "<<editing_pos.node.y);

	return toReturn;
}

void DOPN::insertNeighSample(int inSampleId, int neighIdActual, int neighIdWhereInsert, double newAngle, GraphNode node) {
	int nodeId = node.id;
	//INFO("insertNeighSample to nodeId "<<nodeId);
	std::vector<GraphNode_AngNeigh> angSamplesOld = allSamples[nodeId][neighIdActual];

	//samples[inSampleId].insert(samples[inSampleId].begin() + neighIdWhereInsert, angSamplesOld);
	allSamples[nodeId].insert(allSamples[nodeId].begin() + neighIdWhereInsert, angSamplesOld);
	//INFO("add new samples");
	for (int ang_i = 0; ang_i < allSamples[nodeId][neighIdWhereInsert].size(); ang_i++) {
		allSamples[nodeId][neighIdWhereInsert][ang_i].neigh_ang = newAngle;
		allSamples[nodeId][neighIdWhereInsert][ang_i].node = node;
	}
	//INFO("add new samples done");

//copy actual neigh id to neighIdWhereInsert for all other nodes
	//INFO("resize distances");
	for (int otherNodeId = 0; otherNodeId < allDistances[nodeId].size(); ++otherNodeId) {
		std::vector<std::vector<std::vector<double>>>distances = allDistances[nodeId][otherNodeId][neighIdActual];
		allDistances[nodeId][otherNodeId].insert(allDistances[nodeId][otherNodeId].begin()+neighIdWhereInsert,distances);
		for (int otherNeighId = 0; otherNeighId < allDistances[otherNodeId][nodeId].size(); ++otherNeighId) {
			std::vector<std::vector<double>> distancesAngles = allDistances[otherNodeId][nodeId][otherNeighId][neighIdActual];
			allDistances[otherNodeId][nodeId][otherNeighId].insert(allDistances[otherNodeId][nodeId][otherNeighId].begin()+neighIdWhereInsert,distancesAngles);
		}
	}
	//INFO("resize distances done");

//fill new distances
	//INFO("fill distances");
	for (int otherNodeId = 0; otherNodeId < allDistances[nodeId].size(); ++otherNodeId) {
		//calc distances inside allDistances[nodeId][otherNodeId][neighIdWhereInsert] that was inserted before
		for (int neighID2 = 0; neighID2 < allDistances[nodeId][otherNodeId][neighIdWhereInsert].size(); ++neighID2) {
			for (int idx1 = 0; idx1 < allDistances[nodeId][otherNodeId][neighIdWhereInsert][neighID2].size(); ++idx1) {
				for (int idx2 = 0; idx2 < allDistances[nodeId][otherNodeId][neighIdWhereInsert][neighID2][idx1].size(); ++idx2) {
					GraphNode_AngNeigh & from = allSamples[nodeId][neighIdWhereInsert][idx1];
					GraphNode_AngNeigh & to = allSamples[otherNodeId][neighID2][idx2];
					Dubins dub(from.toState(), to.toState(), radius);
					allDistances[nodeId][otherNodeId][neighIdWhereInsert][neighID2][idx1][idx2] = dub.length;
				}
			}
		}

		for (int otherNeighId = 0; otherNeighId < allDistances[otherNodeId][nodeId].size(); ++otherNeighId) {
			//calc distances inside allDistances[otherNodeId][nodeId][otherNeighId][neighIdWhereInsert] that was inserted before
			for (int idx1 = 0; idx1 < allDistances[otherNodeId][nodeId][otherNeighId][neighIdWhereInsert].size(); ++idx1) {
				for (int idx2 = 0; idx2 < allDistances[otherNodeId][nodeId][otherNeighId][neighIdWhereInsert][idx1].size(); ++idx2) {
					GraphNode_AngNeigh & from = allSamples[otherNodeId][otherNeighId][idx1];
					GraphNode_AngNeigh & to = allSamples[nodeId][neighIdWhereInsert][idx2];
					Dubins dub(from.toState(), to.toState(), radius);
					allDistances[otherNodeId][nodeId][otherNeighId][neighIdWhereInsert][idx1][idx2] = dub.length;
				}
			}
		}
	}

	//INFO("fill distances done");
	//INFO("update");
	//this->update();
	//INFO("update done");
	//INFO("length after update "<<getPathLength());
	//INFO("chechkDistancesAndSamples");
	//checkDistancesAndSamples(radius);
	//INFO("chechkDistancesAndSamples done");

	//INFO("insertNeighSample end");
}

double DOPN::neighborhoodAngDistanceAfter(GraphNode centroid, State editingState, double new_ang, State previousState, State nextState) {
	double new_x = centroid.x + this->neighborhood_radius * cos(new_ang);
	double new_y = centroid.y + this->neighborhood_radius * sin(new_ang);
	State newState = State(new_x, new_y, editingState.ang);		//chagne only the x and y, keep the ang of editing state
	Dubins dubPrevNew(previousState, newState, radius);
	Dubins dubNextNew(newState, nextState, radius);
	double distanceAfter = dubPrevNew.length + dubNextNew.length;
	return distanceAfter;
}

double DOPN::angDistanceAfter(State editingState, double new_ang, State previousState, State nextState) {
	State newState = State(editingState.point.x, editingState.point.y, new_ang);		//chagne only the x and y, keep the ang of editing state
	Dubins dubPrevNew(previousState, newState, radius);
	Dubins dubNextNew(newState, nextState, radius);
	double distanceAfter = dubPrevNew.length + dubNextNew.length;
	return distanceAfter;
}

SimpleTryResult DOPN::tryToAdd(GraphNode p, int idx) const {
	//INFO("tryToAdd single begin");
	if (idx > targets.size() || idx < 0) {
		INFO_RED("tryToAdd idx "<<idx<<" exceeds maximum "<<(targets.size()-1));
		exit(1);
	}
	std::vector<std::vector<GraphNode_AngNeigh>> fromSamples = samples[idx];
	std::vector<std::vector<GraphNode_AngNeigh>> toSamples = samples[idx + 1];
	//INFO("tryToAdd "<< p.id <<" between "<<samples[idx][0][0].node.id<<" and "<<samples[idx+1][0][0].node.id);

//INFO("fromSamples AND toSamples");
	dop_2d_matrix_triple fromLen = shortest[idx];
	dop_2d_matrix_triple toLen = shortest_back[idx + 1];
//INFO("fromLen AND toLen");
// generate samples
//vector<GraphNode_AngNeigh> actSamples;
//generateSamples(actSamples, p,resolution);
	std::vector<std::vector<GraphNode_AngNeigh>> actSamples = allSamples[p.id];
//INFO("actSamples");
	const int fromSamplesSize = fromSamples.size();

// find shortest paths to generate samples
	std::vector<std::vector<double>> actLen;
	actLen.resize(actSamples.size());
	for (int neighID = 0; neighID < actSamples.size(); ++neighID) {
		//INFO("actLen ASSIGN "<<neighID);
		actLen[neighID].assign(actSamples[neighID].size(), M);
	}

	std::vector<std::vector<double>> actLenMax;
	actLenMax.resize(actSamples.size());
	for (int neighID = 0; neighID < actLenMax.size(); ++neighID) {
		actLenMax[neighID].assign(actSamples[neighID].size(), -M);
	}

	for (int fromNeighID = 0; fromNeighID < fromSamples.size(); ++fromNeighID) {
		for (int toNeighID = 0; toNeighID < actSamples.size(); ++toNeighID) {
			for (int idx1 = 0; idx1 < fromSamples[fromNeighID].size(); idx1++) {
				for (int idx2 = 0; idx2 < actSamples[toNeighID].size(); idx2++) {
					GraphNode_AngNeigh & from = fromSamples[fromNeighID][idx1];
					GraphNode_AngNeigh & to = actSamples[toNeighID][idx2];
					//double dl = Dubins(from.toState(), to.toState(), radius).length;

					double dl = allDistances[from.node.id][to.node.id][fromNeighID][toNeighID][idx1][idx2];
					double nl = dl + fromLen[fromNeighID][idx1].distance;

					//for the upper bound
					//if (nl > actLenMax[toNeighID][idx2]) {
					//	actLenMax[toNeighID][idx2] = nl;
					//}

					//for the lower bound and act len
					if (nl < actLen[toNeighID][idx2]) {
						actLen[toNeighID][idx2] = nl;
					}
				}
			}
		}
	}

// find the shortest path from actual samples to next <toSamples>
	double maxLen = -M;
	//INFO_VAR(maxLen);
	double minLen = M;
	for (int fromNeighID = 0; fromNeighID < actSamples.size(); ++fromNeighID) {
		for (int toNeighID = 0; toNeighID < toSamples.size(); ++toNeighID) {
			for (int idx1 = 0; idx1 < actSamples[fromNeighID].size(); idx1++) {
				for (int idx2 = 0; idx2 < toSamples[toNeighID].size(); idx2++) {
					GraphNode_AngNeigh & from = actSamples[fromNeighID][idx1];
					GraphNode_AngNeigh & to = toSamples[toNeighID][idx2];
					//double dl = Dubins(from.toState(), to.toState(), radius).length;

					double dl = allDistances[from.node.id][to.node.id][fromNeighID][toNeighID][idx1][idx2];
					double nl = dl + toLen[toNeighID][idx2].distance + actLen[fromNeighID][idx1];
					if (nl < minLen) {
						minLen = nl;
					}

					//double nl_max = dl + toLen[toNeighID][idx2].distance + actLen[fromNeighID][idx1];
					//INFO_VAR(nl_max);
					if (nl > maxLen) {
						//INFO_VAR(maxLen);
						maxLen = nl;
					}
				}
			}
		}
	}

	SimpleTryResult toReturn;
	toReturn.actual_len = minLen;
	toReturn.maximal_len = maxLen;
	//INFO("tryToAdd single end");
	return toReturn;
}

double DOPN::tryToAdd(std::vector<GraphNode> newPoints, int idx) const {
//INFO("tryToAdd vector begin");
	if (idx > targets.size() || idx < 0) {
		INFO_RED("tryToAdd idx "<<idx<<" exceeds maximum "<<(targets.size()-1));
		exit(1);
	}
	std::vector<std::vector<GraphNode_AngNeigh>> fromSamples = samples[idx];
	std::vector<std::vector<GraphNode_AngNeigh>> toSamples = samples[idx + 1];

	dop_2d_matrix_triple fromLen = shortest[idx];
	dop_2d_matrix_triple toLen = shortest_back[idx + 1];

	const int addingNumPoints = newPoints.size();
//INFO("tryToAdd vector size "<<addingNumPoints);

// generate samples
	std::vector<std::vector<std::vector<GraphNode_AngNeigh>>>actSamples(addingNumPoints);
	for (int var = 0; var < newPoints.size(); ++var) {
		//generateSamples(actSamples[var], p[var], resolution);
		actSamples[var] = allSamples[newPoints[var].id];
	}

	dop_3d_matrix_triple shortestadded;
	shortestadded.resize(addingNumPoints + 2);
	shortestadded[0].resize(fromLen.size());
	for (int neighID = 0; neighID < fromLen.size(); ++neighID) {
		shortestadded[0][neighID].resize(resolution);
		for (int idx1 = 0; idx1 < resolution; ++idx1) {
			shortestadded[0][neighID][idx1] = fromLen[neighID][idx1];
		}
	}
	int fromNodeId = fromSamples[0][0].node.id;
	for (int target = 1; target < addingNumPoints + 2; target++) {
		shortestadded[target].resize(allDistances[fromNodeId][newPoints[target - 1].id].size());
		for (int neighID = 0; neighID < allDistances[fromNodeId][newPoints[target - 1].id].size(); ++neighID) {
			shortestadded[target][neighID].assign(resolution, NeighAngDist(-1, -1, M));
		}

		for (int neighID1 = 0; neighID1 < allDistances[fromNodeId][newPoints[target - 1].id].size(); ++neighID1) {
			for (int neighID2 = 0; neighID2 < allDistances[fromNodeId][newPoints[target - 1].id][neighID1].size(); ++neighID2) {

				for (int idx1 = 0; idx1 < resolution; idx1++) {
					for (int idx2 = 0; idx2 < resolution; idx2++) {
						auto len1 = shortestadded[target - 1][neighID1][idx1].distance;
						auto len2 = allDistances[fromNodeId][newPoints[target - 1].id][neighID1][neighID2][idx1][idx2];
						//INFO(idx1<<" "<<idx2<<" len "<<len1<<" dist "<<distances[target - 1][idx1][idx2]);
						auto nLen = len1 + len2;
						auto &len = shortestadded[target][neighID2][idx2];
						if (nLen < len.distance) {
							len.idNeigh = neighID1;
							len.idAng = idx1;
							len.distance = nLen;
							//INFO("shortest path to "<<idx2<<"  "<<len.second);
						}
					}
				}

			}
		}
		fromNodeId = newPoints[target - 1].id;
	}

// find the shortest path from shortestadded and
	double minLen = M;
	for (int neighID2 = 0; neighID2 < shortestadded[addingNumPoints + 1].size(); neighID2++) {
		for (int idx2 = 0; idx2 < toSamples.size(); idx2++) {
			double nl = shortestadded[addingNumPoints + 1][neighID2][idx2].distance + toLen[neighID2][idx2].distance;
			if (nl < minLen) {
				minLen = nl;
			}
		}
	}
//INFO("tryToAdd vector begin");
	return minLen;
}

double DOPN::tryToReplace(std::vector<GraphNode> p, int idx) const {
//INFO("tryToReplace multi begin");
	std::vector<std::vector<GraphNode_AngNeigh>> fromSamples = samples[idx];
	std::vector<std::vector<GraphNode_AngNeigh>> toSamples = samples[idx + 2];

	dop_2d_matrix_triple fromLen = shortest[idx];
	dop_2d_matrix_triple toLen = shortest_back[idx + 2];

	const int addingNumPoints = p.size();
//INFO("tryToAdd vector size "<<addingNumPoints);

// generate samples
	std::vector<std::vector<std::vector<GraphNode_AngNeigh>>>actSamples(addingNumPoints);
	for (int var = 0; var < p.size(); ++var) {
		//generateSamples(actSamples[var], p[var], resolution);
		actSamples[var] = allSamples[p[var].id];
	}

	dop_3d_matrix_triple shortestadded;
	shortestadded.resize(addingNumPoints + 2);
	shortestadded[0].resize(fromLen.size());
	for (int neighID = 0; neighID < fromLen.size(); ++neighID) {
		shortestadded[0][neighID].resize(resolution);
		for (int idx1 = 0; idx1 < resolution; ++idx1) {
			shortestadded[0][neighID][idx1] = fromLen[neighID][idx1];
		}
	}

	int fromNodeId = fromSamples[0][0].node.id;
	for (int target = 1; target < addingNumPoints + 2; target++) {
		//INFO("target "<<target);
		shortestadded[target].resize(allDistances[fromNodeId][p[target].id][0].size());
		for (int neighID = 0; neighID < shortestadded[target].size(); ++neighID) {
			shortestadded[target][neighID].assign(resolution, NeighAngDist(-1, -1, M));
		}

		for (int neighID1 = 0; neighID1 < allDistances[fromNodeId][p[target - 1].id].size(); ++neighID1) {
			for (int neighID2 = 0; neighID2 < allDistances[fromNodeId][p[target - 1].id][neighID1].size(); ++neighID2) {

				for (int idx1 = 0; idx1 < resolution; idx1++) {
					for (int idx2 = 0; idx2 < resolution; idx2++) {

						auto len1 = shortestadded[target - 1][neighID1][idx1].distance;
						auto len2 = allDistances[fromNodeId][p[target - 1].id][neighID1][neighID2][idx1][idx2];
						//INFO("Shortest len1 "<<len1<<" len2 "<<len2);
						auto nLen = len1 + len2;
						auto &len = shortestadded[target][neighID2][idx2];
						if (nLen < len.distance) {
							len.idNeigh = neighID1;
							len.idAng = idx1;
							len.distance = nLen;
							//INFO("shortest path to "<<idx2<<"  "<<len.second);
						}
					}
				}
			}
		}

		fromNodeId = p[target - 1].id;

	}

// find the shortest path from shortestadded and
	double minLen = M;
	for (int neighID2 = 0; neighID2 < shortestadded[addingNumPoints + 1].size(); neighID2++) {
		for (int idx2 = 0; idx2 < toSamples.size(); idx2++) {
			double nl = shortestadded[addingNumPoints + 1][neighID2][idx2].distance + toLen[neighID2][idx2].distance;
			if (nl < minLen) {
				minLen = nl;
			}
		}
	}
//INFO("tryToReplace multi end");
	return minLen;
}

SimpleTryResult DOPN::tryToReplace(GraphNode p, int idx, int numToReplace) const {
	//INFO("tryToReplace single begin");
	const int S = getSize();
	bool replaceLast = false;
	if (idx + numToReplace == S) {
		replaceLast = true;
		//INFO("try to replace last");
	} else if (idx >= S) {
		INFO_RED("tryToReplace idx "<<idx<<" exceeds maximum "<<S);
		exit(1);
	}

	std::vector<std::vector<GraphNode_AngNeigh>> fromSamples = samples[idx];
	dop_2d_matrix_triple fromLen = shortest[idx];

// generate samples
	std::vector<std::vector<GraphNode_AngNeigh>> replaceWSamples = allSamples[p.id];

//generateSamples(actSamples, p, resolution);

	const int fromSamplesSize = fromSamples.size();

// find shortest paths to generate samples
	std::vector<std::vector<double>> newToDistances;
	newToDistances.resize(replaceWSamples.size());
	for (int neighID = 0; neighID < replaceWSamples.size(); ++neighID) {
		newToDistances[neighID].assign(replaceWSamples[neighID].size(), M);
	}
//INFO("calc to");
	for (int fromNeighID = 0; fromNeighID < fromSamples.size(); ++fromNeighID) {
		for (int idx1 = 0; idx1 < fromSamples[fromNeighID].size(); idx1++) {
			GraphNode_AngNeigh & from = fromSamples[fromNeighID][idx1];
			for (int toNeighID = 0; toNeighID < replaceWSamples.size(); ++toNeighID) {
				for (int idx2 = 0; idx2 < replaceWSamples[toNeighID].size(); idx2++) {
					GraphNode_AngNeigh & to = replaceWSamples[toNeighID][idx2];

					//double dl = Dubins(from.toState(), to.toState(), radius).length;
					double dl = allDistances[from.node.id][to.node.id][fromNeighID][toNeighID][idx1][idx2];
					double nl = dl + fromLen[fromNeighID][idx1].distance;
					if (nl < newToDistances[toNeighID][idx2]) {
						newToDistances[toNeighID][idx2] = nl;
					}
				}
			}
		}
	}
	double minLen = M;
	double maxLen = -M;
//INFO("calc from");
	std::vector<std::vector<GraphNode_AngNeigh>> toSamples = samples[idx + 1 + numToReplace];
	dop_2d_matrix_triple toLen = shortest_back[idx + 1 + numToReplace];
// find the shortest path from actual samples to next <toSamples>

	for (int fromNeighID = 0; fromNeighID < replaceWSamples.size(); ++fromNeighID) {
		for (int idx1 = 0; idx1 < replaceWSamples[fromNeighID].size(); idx1++) {
			GraphNode_AngNeigh & from = replaceWSamples[fromNeighID][idx1];
			for (int toNeighID = 0; toNeighID < toSamples.size(); ++toNeighID) {
				for (int idx2 = 0; idx2 < toSamples[toNeighID].size(); idx2++) {
					GraphNode_AngNeigh & to = toSamples[toNeighID][idx2];
					//double dl = Dubins(from.toState(), to.toState(), radius).length;

					double dl = allDistances[from.node.id][to.node.id][fromNeighID][toNeighID][idx1][idx2];
					double nl = dl + toLen[toNeighID][idx2].distance + newToDistances[fromNeighID][idx1];
					if (nl < minLen) {
						minLen = nl;
					}

					//INFO_VAR(nl_max);
					if (nl > maxLen) {
						//INFO_VAR(maxLen);
						maxLen = nl;
					}
				}
			}
		}
	}

	SimpleTryResult toRet;
	toRet.actual_len = minLen;
	toRet.maximal_len = maxLen;
	//INFO("tryToReplace single end");
	return toRet;
}

double DOPN::tryToRemove(int idx) const {
	//INFO("tryToRemove single begin");
	//INFO("try to remove id "<<samples[idx+1][0][0].node.id<<" that is between "<<samples[idx][0][0].node.id<<" and "<<samples[idx+2][0][0].node.id);
	double minLen = M;
	for (int fromNeighID = 0; fromNeighID < samples[idx].size(); ++fromNeighID) {
		std::vector<GraphNode_AngNeigh> fromSamples = samples[idx][fromNeighID];
		dop_vector_triple fromLen = shortest[idx][fromNeighID];
		for (int toNeighID = 0; toNeighID < samples[idx + 2].size(); ++toNeighID) {
			std::vector<GraphNode_AngNeigh> toSamples = samples[idx + 2][toNeighID];
			dop_vector_triple toLen = shortest_back[idx + 2][toNeighID];
			for (int idx1 = 0; idx1 < fromSamples.size(); idx1++) {
				for (int idx2 = 0; idx2 < toSamples.size(); idx2++) {
					auto & from = fromSamples[idx1];
					auto & to = toSamples[idx2];
					//double dl = Dubins(from.toState(), to.toState(), radius).length;

					double dl = allDistances[from.node.id][to.node.id][fromNeighID][toNeighID][idx1][idx2];
					double nl = dl + toLen[idx2].distance + fromLen[idx1].distance;
					if (nl < minLen) {
						minLen = nl;
					}
				}
			}
		}
	}
	//INFO("tryToRemove single end");
	return minLen;
}

double DOPN::tryToRemove(int idxStart, int idxStop) const {
//INFO("tryToRemove multi begin");
	double minLen = M;
	for (int fromNeighID = 0; fromNeighID < shortest[idxStart].size(); ++fromNeighID) {
		dop_vector_triple fromLen = shortest[idxStart][fromNeighID];
		std::vector<GraphNode_AngNeigh> fromSamples = samples[idxStart][fromNeighID];
		for (int toNeighID = 0; toNeighID < shortest_back[idxStop + 2].size(); ++toNeighID) {
			dop_vector_triple toLen = shortest_back[idxStop + 2][toNeighID];
			std::vector<GraphNode_AngNeigh> toSamples = samples[idxStop + 2][toNeighID];
			for (int idx1 = 0; idx1 < fromSamples.size(); idx1++) {
				for (int idx2 = 0; idx2 < toSamples.size(); idx2++) {
					auto & from = fromSamples[idx1];
					auto & to = toSamples[idx2];
					//double dl = Dubins(from.toState(), to.toState(), radius).length;
					double dl = allDistances[from.node.id][to.node.id][fromNeighID][toNeighID][idx1][idx2];
					double nl = dl + toLen[idx2].distance + fromLen[idx1].distance;
					if (nl < minLen) {
						minLen = nl;
					}
				}
			}
		}
	}
//INFO("tryToRemove multi end");
	return minLen;
}

void DOPN::removeOne() {
	int idx = 0;
	double minLen = numeric_limits<double>::max();
	const int S = getSize();

	for (int i = 0; i < S - 1; i++) {
		double newLen = tryToRemove(i);
		if (newLen < minLen) {
			minLen = newLen;
			idx = i;
		}
	}

	removePoint(idx);
}

double DOPN::tryToExchange(int idx1, int idx2) {
	//INFO("tryToExchange multi begin");
	if (idx1 > targets.size() || idx1 < 0 || idx2 > targets.size() || idx2 < 0 || abs(idx1 - idx2) < 2) {
		INFO_RED("tryToExchange idx "<<idx1<<" or "<<idx2<<" exceeds maximum "<<(targets.size()-1));
		exit(1);
	}
	int idxFrom = MIN(idx1, idx2);
	int idxTo = MAX(idx1, idx2);
//INFO_GREEN("tryToExchange from " << idxFrom << " to " << idxTo);

//exit when dist idxStart to idxEnd < 2 ---- need minimally distance of two*
//reverse path between idx1+1 and idx2, so need
//ensure idxStart <= idxEnd
	if (idxTo - idxFrom < 2) {
		INFO_RED("idxEnd and idxStart are too close "<<(idxTo - idxFrom)<<" must be lower than 2");
		exit(1);
	}
	if (idxTo >= targets.size()) {
		INFO_RED("idxEnd+1 is out of samples array "<<idxTo<<" samples number "<<targets.size());
		exit(1);
	}
	std::stringstream ss;
//in targets
//normal path   - till idxFrom-1
//reversed path - from idxTo till idxFrom
//normal path   - idxTo + 1

//in samples
//normal path   - till idxStartSamples-1
//reversed path - from idxEndSamples till idxStartSamples
//normal path   - idxEndSamples + 1

//change indexing from targets to samples
	int idxStartSamples = idxFrom + 1;
	int idxEndSamples = idxTo + 1;

	int i = idxStartSamples - 1;
	int j = idxEndSamples;
	std::vector<std::vector<GraphNode_AngNeigh>> samples_I = samples[i];		//i
	std::vector<std::vector<GraphNode_AngNeigh>> samples_I_p = samples[i + 1];		//i+1
	std::vector<std::vector<GraphNode_AngNeigh>> samples_J_m = samples[j - 1];		//j -1
	std::vector<std::vector<GraphNode_AngNeigh>> samples_J = samples[j];		//j
	std::vector<std::vector<GraphNode_AngNeigh>> samples_J_p = samples[j + 1];		//j+1
	std::vector<std::vector<GraphNode_AngNeigh>> samples_I_pp = samples[i + 2];		//i+2
	int idI = samples[i][0][0].node.id;
	int idI_p = samples[i + 1][0][0].node.id;
	int idI_pp = samples[i + 2][0][0].node.id;
	int idJ_m = samples[j - 1][0][0].node.id;
	int idJ = samples[j][0][0].node.id;
	int idJ_p = samples[j + 1][0][0].node.id;

	dop_2d_matrix_triple normalOrderTotLen = shortest[i];		// need to use idxStart+1 as to the

	dop_2d_matrix_triple normalOrderFromLen = shortest_back[j + 1];

//INFO("find shortest paths between reverseOrderFromSamples to reverseOrderToSamples");
// find shortest paths between reverseOrderFromSamples to reverseOrderToSamples
// find the shortest part of path that is reversed
// matrix:
//    1) to city (layer)
//    2) to idx i
	dop_3d_matrix_triple pathPartShortest;
	int numCityBetween = (idxEndSamples - idxStartSamples + 2);
//INFO("numCityBetween "<<numCityBetween);
	pathPartShortest.resize(numCityBetween);
	pathPartShortest[0] = normalOrderTotLen;		// set actual shortest distance to idxStart

	int idxPathPartShortest = 1;
//now calc shortest to reverseOrderFromSamples using distToReverseStart
	//INFO("calc shortest from idI "<< idI <<" to idJ "<< idJ);
//pathPartShortest[idxPathPartShortest].assign(reverseOrderFromSamples.size(), dst_pair(-1, M));
	pathPartShortest[idxPathPartShortest].resize(samples_J.size());
	for (int neighID = 0; neighID < pathPartShortest[idxPathPartShortest].size(); ++neighID) {
		pathPartShortest[idxPathPartShortest][neighID].assign(samples_J[neighID].size(), NeighAngDist(-1, -1, M));
	}
	for (int neighID1 = 0; neighID1 < samples_I.size(); ++neighID1) {
		for (int neighID2 = 0; neighID2 < samples_J.size(); ++neighID2) {
			for (int idx1 = 0; idx1 < samples_I[neighID1].size(); idx1++) {
				for (int idx2 = 0; idx2 < samples_J[neighID2].size(); idx2++) {
					auto len1 = pathPartShortest[idxPathPartShortest - 1][neighID1][idx1].distance; //shortest dist from start to [target - 1][idx1]
					auto len2 = allDistances[idI][idJ][neighID1][neighID2][idx1][idx2]; //dist from [target - 1][idx1] to [target][idx2]
					auto nLen = len1 + len2;
					auto &len = pathPartShortest[idxPathPartShortest][neighID2][idx2]; // actual shortest path from start to [target][idx2]
					if (nLen < len.distance) {
						len.idNeigh = neighID1;
						len.idAng = idx1;
						len.distance = nLen;
					}
				}
			}
		}
	}

	idxPathPartShortest++; //2
	//INFO("calc shortest from idJ "<< idJ <<" to idI_pp"<< idI_pp);
//pathPartShortest[idxPathPartShortest].assign(reverseOrderFromSamples.size(), dst_pair(-1, M));
	pathPartShortest[idxPathPartShortest].resize(samples_I_pp.size());
	for (int neighID = 0; neighID < pathPartShortest[idxPathPartShortest].size(); ++neighID) {
		pathPartShortest[idxPathPartShortest][neighID].assign(samples_I_pp[neighID].size(), NeighAngDist(-1, -1, M));
	}
	for (int neighID1 = 0; neighID1 < samples_J.size(); ++neighID1) {
		for (int neighID2 = 0; neighID2 < samples_I_pp.size(); ++neighID2) {
			for (int idx1 = 0; idx1 < samples_J[neighID1].size(); idx1++) {
				for (int idx2 = 0; idx2 < samples_I_pp[neighID2].size(); idx2++) {
					auto len1 = pathPartShortest[idxPathPartShortest - 1][neighID1][idx1].distance; //shortest dist from start to [target - 1][idx1]
					auto len2 = allDistances[idJ][idI_pp][neighID1][neighID2][idx1][idx2]; //dist from [target - 1][idx1] to [target][idx2]
					auto nLen = len1 + len2;
					auto &len = pathPartShortest[idxPathPartShortest][neighID2][idx2]; // actual shortest path from start to [target][idx2]
					if (nLen < len.distance) {
						len.idNeigh = neighID1;
						len.idAng = idx1;
						len.distance = nLen;
					}
				}
			}
		}
	}

	idxPathPartShortest++; //3
//calc from j to i+1
	for (int target = idxStartSamples + 1; target <= idxEndSamples - 2; target++) {
		//INFO("----------------");
		//pathPartShortest[idxPathPartShortest].assign(samples[target].size(), dst_pair(-1, M));
		int idFrom = target;
		int idTo = target + 1;
		pathPartShortest[idxPathPartShortest].resize(samples[idTo].size());
		for (int neighID = 0; neighID < pathPartShortest[idxPathPartShortest].size(); ++neighID) {
			pathPartShortest[idxPathPartShortest][neighID].assign(samples[idTo][neighID].size(), NeighAngDist(-1, -1, M));
		}

		//INFO("----calc shortest from "<< samples[idFrom][0].node.id <<" to "<< samples[idTo][0].node.id);
		//INFO("idFrom " << samples[idFrom][0].node.id);
		//INFO("idto "<< samples[idTo][0].node.id);

		const int S1_neight = samples[idFrom].size();			// from neigh
		const int S2_neight = samples[idTo].size();			//to neigh
		for (int neighID1 = 0; neighID1 < S1_neight; ++neighID1) {
			for (int neighID2 = 0; neighID2 < S2_neight; ++neighID2) {
				const int S1 = samples[idFrom][neighID1].size(); // from neigh
				const int S2 = samples[idTo][neighID2].size(); //to neigh
				for (int idx1 = 0; idx1 < S1; idx1++) {
					for (int idx2 = 0; idx2 < S2; idx2++) {
						auto & from = samples[idFrom][neighID1][idx1];
						auto & to = samples[idTo][neighID2][idx2];

						auto len1 = pathPartShortest[idxPathPartShortest - 1][neighID1][idx1].distance; //shortest dist from start to [target - 1][idx1]
						//auto len2 = distances[idTo][getInverseId(idx2)][getInverseId(idx1)]; //need to use idto because of indexing of distances
						auto len2 = allDistances[from.node.id][to.node.id][neighID1][neighID2][idx1][idx2];

						auto nLen = len1 + len2;
						//INFO(idx1<<" "<<idx2 <<" len1 "<<len1<<" len2 "<<len2<<" nLen "<<nLen);
						//INFO(idx1<<" "<<idx2<<" len "<<len1<<" dist "<< distances[idTo][idx2][idx1]);
						auto &len = pathPartShortest[idxPathPartShortest][neighID2][idx2];					// actual shortest path from start to [target][idx2]

						if (nLen < len.distance) {
							len.idNeigh = neighID1;
							len.idAng = idx1;
							len.distance = nLen;
						}
					}
				}
			}
		}

		idxPathPartShortest++;
	}

	//INFO("calc shortest from idJ_m "<< idJ_m <<" to idI_p "<< idI_p);
//pathPartShortest[idxPathPartShortest].assign(reverseOrderFromSamples.size(), dst_pair(-1, M));
	pathPartShortest[idxPathPartShortest].resize(samples_I_p.size());
	for (int neighID = 0; neighID < samples_I_p.size(); ++neighID) {
		pathPartShortest[idxPathPartShortest][neighID].assign(samples_I_p[neighID].size(), NeighAngDist(-1, -1, M));
	}
	for (int neighID1 = 0; neighID1 < samples_J_m.size(); ++neighID1) {
		for (int neighID2 = 0; neighID2 < samples_I_p.size(); ++neighID2) {
			for (int idx1 = 0; idx1 < samples_J_m[neighID1].size(); idx1++) {
				for (int idx2 = 0; idx2 < samples_I_p[neighID2].size(); idx2++) {
					auto len1 = pathPartShortest[idxPathPartShortest - 1][neighID1][idx1].distance; //shortest dist from start to [target - 1][idx1]
					auto len2 = allDistances[idJ_m][idI_p][neighID1][neighID2][idx1][idx2]; //dist from [target - 1][idx1] to [target][idx2]
					auto nLen = len1 + len2;
					auto &len = pathPartShortest[idxPathPartShortest][neighID2][idx2]; // actual shortest path from start to [target][idx2]
					if (nLen < len.distance) {
						len.idNeigh = neighID1;
						len.idAng = idx1;
						len.distance = nLen;
					}
				}
			}
		}
	}
	idxPathPartShortest++;

	double minLen = M;
	//INFO("calc shortest from "<< idI_p <<" to "<< idJ_p);
	for (int neighID1 = 0; neighID1 < samples_I_p.size(); ++neighID1) {
		for (int neighID2 = 0; neighID2 < samples_J_p.size(); ++neighID2) {

			for (int idx1 = 0; idx1 < samples_I_p[neighID1].size(); idx1++) {
				for (int idx2 = 0; idx2 < samples_J_p[neighID2].size(); idx2++) {
					auto len1 = pathPartShortest[idxPathPartShortest - 1][neighID1][idx1].distance; //shortest dist from start to [target - 1][idx1]
					auto len2 = allDistances[idI_p][idJ_p][neighID1][neighID2][idx1][idx2]; //dist from [target - 1][idx1] to [target][idx2]

					//INFO("shortest "<<" len1 "<<len1 <<" len2 "<<len2<<" normalOrderFromLen[idx2].second "<<normalOrderFromLen[idx2].second);
					auto nLen = len1 + len2 + normalOrderFromLen[neighID2][idx2].distance;
					if (nLen < minLen) {
						minLen = nLen;
					}
				}
			}
		}
	}
	//INFO("tryToExchange multi end");
	return minLen;
}

double DOPN::tryToMoveEuclidean(int idxWhatToMove, int idxWhereToMove) {
	INFO("tryToMoveEuclidean:");
	INFO_VAR(idxWhatToMove);
	INFO_VAR(idxWhereToMove);
	bool moveUp = idxWhereToMove > idxWhatToMove;

	double distanceBefore = getPathLength();
	double distanceAfterMove = 0;
	INFO_VAR(moveUp);
	if (moveUp) {
		double tryToRemoveDist = tryToRemove(idxWhatToMove);
		INFO_VAR(tryToRemoveDist);
		GraphNode nodeToMove = targets[idxWhatToMove];
		INFO("nodeToMove id "<<nodeToMove.id);
		double tryToAddDist = tryToAdd(nodeToMove, idxWhereToMove).actual_len;
		INFO_VAR(tryToAddDist);
		distanceAfterMove = tryToRemoveDist + tryToAddDist - distanceBefore;

		DOPN copyAdd = *this;
		DOPN copyAdd2 = *this;
		copyAdd.removePoint(idxWhatToMove);
		INFO("length after remove "<<copyAdd.getPathLength());
		copyAdd2.addPoint(nodeToMove, idxWhereToMove);
		INFO("length after add "<<copyAdd2.getPathLength());
		copyAdd.addPoint(nodeToMove, idxWhereToMove);
		copyAdd.update();
		double realLength = copyAdd.getPathLength();
		INFO_VAR(realLength);
	} else {
		double tryToRemoveDist = tryToRemove(idxWhatToMove);
		INFO_VAR(tryToRemoveDist);
		GraphNode nodeToMove = targets[idxWhatToMove];
		INFO("nodeToMove id "<<nodeToMove.id);
		double tryToAddDist = tryToAdd(nodeToMove, idxWhereToMove).actual_len;
		INFO_VAR(tryToAddDist);
		distanceAfterMove = tryToRemoveDist + tryToAddDist - distanceBefore;

		double savedLength = distanceBefore - tryToRemoveDist;
		INFO_VAR(savedLength);
		double addedLength = tryToAddDist - distanceBefore;
		INFO_VAR(addedLength);
		double distanceTogether = distanceBefore - savedLength + addedLength;
		INFO_VAR(distanceTogether);

		DOPN copyAdd = *this;
		DOPN copyAdd2 = *this;
		INFO("length before "<<copyAdd.getPathLength());
		copyAdd.removePoint(idxWhatToMove);
		INFO("length after remove "<<copyAdd.getPathLength());
		copyAdd2.addPoint(nodeToMove, idxWhereToMove);
		INFO("length after add "<<copyAdd2.getPathLength());
		copyAdd.addPoint(nodeToMove, idxWhereToMove);
		copyAdd.update();
		double realLength = copyAdd.getPathLength();
		INFO_VAR(realLength);
	}
	//double distanceAfterMove = distanceBefore - (distanceBefore - afterRemove) + (afterAdd - distanceBefore);

	/*
	 double afterAddP = tryToAdd(nodeToMove, idxWhereToMove + 1);
	 double afterAddM = tryToAdd(nodeToMove, idxWhereToMove - 1);
	 double distanceAfterMoveAP = afterRemove + afterAddP - distanceBefore;
	 double distanceAfterMoveAM = afterRemove + afterAddM - distanceBefore;

	 double distanceAfterMoveRP = afterRemoveP + afterAdd - distanceBefore;
	 double distanceAfterMoveRM = afterRemoveM + afterAdd - distanceBefore;

	 double distanceAfterMovePP = afterRemoveP + afterAddP - distanceBefore;
	 double distanceAfterMoveMM = afterRemoveM + afterAddM - distanceBefore;

	 double distanceAfterMovePM = afterRemoveP + afterAddM - distanceBefore;
	 double distanceAfterMoveMP = afterRemoveM + afterAddP - distanceBefore;

	 INFO_VAR(distanceAfterMoveAP);
	 INFO_VAR(distanceAfterMoveAM);

	 INFO_VAR(distanceAfterMoveRP);
	 INFO_VAR(distanceAfterMoveRM);

	 INFO_VAR(distanceAfterMovePP);
	 INFO_VAR(distanceAfterMoveMM);

	 INFO_VAR(distanceAfterMovePM);
	 INFO_VAR(distanceAfterMoveMP);
	 INFO_VAR(distanceAfterMove);
	 */
	/*
	 if (moveUp) {
	 //move up
	 //omit i and put the i between j_m and j
	 //omit idxWhatToMove and put between idxWhereToMove-1 and idxWhereToMove
	 INFO("move up");
	 GraphNode nodeToMove = targets[idxWhatToMove];
	 double afterRemoveUp = tryToRemove(idxWhatToMove);
	 INFO("nodeToMove id "<<nodeToMove.id);
	 double afterAddUp = tryToAdd(nodeToMove, idxWhereToMove + 1);
	 distanceAfterMove = distanceBefore - (distanceBefore - afterRemoveUp) + (afterAddUp - distanceBefore);
	 }
	 */
	return distanceAfterMove;
}

double DOPN::tryToMove(int idxWhatToMove, int idxWhereToMove) {
	//INFO("tryToMove multi begin");
//INFO_GREEN("tryToMove idxWhatToMove " << idxWhatToMove << " idxWhereToMove " << idxWhereToMove);
//INFO("tryToMove getNumTargets "<<getNumTargets());
	if (idxWhatToMove >= targets.size() || idxWhatToMove < 0 || idxWhereToMove > targets.size() || idxWhereToMove < 0
			|| abs(idxWhereToMove - idxWhatToMove) < 2) {
		INFO_RED("tryToMove idxWhatToMove " << idxWhatToMove << " idxWhereToMove " << idxWhereToMove);
		INFO_RED("tryToMove getNumTargets "<<getNumTargets());
		INFO_RED("tryToMove idxWhatToMove "<<idxWhatToMove<<" or idxWhereToMove "<<idxWhereToMove<<" exceeds maximum for target size"<<(targets.size()));
		exit(1);
	}
	//INFO("-------------------------------------------------------------------------");
	bool moveUp = idxWhereToMove > idxWhatToMove;
	int idxFrom = MIN(idxWhatToMove, idxWhereToMove);
	int idxTo = MAX(idxWhatToMove, idxWhereToMove);

	std::stringstream ss;
//in targets
//normal path   - till idxFrom-1
//reversed path - from idxTo till idxFrom
//normal path   - idxTo + 1

//in samples
//normal path   - till idxStartSamples-1
//reversed path - from idxEndSamples till idxStartSamples
//normal path   - idxEndSamples + 1

//change indexing from targets to samples
	int idxStartSamples = idxFrom + 1;
	int idxEndSamples = idxTo + 1;

	int i = idxFrom + 1;

	int j = idxTo + 1;
	//INFO("get samples");
	std::vector<std::vector<GraphNode_AngNeigh>> samples_i_m = samples[i - 1]; //i-
	std::vector<std::vector<GraphNode_AngNeigh>> samples_i = samples[i]; //i
	std::vector<std::vector<GraphNode_AngNeigh>> samples_i_p = samples[i + 1]; //i+1
	std::vector<std::vector<GraphNode_AngNeigh>> samples_j_m = samples[j - 1]; //j-1
	std::vector<std::vector<GraphNode_AngNeigh>> samples_j = samples[j]; //j
	//INFO("get node ids");
	int idI_m = samples[i - 1][0][0].node.id;
	int idI = samples[i][0][0].node.id;
	int idI_p = samples[i + 1][0][0].node.id;

	int idJ_m = samples[j - 1][0][0].node.id;
	int idJ = samples[j][0][0].node.id;
	//INFO("get shortest");
	dop_2d_matrix_triple normalOrderTotLen = shortest[i - 1];		// need to use idxStart+1 as to the

	dop_3d_matrix_triple pathPartShortest;
	int numCityBetween = (idxEndSamples - idxStartSamples + 2);
	//INFO("numCityBetween "<<numCityBetween);
	pathPartShortest.resize(numCityBetween);

	pathPartShortest[0] = normalOrderTotLen;		// set actual shortest distance to idxStart
	double minLen = M;
	int idxPathPartShortest = 1;

	if (moveUp) {
		//INFO("move UP");

		//move up
		//omit i and put the i between j_m and j
		//omit idxWhatToMove and put between idxWhereToMove-1 and idxWhereToMove

		//INFO_GREEN("move up");
		//INFO("calc shortest from idI_m "<< idI_m <<" to idI_p "<< idI_p);
		//INFO_VAR(idxPathPartShortest);
		//INFO_VAR(samples_i_m.size());
		//INFO_VAR(samples_i_p.size());
		pathPartShortest[idxPathPartShortest].resize(samples_i_p.size());
		for (int neighID = 0; neighID < samples_i_p.size(); ++neighID) {
			pathPartShortest[idxPathPartShortest][neighID].assign(samples_i_p[neighID].size(), NeighAngDist(-1, -1, M));
		}

		//INFO("ommit "<<samples_i[0][0].node.id<<" that is between "<<samples_i_m[0][0].node.id<<" and "<<samples_i_p[0][0].node.id);

		for (int neighID1 = 0; neighID1 < pathPartShortest[idxPathPartShortest - 1].size(); ++neighID1) {
			for (int neighID2 = 0; neighID2 < pathPartShortest[idxPathPartShortest].size(); ++neighID2) {
				for (int idx1 = 0; idx1 < samples_i_m[neighID1].size(); idx1++) {
					for (int idx2 = 0; idx2 < samples_i_p[neighID2].size(); idx2++) {
						auto len1 = pathPartShortest[idxPathPartShortest - 1][neighID1][idx1].distance; //shortest dist from start to [target - 1][idx1]
						auto len2 = allDistances[idI_m][idI_p][neighID1][neighID2][idx1][idx2]; //dist from [target - 1][idx1] to [target][idx2]
						auto nLen = len1 + len2;
						auto &len = pathPartShortest[idxPathPartShortest][neighID2][idx2]; // actual shortest path from start to [target][idx2]
						if (nLen < len.distance) {
							len.idNeigh = neighID1;
							len.idAng = idx1;
							len.distance = nLen;
						}
					}
				}
			}
		}
		idxPathPartShortest++; //2
		//INFO("calc shortest from "<< idI_p <<" to other");
		for (int target = i + 1; target < j - 1; target++) {
			//INFO("target "<<target);
			int idFrom = target;
			int idTo = target + 1;
			pathPartShortest[idxPathPartShortest].resize(samples[idTo].size());
			for (int neighID = 0; neighID < samples[idTo].size(); ++neighID) {
				pathPartShortest[idxPathPartShortest][neighID].assign(samples[idTo][neighID].size(), NeighAngDist(-1, -1, M));
				//INFO_VAR(samples[idTo][neighID].size());
			}

			//INFO("a calc shortest from "<< samples[idFrom][0][0].node.id <<" to "<< samples[idTo][0][0].node.id);
			//INFO_VAR(idFrom);
			//INFO_VAR(idTo);
			//INFO_VAR(samples[idFrom][0][0].node.id);
			//INFO_VAR(samples[idTo][0][0].node.id);
			//INFO_VAR(idxPathPartShortest);
			//INFO_VAR(pathPartShortest.size());
			//INFO_VAR(pathPartShortest[idxPathPartShortest - 1].size());
			//INFO_VAR(pathPartShortest[idxPathPartShortest].size());

			//INFO_VAR(samples[idFrom].size());
			//INFO_VAR(samples[idTo].size());
			//INFO("idFrom " << samples[idFrom][0][0].node.id);
			//INFO("idto "<< samples[idTo][0][0].node.id);

			//INFO_VAR(samples[idFrom].size());
			//INFO_VAR(samples[idTo].size());
			for (int neighID1 = 0; neighID1 < samples[idFrom].size(); ++neighID1) {
				for (int neighID2 = 0; neighID2 < samples[idTo].size(); ++neighID2) {
					const int S1 = samples[idFrom][neighID1].size();		// from
					const int S2 = samples[idTo][neighID2].size();		//to
					for (int idx1 = 0; idx1 < S1; idx1++) {
						for (int idx2 = 0; idx2 < S2; idx2++) {
							//			INFO("from");
							auto & from = samples[idFrom][neighID1][idx1];
							//		INFO("to");
							auto & to = samples[idTo][neighID2][idx2];
							//	INFO("len1");
							auto len1 = pathPartShortest[idxPathPartShortest - 1][neighID1][idx1].distance;		//shortest dist from start to [target - 1][idx1]
							//	INFO("len2");
							auto len2 = allDistances[from.node.id][to.node.id][neighID1][neighID2][idx1][idx2];
							//	INFO("nLen");
							auto nLen = len1 + len2;

							auto &len = pathPartShortest[idxPathPartShortest][neighID2][idx2];				// actual shortest path from start to [target][idx2]

							if (nLen < len.distance) {
								len.idNeigh = neighID1;
								len.idAng = idx1;
								len.distance = nLen;
							}
						}
					}
				}
			}
			idxPathPartShortest++;
		}
		//INFO("after");

		pathPartShortest[idxPathPartShortest].resize(samples_i.size());
		for (int neighID = 0; neighID < samples_i.size(); ++neighID) {
			pathPartShortest[idxPathPartShortest][neighID].assign(samples_i[neighID].size(), NeighAngDist(-1, -1, M));
		}
		//INFO("b calc shortest from idJ_m "<< idJ_m <<" to idI "<< idI);
		for (int neighID1 = 0; neighID1 < pathPartShortest[idxPathPartShortest - 1].size(); ++neighID1) {
			for (int neighID2 = 0; neighID2 < pathPartShortest[idxPathPartShortest].size(); ++neighID2) {
				for (int idx1 = 0; idx1 < samples_j_m[neighID1].size(); idx1++) {
					for (int idx2 = 0; idx2 < samples_i[neighID2].size(); idx2++) {
						//INFO_VAR(neighID1);
						//INFO_VAR(neighID2);
						//INFO_VAR(idx1);
						//INFO_VAR(idx2);
						//INFO("b get pathPartShortest");
						auto len1 = pathPartShortest[idxPathPartShortest - 1][neighID1][idx1].distance;			//shortest dist from start to [target - 1][idx1]
						//INFO("b get all distances");
						auto len2 = allDistances[idJ_m][idI][neighID1][neighID2][idx1][idx2];					//dist from [target - 1][idx1] to [target][idx2]
						auto nLen = len1 + len2;
						//INFO("b get pathPartShortest2");
						auto &len = pathPartShortest[idxPathPartShortest][neighID2][idx2];					// actual shortest path from start to [target][idx2]
						if (nLen < len.distance) {
							len.idNeigh = neighID1;
							len.idAng = idx1;
							len.distance = nLen;
						}
					}
				}
			}
		}
		idxPathPartShortest++;

		//INFO("add "<<samples_i_m[0][0].node.id<<" beween "<<samples_j_m[0][0].node.id<<" and "<<samples_j[0][0].node.id);

		dop_2d_matrix_triple normalOrderFromLen = shortest_back[j];
		//INFO("c calc shortest from idI "<< idI <<" to idJ "<< idJ);
		for (int neighID1 = 0; neighID1 < pathPartShortest[idxPathPartShortest - 1].size(); ++neighID1) {
			for (int neighID2 = 0; neighID2 < normalOrderFromLen.size(); ++neighID2) {
				for (int idx1 = 0; idx1 < samples_i[neighID1].size(); idx1++) {
					for (int idx2 = 0; idx2 < samples_j[neighID2].size(); idx2++) {
						auto len1 = pathPartShortest[idxPathPartShortest - 1][neighID1][idx1].distance; //shortest dist from start to [target - 1][idx1]
						auto len2 = allDistances[idI][idJ][neighID1][neighID2][idx1][idx2]; //dist from [target - 1][idx1] to [target][idx2]

						//INFO("shortest "<<" len1 "<<len1 <<" len2 "<<len2<<" normalOrderFromLen[idx2].second "<<normalOrderFromLen[idx2].second);
						auto nLen = len1 + len2 + normalOrderFromLen[neighID2][idx2].distance;
						if (nLen < minLen) {
							minLen = nLen;
						}
					}
				}
			}
		}
		//INFO("end move up");
		//end move up

	} else {
		//INFO("move DOWN");

		//end move down
		//INFO("move down");
		//INFO("d calc shortest from idI_m "<< idI_m <<" to idJ "<< idJ);
		pathPartShortest[idxPathPartShortest].resize(samples_j.size());
		for (int neighID = 0; neighID < samples_j.size(); ++neighID) {
			pathPartShortest[idxPathPartShortest][neighID].assign(samples_j[neighID].size(), NeighAngDist(-1, -1, M));
		}

		//INFO("add "<<idJ <<" between "<<idI_m<<" and "<<idI);

		for (int neighID1 = 0; neighID1 < pathPartShortest[idxPathPartShortest - 1].size(); ++neighID1) {
			for (int neighID2 = 0; neighID2 < pathPartShortest[idxPathPartShortest].size(); ++neighID2) {
				for (int idx1 = 0; idx1 < pathPartShortest[idxPathPartShortest - 1][neighID1].size(); idx1++) {
					for (int idx2 = 0; idx2 < pathPartShortest[idxPathPartShortest][neighID2].size(); idx2++) {
						//INFO_VAR(neighID1);
						//INFO_VAR(neighID2);
						//INFO_VAR(idx1);
						//INFO_VAR(idx2);
						auto len1 = pathPartShortest[idxPathPartShortest - 1][neighID1][idx1].distance;			//shortest dist from start to [target - 1][idx1]
						//INFO("d get all distances");
						auto len2 = allDistances[idI_m][idJ][neighID1][neighID2][idx1][idx2];					//dist from [target - 1][idx1] to [target][idx2]
						auto nLen = len1 + len2;
						//INFO("d get path part shortest");
						auto &len = pathPartShortest[idxPathPartShortest][neighID2][idx2];					// actual shortest path from start to [target][idx2]
						//INFO("ok");
						if (nLen < len.distance) {
							len.idNeigh = neighID1;
							len.idAng = idx1;
							len.distance = nLen;
						}
					}
				}
			}
		}
		idxPathPartShortest++; //2

		//INFO("e calc shortest from idJ "<< idJ <<" to idI "<< idI);
		pathPartShortest[idxPathPartShortest].resize(samples_i.size());
		for (int neighID = 0; neighID < samples_i.size(); ++neighID) {
			pathPartShortest[idxPathPartShortest][neighID].assign(samples_i[neighID].size(), NeighAngDist(-1, -1, M));
		}
		for (int neighID1 = 0; neighID1 < pathPartShortest[idxPathPartShortest - 1].size(); ++neighID1) {
			for (int neighID2 = 0; neighID2 < pathPartShortest[idxPathPartShortest].size(); ++neighID2) {
				for (int idx1 = 0; idx1 < pathPartShortest[idxPathPartShortest - 1][neighID1].size(); idx1++) {
					for (int idx2 = 0; idx2 < pathPartShortest[idxPathPartShortest][neighID2].size(); idx2++) {
						auto len1 = pathPartShortest[idxPathPartShortest - 1][neighID1][idx1].distance; //shortest dist from start to [target - 1][idx1]
						auto len2 = allDistances[idJ][idI][neighID1][neighID2][idx1][idx2]; //dist from [target - 1][idx1] to [target][idx2]
						auto nLen = len1 + len2;
						auto &len = pathPartShortest[idxPathPartShortest][neighID2][idx2]; // actual shortest path from start to [target][idx2]
						if (nLen < len.distance) {
							len.idNeigh = neighID1;
							len.idAng = idx1;
							len.distance = nLen;
						}
					}
				}
			}
		}
		idxPathPartShortest++; //3
		//INFO("f calc shortest from idI "<< idI <<" to other");
		for (int target = i; target < j - 1; target++) {
			//INFO("target "<<target);
			int idFrom = target;
			int idTo = target + 1;
			pathPartShortest[idxPathPartShortest].resize(samples[idTo].size());
			for (int neighID = 0; neighID < samples[idTo].size(); ++neighID) {
				pathPartShortest[idxPathPartShortest][neighID].assign(samples[idTo][neighID].size(), NeighAngDist(-1, -1, M));
			}

			for (int neighID1 = 0; neighID1 < pathPartShortest[idxPathPartShortest - 1].size(); ++neighID1) {
				for (int neighID2 = 0; neighID2 < pathPartShortest[idxPathPartShortest].size(); ++neighID2) {
					for (int idx1 = 0; idx1 < pathPartShortest[idxPathPartShortest - 1][neighID1].size(); idx1++) {
						for (int idx2 = 0; idx2 < pathPartShortest[idxPathPartShortest][neighID2].size(); idx2++) {
							//INFO("g idFrom"<<idFrom<<" idTo "<<idTo);
							//INFO_VAR(neighID1);
							//INFO_VAR(neighID2);
							//INFO_VAR(idx1);
							//INFO_VAR(idx2);
							auto & from = samples[idFrom][neighID1][idx1];
							auto & to = samples[idTo][neighID2][idx2];

							auto len1 = pathPartShortest[idxPathPartShortest - 1][neighID1][idx1].distance;		//shortest dist from start to [target - 1][idx1]
							//auto len2 = distances[idTo][getInverseId(idx2)][getInverseId(idx1)]; //need to use idto because of indexing of distances
							auto len2 = allDistances[from.node.id][to.node.id][neighID1][neighID2][idx1][idx2];

							auto nLen = len1 + len2;
							//INFO(idx1<<" "<<idx2 <<" len1 "<<len1<<" len2 "<<len2<<" nLen "<<nLen);
							//INFO(idx1<<" "<<idx2<<" len "<<len1<<" dist "<< distances[idTo][idx2][idx1]);
							auto &len = pathPartShortest[idxPathPartShortest][neighID2][idx2];				// actual shortest path from start to [target][idx2]

							if (nLen < len.distance) {
								len.idNeigh = neighID1;
								len.idAng = idx1;
								len.distance = nLen;
							}
						}
					}
				}
			}

			idxPathPartShortest++;
		}
		//std::vector<GraphNode_AngNeigh> samples_j_p = samples[j + 1]; //j+1
		int idJ_p = samples[j + 1][0][0].node.id;
		dop_2d_matrix_triple normalOrderFromLen = shortest_back[j + 1];

		//INFO("ommit "<<idJ<<" that is between "<<idJ_m<<" and "<<idJ_p);

		for (int neighID1 = 0; neighID1 < pathPartShortest[idxPathPartShortest - 1].size(); ++neighID1) {
			for (int neighID2 = 0; neighID2 < normalOrderFromLen.size(); ++neighID2) {
				for (int idx1 = 0; idx1 < pathPartShortest[idxPathPartShortest - 1][neighID1].size(); idx1++) {
					for (int idx2 = 0; idx2 < normalOrderFromLen[neighID2].size(); idx2++) {
						auto len1 = pathPartShortest[idxPathPartShortest - 1][neighID1][idx1].distance; //shortest dist from start to [target - 1][idx1]
						auto len2 = allDistances[idJ_m][idJ_p][neighID1][neighID2][idx1][idx2]; //dist from [target - 1][idx1] to [target][idx2]

						//INFO("shortest "<<" len1 "<<len1 <<" len2 "<<len2<<" normalOrderFromLen[idx2].second "<<normalOrderFromLen[idx2].second);
						auto nLen = len1 + len2 + normalOrderFromLen[neighID2][idx2].distance;
						if (nLen < minLen) {
							minLen = nLen;
						}
					}
				}
			}
		}

	}
	//INFO("tryToMove multi end");

	//TODO
	/*
	 if (radius == 0) {
	 double euclideanMove = tryToMoveEuclidean(idxWhatToMove, idxWhereToMove);
	 if (fabs(minLen - euclideanMove) > 0.001) {
	 INFO_VAR(euclideanMove);
	 ERROR("badly calculated tryToMoveEuclidean ");
	 INFO_VAR(minLen);
	 exit(1);
	 }
	 }
	 */
	return minLen;
}

double DOPN::tryTwoOpt(int idxFrom, int idxTo) {
//INFO_GREEN("tryTwoOpt from "<<idxFrom <<" to "<<idxTo);

//exit when dist idxStart to idxEnd < 2 ---- need minimally distance of two*
//reverse path between idx1+1 and idx2, so need
//ensure idxStart <= idxEnd
	if (idxTo - idxFrom < 2) {
		INFO_RED("idxEnd and idxStart are too close "<<(idxTo - idxFrom)<<" must be lower than 2");
		exit(1);
	}
	if (idxTo >= targets.size()) {
		INFO_RED("idxEnd+1 is out of samples array "<<idxTo<<" samples number "<<targets.size());
		exit(1);
	}
	std::stringstream ss;
//in targets
//normal path   - till idxFrom-1
//reversed path - from idxTo till idxFrom
//normal path   - idxTo + 1

//in samples
//normal path   - till idxStartSamples-1
//reversed path - from idxEndSamples till idxStartSamples
//normal path   - idxEndSamples + 1

//change indexing from targets to samples
	int idxStartSamples = idxFrom + 1;
	int idxEndSamples = idxTo + 1;
//INFO("samples size "<<samples.size());
//INFO("targets size "<<targets.size());
//INFO("idxStartSamples "<<idxStartSamples);
//INFO("idxEndSamples "<<idxEndSamples);

	int i = idxStartSamples - 1;
	int j = idxEndSamples;
	std::vector<std::vector<GraphNode_AngNeigh>> normalOrderToSamples = samples[i];		//i
	std::vector<std::vector<GraphNode_AngNeigh>> reverseOrderToSamples = samples[i + 1];		//i+1
	std::vector<std::vector<GraphNode_AngNeigh>> reverseOrderFromSamples = samples[j];		//j
	std::vector<std::vector<GraphNode_AngNeigh>> normalOrderFromSamples = samples[j + 1];		//j+1
	int idI = samples[i][0][0].node.id;
	int idI_p = samples[i + 1][0][0].node.id;
	int idJ = samples[j][0][0].node.id;
	int idJ_p = samples[j + 1][0][0].node.id;
//INFO("i "<<i);
//INFO("i+1 "<<(i+1 ));
//INFO("j "<<(j));
//INFO("j+1 "<<(j + 1));

//INFO("new two opt order is:")
//INFO("..... " <<" "<<idxStartSamples-1<<" "<<idxEndSamples<<" ..... "<<idxStartSamples <<" "<<idxEndSamples + 1<<" .....")
//INFO("shortest.size() "<<shortest.size());
//INFO("shortest_back.size() "<<shortest_back.size());
	dop_2d_matrix_triple normalOrderTotLen = shortest[i];		// need to use idxStart+1 as to the
//INFO("shortest_back.size() "<<shortest_back.size());
//INFO("(j+1) is "<<(j + 1));
	dop_2d_matrix_triple normalOrderFromLen = shortest_back[j + 1];

//INFO("find shortest paths between reverseOrderFromSamples to reverseOrderToSamples");
// find shortest paths between reverseOrderFromSamples to reverseOrderToSamples
// find the shortest part of path that is reversed
// matrix:
//    1) to city (layer)
//    2) to idx i
	dop_3d_matrix_triple pathPartShortest;
	int numCityBetween = (idxEndSamples - idxStartSamples + 2);
//INFO("numCityBetween "<<numCityBetween);
	pathPartShortest.resize(numCityBetween);
	pathPartShortest[0] = normalOrderTotLen;		// set actual shortest distance to idxStart

	int idxPathPartShortest = 1;
//now calc shortest to reverseOrderFromSamples using distToReverseStart
//INFO("calc shortest from "<<normalOrderToSamples[0].node.id <<" to "<< reverseOrderFromSamples[0].node.id);
//pathPartShortest[idxPathPartShortest].assign(reverseOrderFromSamples.size(), dst_pair(-1, M));
	pathPartShortest[idxPathPartShortest].resize(neighborhood_resolution);
	for (int neighID = 0; neighID < neighborhood_resolution; ++neighID) {
		pathPartShortest[idxPathPartShortest][neighID].assign(reverseOrderFromSamples[neighID].size(), NeighAngDist(-1, -1, M));
	}
	for (int neighID1 = 0; neighID1 < neighborhood_resolution; ++neighID1) {
		for (int neighID2 = 0; neighID2 < neighborhood_resolution; ++neighID2) {

			for (int idx1 = 0; idx1 < normalOrderToSamples.size(); idx1++) {
				for (int idx2 = 0; idx2 < reverseOrderFromSamples.size(); idx2++) {
					auto len1 = pathPartShortest[idxPathPartShortest - 1][neighID1][idx1].distance; //shortest dist from start to [target - 1][idx1]
					auto len2 = allDistances[idI][idJ][neighID1][neighID2][idx1][idx2]; //dist from [target - 1][idx1] to [target][idx2]
					auto nLen = len1 + len2;
					auto &len = pathPartShortest[idxPathPartShortest][neighID2][idx2]; // actual shortest path from start to [target][idx2]
					if (nLen < len.distance) {
						len.idNeigh = neighID1;
						len.idAng = idx1;
						len.distance = nLen;
					}
				}
			}
		}
	}

	idxPathPartShortest++; //2

//calc from j to i+1
	for (int target = idxEndSamples - 1; target >= idxStartSamples; target--) {
		//INFO("----------------");
		//pathPartShortest[idxPathPartShortest].assign(samples[target].size(), dst_pair(-1, M));
		pathPartShortest[idxPathPartShortest].resize(neighborhood_resolution);
		for (int neighID = 0; neighID < neighborhood_resolution; ++neighID) {
			pathPartShortest[idxPathPartShortest][neighID].assign(samples[target][neighID].size(), NeighAngDist(-1, -1, M));
		}
		int idFrom = target + 1;
		int idTo = target;
		//INFO("idFrom " << samples[idFrom][0].node.id);
		//INFO("idto "<< samples[idTo][0].node.id);

		for (int neighID1 = 0; neighID1 < neighborhood_resolution; ++neighID1) {
			for (int neighID2 = 0; neighID2 < neighborhood_resolution; ++neighID2) {
				const int S1 = samples[idFrom][neighID1].size(); // from
				const int S2 = samples[idTo][neighID2].size(); //to

				for (int idx1 = 0; idx1 < S1; idx1++) {
					for (int idx2 = 0; idx2 < S2; idx2++) {
						auto & from = samples[idFrom][neighID1][idx1];
						auto & to = samples[idTo][neighID2][idx2];

						auto len1 = pathPartShortest[idxPathPartShortest - 1][neighID1][idx1].distance; //shortest dist from start to [target - 1][idx1]
						//auto len2 = distances[idTo][getInverseId(idx2)][getInverseId(idx1)]; //need to use idto because of indexing of distances
						auto len2 = allDistances[from.node.id][to.node.id][neighID1][neighID2][idx1][idx2];

						auto nLen = len1 + len2;
						//INFO(idx1<<" "<<idx2 <<" len1 "<<len1<<" len2 "<<len2<<" nLen "<<nLen);
						//INFO(idx1<<" "<<idx2<<" len "<<len1<<" dist "<< distances[idTo][idx2][idx1]);
						auto &len = pathPartShortest[idxPathPartShortest][neighID2][idx2];					// actual shortest path from start to [target][idx2]

						if (nLen < len.distance) {
							len.idNeigh = neighID1;
							len.idAng = idx1;
							len.distance = nLen;
						}
					}
				}
			}
		}
		idxPathPartShortest++;
	}

	double minLen = M;
	for (int neighID1 = 0; neighID1 < neighborhood_resolution; ++neighID1) {
		for (int neighID2 = 0; neighID2 < neighborhood_resolution; ++neighID2) {

			for (int idx1 = 0; idx1 < reverseOrderToSamples.size(); idx1++) {
				for (int idx2 = 0; idx2 < normalOrderFromSamples.size(); idx2++) {
					auto len1 = pathPartShortest[idxPathPartShortest - 1][neighID1][idx1].distance; //shortest dist from start to [target - 1][idx1]
					auto len2 = allDistances[idI_p][idJ_p][neighID1][neighID2][idx1][idx2]; //dist from [target - 1][idx1] to [target][idx2]

					//INFO("shortest "<<" len1 "<<len1 <<" len2 "<<len2<<" normalOrderFromLen[idx2].second "<<normalOrderFromLen[idx2].second);
					auto nLen = len1 + len2 + normalOrderFromLen[neighID2][idx2].distance;
					if (nLen < minLen) {
						minLen = nLen;
					}
				}
			}
		}
	}
//INFO("minLen "<<minLen);
	return minLen;
}

void DOPN::twoOpt(int idxFrom, int idxTo) {

	int idxStartSamples = idxFrom + 1;
	int idxEndSamples = idxTo + 1;
//INFO("new two opt order is:")
//INFO("..... " <<" "<<idxStartSamples-1<<" "<<idxEndSamples<<" ..... "<<idxStartSamples <<" "<<idxEndSamples + 1<<" .....")
//exit when dist idxStart to idxEnd < 2 ---- need minimally distance of two*
// reverse path between idx1+1 and idx2, so need
	if (idxTo - idxFrom < 2) {
		INFO_RED("idxEnd and idxStart are too close "<<(idxTo - idxFrom)<<" must be lower than 2");
		exit(1);
	}
	if (idxTo >= targets.size()) {
		INFO_RED("idxTo is out of samples array "<<idxTo<<" samples number "<<targets.size());
		exit(1);
	}

	auto copy = targets;
	targets.clear();
//normal path   - till idxFrom-1
//reversed path - from idxTo till idxFrom
//normal path idxTo + 1
	for (int i = 0; i <= idxFrom - 1; i++) {
		targets.push_back(copy[i]);
	}
	for (int i = idxTo; i >= idxFrom; i--) {
		targets.push_back(copy[i]);
	}
	for (int i = idxTo + 1; i < copy.size(); i++) {
		targets.push_back(copy[i]);
	}

	update();
}

void DOPN::update() {
//INFO("update begin");
	const int S = getSize(); //number of dubins maneuvers

// generate all samples
//INFO("samples.resize(S + 1)"<<(S + 1));
	samples.resize(S + 1);
	int idx = 0;

	samples[0] = allSamples[start.id];
	idx++;
	for (auto & t : targets) {
		samples[idx] = allSamples[t.id];
		idx++;
	}
	samples[idx] = allSamples[end.id];
	/*
	 //oldcalc of samples
	 generateSamples(samples[idx++], start, resolution, neighborhood_radius, neighborhood_resolution, null_start_goal_radius, start.id, end.id);
	 for (auto & t : targets) {
	 generateSamples(samples[idx++], t, resolution, neighborhood_radius, neighborhood_resolution, null_start_goal_radius, start.id, end.id);
	 }
	 generateSamples(samples[idx], end, resolution, neighborhood_radius, neighborhood_resolution, null_start_goal_radius, start.id, end.id);
	 */

//INFO("samples generated");
// find the shortest path - forward path
//INFO("after update generate samples");
	shortest.resize(S + 1);
	shortest[0].resize(samples[0].size());
	for (int neighID = 0; neighID < samples[0].size(); ++neighID) {
		shortest[0][neighID].assign(samples[0][neighID].size(), NeighAngDist());
	}
//INFO("shortest resized");
	for (int target = 1; target < S + 1; target++) {
		shortest[target].resize(samples[target].size());
		for (int neighID = 0; neighID < samples[target].size(); ++neighID) {
			shortest[target][neighID].assign(samples[target][neighID].size(), NeighAngDist(-1, -1, M));
		}
		for (int fromNeighID = 0; fromNeighID < samples[target - 1].size(); ++fromNeighID) {
			auto & fromStates = samples[target - 1][fromNeighID];
			const int S1 = fromStates.size();

			for (int toNeighID = 0; toNeighID < samples[target].size(); ++toNeighID) {
				auto & toSamples = samples[target][toNeighID];
				const int S2 = toSamples.size();

				for (int idx1 = 0; idx1 < S1; idx1++) {
					for (int idx2 = 0; idx2 < S2; idx2++) {
						auto len1 = shortest[target - 1][fromNeighID][idx1].distance;

						auto len2 = allDistances[fromStates[0].node.id][toSamples[0].node.id][fromNeighID][toNeighID][idx1][idx2];
						auto nLen = len1 + len2;
						auto &len = shortest[target][toNeighID][idx2];
						if (nLen < len.distance) {
							len.idNeigh = fromNeighID;
							len.idAng = idx1;
							len.distance = nLen;
						}
					}
				}
			}
		}

	}

//INFO("find the shortest path - backward path");
// find the shortest path - backward path
	shortest_back.resize(S + 1);
	shortest_back[S].resize(samples[S].size());
	for (int neighID = 0; neighID < shortest_back[S].size(); ++neighID) {
		shortest_back[S][neighID].assign(samples[S][neighID].size(), NeighAngDist());
	}
//INFO("shortest_back resized");

	for (int target = S - 1; target >= 0; target--) {
		shortest_back[target].resize(samples[target].size());
		for (int neighID = 0; neighID < shortest_back[target].size(); ++neighID) {
			shortest_back[target][neighID].assign(samples[target][neighID].size(), NeighAngDist(-1, -1, M));
		}
		for (int fromNeighID = 0; fromNeighID < samples[target].size(); ++fromNeighID) {
			auto & fromStates = samples[target][fromNeighID];
			const int S1 = fromStates.size();
			for (int toNeighID = 0; toNeighID < samples[target + 1].size(); ++toNeighID) {
				auto & toSamples = samples[target + 1][toNeighID];
				const int S2 = toSamples.size();
				for (int idx1 = 0; idx1 < S1; idx1++) {
					for (int idx2 = 0; idx2 < S2; idx2++) {
						auto len1 = shortest_back[target + 1][toNeighID][idx2].distance;

						auto len2 = allDistances[fromStates[0].node.id][toSamples[0].node.id][fromNeighID][toNeighID][idx1][idx2];

						auto nLen = len1 + len2;
						auto &len = shortest_back[target][fromNeighID][idx1];
						if (nLen < len.distance) {
							len.idNeigh = toNeighID;
							len.idAng = idx2;
							len.distance = nLen;
						}
					}
				}
			}
		}
	}
//INFO("update end");
}

void DOPN::updateAfterInsert(int idxStart, int idxEnd) {
//INFO(" ------------------- ");
//INFO("updateAfterInsert "<<idxStart<<" "<<idxEnd);
	const int S = getSize(); //number of dubins maneuvers

//INFO("generateSamples");
	std::vector<std::vector<std::vector<GraphNode_AngNeigh>>>insertedSamples;
	insertedSamples.resize(idxEnd - idxStart + 1);
	for (int var = idxStart; var <= idxEnd; ++var) {
		insertedSamples[var - idxStart] = allSamples[targets[var].id];
	}
	/*
	 //old generation of aditional samples
	 for (int var = idxStart; var <= idxEnd; ++var) {
	 generateSamples(insertedSamples[var - idxStart], targets[var], resolution, neighborhood_radius, neighborhood_resolution, null_start_goal_radius,
	 start.id, end.id);
	 }
	 */
//INFO("ids before:");
//this->listIds();
	samples.insert(samples.begin() + idxStart + 1, insertedSamples.begin(), insertedSamples.end());
//INFO("ids after:");
//this->listIds();
//	INFO("shortest insert ");
	shortest.insert(shortest.begin() + idxStart + 1, idxEnd - idxStart + 1, dop_2d_matrix_triple());

	for (int target = idxStart + 1; target < S + 1; target++) {
		//INFO("resize shortes to "<<samples[target].size());

		shortest[target].resize(samples[target].size());
		for (int neighID = 0; neighID < samples[target].size(); ++neighID) {
			shortest[target][neighID].assign(samples[target][neighID].size(), NeighAngDist(-1, -1, M));
			//INFO("resize inside shortes to "<<samples[target][neighID].size());
		}

		//INFO("get dist from "<<samples[target - 1][0][0].node.id);
		//INFO("get dist to "<<samples[target ][0][0].node.id);

		for (int fromNeighID = 0; fromNeighID < samples[target - 1].size(); ++fromNeighID) {
			auto & fromStates = samples[target - 1][fromNeighID];
			const int S1 = fromStates.size();

			for (int toNeighID = 0; toNeighID < samples[target].size(); ++toNeighID) {
				auto & toSamples = samples[target][toNeighID];
				const int S2 = toSamples.size();

				for (int idx1 = 0; idx1 < S1; idx1++) {
					for (int idx2 = 0; idx2 < S2; idx2++) {
						auto len1 = shortest[target - 1][fromNeighID][idx1].distance;

						auto len2 = allDistances[fromStates[0].node.id][toSamples[0].node.id][fromNeighID][toNeighID][idx1][idx2];
						auto nLen = len1 + len2;
						auto &len = shortest[target][toNeighID][idx2];
						if (nLen < len.distance) {
							len.idNeigh = fromNeighID;
							len.idAng = idx1;
							len.distance = nLen;
						}
					}
				}
			}
		}
	}
//INFO("shortest_back insert ");
	shortest_back.insert(shortest_back.begin() + idxStart + 1, idxEnd - idxStart + 1, dop_2d_matrix_triple());

	for (int target = idxEnd + 1; target >= 0; target--) {
		shortest_back[target].resize(samples[target].size());
		//INFO("resize shortest_back from to "<<samples[target].size());
		for (int neighID = 0; neighID < shortest_back[target].size(); ++neighID) {
			shortest_back[target][neighID].assign(samples[target][neighID].size(), NeighAngDist(-1, -1, M));
			//INFO("resize inside shortest_back  to "<<samples[target][neighID].size());
		}

		//INFO("get dist from "<< samples[target ][0][0].node.id);
		//INFO("get dist to "<< samples[target +1][0][0].node.id);

		for (int fromNeighID = 0; fromNeighID < samples[target].size(); ++fromNeighID) {
			auto & fromStates = samples[target][fromNeighID];
			const int S1 = fromStates.size();
			for (int toNeighID = 0; toNeighID < samples[target + 1].size(); ++toNeighID) {
				auto & toSamples = samples[target + 1][toNeighID];
				const int S2 = toSamples.size();
				for (int idx1 = 0; idx1 < S1; idx1++) {
					for (int idx2 = 0; idx2 < S2; idx2++) {
						auto len1 = shortest_back[target + 1][toNeighID][idx2].distance;

						auto len2 = allDistances[fromStates[0].node.id][toSamples[0].node.id][fromNeighID][toNeighID][idx1][idx2];

						auto nLen = len1 + len2;
						auto &len = shortest_back[target][fromNeighID][idx1];
						if (nLen < len.distance) {
							len.idNeigh = toNeighID;
							len.idAng = idx2;
							len.distance = nLen;
						}
					}
				}
			}
		}
	}

//INFO("shortest_back evaluated ");
}

void DOPN::updateAfterRemove(int idxStart, int idxEnd) {
//INFO("updateAfterRemove "<<idxStart<<" "<<idxEnd);
	const int S = getSize();			//number of dubins maneuvers
//INFO("samples erase ");
	samples.erase(samples.begin() + idxStart + 1, samples.begin() + idxEnd + 2);

//INFO("shortest erase ");
	shortest.erase(shortest.begin() + idxStart + 1, shortest.begin() + idxEnd + 2);
//INFO("recalc shortest");

	for (int target = idxStart + 1; target < S + 1; target++) {
		//INFO("----------------");
		shortest[target].resize(samples[target].size());
		for (int neighID = 0; neighID < samples[target].size(); ++neighID) {
			shortest[target][neighID].assign(samples[target][neighID].size(), NeighAngDist(-1, -1, M));
		}

		for (int fromNeighID = 0; fromNeighID < samples[target - 1].size(); ++fromNeighID) {
			auto & fromStates = samples[target - 1][fromNeighID];
			const int S1 = fromStates.size();
			const int fromID = fromStates[0].node.id;

			for (int toNeighID = 0; toNeighID < samples[target].size(); ++toNeighID) {
				auto & toSamples = samples[target][toNeighID];
				const int S2 = toSamples.size();
				const int toID = toSamples[0].node.id;

				for (int idx1 = 0; idx1 < S1; idx1++) {
					for (int idx2 = 0; idx2 < S2; idx2++) {
						auto len1 = shortest[target - 1][fromNeighID][idx1].distance;
						//auto len2 = distances[target - 1][idx1][idx2];
						auto len2 = allDistances[fromID][toID][fromNeighID][toNeighID][idx1][idx2];
						//INFO(idx1<<" "<<idx2<<" len "<<len1<<" dist "<<distances[target - 1][idx1][idx2]);
						auto nLen = len1 + len2;
						auto &len = shortest[target][toNeighID][idx2];
						if (nLen < len.distance) {
							len.idNeigh = fromNeighID;
							len.idAng = idx1;
							len.distance = nLen;
						}

					}
				}
			}
		}
	}

	shortest_back.erase(shortest_back.begin() + idxStart + 1, shortest_back.begin() + idxEnd + 2);

	for (int target = idxStart; target >= 0; target--) {
		shortest_back[target].resize(samples[target].size());
		for (int neighID = 0; neighID < shortest_back[target].size(); ++neighID) {
			shortest_back[target][neighID].assign(samples[target][neighID].size(), NeighAngDist(-1, -1, M));
			//INFO("resize inside shortest_back  to "<<samples[target][neighID].size());
		}
		//INFO("target "<<target);
		//INFO("target+1 "<<target+1);
		for (int fromNeighID = 0; fromNeighID < samples[target].size(); ++fromNeighID) {
			auto & fromStates = samples[target][fromNeighID];
			const int S1 = fromStates.size();
			const int fromID = fromStates[0].node.id;

			for (int toNeighID = 0; toNeighID < samples[target + 1].size(); ++toNeighID) {
				auto & toSamples = samples[target + 1][toNeighID];
				const int S2 = toSamples.size();
				const int toID = toSamples[0].node.id;

				for (int idx1 = 0; idx1 < S1; idx1++) {
					for (int idx2 = 0; idx2 < S2; idx2++) {
						auto len1 = shortest_back[target + 1][toNeighID][idx2].distance;
						//auto len2 = distances[target][idx1][idx2];
						auto len2 = allDistances[fromID][toID][fromNeighID][toNeighID][idx1][idx2];
						auto nLen = len1 + len2;
						//INFO("len1 "<<len1<<" len2 "<<len2);
						auto &len = shortest_back[target][fromNeighID][idx1];
						if (nLen < len.distance) {
							len.idNeigh = toNeighID;
							len.idAng = idx2;
							len.distance = nLen;
						}
					}
				}
			}
		}
	}
}

std::vector<Dubins> DOPN::getPath() const {
//INFO("getPath begin");
	vector<Dubins> ret;

	const int S = getSize();

	int target = S;
	int idx = 0;
	int neighID = 0;
	double minLen = M;

	double sum = 0;

	for (int i = 0; i < shortest[S].size(); i++) {
		for (int j = 0; j < shortest[S][i].size(); j++) {
			auto & p = shortest[S][i][j];
			if (p.distance < minLen) {
				minLen = p.distance;
				idx = j;
				neighID = i;
			}
		}
	}

	while (target > 0) {
		std::vector<std::vector<GraphNode_AngNeigh>> fromStates = samples[target - 1];
		std::vector<std::vector<GraphNode_AngNeigh>> toSamples = samples[target];

		int lastIdx = shortest[target][neighID][idx].idAng;
		int lastNeighID = shortest[target][neighID][idx].idNeigh;

		GraphNode_AngNeigh & from = fromStates[lastNeighID][lastIdx];
		GraphNode_AngNeigh & to = toSamples[neighID][idx];
		Dubins dub(from.toState(), to.toState(), radius);

		sum += dub.length;
		ret.push_back(dub);

		idx = lastIdx;
		neighID = lastNeighID;
		target--;
	}

	std::reverse(ret.begin(), ret.end());
//INFO("getPath end");
	return ret;
}

std::vector<NeighAngValuesIds> DOPN::getPathNeighAngIds() const {
	//INFO("getPathNeighAngIds begin");
	vector<NeighAngValuesIds> ret;

	const int S = getSize();

	int target = S;
	int idx = 0;
	int neighID = 0;
	double minLen = M;

	double sum = 0;

	for (int i = 0; i < shortest[S].size(); i++) {
		for (int j = 0; j < shortest[S][i].size(); j++) {
			auto & p = shortest[S][i][j];
			if (p.distance < minLen) {
				minLen = p.distance;
				idx = j;
				neighID = i;
			}
		}
	}
	//INFO("shortest found");
	NeighAngValuesIds neighAngId;
	neighAngId.idAng = idx;
	neighAngId.idNeigh = neighID;
	neighAngId.ang = samples[S][neighID][idx].ang;
	neighAngId.neigh_ang = samples[S][neighID][idx].neigh_ang;
	neighAngId.node = samples[S][neighID][idx].node;
	ret.push_back(neighAngId);
	//INFO("first added "<<S);

	while (target > 0) {
		std::vector<std::vector<GraphNode_AngNeigh>> fromStates = samples[target - 1];
		std::vector<std::vector<GraphNode_AngNeigh>> toSamples = samples[target];

		int lastIdx = shortest[target][neighID][idx].idAng;
		int lastNeighID = shortest[target][neighID][idx].idNeigh;

		idx = lastIdx;
		neighID = lastNeighID;

		NeighAngValuesIds neighAngId;
		neighAngId.idAng = idx;
		neighAngId.idNeigh = neighID;
		neighAngId.ang = samples[target - 1][neighID][idx].ang;
		neighAngId.neigh_ang = samples[target - 1][neighID][idx].neigh_ang;
		neighAngId.node = samples[target - 1][neighID][idx].node;
		ret.push_back(neighAngId);

		//INFO("other added "<<target-1);
		target--;
	}

	std::reverse(ret.begin(), ret.end());
	//INFO("getPathNeighAngIds end");
	return ret;
}

double DOPN::getPathLength() const {
	//INFO("getPathLength beg");
	const int S = getSize();
//INFO("shortest.size() "<<shortest.size());
	double minLen = M;
	for (int neigh = 0; neigh < shortest[S].size(); neigh++) {
		for (int idx = 0; idx < shortest[S][neigh].size(); idx++) {
			NeighAngDist p = shortest[S][neigh][idx];
			if (p.distance < minLen) {
				minLen = p.distance;
			}
		}
	}
	//INFO("getPathLength end");
	return minLen;
}

int DOPN::getNumTargets() {
	return targets.size();
}

GraphNode DOPN::getTarget(int idx) {
	if (idx >= targets.size() || idx < 0) {
		INFO_RED("getTarget idx "<<idx<<" exceeds maximum "<<(targets.size()-1));
		exit(1);
	}
	return targets[idx];
}

std::vector<GraphNode> DOPN::getTargets(int idxFrom, int idxTo) {
	return std::vector<GraphNode>(targets.begin() + idxFrom, targets.begin() + idxTo + 1);
}

std::vector<std::vector<std::vector<GraphNode_AngNeigh>>>DOPN::getSamples()
{
	return samples;
}

int DOPN::getNumSamples() {
	return samples.size();
}

void DOPN::listIds() {
	std::cout << "target ids: " << std::endl;
	for (int var = 0; var < samples.size(); ++var) {
		std::cout << samples[var][0][0].node.id << " ";
	}
	std::cout << std::endl;
}

std::vector<GraphNode>::iterator DOPN::targetsBegin() {
	return targets.begin();
}

std::vector<GraphNode>::iterator DOPN::targetsEnd() {
	return targets.end();
}

/*
 std::vector<State> DOPN::getPathSampled(double samplePath) {
 std::vector<State> path;
 std::vector<Dubins> dubinsPath = getPath();
 //INFO("we have "<<dubinsPath.size()<<" dubins paths");
 bool allSampled = false;
 int dubinsIndex = 0;
 double actualDistance = 0;
 while (!allSampled) {
 if (dubinsIndex < dubinsPath.size()) {
 Dubins actualDubins = dubinsPath[dubinsIndex];
 double actualDubinsLen = actualDubins.length;
 if (actualDistance <= actualDubinsLen) {
 State addState = actualDubins.getState(actualDistance);
 path.push_back(addState);
 actualDistance += samplePath;
 } else {
 actualDistance = actualDubinsLen - actualDubinsLen;
 dubinsIndex++;
 }
 } else {
 path.push_back(dubinsPath[dubinsPath.size() - 1].end);
 allSampled = true;
 }
 }
 return path;
 }

 std::vector<State> DOPN::getPathSampled(double initialSpeed, double maximal_speed, double maximal_acceleration,
 double time_sample_rate, double finalSpeed) {
 std::vector<State> path;
 std::vector<Dubins> dubinsPath = getPath();
 INFO("getPathSampled we have "<<dubinsPath.size()<<" dubins paths");
 double wholePathLength = getPathLength();
 bool allSampled = false;
 double actualSpeed = initialSpeed;
 int dubinsIndex = 0;
 bool deceleration = false;
 double decelerationAcc = -maximal_acceleration;
 double actualDistanceWhole = 0;
 double time = 0;
 double actualDistance = 0;
 if (dubinsPath.size() > 0) {
 while (!allSampled) {
 if (dubinsIndex < dubinsPath.size()) {
 Dubins actualDubins = dubinsPath[dubinsIndex];
 double actualDubinsLen = actualDubins.length;
 if (actualDistance <= actualDubinsLen) {

 time += time_sample_rate;
 double distanceLeft = wholePathLength - (actualDistanceWhole + actualSpeed * time_sample_rate);
 double finalVelInSec = (actualSpeed - finalSpeed) / maximal_acceleration;
 double distanceToStop = 0.5 * (-maximal_acceleration) * finalVelInSec * finalVelInSec
 + actualSpeed * finalVelInSec;
 if (!deceleration && distanceLeft < distanceToStop) {
 //INFO("distanceLeft "<<distanceLeft);
 //INFO("finalVelInSec "<<finalVelInSec);
 //INFO("distanceToStop "<<distanceToStop);
 distanceLeft = wholePathLength - (actualDistanceWhole);
 deceleration = true;
 decelerationAcc = -(distanceLeft - (actualSpeed * finalVelInSec))
 / (0.5 * finalVelInSec * finalVelInSec);
 //INFO("decelerationAcc "<<decelerationAcc);
 }
 if (deceleration) {
 actualSpeed -= decelerationAcc * time_sample_rate;
 distanceLeft = wholePathLength - (actualDistanceWhole);
 decelerationAcc = -(distanceLeft - (actualSpeed * finalVelInSec))
 / (0.5 * finalVelInSec * finalVelInSec);
 if (decelerationAcc > maximal_acceleration) {
 decelerationAcc = maximal_acceleration;
 }
 //INFO("decelerationAcc "<<decelerationAcc);
 if (actualSpeed < decelerationAcc * time_sample_rate) {
 actualSpeed = decelerationAcc * time_sample_rate;
 }
 } else {
 actualSpeed += maximal_acceleration * time_sample_rate;
 actualSpeed = MIN(actualSpeed, maximal_speed);
 }
 INFO(time<<" - "<<actualSpeed);
 State addState = actualDubins.getState(actualDistance);
 path.push_back(addState);
 double actualSamle = actualSpeed * time_sample_rate;
 //INFO_BLUE("actualDistance " <<actualDistance<<"sample "<<actualSamle);
 actualDistance += actualSamle;
 actualDistanceWhole += actualSamle;
 } else {
 actualDistance = actualDistance - actualDubinsLen;
 dubinsIndex++;
 }
 } else {
 path.push_back(dubinsPath[dubinsPath.size() - 1].end);
 allSampled = true;
 }
 }
 } else {
 path.push_back(State(start.x, start.y, 0));
 }
 return path;
 }
 */

double DOPN::getReward() {
	double reward = 0;
	for (int var = 0; var < targets.size(); ++var) {
		reward += targets[var].reward;
	}
	return reward;
}

}
