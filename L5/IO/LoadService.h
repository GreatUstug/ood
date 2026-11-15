#ifndef L5_LOADSERVICE_H
#define L5_LOADSERVICE_H

#include "../Model/Picture.h"
#include "../Model/PrimitiveFigure.h"
#include "../Model/GroupFigure.h"
#include "../Model/Figures/Rectangle.h"
#include "../Model/Figures/Ellipse.h"
#include "../Model/Figures/Triangle.h"
#include "../Model/Figures/Line.h"
#include "../Model/Figures/Polygon.h"

#include <istream>
#include <sstream>
#include <memory>
#include <stdexcept>
#include <string>

class LoadService
{
public:
    static shapes::Picture Load(std::istream& in)
    {
        shapes::Picture picture;
        std::string line;
        int lineNum = 0;

        while (std::getline(in, line)) {
            ++lineNum;
            if (IsEmptyOrWhitespace(line)) continue;
            auto fig = ParseFigure(in, line, lineNum);
            if (fig) picture.AddShape(std::move(fig));
        }
        return picture;
    }

private:
    static bool IsEmptyOrWhitespace(const std::string& s) {
        return s.find_first_not_of(" \t\r\n") == std::string::npos;
    }

    static std::unique_ptr<shapes::Figure> ParseFigure(std::istream& in,
                                                       std::string currentLine,
                                                       int& lineNum)
    {
        std::istringstream iss(currentLine);
        std::string type, id, fill, stroke;
        if (!(iss >> type >> id >> fill >> stroke)) {
            throw std::runtime_error("Line " + std::to_string(lineNum) + ": Invalid header");
        }

        if (type == "group") {
            double t = 0.0, d = 1.0;
            if (!(iss >> t >> d)) {
                throw std::runtime_error("Line " + std::to_string(lineNum) + ": group: expected t d");
            }
            char brace;
            if (!(iss >> brace) || brace != '{') {
                throw std::runtime_error("Line " + std::to_string(lineNum) + ": group: expected '{'");
            }

            auto group = std::make_unique<shapes::GroupFigure>(id, fill, stroke);
            group->SetTime(t);
            group->SetDuration(d);

            bool closed = false;
            std::string childLine;
            while (std::getline(in, childLine)) {
                ++lineNum;
                if (IsEmptyOrWhitespace(childLine)) continue;

                std::istringstream probe(childLine);
                probe >> std::ws;
                if (probe.peek() == '}') { closed = true; break; }

                auto child = ParseFigure(in, childLine, lineNum);
                if (child) group->AddChild(std::move(child));
            }
            if (!closed) {
                throw std::runtime_error("Line " + std::to_string(lineNum) + ": group: missing '}'");
            }
            return group;
        }

        // primitive
        std::unique_ptr<shapes::IShapeGeometry> geo;
        if (type == "rectangle") {
            double x, y, w, h;
            if (!(iss >> x >> y >> w >> h))
                throw std::runtime_error("Line " + std::to_string(lineNum) + ": rectangle: bad params");
            geo = std::make_unique<shapes::Rectangle>(x, y, w, h);
        } else if (type == "ellipse") {
            double cx, cy, rx, ry;
            if (!(iss >> cx >> cy >> rx >> ry))
                throw std::runtime_error("Line " + std::to_string(lineNum) + ": ellipse: bad params");
            geo = std::make_unique<shapes::Ellipse>(cx, cy, rx, ry);
        } else if (type == "triangle") {
            double x1, y1, x2, y2, x3, y3;
            if (!(iss >> x1 >> y1 >> x2 >> y2 >> x3 >> y3))
                throw std::runtime_error("Line " + std::to_string(lineNum) + ": triangle: bad params");
            geo = std::make_unique<shapes::Triangle>(x1, y1, x2, y2, x3, y3);
        } else if (type == "line") {
            double x1, y1, x2, y2;
            if (!(iss >> x1 >> y1 >> x2 >> y2))
                throw std::runtime_error("Line " + std::to_string(lineNum) + ": line: bad params");
            geo = std::make_unique<shapes::Line>(x1, y1, x2, y2);
        } else if (type == "polygon") {
            double cx, cy, r, rot;
            int sides;
            if (!(iss >> cx >> cy >> r >> sides >> rot))
                throw std::runtime_error("Line " + std::to_string(lineNum) + ": polygon: bad params");
            geo = std::make_unique<shapes::Polygon>(cx, cy, r, sides, rot);
        } else {
            throw std::runtime_error("Line " + std::to_string(lineNum) + ": unknown type '" + type + "'");
        }

        double t = 0.0, d = 1.0;
        if (!(iss >> t >> d))
            throw std::runtime_error("Line " + std::to_string(lineNum) + ": expected t d");

        auto prim = std::make_unique<shapes::PrimitiveFigure>(id, fill, std::move(geo), stroke);
        prim->SetTime(t);
        prim->SetDuration(d);
        return prim;
    }
};

#endif