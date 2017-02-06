//
// Created by petr on 4/26/16.
//

#include "gui.h"

using namespace imr::gui;
using namespace opendubins;

using imr::logger;
/*
CCanvasBase &operator<<(CCanvasBase &base, opendubins::Shape &s) {
	Shape *shape = &s;
	switch (shape->getType()) {
	case ShapeType::Polygon: // polygon
	{
		base << canvas::POLYGON;
		Polygon * poly = (Polygon *) shape;
		for (auto pp : poly->pnts) {
			base << pp.x << pp.y;
		}

	}
		break;
	case ShapeType::Point: // point
		// TODO
		//WARN("Unknown type.");
		break;
	case ShapeType::Circle: // circle
	{
		base << canvas::ARC;
		ShapeCircle *circle = (ShapeCircle *) shape;
		auto cen = circle->getCenter();
		base << cen.x << cen.y << circle->getRadius() << (double) 0 << 2 * M_PI;
	}
		break;
	case ShapeType::Ellipse: // elipse
	{
		base << canvas::POLYGON;
		Ellipse * eclipse = (Ellipse *) shape;
		for (float alpha = 0; alpha < 1; alpha += 0.01) {
			Point p = eclipse->getEdgePoint(alpha);
			base << p.x << p.y;
		}
	}
		break;
	default:
		//ERROR("Bad shape type.");
		exit(-1);
	}
	base << canvas::END;
	return base;
}
*/
imr::gui::CCanvasBase &operator<<(imr::gui::CCanvasBase &base, opendubins::Dubins &dubins) {
	//INFO("gui dubins write");
	Arc firstArc = dubins.getFirstArc();
	Arc secondArc = dubins.getSecondArc();

	base << firstArc;
	base << secondArc;

	if (dubins.isCCC) {
		Arc carc = dubins.getCenterArc();
		base << carc;
	} else {
		Line line = dubins.getCenter();
		base << line;
	}
	return base;
}

imr::gui::CCanvasBase &operator<<(imr::gui::CCanvasBase &base, opendubins::Line &line) {
	base << canvas::LINE;
	base << line.p1.x << line.p1.y;
	base << line.p2.x << line.p2.y;
	base << canvas::END;
	return base;
}

imr::gui::CCanvasBase &operator<<(imr::gui::CCanvasBase &base, opendubins::Arc &arc) {
	if (fabs(arc.angle) > TOLERANCE) {

		base << canvas::ARC << Fill(false);
		Point p1 = arc.getCenter();
		double startAngle = -M_PI / 2 - arc.state.ang;
		if (arc.angle >= 0) {
			startAngle += M_PI;
		}

		double endAngle = -M_PI / 2 - arc.state.ang - arc.angle;
		if (arc.angle >= 0) {
			endAngle += M_PI;
		}

		if (arc.angle >= 0) {
			std::swap(startAngle, endAngle);
		}

		while (endAngle <= startAngle) {
			endAngle += 2 * M_PI;
		}

		base << p1.x << p1.y << arc.radius << startAngle << endAngle;
	}
	base << canvas::END;
	return base;
}

imr::gui::CCanvasBase &operator<<(imr::gui::CCanvasBase &base, opendubins::Point& point) {
	base << canvas::POINT << point.x << point.y << canvas::END;
	return base;
}
