//
// Created by maxim on 26.10.2026.
//

#ifndef L4_TESTUNDOREDO_H
#define L4_TESTUNDOREDO_H
#include <gtest/gtest.h>

#include "TestHelpers.h"

#include "Model/Picture.h"
#include "Model/MakeShapeUtils.h"
#include "Commands/CommandHistory.h"
#include "Commands/AddCommand.h"
#include "Commands/DeleteCommand.h"
#include "Commands/MoveShapesCommand.h"
#include "Commands/ResizeCommand.h"
#include "IO/SaveService.h"
#include "IO/LoadService.h"

#include <cmath>
#include <sstream>

using namespace testhelp;

TEST(UndoRedo, SequentialUndo)
{
    shapes::Picture pic;
    CommandHistory h;

    auto a = AddRect(pic, h);
    auto b = AddRect(pic, h);
    auto c = AddRect(pic, h);

    ASSERT_TRUE(pic.HasShape(a));
    ASSERT_TRUE(pic.HasShape(b));
    ASSERT_TRUE(pic.HasShape(c));

    h.Undo();
    EXPECT_FALSE(pic.HasShape(c));
    EXPECT_TRUE(pic.HasShape(a));
    EXPECT_TRUE(pic.HasShape(b));

    h.Undo();
    EXPECT_FALSE(pic.HasShape(b));
    EXPECT_TRUE(pic.HasShape(a));

    h.Undo();
    EXPECT_FALSE(pic.HasShape(a));
}

TEST(UndoRedo, SequentialRedo)
{
    shapes::Picture pic;
    CommandHistory h;

    auto a = AddRect(pic, h);
    auto b = AddRect(pic, h);

    h.Undo();
    h.Undo();
    EXPECT_FALSE(pic.HasShape(a));
    EXPECT_FALSE(pic.HasShape(b));

    h.Redo();
    EXPECT_TRUE(pic.HasShape(a));
    EXPECT_FALSE(pic.HasShape(b));

    h.Redo();
    EXPECT_TRUE(pic.HasShape(a));
    EXPECT_TRUE(pic.HasShape(b));
}

TEST(UndoRedo, RedoClearedAfterNewAction)
{
    shapes::Picture pic;
    CommandHistory h;

    auto a = AddRect(pic, h);
    auto b = AddRect(pic, h);
    h.Undo();
    EXPECT_TRUE(pic.HasShape(a));
    EXPECT_FALSE(pic.HasShape(b));

    auto c = AddRect(pic, h);   // C — новое действие
    EXPECT_FALSE(h.Redo());     // redo пуст
    EXPECT_TRUE(pic.HasShape(a));
    EXPECT_TRUE(pic.HasShape(c));
    EXPECT_FALSE(pic.HasShape(b));
}

TEST(UndoRedo, AddDeleteRestore)
{
    shapes::Picture pic;
    CommandHistory h;

    auto a = AddRect(pic, h);
    h.Execute(std::make_unique<DeleteCommand>(pic, std::vector<std::string>{a}));
    EXPECT_FALSE(pic.HasShape(a));

    h.Undo();
    EXPECT_TRUE(pic.HasShape(a));

    h.Undo();
    EXPECT_FALSE(pic.HasShape(a));

    h.Redo();
    EXPECT_TRUE(pic.HasShape(a));

    h.Redo();
    EXPECT_FALSE(pic.HasShape(a));
}

TEST(UndoRedo, MoveUndo)
{
    shapes::Picture pic;
    CommandHistory h;

    auto a = AddRect(pic, h, {10, 10, 50, 50});
    shapes::Bounds before = pic.GetShape(a).GetBounds();

    h.Execute(std::make_unique<MoveShapesCommand>(
        pic, std::vector<std::string>{a}, 100.0, 50.0, /*alreadyApplied=*/false));

    shapes::Bounds after = pic.GetShape(a).GetBounds();
    EXPECT_NEAR(after.x, before.x + 100.0, 1e-9);
    EXPECT_NEAR(after.y, before.y +  50.0, 1e-9);

    h.Undo();
    shapes::Bounds restored = pic.GetShape(a).GetBounds();
    EXPECT_NEAR(restored.x, before.x, 1e-9);
    EXPECT_NEAR(restored.y, before.y, 1e-9);

    h.Redo();
    shapes::Bounds redone = pic.GetShape(a).GetBounds();
    EXPECT_NEAR(redone.x, before.x + 100.0, 1e-9);
    EXPECT_NEAR(redone.y, before.y +  50.0, 1e-9);
}

