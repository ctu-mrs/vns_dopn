/*
 * File name: canvasview_coords.h
 * Date:      2013/10/13 09:26
 * Author:    Jan Faigl
 */

#ifndef __CANVASVIEW_COORDS_H__
#define __CANVASVIEW_COORDS_H__

#include <boost/foreach.hpp>

#include "gui.h"
#include "colors.h"
#include "renderer.h"
#include "canvas.h"

#include "coords.h"

#include "logging.h"
using imr::logger;

#include "HeuristicTypes.h"

/// ----------------------------------------------------------------------------
inline imr::gui::CCanvasBase& operator<<(imr::gui::CCanvasBase& canvas, const Coords& coords) {
	//INFO(coords.x <<" , "<< coords.y);
	canvas << coords.x << coords.y;
	return canvas;
}

/// ----------------------------------------------------------------------------
inline imr::gui::CCanvasBase& operator<<(imr::gui::CCanvasBase& canvas, const std::vector<Coords>& points) {
	BOOST_FOREACH(const Coords& pt, points) {
		canvas << pt;
	}
	return canvas;
}



/// ----------------------------------------------------------------------------
inline imr::gui::CCanvasBase& operator<<(imr::gui::CCanvasBase& canvas, const GraphNode& coords) {
	//INFO(coords.x <<" , "<< coords.y);
	canvas << coords.x << coords.y;
	return canvas;
}



/// ----------------------------------------------------------------------------
inline imr::gui::CCanvasBase& operator<<(imr::gui::CCanvasBase& canvas, const Tour tour) {
	for (int var = 0; var < tour.tour.size(); ++var) {
		canvas << tour.tour[var];
	}
	return canvas;
}

#endif

/* end of canvasview_coords.h */
