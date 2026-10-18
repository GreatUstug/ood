#include <gtest/gtest.h>
#include "../Observer/ObserverList.h"

#include <vector>
#include <string>

class TestObserver {
public:
    std::string name;
    std::vector<std::string>* log = nullptr;
    ObserverList<TestObserver>* list = nullptr;

    std::function<void()> action;

    explicit TestObserver(std::string n) : name(std::move(n)) {}

    void OnChanged() {
        if (log) log->push_back(name);
        if (action) action();
    }
};

TEST(ObserverListTest, ObserverUnsubscribesItself) {
    ObserverList<TestObserver> list;
    std::vector<std::string> log;

    TestObserver a("A"), b("B"), c("C");
    a.log = &log; b.log = &log; c.log = &log;
    a.list = &list;

    list.AddObserver(&a);
    list.AddObserver(&b);
    list.AddObserver(&c);

    a.action = [&]() { list.RemoveObserver(&a); };

    list.Notify([](TestObserver* o) { o->OnChanged(); });

    ASSERT_EQ(log.size(), 3u);
    EXPECT_EQ(log[0], "A");
    EXPECT_EQ(log[1], "B");
    EXPECT_EQ(log[2], "C");

    log.clear();
    list.Notify([](TestObserver* o) { o->OnChanged(); });
    ASSERT_EQ(log.size(), 2u);
    EXPECT_EQ(log[0], "B");
    EXPECT_EQ(log[1], "C");
}

TEST(ObserverListTest, AUnsubscribesBBeforeItsTurn) {
    ObserverList<TestObserver> list;
    std::vector<std::string> log;

    TestObserver a("A"), b("B"), c("C");
    a.log = &log; b.log = &log; c.log = &log;
    a.list = &list;

    list.AddObserver(&a);
    list.AddObserver(&b);
    list.AddObserver(&c);

    a.action = [&]() { list.RemoveObserver(&b); };

    list.Notify([](TestObserver* o) { o->OnChanged(); });

    ASSERT_EQ(log.size(), 2u);
    EXPECT_EQ(log[0], "A");
    EXPECT_EQ(log[1], "C");
}

TEST(ObserverListTest, UnsubscribeDoesNotAffectOthers) {
    ObserverList<TestObserver> list;
    std::vector<std::string> log;

    TestObserver a("A"), b("B"), c("C"), d("D");
    a.log = &log; b.log = &log; c.log = &log; d.log = &log;
    a.list = &list;

    list.AddObserver(&a);
    list.AddObserver(&b);
    list.AddObserver(&c);
    list.AddObserver(&d);

    a.action = [&]() { list.RemoveObserver(&c); };

    list.Notify([](TestObserver* o) { o->OnChanged(); });

	ASSERT_EQ(log.size(), 3u);
    EXPECT_EQ(log[0], "A");
    EXPECT_EQ(log[1], "B");
    EXPECT_EQ(log[2], "D");
}

TEST(ObserverListTest, NewObserverNotCalledInCurrentNotify) {
    ObserverList<TestObserver> list;
    std::vector<std::string> log;

    TestObserver a("A"), b("B"), c("C");
    a.log = &log; b.log = &log; c.log = &log;
    a.list = &list;

    list.AddObserver(&a);
    list.AddObserver(&b);

    a.action = [&]() { list.AddObserver(&c); };

    list.Notify([](TestObserver* o) { o->OnChanged(); });

    // Текущая рассылка: A, B. C не должен получить.
    ASSERT_EQ(log.size(), 2u);
    EXPECT_EQ(log[0], "A");
    EXPECT_EQ(log[1], "B");
}

TEST(ObserverListTest, NewObserverCalledInNextNotify) {
    ObserverList<TestObserver> list;
    std::vector<std::string> log;

    TestObserver a("A"), b("B"), c("C");
    a.log = &log; b.log = &log; c.log = &log;
    a.list = &list;

    list.AddObserver(&a);
    list.AddObserver(&b);

    a.action = [&]() { list.AddObserver(&c); };

    list.Notify([](TestObserver* o) { o->OnChanged(); });
    log.clear();

    list.Notify([](TestObserver* o) { o->OnChanged(); });

    ASSERT_EQ(log.size(), 3u);
    EXPECT_EQ(log[0], "A");
    EXPECT_EQ(log[1], "B");
    EXPECT_EQ(log[2], "C");
}

TEST(ObserverListTest, ResubscribeTakesEffectNextNotify) {
    ObserverList<TestObserver> list;
    std::vector<std::string> log;

    TestObserver a("A"), b("B");
    a.log = &log; b.log = &log;
    a.list = &list;

    list.AddObserver(&a);
    list.AddObserver(&b);

	a.action = [&]() {
        list.RemoveObserver(&a);
        list.AddObserver(&a);
    };

    list.Notify([](TestObserver* o) { o->OnChanged(); });

    ASSERT_EQ(log.size(), 2u);
    EXPECT_EQ(log[0], "A");
    EXPECT_EQ(log[1], "B");

    log.clear();
    list.Notify([](TestObserver* o) { o->OnChanged(); });

    ASSERT_EQ(log.size(), 2u);
    EXPECT_EQ(log[0], "B");
    EXPECT_EQ(log[1], "A");
}

TEST(ObserverListTest, NewObserverAddedAtEndNotCalled) {
    ObserverList<TestObserver> list;
    std::vector<std::string> log;

    TestObserver a("A"), b("B"), c("C");
    a.log = &log; b.log = &log; c.log = &log;
    a.list = &list;

    list.AddObserver(&a);
    list.AddObserver(&b);

    a.action = [&]() { list.AddObserver(&c); };

    list.Notify([](TestObserver* o) { o->OnChanged(); });

    EXPECT_EQ(log.size(), 2u);

    EXPECT_EQ(list.CountActive(), 3u);
}