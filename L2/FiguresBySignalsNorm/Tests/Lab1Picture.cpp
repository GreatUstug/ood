#include <gtest/gtest.h>
#include "../Shapes/Picture.h"
#include "../Shapes/IFigure.h"
#include "../Shapes/Figures/Circle.h"
#include "../Shapes/Figures/Rectangle.h"
#include "MockCanvas.h"

#include <memory>

namespace {

std::unique_ptr<shapes::IFigure>
MakeCircle(const std::string& id = "c1", const std::string& color = "#ff0000") {
    auto geo = std::make_unique<shapes::Circle>(0, 0, 10);
    return std::make_unique<shapes::IFigure>(id, color, std::move(geo));
}

std::unique_ptr<shapes::IFigure>
MakeRectangle(const std::string& id = "r1", const std::string& color = "#0000ff") {
    auto geo = std::make_unique<shapes::Rectangle>(0, 0, 10, 20);
    return std::make_unique<shapes::IFigure>(id, color, std::move(geo));
}

}

TEST(PictureTest, DeleteMissingShapeThrows) {
    shapes::Picture picture;
    EXPECT_THROW(picture.DeleteShape("nope"), std::invalid_argument);
}

TEST(PictureTest, GetShapeReturnsCorrectShape) {
    shapes::Picture picture;
    picture.AddShape(MakeCircle("c1", "#ff0000"));
    picture.AddShape(MakeRectangle("r1", "#0000ff"));

    EXPECT_EQ(picture.GetShape("c1").GetId(), "c1");
    EXPECT_EQ(picture.GetShape("r1").GetId(), "r1");
}

TEST(PictureTest, GetMissingShapeThrows) {
    shapes::Picture picture;
    EXPECT_THROW(picture.GetShape("nope"), std::invalid_argument);
}

TEST(PictureTest, GetShapeReturnsWritableReference) {
    shapes::Picture picture;
    picture.AddShape(MakeCircle("c1", "#ff0000"));

    auto& shape = picture.GetShape("c1");
    shape.SetColor("#00ff00");

    EXPECT_NE(picture.GetShape("c1").GetInfo().find("#00ff00"), std::string::npos);
}

TEST(PictureTest, ListAllShapesInInsertionOrder) {
    shapes::Picture picture;
    picture.AddShape(MakeCircle("first"));
    picture.AddShape(MakeCircle("second"));
    picture.AddShape(MakeCircle("third"));

    auto list = picture.ListAllShapes();
    ASSERT_EQ(list.size(), 3u);

    EXPECT_NE(list[0].find("first"),  std::string::npos);
    EXPECT_NE(list[1].find("second"), std::string::npos);
    EXPECT_NE(list[2].find("third"),  std::string::npos);
}

TEST(PictureTest, ListAllShapesNumbersFromOne) {
    shapes::Picture picture;
    picture.AddShape(MakeCircle("a"));
    picture.AddShape(MakeCircle("b"));

    auto list = picture.ListAllShapes();
    ASSERT_EQ(list.size(), 2u);

    EXPECT_EQ(list[0].substr(0, 1), "1");
    EXPECT_EQ(list[1].substr(0, 1), "2");
}

TEST(PictureTest, MoveShapeMovesOnlyOne) {
    shapes::Picture picture;
    picture.AddShape(MakeCircle("c1"));   // в (0,0)
    picture.AddShape(MakeCircle("c2"));   // в (0,0)

    picture.MoveShape("c1", 5, 7);

    EXPECT_NE(picture.GetShape("c1").GetInfo().find("5"), std::string::npos);
    EXPECT_NE(picture.GetShape("c2").GetInfo().find("0"), std::string::npos);
}

TEST(PictureTest, MoveMissingShapeThrows) {
    shapes::Picture picture;
    EXPECT_THROW(picture.MoveShape("nope", 1, 1), std::invalid_argument);
}

