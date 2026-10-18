#include <gtest/gtest.h>
#include "../Shapes/IFigure.h"
#include "../Shapes/Figures/Circle.h"
#include "../Shapes/Figures/Rectangle.h"
#include "../Shapes/Figures/Line.h"
#include "../Shapes/Figures/Text.h"
#include "../Shapes/Figures/Triangle.h"
#include "MockCanvas.h"

#include <memory>

namespace {

std::unique_ptr<shapes::IFigure>
MakeCircle(const std::string& id = "fig1", const std::string& color = "#ff0000") {
    auto geo = std::make_unique<shapes::Circle>(0, 0, 10);
    return std::make_unique<shapes::IFigure>(id, color, std::move(geo));
}

} // namespace

TEST(ShapeTest, GetIdReturnsConstructorId) {
    auto figure = MakeCircle("myShape");
    EXPECT_EQ(figure->GetId(), "myShape");
}

TEST(ShapeTest, GetInfoContainsTypeIdColor) {
    auto figure = MakeCircle("c1", "#ff0000");
    auto info = figure->GetInfo();

    EXPECT_NE(info.find("circle"),  std::string::npos);
    EXPECT_NE(info.find("c1"),      std::string::npos);
    EXPECT_NE(info.find("#ff0000"), std::string::npos);
}

TEST(ShapeTest, SetColorChangesInfo) {
    auto figure = MakeCircle("c1", "#ff0000");
    figure->SetColor("#00ff00");

    auto info = figure->GetInfo();
    EXPECT_NE(info.find("#00ff00"), std::string::npos);
    EXPECT_EQ(info.find("#ff0000"), std::string::npos);
}

TEST(ShapeTest, SetColorInvalidThrows) {
    auto figure = MakeCircle();
    EXPECT_THROW(figure->SetColor("#ZZZZZZ"), std::runtime_error);
}

TEST(ShapeTest, SetColorInvalidLeavesStateUnchanged) {
    auto figure = MakeCircle("c1", "#ff0000");
    EXPECT_THROW(figure->SetColor("#ZZZZZZ"), std::runtime_error);

    auto info = figure->GetInfo();
    EXPECT_NE(info.find("#ff0000"), std::string::npos);
}

TEST(ShapeTest, SetGeometryChangesType) {
    auto figure = MakeCircle("c1");
    figure->SetGeometry(std::make_unique<shapes::Rectangle>(0, 0, 10, 10));

    auto info = figure->GetInfo();
    EXPECT_NE(info.find("rectangle"), std::string::npos);
    EXPECT_EQ(info.find("circle"), std::string::npos);
}

TEST(ShapeTest, MoveChangesInfoCoordinates) {
    auto figure = MakeCircle("c1");  // круг в (0,0)
    figure->Move(5, 7);

    auto info = figure->GetInfo();
    EXPECT_NE(info.find("5"), std::string::npos);
    EXPECT_NE(info.find("7"), std::string::npos);
}

TEST(ShapeTest, DrawCircleCallsSetColorAndDrawEllipse) {
    auto figure = MakeCircle("c1", "#ff0000");
    MockCanvas canvas;

    figure->Draw(canvas);

    ASSERT_EQ(canvas.setColorCalls.size(), 1u);
    EXPECT_EQ(canvas.setColorCalls[0].color.r, 0xff);
    EXPECT_EQ(canvas.setColorCalls[0].color.g, 0x00);
    EXPECT_EQ(canvas.setColorCalls[0].color.b, 0x00);

    ASSERT_EQ(canvas.drawEllipseCalls.size(), 1u);
    EXPECT_DOUBLE_EQ(canvas.drawEllipseCalls[0].cx, 0);
    EXPECT_DOUBLE_EQ(canvas.drawEllipseCalls[0].cy, 0);
    EXPECT_DOUBLE_EQ(canvas.drawEllipseCalls[0].rx, 10);
    EXPECT_DOUBLE_EQ(canvas.drawEllipseCalls[0].ry, 10);
}

TEST(ShapeTest, DrawRectangleCallsMoveToAndFourLines) {
    auto geo = std::make_unique<shapes::Rectangle>(0, 0, 10, 20);
    shapes::IFigure figure("r1", "#0000ff", std::move(geo));
    MockCanvas canvas;

    figure.Draw(canvas);

    ASSERT_EQ(canvas.moveToCalls.size(), 1u);
    EXPECT_DOUBLE_EQ(canvas.moveToCalls[0].x, 0);
    EXPECT_DOUBLE_EQ(canvas.moveToCalls[0].y, 0);

    ASSERT_EQ(canvas.lineToCalls.size(), 4u);
    EXPECT_DOUBLE_EQ(canvas.lineToCalls[0].x, 10);
    EXPECT_DOUBLE_EQ(canvas.lineToCalls[0].y, 0);
    EXPECT_DOUBLE_EQ(canvas.lineToCalls[1].x, 10);
    EXPECT_DOUBLE_EQ(canvas.lineToCalls[1].y, 20);
    EXPECT_DOUBLE_EQ(canvas.lineToCalls[2].x, 0);
    EXPECT_DOUBLE_EQ(canvas.lineToCalls[2].y, 20);
    EXPECT_DOUBLE_EQ(canvas.lineToCalls[3].x, 0);
    EXPECT_DOUBLE_EQ(canvas.lineToCalls[3].y, 0);
}

TEST(ShapeTest, DrawLineCallsMoveToAndOneLine) {
    auto geo = std::make_unique<shapes::Line>(10, 20, 30, 40);
    shapes::IFigure figure("l1", "#000000", std::move(geo));
    MockCanvas canvas;

    figure.Draw(canvas);

    ASSERT_EQ(canvas.moveToCalls.size(), 1u);
    EXPECT_DOUBLE_EQ(canvas.moveToCalls[0].x, 10);
    EXPECT_DOUBLE_EQ(canvas.moveToCalls[0].y, 20);

    ASSERT_EQ(canvas.lineToCalls.size(), 1u);
    EXPECT_DOUBLE_EQ(canvas.lineToCalls[0].x, 30);
    EXPECT_DOUBLE_EQ(canvas.lineToCalls[0].y, 40);
}

TEST(ShapeTest, DrawTextCallsDrawText) {
    auto geo = std::make_unique<shapes::Text>(10, 20, 12, "Hello");
    shapes::IFigure figure("t1", "#333333", std::move(geo));
    MockCanvas canvas;

    figure.Draw(canvas);

    ASSERT_EQ(canvas.drawTextCalls.size(), 1u);
    EXPECT_DOUBLE_EQ(canvas.drawTextCalls[0].left, 10);
    EXPECT_DOUBLE_EQ(canvas.drawTextCalls[0].top, 20);
    EXPECT_DOUBLE_EQ(canvas.drawTextCalls[0].fontSize, 12);
    EXPECT_EQ(canvas.drawTextCalls[0].text, "Hello");
}

TEST(ShapeTest, DrawTriangleCallsMoveToAndThreeLines) {
    auto geo = std::make_unique<shapes::Triangle>(0, 0, 10, 0, 0, 10);
    shapes::IFigure figure("tr1", "#00ff00", std::move(geo));
    MockCanvas canvas;

    figure.Draw(canvas);

    ASSERT_EQ(canvas.moveToCalls.size(), 1u);
    ASSERT_EQ(canvas.lineToCalls.size(), 3u);
}