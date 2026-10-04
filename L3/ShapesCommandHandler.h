#ifndef FIGURES_SHAPESCOMMANDHANDLER_H
#define FIGURES_SHAPESCOMMANDHANDLER_H
#pragma once
#include "Shapes/Picture.h"
#include "Shapes/IFigure.h"
#include "Shapes/Figures/Circle.h"
#include "Shapes/Figures/Rectangle.h"
#include "Shapes/Figures/Triangle.h"

#include <iostream>
#include <sstream>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>
#include <memory>

namespace CommandHandler {

class ShapesCommandHandler {
public:
    ShapesCommandHandler(shapes::Picture& picture,
                         gfx::ICanvas& canvas)
        : m_picture(picture), m_canvas(canvas) {}

    void Execute(std::istream& in = std::cin, std::ostream& out = std::cout) {
        std::string line;
        out << "=== Shapes Console ===\n";

        while (std::getline(in, line)) {
            if (line.empty()) continue;
            std::istringstream iss(line);
            std::string cmd;
            iss >> cmd;

            try {
                if (cmd == "AddShape")         HandleAddShape(iss, out);
                else if (cmd == "MoveShape")   HandleMoveShape(iss, out);
                else if (cmd == "MovePicture") HandleMovePicture(iss, out);
                else if (cmd == "DeleteShape") HandleDeleteShape(iss, out);
                else if (cmd == "ChangeColor") HandleChangeColor(iss, out);
                else if (cmd == "ChangeShape") HandleChangeShape(iss, out);
                else if (cmd == "List")        HandleList(out);
                else if (cmd == "DrawShape")   HandleDrawShape(iss, out);
                else if (cmd == "DrawPicture") HandleDrawPicture(out);
                else if (cmd == "Exit")        break;
                else out << "Unknown command: " << cmd << "\n";
            } catch (const std::exception& e) {
                out << "Error: " << e.what() << "\n";
            }
        }
    }

private:
    std::unique_ptr<shapes::IShapeGeometry>
    CreateGeometry(const std::string& typeName, const ShapeParams& p) {
        if (typeName == "circle") {
            if (p.params.empty()) throw std::runtime_error("Circle needs radius");
            double r = std::stod(p.params[0]);
            if (r < 0) throw std::runtime_error("Radius must be non-negative");
            return std::make_unique<shapes::Circle>(p.x, p.y, r);
        }
        if (typeName == "rectangle") {
            if (p.params.size() < 2) throw std::runtime_error("Rectangle needs width and height");
            double w = std::stod(p.params[0]);
            double h = std::stod(p.params[1]);
            if (w < 0 || h < 0) throw std::runtime_error("Width and height must be non-negative");
            return std::make_unique<shapes::Rectangle>(p.x, p.y, w, h);
        }
        if (typeName == "triangle") {
            if (p.params.size() < 4) throw std::runtime_error("Triangle needs 3 points");
            return std::make_unique<shapes::Triangle>(
                p.x, p.y,
                std::stod(p.params[0]), std::stod(p.params[1]),
                std::stod(p.params[2]), std::stod(p.params[3]));
        }
        throw std::runtime_error("Unknown type: " + typeName);
    }

    void HandleAddShape(std::istringstream& iss, std::ostream& out) {
        ShapeParams p;
        if (!(iss >> p.id >> p.color >> p.type))
            throw std::runtime_error("Invalid AddShape syntax");
        if (!(iss >> p.x >> p.y))
            throw std::runtime_error("Missing coordinates");
        p.params = GetParams(p.type, iss);

        auto geo = CreateGeometry(p.type, p);
        auto figure = std::make_unique<shapes::IFigure>(
            p.id, p.color, std::move(geo));

        m_picture.AddShape(std::move(figure));
        out << "Added: " << p.id << "\n";
    }

    void HandleChangeShape(std::istringstream& iss, std::ostream& out) {
        std::string id, typeName;
        if (!(iss >> id >> typeName))
            throw std::runtime_error("Invalid ChangeShape syntax");

        ShapeParams p;
        p.id = id;
        p.type = typeName;
        if (!(iss >> p.x >> p.y))
            throw std::runtime_error("Missing coordinates");
        p.params = GetParams(p.type, iss);

        auto geo = CreateGeometry(p.type, p);
        m_picture.ChangeShape(id, std::move(geo));
        out << "Changed: " << id << "\n";
    }

    void HandleMoveShape(std::istringstream& iss, std::ostream&) {
        std::string id; double dx, dy;
        if (!(iss >> id >> dx >> dy)) throw std::runtime_error("Invalid MoveShape");
        m_picture.MoveShape(id, dx, dy);
    }

    void HandleMovePicture(std::istringstream& iss, std::ostream&) {
        double dx, dy;
        if (!(iss >> dx >> dy)) throw std::runtime_error("Invalid MovePicture");
        m_picture.MovePicture(dx, dy);
    }

    void HandleDeleteShape(std::istringstream& iss, std::ostream&) {
        std::string id;
        if (!(iss >> id)) throw std::runtime_error("Invalid DeleteShape");
        m_picture.DeleteShape(id);
    }

    void HandleChangeColor(std::istringstream& iss, std::ostream&) {
        std::string id, color;
        if (!(iss >> id >> color)) throw std::runtime_error("Invalid ChangeColor");
        m_picture.EditShapeColor(id, color);
    }

    void HandleList(std::ostream& out) {
        for (const auto& s : m_picture.ListAllShapes()) out << s << "\n";
    }

    void HandleDrawShape(std::istringstream& iss, std::ostream&) {
        std::string id;
        if (!(iss >> id)) throw std::runtime_error("Invalid DrawShape");
        m_picture.DrawShape(id, m_canvas);
    }

    void HandleDrawPicture(std::ostream&) {
        m_picture.DrawPicture(m_canvas);
    }

    std::vector<std::string> GetParams(const std::string& typeName,
                                       std::istringstream& iss) {
        std::vector<std::string> params;
        if (typeName == "text") {
            std::string sizeStr, textLine;
            iss >> sizeStr;
            std::getline(iss, textLine);
            if (!textLine.empty() && textLine[0] == ' ') textLine.erase(0, 1);
            params.push_back(sizeStr);
            params.push_back(textLine);
        } else {
            std::string param;
            while (iss >> param) params.push_back(param);
        }
        return params;
    }

    shapes::Picture&       m_picture;
    gfx::ICanvas&          m_canvas;
};

} // namespace CommandHandler
#endif //FIGURES_SHAPESCOMMANDHANDLER_H