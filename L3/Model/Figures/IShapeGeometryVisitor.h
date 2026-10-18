//
// Created by maxim on 19.10.2026.
//

#ifndef FIGURES_ISHAPEGEOMETRYVISITOR_H
#define FIGURES_ISHAPEGEOMETRYVISITOR_H

namespace shapes
{
class Rectangle;
class Ellipse;
class Triangle;

class IShapeGeometryVisitor
{
public:
	virtual ~IShapeGeometryVisitor() = default;
	virtual void Visit(const Rectangle& r) = 0;
	virtual void Visit(const Ellipse& e) = 0;
	virtual void Visit(const Triangle& t) = 0;
};
}

#endif //FIGURES_ISHAPEGEOMETRYVISITOR_H