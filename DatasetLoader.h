/*
 * DatasetLoader.h
 *
 *  Created on: 15. 3. 2016
 *      Author: Robert Penicka
 */

#ifndef SRC_COMMON_DATASETLOADER_H_
#define SRC_COMMON_DATASETLOADER_H_
#include "HeuristicTypes.h"
#include "logging.h"
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream

class DatasetLoader {
public:
	DatasetLoader();
	virtual ~DatasetLoader();
	static DatasetOP loadDataset(std::string filename);
};

#endif /* SRC_COMMON_DATASETLOADER_H_ */
