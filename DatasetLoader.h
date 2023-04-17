/*
 * DatasetLoader.h
 *
 *  Created on: 15. 3. 2016
 *      Author: Robert Penicka
 */

#ifndef SRC_COMMON_DATASETLOADER_H_
#define SRC_COMMON_DATASETLOADER_H_
#include "HeuristicTypes.h"
#include "crl/logging.h"
#include <fstream>  // std::ifstream
#include <iostream> // std::cout

class DatasetLoader {
public:
  DatasetLoader();
  virtual ~DatasetLoader();
  static DatasetOP loadDataset(std::string filename);
};

#endif /* SRC_COMMON_DATASETLOADER_H_ */