TEST(UndoRedo, ResizeUndo)
{
    shapes::Picture pic;
    CommandHistory h;

    auto a = AddRect(pic, h, {10, 10, 50, 50});
    shapes::Bounds oldB = pic.GetShape(a).GetBounds();
    shapes::Bounds newB = {20, 20, 100, 80};

    h.Execute(std::make_unique<ResizeCommand>(
        pic, a, oldB, newB, /*alreadyApplied=*/false));

    shapes::Bounds after = pic.GetShape(a).GetBounds();
    EXPECT_NEAR(after.width,  100.0, 1e-9);
    EXPECT_NEAR(after.height,  80.0, 1e-9);

    h.Undo();
    shapes::Bounds restored = pic.GetShape(a).GetBounds();
    EXPECT_NEAR(restored.x,      oldB.x,      1e-9);
    EXPECT_NEAR(restored.width,  oldB.width,  1e-9);
    EXPECT_NEAR(restored.height, oldB.height, 1e-9);

    h.Redo();
    shapes::Bounds redone = pic.GetShape(a).GetBounds();
    EXPECT_NEAR(redone.width,  newB.width,  1e-9);
    EXPECT_NEAR(redone.height, newB.height, 1e-9);
}

TEST(UndoRedo, HistoryBoundaries)
{
    shapes::Picture pic;
    CommandHistory h;

    EXPECT_FALSE(h.Undo());
    EXPECT_FALSE(h.Redo());

    auto a = AddRect(pic, h);
    h.Undo();
    EXPECT_FALSE(h.Undo());
    h.Redo();
    EXPECT_FALSE(h.Redo());
    EXPECT_TRUE(pic.HasShape(a));
}

TEST(UndoRedo, DeleteUndoPreservesZOrder)
{
    shapes::Picture pic;
    CommandHistory h;

    auto a = AddRect(pic, h, {0,  0, 10, 10});
    auto b = AddRect(pic, h, {20,20, 10, 10});
    auto c = AddRect(pic, h, {40,40, 10, 10});

    ASSERT_EQ(pic.GetOrder().size(), 3u);
    EXPECT_EQ(pic.GetOrder()[0], a);
    EXPECT_EQ(pic.GetOrder()[1], b);
    EXPECT_EQ(pic.GetOrder()[2], c);

    h.Execute(std::make_unique<DeleteCommand>(
        pic, std::vector<std::string>{b}));
    ASSERT_EQ(pic.GetOrder().size(), 2u);
    EXPECT_EQ(pic.GetOrder()[0], a);
    EXPECT_EQ(pic.GetOrder()[1], c);

    h.Undo();
    ASSERT_EQ(pic.GetOrder().size(), 3u);
    EXPECT_EQ(pic.GetOrder()[0], a);
    EXPECT_EQ(pic.GetOrder()[1], b);
    EXPECT_EQ(pic.GetOrder()[2], c);
}

TEST(UndoRedo, SaveLoadRoundtrip)
{
    shapes::Picture pic;
    CommandHistory h;

    auto a = AddRect(pic, h, {10, 10, 50, 50});
    auto e = AddEllipse(pic, h, {100, 100, 60, 40});

    std::stringstream ss;
    SaveService::Save(pic, ss);
    shapes::Picture restored = LoadService::Load(ss);

    ASSERT_EQ(restored.GetOrder().size(), 2u);
    EXPECT_TRUE(restored.HasShape(a));
    EXPECT_TRUE(restored.HasShape(e));

    auto b1 = pic.GetShape(a).GetBounds();
    auto b2 = restored.GetShape(a).GetBounds();
    EXPECT_NEAR(b1.x,      b2.x,      1e-6);
    EXPECT_NEAR(b1.y,      b2.y,      1e-6);
    EXPECT_NEAR(b1.width,  b2.width,  1e-6);
    EXPECT_NEAR(b1.height, b2.height, 1e-6);

    EXPECT_EQ(restored.GetOrder()[0], a);
    EXPECT_EQ(restored.GetOrder()[1], e);
}
#endif //L4_TESTUNDOREDO_H