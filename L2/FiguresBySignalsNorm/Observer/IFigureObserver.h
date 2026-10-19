//
// Created by maxim on 21.09.2026.
//

#ifndef FIGURES_ISHAPEOBSERVER_H
#define FIGURES_ISHAPEOBSERVER_H

namespace shapes
{
class IFigure;
}

class IFigureObserver
{
public:
	virtual ~IFigureObserver() = default;

	virtual void OnShapeMoved(const shapes::IFigure&, double /*dx*/, double /*dy*/) {}
	virtual void OnShapeColorChanged(const shapes::IFigure&, const std::string& /*color*/) {}
	virtual void OnShapeGeometryChanged(const shapes::IFigure&) {}
};
#endif //FIGURES_ISHAPEOBSERVER_H