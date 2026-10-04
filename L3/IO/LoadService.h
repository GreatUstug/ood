//
// Created by maxim on 04.10.2026.
//

#ifndef L3_LOADSERVICE_H
#define L3_LOADSERVICE_H

#include "../Shapes/Picture.h"
#include "../Shapes/IFigure.h"
#include "../Shapes/Figures/Rectangle.h"
#include "../Shapes/Figures/Ellipse.h"
#include "../Shapes/Figures/Triangle.h"

#include <istream>
#include <sstream>
#include <memory>
#include <stdexcept>
#include <string>

class LoadService {
public:
    static shapes::Picture Load(std::istream& in) {
        shapes::Picture picture;

        std::string line;
        int lineNum = 0;
        while (std::getline(in, line)) {
            ++lineNum;
            if (line.empty()) continue;

            try {
                auto figure = ParseLine(line);
                picture.AddShape(std::move(figure));
            } catch (const std::exception& e) {
                throw std::runtime_error(
                    "Line " + std::to_string(lineNum) + ": " + e.what());
            }
        }

        return picture;
    }

private:
    static std::unique_ptr<shapes::IFigure> ParseLine(const std::string& line) {
        std::istringstream iss(line);
        std::string type, id, color;
        if (!(iss >> type >> id >> color)) {
            throw std::runtime_error("Invalid line format");
        }

        (void)gfx::Color::ParseToRGB(color);

        std::unique_ptr<shapes::IShapeGeometry> geo;

        if (type == "rectangle") {
            double x, y, w, h;
            if (!(iss >> x >> y >> w >> h))
                throw std::runtime_error("Invalid rectangle params");
            if (w < 0 || h < 0)
                throw std::runtime_error("Negative width/height");
            geo = std::make_unique<shapes::Rectangle>(x, y, w, h);
        }
        else if (type == "ellipse") {
            double cx, cy, rx, ry;
            if (!(iss >> cx >> cy >> rx >> ry))
                throw std::runtime_error("Invalid ellipse params");
            if (rx < 0 || ry < 0)
                throw std::runtime_error("Negative radii");
            geo = std::make_unique<shapes::Ellipse>(cx, cy, rx, ry);
        }
        else if (type == "triangle") {
            double x1, y1, x2, y2, x3, y3;
            if (!(iss >> x1 >> y1 >> x2 >> y2 >> x3 >> y3))
                throw std::runtime_error("Invalid triangle params");
            geo = std::make_unique<shapes::Triangle>(x1, y1, x2, y2, x3, y3);
        }
        else {
            throw std::runtime_error("Unknown type: " + type);
        }

        return std::make_unique<shapes::IFigure>(id, color, std::move(geo));
    }
};

#endif //L3_LOADSERVICE_H
