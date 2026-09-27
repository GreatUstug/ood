//
// Created by maxim on 12.09.2026.
//

#include "../ShapesCommandHandler.h"

#include <gtest/gtest.h>
#include "../Shapes/Picture.h"
#include "../Shapes/IFigure.h"
#include "../Shapes/Figures/Circle.h"
#include "../Observer/IPictureObserver.h"
#include "../Observer/IFigureObserver.h"

class CountingPictureObserver : public IPictureObserver {
public:
    int count = 0;
    std::size_t lastShapeCount = 0;

    void OnPictureChanged(std::size_t shapeCount) override {
        ++count;
        lastShapeCount = shapeCount;
    }
};

class CountingFigureObserver : public IFigureObserver {
public:
    int count = 0;

    void OnShapeChanged() override {
        ++count;
    }
};

TEST(ObserverTest, ShapeObserverGetsNotificationOnChange) {
    auto geo = std::make_unique<shapes::Circle>(0, 0, 10);
    shapes::IFigure figure("fig1", "#ff0000", std::move(geo));

    CountingFigureObserver obs;
    figure.AddObserver(&obs);

    figure.SetColor("#00ff00");
    EXPECT_EQ(obs.count, 1);

    figure.Move(5, 5);
    EXPECT_EQ(obs.count, 2);
}

TEST(ObserverTest, PictureObserverGetsNotificationOnShapeChange) {
    shapes::Picture picture;
    CountingPictureObserver obs;
    picture.AddObserver(&obs);

    auto geo = std::make_unique<shapes::Circle>(0, 0, 10);
    auto figure = std::make_unique<shapes::IFigure>("fig1", "#ff0000", std::move(geo));
    picture.AddShape(std::move(figure));

    EXPECT_EQ(obs.count, 1);

    picture.EditShapeColor("fig1", "#00ff00");
    EXPECT_EQ(obs.count, 2);

    picture.MoveShape("fig1", 10, 0);
    EXPECT_EQ(obs.count, 3);
}

TEST(ObserverTest, PictureObserverGetsNotificationOnDirectShapeChange) {
    shapes::Picture picture;
    CountingPictureObserver obs;
    picture.AddObserver(&obs);

    auto geo = std::make_unique<shapes::Circle>(0, 0, 10);
    auto figure = std::make_unique<shapes::IFigure>("fig1", "#ff0000", std::move(geo));
    picture.AddShape(std::move(figure));

    EXPECT_EQ(obs.count, 1);  // AddShape

    auto& shape = picture.GetShape("fig1");
    shape.SetColor("#0000ff");

    EXPECT_EQ(obs.count, 2);  // Picture узнала через подписку
}

TEST(ObserverTest, AddAndDeleteNotifyObservers) {
    shapes::Picture picture;
    CountingPictureObserver obs;
    picture.AddObserver(&obs);

    auto geo1 = std::make_unique<shapes::Circle>(0, 0, 10);
    picture.AddShape(std::make_unique<shapes::IFigure>("fig1", "#ff0000", std::move(geo1)));
    EXPECT_EQ(obs.count, 1);
    EXPECT_EQ(obs.lastShapeCount, 1);

    picture.DeleteShape("fig1");
    EXPECT_EQ(obs.count, 2);
    EXPECT_EQ(obs.lastShapeCount, 0);
}

TEST(ObserverTest, PictureSubscribesToAddedShape) {
    shapes::Picture picture;
    CountingPictureObserver obs;
    picture.AddObserver(&obs);

    auto geo = std::make_unique<shapes::Circle>(0, 0, 10);
    auto figure = std::make_unique<shapes::IFigure>("fig1", "#ff0000", std::move(geo));
    picture.AddShape(std::move(figure));

    int before = obs.count;

    auto& shape = picture.GetShape("fig1");
    shape.Move(1, 1);

    EXPECT_EQ(obs.count, before + 1);
}

TEST(ObserverTest, PictureUnsubscribesOnDelete) {
    shapes::Picture picture;
    CountingPictureObserver obs;
    picture.AddObserver(&obs);

    auto geo = std::make_unique<shapes::Circle>(0, 0, 10);
    auto figure = std::make_unique<shapes::IFigure>("fig1", "#ff0000", std::move(geo));
    picture.AddShape(std::move(figure));

    picture.DeleteShape("fig1");
    int afterDelete = obs.count;

    EXPECT_EQ(afterDelete, 2);
}

TEST(ObserverTest, MultipleObserversGetNotifications) {
    shapes::Picture picture;
    CountingPictureObserver obs1, obs2, obs3;
    picture.AddObserver(&obs1);
    picture.AddObserver(&obs2);
    picture.AddObserver(&obs3);

    auto geo = std::make_unique<shapes::Circle>(0, 0, 10);
    picture.AddShape(std::make_unique<shapes::IFigure>("fig1", "#ff0000", std::move(geo)));

    EXPECT_EQ(obs1.count, 1);
    EXPECT_EQ(obs2.count, 1);
    EXPECT_EQ(obs3.count, 1);
}

TEST(ObserverTest, UnsubscribedObserverStopsReceiving) {
    shapes::Picture picture;
    CountingPictureObserver obs;
    picture.AddObserver(&obs);

    auto geo = std::make_unique<shapes::Circle>(0, 0, 10);
    picture.AddShape(std::make_unique<shapes::IFigure>("fig1", "#ff0000", std::move(geo)));
    EXPECT_EQ(obs.count, 1);

    picture.RemoveObserver(&obs);

    auto geo2 = std::make_unique<shapes::Circle>(0, 0, 10);
    picture.AddShape(std::make_unique<shapes::IFigure>("fig2", "#00ff00", std::move(geo2)));

    EXPECT_EQ(obs.count, 1);  // не изменился
}

TEST(ObserverTest, DuplicateRegistrationDoesNotDuplicateNotifications) {
    shapes::Picture picture;
    CountingPictureObserver obs;
    picture.AddObserver(&obs);
    picture.AddObserver(&obs);

    auto geo = std::make_unique<shapes::Circle>(0, 0, 10);
    picture.AddShape(std::make_unique<shapes::IFigure>("fig1", "#ff0000", std::move(geo)));

    EXPECT_EQ(obs.count, 1);
}

TEST(ObserverTest, FailedOperationDoesNotNotify) {
    shapes::Picture picture;
    CountingPictureObserver obs;
    picture.AddObserver(&obs);

    auto geo = std::make_unique<shapes::Circle>(0, 0, 10);
    picture.AddShape(std::make_unique<shapes::IFigure>("fig1", "#ff0000", std::move(geo)));
    EXPECT_EQ(obs.count, 1);

    EXPECT_THROW(picture.EditShapeColor("fig1", "#ZZZZZZ"), std::runtime_error);

    EXPECT_EQ(obs.count, 1);
}
