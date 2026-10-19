//
// Created by maxim on 12.10.2026.
//

#include <gtest/gtest.h>
#include "../Shapes/Picture.h"
#include "../Shapes/IFigure.h"
#include "../Shapes/Figures/Circle.h"
#include "../Observer/Event.h"
#include "../Observer/SubscriptionById.h"

#include <memory>
#include <string>
#include <vector>

static std::unique_ptr<shapes::IFigure> MakeCircle(const std::string& id) {
    auto geo = std::make_unique<shapes::Circle>(0, 0, 10);
    return std::make_unique<shapes::IFigure>(id, "#ff0000", std::move(geo));
}

TEST(EventTest, AddedHandlerCalledOnAdd) {
    shapes::Picture picture;
    int added = 0;
    auto sub = picture.SubscribeToShapeAdded(
        [&](const shapes::IFigure&, std::size_t) { ++added; });

    picture.AddShape(MakeCircle("c1"));
    EXPECT_EQ(added, 1);
}

TEST(EventTest, AddedHandlerNotCalledOnMove) {
    shapes::Picture picture;
    int added = 0;
    auto sub = picture.SubscribeToShapeAdded(
        [&](const shapes::IFigure&, std::size_t) { ++added; });

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
    auto s1 = picture.SubscribeToShapeAdded(
        [&](const shapes::IFigure&, std::size_t) { ++added; });
    auto s2 = picture.SubscribeToShapeMoved(
        [&](const shapes::IFigure&, double, double) { ++moved; });
    auto s3 = picture.SubscribeToShapeColorChanged(
        [&](const shapes::IFigure&, const std::string&) { ++recolored; });

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
    auto s1 = picture.SubscribeToShapeAdded(
        [&](const shapes::IFigure&, std::size_t) { ++a; });
    auto s2 = picture.SubscribeToShapeAdded(
        [&](const shapes::IFigure&, std::size_t) { ++b; });

    picture.AddShape(MakeCircle("c1"));
    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, 1);
}

TEST(EventTest, SubscriptionsDisconnectIndependently) {
    shapes::Picture picture;
    int a = 0, b = 0;
    auto s1 = picture.SubscribeToShapeAdded(
        [&](const shapes::IFigure&, std::size_t) { ++a; });
    auto s2 = picture.SubscribeToShapeAdded(
        [&](const shapes::IFigure&, std::size_t) { ++b; });

    s1->Disconnect();
    picture.AddShape(MakeCircle("c1"));

    EXPECT_EQ(a, 0);
    EXPECT_EQ(b, 1);
}

TEST(EventTest, RAIIForEachEventType) {
    shapes::Picture picture;
    int added = 0, moved = 0;

    {
        auto s1 = picture.SubscribeToShapeAdded(
            [&](const shapes::IFigure&, std::size_t) { ++added; });
        auto s2 = picture.SubscribeToShapeMoved(
            [&](const shapes::IFigure&, double, double) { ++moved; });
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

	using SubPtr = decltype(picture.SubscribeToShapeAdded(
        [](const shapes::IFigure&, std::size_t) {}));
    SubPtr subB;

    auto subA = picture.SubscribeToShapeAdded(
        [&](const shapes::IFigure&, std::size_t) {
            ++a;
            subB->Disconnect();
        });
    subB = picture.SubscribeToShapeAdded(
        [&](const shapes::IFigure&, std::size_t) { ++b; });

    picture.AddShape(MakeCircle("c1"));
    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, 0);
}

TEST(EventTest, HandlerAddedDuringNotifyStartsNextTime) {
    shapes::Picture picture;
    std::vector<int> calls;

    using SubPtr = decltype(picture.SubscribeToShapeAdded(
        [](const shapes::IFigure&, std::size_t) {}));
    SubPtr subC;

    auto subA = picture.SubscribeToShapeAdded(
        [&](const shapes::IFigure&, std::size_t) {
            calls.push_back(1);
            if (!subC) {
                subC = picture.SubscribeToShapeAdded(
                    [&](const shapes::IFigure&, std::size_t) {
                        calls.push_back(3);
                    });
            }
        });
    auto subB = picture.SubscribeToShapeAdded(
        [&](const shapes::IFigure&, std::size_t) {
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

TEST(EventTest, RemovedHandlerCalledOnDelete) {
    shapes::Picture picture;
    std::string removedId;
    std::size_t lastCount = 99;
    auto sub = picture.SubscribeToShapeRemoved(
        [&](const std::string& id, std::size_t count) {
            removedId = id;
            lastCount = count;
        });

    picture.AddShape(MakeCircle("c1"));
    picture.AddShape(MakeCircle("c2"));
    picture.DeleteShape("c1");

    EXPECT_EQ(removedId, "c1");
    EXPECT_EQ(lastCount, 1u);
}

TEST(EventTest, GeometryChangeIsSeparateFromColorChange) {
    shapes::Picture picture;
    int geom = 0, color = 0;
    auto s1 = picture.SubscribeToShapeGeometryChanged(
        [&](const shapes::IFigure&) { ++geom; });
    auto s2 = picture.SubscribeToShapeColorChanged(
        [&](const shapes::IFigure&, const std::string&) { ++color; });

    picture.AddShape(MakeCircle("c1"));
    auto newGeo = std::make_unique<shapes::Circle>(0, 0, 20);
    picture.ChangeShape("c1", std::move(newGeo));

    EXPECT_EQ(geom, 1);
    EXPECT_EQ(color, 0);
}

TEST(EventTest, MoveSubscriptionKeepsAlive) {
    shapes::Picture picture;
    int added = 0;

    auto subA = picture.SubscribeToShapeAdded(
        [&](const shapes::IFigure&, std::size_t) { ++added; });
    auto subB = std::move(subA);

    picture.AddShape(MakeCircle("c1"));
    EXPECT_EQ(added, 1);

    subB->Disconnect();
    picture.AddShape(MakeCircle("c2"));
    EXPECT_EQ(added, 1);
}

TEST(EventTest, PictureDestroyedBeforeSubscriptionIsSafe) {
    using SubPtr = decltype(
        std::declval<shapes::Picture&>().SubscribeToShapeAdded(
            [](const shapes::IFigure&, std::size_t) {}));
    SubPtr sub;

    {
        shapes::Picture picture;
        sub = picture.SubscribeToShapeAdded(
            [](const shapes::IFigure&, std::size_t) {});
    }

    EXPECT_NO_THROW(sub->Disconnect());
    EXPECT_FALSE(sub->IsActive());
}