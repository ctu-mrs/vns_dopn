//
// Created by petr on 4/26/16.
//

#pragma once

#include "canvas.h"
//#include "opendubins/shape/all.h"
#include "logging.h"
#include "opendubins/line.h"
#include "opendubins/point.h"
#include "opendubins/dubins.h"
#include "opendubins/arc.h"
#include "opendubins/circle.h"




//imr::gui::CCanvasBase &operator<<(imr::gui::CCanvasBase &base, opendubins::Shape& shape);
imr::gui::CCanvasBase &operator<<(imr::gui::CCanvasBase &base, opendubins::Dubins& dubins);
imr::gui::CCanvasBase &operator<<(imr::gui::CCanvasBase &base, opendubins::Line& line);
imr::gui::CCanvasBase &operator<<(imr::gui::CCanvasBase &base, opendubins::Arc& arc);
imr::gui::CCanvasBase &operator<<(imr::gui::CCanvasBase &base, opendubins::Point& point);

