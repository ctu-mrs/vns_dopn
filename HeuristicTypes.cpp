/*
 * HeuristicTypes.cpp
 *
 *  Created on: Feb 22, 2016
 *      Author: Robert Penicka
 */

#include "HeuristicTypes.h"

HeuristicTypes::HeuristicTypes() {


}

HeuristicTypes::~HeuristicTypes() {

}

bool operator==(const GraphNode& lhs, const GraphNode& rhs) {
	//WINFO("operator== " << lhs.x  <<" == "<< rhs.x);
	return lhs.x == rhs.x && lhs.y == rhs.y && lhs.reward == rhs.reward;
}

std::ostream& operator <<(std::ostream &o, GraphNode& p) {
	std::cout.precision(6);
	o << std::fixed << " " << std::setprecision(6) << p.x << " " << std::setprecision(6) << p.y << " " << std::setprecision(6) << p.reward;
	return o;
}

bool operator>(GraphNode &dt1, GraphNode &dt2) {
	return dt1.reward > dt2.reward;
}

bool operator<(GraphNode &dt1, GraphNode &dt2) {
	return dt1.reward < dt2.reward;
}