TEST(PictureTest, MovePictureMovesAllShapes) {
    shapes::Picture picture;
    picture.AddShape(MakeCircle("c1"));
    picture.AddShape(MakeCircle("c2"));

    picture.MovePicture(3, 4);

    auto info1 = picture.GetShape("c1").GetInfo();
    auto info2 = picture.GetShape("c2").GetInfo();

    EXPECT_NE(info1.find("3"), std::string::npos);
    EXPECT_NE(info1.find("4"), std::string::npos);
    EXPECT_NE(info2.find("3"), std::string::npos);
    EXPECT_NE(info2.find("4"), std::string::npos);
}

TEST(PictureTest, EditShapeColorChangesInfo) {
    shapes::Picture picture;
    picture.AddShape(MakeCircle("c1", "#ff0000"));

    picture.EditShapeColor("c1", "#00ff00");
    EXPECT_NE(picture.GetShape("c1").GetInfo().find("#00ff00"), std::string::npos);
}

TEST(PictureTest, EditShapeColorMissingThrows) {
    shapes::Picture picture;
    EXPECT_THROW(picture.EditShapeColor("nope", "#000000"), std::invalid_argument);
}

TEST(PictureTest, EditShapeColorInvalidDoesNotChange) {
    shapes::Picture picture;
    picture.AddShape(MakeCircle("c1", "#ff0000"));

    EXPECT_THROW(picture.EditShapeColor("c1", "#ZZZZZZ"), std::runtime_error);
    EXPECT_NE(picture.GetShape("c1").GetInfo().find("#ff0000"), std::string::npos);
}

TEST(PictureTest, ChangeShapeChangesType) {
    shapes::Picture picture;
    picture.AddShape(MakeCircle("c1"));

    picture.ChangeShape("c1", std::make_unique<shapes::Rectangle>(0, 0, 10, 20));

    auto info = picture.GetShape("c1").GetInfo();
    EXPECT_NE(info.find("rectangle"), std::string::npos);
    EXPECT_EQ(info.find("circle"), std::string::npos);
}

TEST(PictureTest, DrawShapeDrawsSingleShape) {
    shapes::Picture picture;
    picture.AddShape(MakeCircle("c1", "#ff0000"));

    MockCanvas canvas;
    picture.DrawShape("c1", canvas);

    ASSERT_EQ(canvas.setColorCalls.size(), 1u);
    ASSERT_EQ(canvas.drawEllipseCalls.size(), 1u);
}

TEST(PictureTest, DrawMissingShapeThrows) {
    shapes::Picture picture;
    MockCanvas canvas;
    EXPECT_THROW(picture.DrawShape("nope", canvas), std::invalid_argument);
}

TEST(PictureTest, DrawPictureDrawsAllShapes) {
    shapes::Picture picture;
    picture.AddShape(MakeCircle("c1"));
    picture.AddShape(MakeCircle("c2"));
    picture.AddShape(MakeCircle("c3"));

    MockCanvas canvas;
    picture.DrawPicture(canvas);

    // Каждый круг = SetColor + DrawEllipse
    EXPECT_EQ(canvas.setColorCalls.size(), 3u);
    EXPECT_EQ(canvas.drawEllipseCalls.size(), 3u);
}

TEST(PictureTest, DrawPictureEmptyDoesNothing) {
    shapes::Picture picture;
    MockCanvas canvas;

    picture.DrawPicture(canvas);

    EXPECT_EQ(canvas.setColorCalls.size(), 0u);
    EXPECT_EQ(canvas.drawEllipseCalls.size(), 0u);
    EXPECT_EQ(canvas.moveToCalls.size(), 0u);
    EXPECT_EQ(canvas.lineToCalls.size(), 0u);
    EXPECT_EQ(canvas.drawTextCalls.size(), 0u);
}

TEST(PictureTest, DrawPicturePreservesInsertionOrder) {
    shapes::Picture picture;
    // Первый — синий круг, второй — зелёный круг
    picture.AddShape(MakeCircle("c1", "#0000ff"));
    picture.AddShape(MakeCircle("c2", "#00ff00"));

    MockCanvas canvas;
    picture.DrawPicture(canvas);

    ASSERT_GE(canvas.setColorCalls.size(), 2u);
    EXPECT_EQ(canvas.setColorCalls[0].color.b, 0xff);  // синий
    EXPECT_EQ(canvas.setColorCalls[1].color.g, 0xff);  // зелёный
}