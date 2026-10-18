//
// Created by maxim on 12.10.2026.
//

#include <gtest/gtest.h>
#include "../Shapes/Picture.h"
#include "../Shapes/IFigure.h"
#include "../Shapes/Figures/Circle.h"
#include "../Observer/Event.h"

static std::unique_ptr<shapes::IFigure> MakeCircle(const std::string& id) {
    auto geo = std::make_unique<shapes::Circle>(0, 0, 10);
    return std::make_unique<shapes::IFigure>(id, "#ff0000", std::move(geo));
}

TEST(EventTest, AddedHandlerCalledOnAdd) {
    shapes::Picture picture;
    int added = 0;
    auto sub = picture.SubscribeToShapeAdded(
        [&](const shapes::IFigure&) { ++added; });

    picture.AddShape(MakeCircle("c1"));
    EXPECT_EQ(added, 1);
}

TEST(EventTest, AddedHandlerNotCalledOnMove) {
    shapes::Picture picture;
    int added = 0;
    auto sub = picture.SubscribeToShapeAdded(
        [&](const shapes::IFigure&) { ++added; });

    picture.AddShape(MakeCircle("c1"));
    picture.MoveShape("c1", 10, 0);
    EXPECT_EQ(added, 1);   // только добавление
}

TEST(EventTest, MovedHandlerReceivesDelta) {
    shapes::Picture picture;
    double dx = 0, dy = 0;
    auto sub = picture.SubscribeToShapeMoved(
        [&](const shapes::IFigure&, double x, double y) { dx = x; dy = y; });

    picture.AddShape(MakeCircle("c1"));
    picture.MoveShape("c1", 5, 7);
    EXPECT_DOUBLE_EQ(dx, 5);
    EXPECT_DOUBLE_EQ(dy, 7);
}

TEST(EventTest, MultipleEventsIndependent) {
    shapes::Picture picture;
    int added = 0, moved = 0, recolored = 0;
    auto s1 = picture.SubscribeToShapeAdded([&](const shapes::IFigure&) { ++added; });
    auto s2 = picture.SubscribeToShapeMoved([&](const shapes::IFigure&, double, double) { ++moved; });
    auto s3 = picture.SubscribeToShapeColorChanged([&](const shapes::IFigure&, const std::string&) { ++recolored; });

    picture.AddShape(MakeCircle("c1"));
    picture.MoveShape("c1", 1, 1);
    picture.EditShapeColor("c1", "#00ff00");

    EXPECT_EQ(added, 1);
    EXPECT_EQ(moved, 1);
    EXPECT_EQ(recolored, 1);
}

TEST(EventTest, MultipleHandlersSameEvent) {
    shapes::Picture picture;
    int a = 0, b = 0;
    auto s1 = picture.SubscribeToShapeAdded([&](const shapes::IFigure&) { ++a; });
    auto s2 = picture.SubscribeToShapeAdded([&](const shapes::IFigure&) { ++b; });

    picture.AddShape(MakeCircle("c1"));
    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, 1);
}

TEST(EventTest, SubscriptionsDisconnectIndependently) {
    shapes::Picture picture;
    int a = 0, b = 0;
    auto s1 = picture.SubscribeToShapeAdded([&](const shapes::IFigure&) { ++a; });
    auto s2 = picture.SubscribeToShapeAdded([&](const shapes::IFigure&) { ++b; });

    s1->Disconnect();
    picture.AddShape(MakeCircle("c1"));

    EXPECT_EQ(a, 0);
    EXPECT_EQ(b, 1);
}

TEST(EventTest, RAIIForEachEventType) {
    shapes::Picture picture;
    int added = 0, moved = 0;

    {
        auto s1 = picture.SubscribeToShapeAdded([&](const shapes::IFigure&) { ++added; });
        auto s2 = picture.SubscribeToShapeMoved([&](const shapes::IFigure&, double, double) { ++moved; });
        picture.AddShape(MakeCircle("c1"));
        picture.MoveShape("c1", 1, 1);
    }

    picture.AddShape(MakeCircle("c2"));
    picture.MoveShape("c2", 1, 1);

    EXPECT_EQ(added, 1);
    EXPECT_EQ(moved, 1);
}

TEST(EventTest, HandlerDisconnectsDuringNotify) {
    shapes::Picture picture;
    int a = 0, b = 0;

    std::unique_ptr<Subscription<Event<void(const shapes::IFigure&)>, std::function<void(const shapes::IFigure&)>>> subB;

    auto subA = picture.SubscribeToShapeAdded([&](const shapes::IFigure&) {
        ++a;
        subB->Disconnect();
    });
    subB = picture.SubscribeToShapeAdded([&](const shapes::IFigure&) { ++b; });

    picture.AddShape(MakeCircle("c1"));
    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, 0);   // B отписан до своей очереди
}

TEST(EventTest, HandlerAddedDuringNotifyStartsNextTime) {
    shapes::Picture picture;
    std::vector<int> calls;

    std::unique_ptr<Subscription<Event<void(const shapes::IFigure&)>, std::function<void(const shapes::IFigure&)>>> subC;

    auto subA = picture.SubscribeToShapeAdded([&](const shapes::IFigure&) {
        calls.push_back(1);
        if (!subC) {
            subC = picture.SubscribeToShapeAdded([&](const shapes::IFigure&) {
                calls.push_back(3);
            });
        }
    });
    auto subB = picture.SubscribeToShapeAdded([&](const shapes::IFigure&) {
        calls.push_back(2);
    });

    picture.AddShape(MakeCircle("c1"));
    EXPECT_EQ(calls.size(), 2u);   // 1, 2

    calls.clear();
    picture.AddShape(MakeCircle("c2"));
    EXPECT_EQ(calls.size(), 3u);   // 1, 2, 3
}

TEST(EventTest, DirectShapeChangeTriggersPictureEvent) {
    shapes::Picture picture;
    int recolored = 0;
    auto sub = picture.SubscribeToShapeColorChanged(
        [&](const shapes::IFigure&, const std::string&) { ++recolored; });

    picture.AddShape(MakeCircle("c1"));

    auto& shape = picture.GetShape("c1");
    shape.SetColor("#0000ff");

    EXPECT_EQ(recolored, 1);
}