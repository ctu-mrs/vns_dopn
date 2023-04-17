//============================================================================
// Name        : h_dop.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description :
//============================================================================

#include "iostream"
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>

#include <boost/foreach.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/program_options.hpp>

#include "crl/exceptions.h"
#include "crl/gui/guifactory.h"
#include "crl/gui/shapes.h"
#include "crl/gui/win_adjust_size.h"
#include "crl/logging.h"
#include "crl/perf_timer.h"
#include "crl/timerN.h"

#include "canvasview_coords.h"

#include "crl/boost_args_config.h"
#include "crl/config.h"
#include "vns_dopn.h"

const std::string FP_OP_VERSION = "0.1";

using crl::logger;

namespace po = boost::program_options;
namespace fs = boost::filesystem;

using namespace crl;
using namespace crl::gui;

typedef crl::gui::CCanvasBase Canvas;
typedef op::VNSDOPN Solver;

typedef std::vector<Coords> CoordsVector;
typedef std::vector<CoordsVector> CoordsVectorVector;

crl::gui::CGui *g = 0;

/// ----------------------------------------------------------------------------
/// Program options variables
/// ----------------------------------------------------------------------------
std::string guiType = "none";
std::string problemFile;

crl::CConfig guiConfig;
crl::CConfig tspConfig;
std::string canvasOutput = "";

#define GUI(x)                                                                 \
  if (gui) {                                                                   \
    x;                                                                         \
  }

bool parseArgs(int argc, char *argv[]) {
  bool ret = true;
  std::string configFile;
  std::string guiConfigFile;
  std::string loggerCfg = "";

  po::options_description desc("General options");
  desc.add_options()("help,h", "produce help message")(
      "config,c",
      po::value<std::string>(&configFile)
          ->default_value(std::string(argv[0]) + ".cfg"),
      "configuration file")(
      "logger-config,l",
      po::value<std::string>(&loggerCfg)->default_value(loggerCfg),
      "logger configuration file")(
      "config-gui",
      po::value<std::string>(&guiConfigFile)
          ->default_value(std::string(argv[0]) + "-gui.cfg"),
      "dedicated gui configuration file")(
      "problem", po::value<std::string>(&problemFile), "problem file");
  try {
    po::options_description guiOptions("Gui options");
    crl::gui::CGuiFactory::getConfig(guiConfig);
    crl::gui::CWinAdjustSize::getConfig(guiConfig);
    guiConfig.add<double>("gui-add-x",
                          "add the given value to the loaded goals x coord to "
                          "determine the canvas size and transformation",
                          0);
    guiConfig.add<double>("gui-add-y",
                          "add the given value to the loaded goals y coord to "
                          "determine the canvas size and transformation",
                          0);
    boost_args_add_options(guiConfig, "", guiOptions);
    guiOptions.add_options()("canvas-output",
                             po::value<std::string>(&canvasOutput),
                             "result canvas outputfile");

    po::options_description tspOptions("PC-TSP solver options");
    boost_args_add_options(Solver::getConfig(tspConfig), "", tspOptions);

    po::options_description cmdline_options;
    cmdline_options.add(desc).add(guiOptions).add(tspOptions);

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, cmdline_options), vm);
    po::notify(vm);

    std::ifstream ifs(configFile.c_str());
    store(parse_config_file(ifs, cmdline_options), vm);
    po::notify(vm);
    ifs.close();
    ifs.open(guiConfigFile.c_str());
    store(parse_config_file(ifs, cmdline_options), vm);
    po::notify(vm);
    ifs.close();

    if (vm.count("help")) {
      std::cerr << std::endl;
      std::cerr
          << "VNS Metaheuristic for Orienteering Problem with Neighborhoods "
          << FP_OP_VERSION << std::endl;
      std::cerr << cmdline_options << std::endl;
      ret = false;
    }
    if (ret && loggerCfg != "" && fs::exists(fs::path(loggerCfg))) {
      crl::initLogger("dopn", loggerCfg.c_str());
    } else {
      crl::initLogger("dopn");
    }
    if (!fs::exists(fs::path(problemFile))) {
      ERROR("Problem file '" + problemFile + "' does not exists");
      ret = false;
    }
  } catch (std::exception &e) {
    std::cerr << std::endl;
    std::cerr << "Error in parsing arguments: " << e.what() << std::endl;
    ret = false;
  }
  return ret;
}

CoordsVector &load_goals_coords(const std::string &filename,
                                CoordsVector &pts) {
  Coords pt;
  DatasetOP loadedDataset = DatasetLoader::loadDataset(problemFile);
  double neighborhood_radius = tspConfig.get<double>("neighborhood-radius");
  for (int var = 0; var < loadedDataset.graph.size(); ++var) {
    // std::cout << "neighborhood-radius for load goal coords:" <<
    // neighborhood_radius << std::endl;
    pts.push_back(Coords(loadedDataset.graph[var].x + neighborhood_radius,
                         loadedDataset.graph[var].y));
    pts.push_back(Coords(loadedDataset.graph[var].x - neighborhood_radius,
                         loadedDataset.graph[var].y));
    pts.push_back(Coords(loadedDataset.graph[var].x,
                         loadedDataset.graph[var].y + neighborhood_radius));
    pts.push_back(Coords(loadedDataset.graph[var].x,
                         loadedDataset.graph[var].y - neighborhood_radius));
  }
  return pts;
}

int main(int argc, char **argv) {
  Canvas *canvas = 0;
  int ret = -1;
  if (parseArgs(argc, argv)) {
    INFO("Start Logging");
    try {
      CoordsVector pts;
      INFO("testing " << problemFile);
      load_goals_coords(problemFile, pts);
      crl::gui::CWinAdjustSize::adjust(pts, guiConfig);
      if ((g = gui::CGuiFactory::createGui(guiConfig)) != 0) {
        INFO("Start gui " + guiConfig.get<std::string>("gui"));
        canvas = new Canvas(*g);
      }
      char logName[40];

      crl::initLogger("vns_dopn");

      Solver fph(tspConfig, problemFile);
      fph.setCanvas(canvas);
      fph.solve();
      INFO("after solve");
      usleep(100);
      if (canvas) {
        if (canvasOutput.size()) {
          canvas->save(canvasOutput);
        }
        if (!guiConfig.get<bool>("nowait")) {
          INFO("click to exit");
          canvas->click();
          usleep(100);
        }
        INFO("delete canvas");
        delete canvas;
        delete g;
      }

    } catch (crl::exception &e) {
      ERROR("Exception " << e.what() << "!");
    } catch (std::exception &e) {
      ERROR("Runtime error " << e.what() << "!");
    }
    ret = EXIT_SUCCESS;
  }
  crl::shutdownLogger();
  return ret;
}
