//
// Created by petr on 4/26/16.
//

#pragma once

#include "crl/gui/canvas.h"
//#include "opendubins/shape/all.h"
#include "../opendubins/arc.h"
#include "../opendubins/circle.h"
#include "../opendubins/dubins.h"
#include "../opendubins/line.h"
#include "../opendubins/point.h"
#include "crl/logging.h"

// crl::gui::CCanvasBase &operator<<(crl::gui::CCanvasBase &base,
// opendubins::Shape& shape);
crl::gui::CCanvasBase &operator<<(crl::gui::CCanvasBase &base,
                                  opendubins::Dubins &dubins);
crl::gui::CCanvasBase &operator<<(crl::gui::CCanvasBase &base,
                                  opendubins::Line &line);
crl::gui::CCanvasBase &operator<<(crl::gui::CCanvasBase &base,
                                  opendubins::Arc &arc);
crl::gui::CCanvasBase &operator<<(crl::gui::CCanvasBase &base,
                                  opendubins::Point &point);
