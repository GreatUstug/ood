//
// Created by maxim on 21.09.2026.
//

#ifndef FIGURES_ISHAPEOBSERVER_H
#define FIGURES_ISHAPEOBSERVER_H

namespace shapes
{
class IFigure;
}

class IFigureObserver {
public:
	virtual ~IFigureObserver() = default;
	virtual void OnShapeChanged(const shapes::IFigure& figure) = 0;
};
#endif //FIGURES_ISHAPEOBSERVER_H