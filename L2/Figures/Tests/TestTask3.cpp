#include <gtest/gtest.h>
#include "../Shapes/Picture.h"
#include "../Shapes/IFigure.h"
#include "../Shapes/Figures/Circle.h"
#include "../Observer/IPictureObserver.h"
#include "../Observer/IFigureObserver.h"
#include "../Observer/Subscription.h"

#include <memory>

class CountingPictureObserver : public IPictureObserver {
public:
    int count = 0;
    void OnPictureChanged(std::size_t) override { ++count; }
};

class CountingFigureObserver : public IFigureObserver {
public:
    int count = 0;
    void OnShapeChanged() override { ++count; }
};

static std::unique_ptr<shapes::IFigure> MakeCircle(const std::string& id) {
    auto geo = std::make_unique<shapes::Circle>(0, 0, 10);
    return std::make_unique<shapes::IFigure>(id, "#ff0000", std::move(geo));
}

TEST(SubscriptionTest, ActiveSubscriptionReceivesNotifications) {
    shapes::Picture picture;
    CountingPictureObserver obs;

    {
        auto sub = picture.Subscribe(&obs);
        picture.AddShape(MakeCircle("c1"));
        EXPECT_EQ(obs.count, 1);

        picture.AddShape(MakeCircle("c2"));
        EXPECT_EQ(obs.count, 2);
    }
}

TEST(SubscriptionTest, DestroyedSubscriptionStopsNotifications) {
    shapes::Picture picture;
    CountingPictureObserver obs;

    {
        auto sub = picture.Subscribe(&obs);
        picture.AddShape(MakeCircle("c1"));
        EXPECT_EQ(obs.count, 1);
    }   // sub уничтожен → Disconnect → отписка

    picture.AddShape(MakeCircle("c2"));
    EXPECT_EQ(obs.count, 1);  // не изменился
}

TEST(SubscriptionTest, DisconnectStopsNotifications) {
    shapes::Picture picture;
    CountingPictureObserver obs;

    auto sub = picture.Subscribe(&obs);
    picture.AddShape(MakeCircle("c1"));
    EXPECT_EQ(obs.count, 1);

    sub->Disconnect();

    picture.AddShape(MakeCircle("c2"));
    EXPECT_EQ(obs.count, 1);
}

TEST(SubscriptionTest, RepeatedDisconnectIsSafe) {
    shapes::Picture picture;
    CountingPictureObserver obs;

    auto sub = picture.Subscribe(&obs);
    sub->Disconnect();
    EXPECT_NO_THROW(sub->Disconnect());
    EXPECT_NO_THROW(sub->Disconnect());
    EXPECT_FALSE(sub->IsActive());
}

class SelfDisconnectingObserver : public IPictureObserver {
public:
    int count = 0;
    Subscription<shapes::Picture, IPictureObserver>* sub = nullptr;

    void OnPictureChanged(std::size_t) override {
        ++count;
        if (sub) {
            sub->Disconnect();
            sub = nullptr;
        }
    }
};

TEST(SubscriptionTest, ObserverCanDisconnectFromHandler) {
    shapes::Picture picture;
    SelfDisconnectingObserver obs;

    auto sub = picture.Subscribe(&obs);
    obs.sub = sub.get();

    picture.AddShape(MakeCircle("c1"));
    EXPECT_EQ(obs.count, 1);

    picture.AddShape(MakeCircle("c2"));
    EXPECT_EQ(obs.count, 1);   // отписан — уведомлений нет
}

TEST(SubscriptionTest, ObserverDestroyedBeforeSubject) {
    shapes::Picture picture;

    {
        CountingPictureObserver obs;
        auto sub = picture.Subscribe(&obs);

        picture.AddShape(MakeCircle("c1"));
        EXPECT_EQ(obs.count, 1);
    }

    picture.AddShape(MakeCircle("c2"));
}

TEST(SubscriptionTest, SubjectDestroyedBeforeSubscription) {
    CountingPictureObserver obs;
    std::unique_ptr<Subscription<shapes::Picture, IPictureObserver>> sub;

    {
        shapes::Picture picture;
        sub = picture.Subscribe(&obs);
        picture.AddShape(MakeCircle("c1"));
        EXPECT_EQ(obs.count, 1);
    }

    EXPECT_NO_THROW(sub->Disconnect());
    EXPECT_NO_THROW(sub->Disconnect());
    EXPECT_FALSE(sub->IsActive());
}

TEST(SubscriptionTest, DestroySubscriptionAfterSubjectIsSafe) {
    CountingPictureObserver obs;
    std::unique_ptr<Subscription<shapes::Picture, IPictureObserver>> sub;

    {
        shapes::Picture picture;
        sub = picture.Subscribe(&obs);
    }

    EXPECT_NO_THROW(sub.reset());
}

TEST(SubscriptionTest, FigureSubscriptionWorks) {
    auto figure = MakeCircle("f1");
    CountingFigureObserver obs;

    {
        auto sub = figure->Subscribe(&obs);
        figure->Move(1, 1);
        EXPECT_EQ(obs.count, 1);

        figure->SetColor("#00ff00");
        EXPECT_EQ(obs.count, 2);
    }   // отписка

    figure->Move(1, 1);
    EXPECT_EQ(obs.count, 2);
}

TEST(SubscriptionTest, FigureDestroyedBeforeSubscription) {
    CountingFigureObserver obs;
    std::unique_ptr<Subscription<shapes::IFigure, IFigureObserver>> sub;

    {
        auto figure = MakeCircle("f1");
        sub = figure->Subscribe(&obs);
        figure->Move(1, 1);
        EXPECT_EQ(obs.count, 1);
    }   // figure уничтожена, sub жив

    EXPECT_NO_THROW(sub->Disconnect());
    EXPECT_FALSE(sub->IsActive());
}

TEST(SubscriptionTest, UniquePtrMoveKeepsSubscriptionAlive) {
    shapes::Picture picture;
    CountingPictureObserver obs;

    auto sub1 = picture.Subscribe(&obs);
    auto* raw = sub1.get();

    auto sub2 = std::move(sub1);

    picture.AddShape(MakeCircle("c1"));
    EXPECT_EQ(obs.count, 1);

    EXPECT_TRUE(sub2->IsActive());
    EXPECT_EQ(raw, sub2.get());

    sub2->Disconnect();
    picture.AddShape(MakeCircle("c2"));
    EXPECT_EQ(obs.count, 1);
}

TEST(SubscriptionTest, ObserverCanUnsubscribeAnother) {
    shapes::Picture picture;
    CountingPictureObserver a, b, c;

    auto subA = picture.Subscribe(&a);
    auto subB = picture.Subscribe(&b);
    auto subC = picture.Subscribe(&c);

    subB->Disconnect();

    picture.AddShape(MakeCircle("c1"));

    EXPECT_EQ(a.count, 1);
    EXPECT_EQ(b.count, 0);
    EXPECT_EQ(c.count, 1);
}