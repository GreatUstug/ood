//
// Created by maxim on 10.10.2026.
//

#ifndef L3_SHAPEFACTORY_H
#define L3_SHAPEFACTORY_H

#include "Figure.h"
#include "Figures/Rectangle.h"
#include "Figures/Ellipse.h"
#include "Figures/Triangle.h"

#include <memory>
#include <string>
#include <random>

namespace shapes {

    namespace defaults {
        constexpr double RECT_W     = 120.0;
        constexpr double RECT_H     = 80.0;
        constexpr double ELLIPSE_RX = 60.0;
        constexpr double ELLIPSE_RY = 40.0;
        constexpr double TRIANGLE_W = 120.0;
        constexpr double TRIANGLE_H = 80.0;
        constexpr const char* COLOR = "#c678ff";
    }

    inline std::string MakeId(const std::string& prefix) {
        static std::mt19937 rng{std::random_device{}()};
        return prefix + std::to_string(rng());
    }

    inline std::unique_ptr<Figure> MakeRectangle(const std::string& id, double cx, double cy) {
        return std::make_unique<Figure>(id, defaults::COLOR,
            std::make_unique<Rectangle>(
                cx - defaults::RECT_W / 2,
                cy - defaults::RECT_H / 2,
                defaults::RECT_W,
                defaults::RECT_H));
    }

    inline std::unique_ptr<Figure> MakeEllipse(const std::string& id, double cx, double cy) {
        return std::make_unique<Figure>(id, defaults::COLOR,
            std::make_unique<Ellipse>(cx, cy,
                defaults::ELLIPSE_RX,
                defaults::ELLIPSE_RY));
    }

    inline std::unique_ptr<Figure> MakeTriangle(const std::string& id, double cx, double cy) {
        double halfW = defaults::TRIANGLE_W / 2;
        double halfH = defaults::TRIANGLE_H / 2;
        return std::make_unique<Figure>(id, defaults::COLOR,
            std::make_unique<Triangle>(
                cx,         cy - halfH,
                cx + halfW, cy + halfH,
                cx - halfW, cy + halfH));
    }

    inline std::unique_ptr<Figure> MakeShape(const std::string& type,
                                              const std::string& id,
                                              double cx, double cy)
    {
        if (type == "rectangle") return MakeRectangle(id, cx, cy);
        if (type == "ellipse")   return MakeEllipse(id, cx, cy);
        if (type == "triangle")  return MakeTriangle(id, cx, cy);
        return nullptr;
    }

}

#endif //L3_SHAPEFACTORY_H