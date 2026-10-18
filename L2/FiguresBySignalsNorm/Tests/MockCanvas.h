#ifndef FIGURES_MOCKCANVAS_H
#define FIGURES_MOCKCANVAS_H
#pragma once
#include "../gfx/ICanvas.h"

#include <string>
#include <vector>

class MockCanvas : public gfx::ICanvas {
public:
	struct SetColorCall    { gfx::Color color; };
	struct MoveToCall      { double x, y; };
	struct LineToCall      { double x, y; };
	struct DrawEllipseCall { double cx, cy, rx, ry; };
	struct DrawTextCall    { double left, top, fontSize; std::string text; };

	std::vector<SetColorCall>    setColorCalls;
	std::vector<MoveToCall>      moveToCalls;
	std::vector<LineToCall>      lineToCalls;
	std::vector<DrawEllipseCall> drawEllipseCalls;
	std::vector<DrawTextCall>    drawTextCalls;

	void SetColor(const gfx::Color& c) override {
		setColorCalls.push_back({c});
	}

	void MoveTo(double x, double y) override {
		moveToCalls.push_back({x, y});
	}

	void LineTo(double x, double y) override {
		lineToCalls.push_back({x, y});
	}

	void DrawEllipse(double cx, double cy, double rx, double ry) override {
		drawEllipseCalls.push_back({cx, cy, rx, ry});
	}

	void DrawText(double left, double top, double fontSize, const std::string& text) override {
		drawTextCalls.push_back({left, top, fontSize, text});
	}

	void Clear() {
		setColorCalls.clear();
		moveToCalls.clear();
		lineToCalls.clear();
		drawEllipseCalls.clear();
		drawTextCalls.clear();
	}
};

#endif //FIGURES_MOCKCANVAS_H