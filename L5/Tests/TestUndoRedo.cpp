//
// Created by maxim on 26.10.2026.
//

#include <gtest/gtest.h>

#include "TestHelpers.h"

#include "Model/Picture.h"
#include "Model/MakeShapeUtils.h"
#include "Commands/CommandHistory.h"
#include "Commands/AddCommand.h"
#include "Commands/ColorCommand.h"
#include "Commands/DeleteCommand.h"
#include "Commands/GroupCommand.h"
#include "Commands/MoveShapesCommand.h"
#include "Commands/ReorderCommand.h"
#include "Commands/ResizeCommand.h"
#include "Commands/SetTimeCommand.h"
#include "Commands/UngroupCommand.h"
#include "IO/SaveService.h"
#include "IO/LoadService.h"

#include <algorithm>
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

    auto c = AddRect(pic, h);
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
        pic, std::vector<std::string>{a}, 100.0, 50.0));

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

    h.Execute(std::make_unique<ResizeCommand>(pic, a, oldB, newB));

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
    shapes::Picture pic; CommandHistory h;

    EXPECT_TRUE(h.Undo().empty());
    EXPECT_TRUE(h.Redo().empty());

    auto a = AddRect(pic, h);
    EXPECT_TRUE(h.Redo().empty());
    EXPECT_FALSE(h.Undo().empty());
    EXPECT_TRUE(h.Undo().empty());
    EXPECT_FALSE(h.Redo().empty());
    EXPECT_TRUE(h.Redo().empty());
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

    h.Execute(std::make_unique<DeleteCommand>(pic, std::vector<std::string>{b}));
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

TEST(Color, FillUndoRedo)
{
    shapes::Picture pic; CommandHistory h;
    auto a = AddRect(pic, h);
    gfx::Color oldFill = pic.GetShape(a).GetFillColor();
    gfx::Color red(255, 0, 0, 255);

    h.Execute(std::make_unique<ColorCommand>(pic, std::vector<std::string>{a},
        red, shapes::ColorTarget::Fill));
    EXPECT_EQ(pic.GetShape(a).GetFillColor(), red);
    h.Undo();
    EXPECT_EQ(pic.GetShape(a).GetFillColor(), oldFill);
    h.Redo();
    EXPECT_EQ(pic.GetShape(a).GetFillColor(), red);
}

TEST(Color, StrokeUndoRedo)
{
    shapes::Picture pic; CommandHistory h;
    auto a = AddRect(pic, h);
    gfx::Color oldStroke = pic.GetShape(a).GetStrokeColor();
    gfx::Color blue(0, 0, 255, 255);

    h.Execute(std::make_unique<ColorCommand>(pic, std::vector<std::string>{a},
        blue, shapes::ColorTarget::Stroke));
    EXPECT_EQ(pic.GetShape(a).GetStrokeColor(), blue);
    h.Undo();
    EXPECT_EQ(pic.GetShape(a).GetStrokeColor(), oldStroke);
}

TEST(Color, SaveLoadBothColors)
{
    shapes::Picture pic; CommandHistory h;
    auto a = AddRect(pic, h);
    h.Execute(std::make_unique<ColorCommand>(pic, std::vector<std::string>{a},
        gfx::Color(10, 20, 30, 255), shapes::ColorTarget::Fill));
    h.Execute(std::make_unique<ColorCommand>(pic, std::vector<std::string>{a},
        gfx::Color(40, 50, 60, 255), shapes::ColorTarget::Stroke));

    std::stringstream ss;
    SaveService::Save(pic, ss);
    shapes::Picture restored = LoadService::Load(ss);

    EXPECT_EQ(restored.GetShape(a).GetFillColor(),   gfx::Color(10, 20, 30, 255));
    EXPECT_EQ(restored.GetShape(a).GetStrokeColor(), gfx::Color(40, 50, 60, 255));
}

TEST(Color, MultipleShapesOneCommand)
{
    shapes::Picture pic; CommandHistory h;
    auto a = AddRect(pic, h);
    auto b = AddRect(pic, h);
    auto c = AddRect(pic, h);

    gfx::Color oldA = pic.GetShape(a).GetFillColor();
    gfx::Color oldB = pic.GetShape(b).GetFillColor();
    gfx::Color red(255, 0, 0, 255);

    h.Execute(std::make_unique<ColorCommand>(
        pic, std::vector<std::string>{a, b}, red, shapes::ColorTarget::Fill));

    EXPECT_EQ(pic.GetShape(a).GetFillColor(), red);
    EXPECT_EQ(pic.GetShape(b).GetFillColor(), red);
    EXPECT_EQ(pic.GetShape(c).GetFillColor(), oldA);

    h.Undo();
    EXPECT_EQ(pic.GetShape(a).GetFillColor(), oldA);
    EXPECT_EQ(pic.GetShape(b).GetFillColor(), oldB);
}

TEST(Reorder, OutOfRangeClamped)
{
    shapes::Picture pic; CommandHistory h;
    auto a = AddRect(pic, h);
    auto b = AddRect(pic, h);

    h.Execute(std::make_unique<ReorderCommand>(pic, a, 0, 100));
    EXPECT_EQ(pic.GetOrder(), std::vector<std::string>({b, a}));

    h.Undo();
    EXPECT_EQ(pic.GetOrder(), std::vector<std::string>({a, b}));
}

TEST(SetTime, UndoRedo)
{
    shapes::Picture pic; CommandHistory h;
    auto a = AddRect(pic, h);
    ASSERT_DOUBLE_EQ(pic.GetShape(a).GetTime(), 0.0);

    h.Execute(std::make_unique<SetTimeCommand>(
        pic, std::vector<std::string>{a}, std::vector<double>{3.5}, false));
    EXPECT_DOUBLE_EQ(pic.GetShape(a).GetTime(), 3.5);

    h.Undo();
    EXPECT_DOUBLE_EQ(pic.GetShape(a).GetTime(), 0.0);

    h.Redo();
    EXPECT_DOUBLE_EQ(pic.GetShape(a).GetTime(), 3.5);
}

TEST(SetTime, SaveLoadPreservesTime)
{
    shapes::Picture pic; CommandHistory h;
    auto a = AddRect(pic, h);
    h.Execute(std::make_unique<SetTimeCommand>(
        pic, std::vector<std::string>{a}, std::vector<double>{7.25}, false));

    std::stringstream ss;
    SaveService::Save(pic, ss);
    auto restored = LoadService::Load(ss);

    EXPECT_NEAR(restored.GetShape(a).GetTime(), 7.25, 1e-9);
}

TEST(AffectedIds, AddReturnsId)
{
    shapes::Picture pic; CommandHistory h;
    auto a = AddRect(pic, h);
    auto ids = h.Undo();
    ASSERT_EQ(ids.size(), 1u);
    EXPECT_EQ(ids[0], a);
}

TEST(AffectedIds, MoveReturnsAllIds)
{
    shapes::Picture pic; CommandHistory h;
    auto a = AddRect(pic, h);
    auto b = AddRect(pic, h);
    h.Execute(std::make_unique<MoveShapesCommand>(
        pic, std::vector<std::string>{a, b}, 10, 10));

    auto ids = h.Undo();
    EXPECT_EQ(ids.size(), 2u);
    EXPECT_NE(std::find(ids.begin(), ids.end(), a), ids.end());
    EXPECT_NE(std::find(ids.begin(), ids.end(), b), ids.end());
}

TEST(Delete, MultipleShapesRestoreOrder)
{
    shapes::Picture pic; CommandHistory h;
    auto a = AddRect(pic, h);
    auto b = AddRect(pic, h);
    auto c = AddRect(pic, h);
    auto d = AddRect(pic, h);

    h.Execute(std::make_unique<DeleteCommand>(
        pic, std::vector<std::string>{b, d}));

    ASSERT_EQ(pic.GetOrder(), std::vector<std::string>({a, c}));

    h.Undo();
    EXPECT_EQ(pic.GetOrder(), std::vector<std::string>({a, b, c, d}));

    h.Redo();
    EXPECT_EQ(pic.GetOrder(), std::vector<std::string>({a, c}));
}

TEST(GroupTests, GroupNonAdjacentAndZOrder)
{
    shapes::Picture pic; CommandHistory h;
    auto a = AddRect(pic, h, {0, 0, 10, 10});
    auto b = AddRect(pic, h, {20, 20, 10, 10});
    auto c = AddRect(pic, h, {40, 40, 10, 10});
    auto d = AddRect(pic, h, {60, 60, 10, 10});

    h.Execute(std::make_unique<GroupCommand>(pic, std::vector<std::string>{b, d}));

    std::string gId = FindFirstGroupId(pic);
    ASSERT_FALSE(gId.empty());

    ASSERT_EQ(pic.GetOrder().size(), 3u);
    EXPECT_EQ(pic.GetOrder()[0], a);
    EXPECT_EQ(pic.GetOrder()[1], c);
    EXPECT_EQ(pic.GetOrder()[2], gId);

    auto children = pic.GetShape(gId).GetChildIds();
    ASSERT_EQ(children.size(), 2u);
    EXPECT_EQ(children[0], b);
    EXPECT_EQ(children[1], d);
}

TEST(GroupTests, UndoGroupRestoresIndices)
{
    shapes::Picture pic; CommandHistory h;
    auto a = AddRect(pic, h);
    auto b = AddRect(pic, h);
    auto c = AddRect(pic, h);

    h.Execute(std::make_unique<GroupCommand>(pic, std::vector<std::string>{a, c}));
    h.Undo();

    ASSERT_EQ(pic.GetOrder().size(), 3u);
    EXPECT_EQ(pic.GetOrder()[0], a);
    EXPECT_EQ(pic.GetOrder()[1], b);
    EXPECT_EQ(pic.GetOrder()[2], c);
}

TEST(GroupTests, UngroupAndUndoRedo)
{
    shapes::Picture pic; CommandHistory h;
    auto a = AddRect(pic, h);
    auto b = AddRect(pic, h);
    auto c = AddRect(pic, h);

    h.Execute(std::make_unique<GroupCommand>(pic, std::vector<std::string>{a, b}));
    std::string gId = FindFirstGroupId(pic);
    ASSERT_EQ(pic.GetOrder().size(), 2u);
    EXPECT_EQ(pic.GetOrder()[0], c);
    EXPECT_EQ(pic.GetOrder()[1], gId);

    h.Execute(std::make_unique<UngroupCommand>(pic, gId));
    ASSERT_EQ(pic.GetOrder().size(), 3u);
    EXPECT_EQ(pic.GetOrder()[0], c);
    EXPECT_EQ(pic.GetOrder()[1], a);
    EXPECT_EQ(pic.GetOrder()[2], b);

    h.Undo();
    ASSERT_EQ(pic.GetOrder().size(), 2u);
    EXPECT_TRUE(pic.HasShape(gId));
    EXPECT_EQ(pic.GetOrder()[0], c);
    EXPECT_EQ(pic.GetOrder()[1], gId);
    EXPECT_EQ(pic.GetShape(gId).GetChildIds(),
              std::vector<std::string>({a, b}));

    h.Redo();
    ASSERT_EQ(pic.GetOrder().size(), 3u);
    EXPECT_FALSE(pic.HasShape(gId));
    EXPECT_EQ(pic.GetOrder()[0], c);
    EXPECT_EQ(pic.GetOrder()[1], a);
    EXPECT_EQ(pic.GetOrder()[2], b);
}

TEST(GroupTests, MoveGroupMovesChildren)
{
    shapes::Picture pic; CommandHistory h;
    auto a = AddRect(pic, h, {10, 10, 10, 10});
    auto b = AddRect(pic, h, {50, 50, 10, 10});

    h.Execute(std::make_unique<GroupCommand>(pic, std::vector<std::string>{a, b}));
    std::string gId = FindFirstGroupId(pic);
    ASSERT_FALSE(gId.empty());

    auto beforeG = pic.GetShape(gId).GetBounds();

    h.Execute(std::make_unique<MoveShapesCommand>(
        pic, std::vector<std::string>{gId}, 100.0, 50.0));

    auto afterG = pic.GetShape(gId).GetBounds();
    EXPECT_NEAR(afterG.x, beforeG.x + 100.0, 1e-9);
    EXPECT_NEAR(afterG.y, beforeG.y +  50.0, 1e-9);

    h.Execute(std::make_unique<UngroupCommand>(pic, gId));
    auto bA = pic.GetShape(a).GetBounds();
    auto bB = pic.GetShape(b).GetBounds();
    EXPECT_NEAR(bA.x, 10.0 + 100.0, 1e-9);
    EXPECT_NEAR(bA.y, 10.0 +  50.0, 1e-9);
    EXPECT_NEAR(bB.x, 50.0 + 100.0, 1e-9);
    EXPECT_NEAR(bB.y, 50.0 +  50.0, 1e-9);
}

TEST(GroupTests, DeleteGroupAndUndoRedo)
{
    shapes::Picture pic; CommandHistory h;
    auto a = AddRect(pic, h);
    auto b = AddRect(pic, h);

    h.Execute(std::make_unique<GroupCommand>(pic, std::vector<std::string>{a, b}));
    std::string gId = FindFirstGroupId(pic);
    ASSERT_FALSE(gId.empty());

    h.Execute(std::make_unique<DeleteCommand>(pic, std::vector<std::string>{gId}));
    EXPECT_FALSE(pic.HasShape(gId));
    EXPECT_FALSE(pic.HasShape(a));
    EXPECT_FALSE(pic.HasShape(b));
    EXPECT_EQ(pic.GetOrder().size(), 0u);

    h.Undo();
    ASSERT_EQ(pic.GetOrder().size(), 1u);
    EXPECT_TRUE(pic.HasShape(gId));
    EXPECT_EQ(pic.GetOrder()[0], gId);

    auto children = pic.GetShape(gId).GetChildIds();
    ASSERT_EQ(children.size(), 2u);
    EXPECT_EQ(children[0], a);
    EXPECT_EQ(children[1], b);

    const auto* childA = FindChild(pic.GetShape(gId), a);
    ASSERT_NE(childA, nullptr);
    EXPECT_EQ(childA->GetType(), "rectangle");
}

TEST(GroupTests, LongChainUndoRedo)
{
    shapes::Picture pic; CommandHistory h;
    auto a = AddRect(pic, h, {0, 0, 10, 10});
    auto b = AddRect(pic, h, {20, 20, 10, 10});

    h.Execute(std::make_unique<GroupCommand>(pic, std::vector<std::string>{a, b}));
    std::string gId = FindFirstGroupId(pic);

    h.Execute(std::make_unique<MoveShapesCommand>(
        pic, std::vector<std::string>{gId}, 50, 50));

    auto bGroup = pic.GetShape(gId).GetBounds();
    shapes::Bounds newB = {bGroup.x, bGroup.y,
                           bGroup.width + 100.0, bGroup.height + 100.0};
    h.Execute(std::make_unique<ResizeCommand>(pic, gId, bGroup, newB));

    shapes::Bounds finalG = pic.GetShape(gId).GetBounds();

    h.Undo(); h.Undo(); h.Undo();
    ASSERT_FALSE(pic.HasShape(gId));
    ASSERT_EQ(pic.GetOrder().size(), 2u);
    EXPECT_NEAR(pic.GetShape(a).GetBounds().x, 0.0, 1e-9);
    EXPECT_NEAR(pic.GetShape(b).GetBounds().x, 20.0, 1e-9);

    h.Redo(); h.Redo(); h.Redo();
    ASSERT_TRUE(pic.HasShape(gId));
    ASSERT_EQ(pic.GetOrder().size(), 1u);

    auto rG = pic.GetShape(gId).GetBounds();
    EXPECT_NEAR(rG.x,      finalG.x,      1e-9);
    EXPECT_NEAR(rG.y,      finalG.y,      1e-9);
    EXPECT_NEAR(rG.width,  finalG.width,  1e-9);
    EXPECT_NEAR(rG.height, finalG.height, 1e-9);
}

TEST(GroupTests, GroupUndoRedoPreservesChildrenIdentity)
{
    shapes::Picture pic; CommandHistory h;
    auto a = AddRect(pic, h);
    auto b = AddRect(pic, h);

    h.Execute(std::make_unique<GroupCommand>(pic, std::vector<std::string>{a, b}));
    std::string gId = FindFirstGroupId(pic);

    h.Undo();
    EXPECT_TRUE(pic.HasShape(a));
    EXPECT_TRUE(pic.HasShape(b));
    EXPECT_FALSE(pic.HasShape(gId));

    EXPECT_EQ(pic.GetShape(a).GetType(), "rectangle");
    EXPECT_EQ(pic.GetShape(b).GetType(), "rectangle");
}

TEST(GroupTests, UngroupUndoRestoresGroupProperties)
{
    shapes::Picture pic; CommandHistory h;
    auto a = AddRect(pic, h, {0, 0, 10, 10});
    auto b = AddRect(pic, h, {20, 0, 10, 10});

    h.Execute(std::make_unique<GroupCommand>(pic, std::vector<std::string>{a, b}));
    std::string gId = FindFirstGroupId(pic);

    pic.GetShape(gId).SetTime(2.5);
    pic.GetShape(gId).SetDuration(3.0);

    h.Execute(std::make_unique<UngroupCommand>(pic, gId));
    EXPECT_FALSE(pic.HasShape(gId));

    h.Undo();
    ASSERT_TRUE(pic.HasShape(gId));
    const auto& restoredGroup = pic.GetShape(gId);
    EXPECT_NEAR(restoredGroup.GetTime(),     2.5, 1e-9);
    EXPECT_NEAR(restoredGroup.GetDuration(), 3.0, 1e-9);
}

TEST(GroupTests, SaveLoadPreservesGroupTimeAndDuration)
{
    shapes::Picture pic; CommandHistory h;
    auto a = AddRect(pic, h);
    auto b = AddRect(pic, h);
    h.Execute(std::make_unique<GroupCommand>(pic, std::vector<std::string>{a, b}));
    std::string gId = FindFirstGroupId(pic);
    pic.GetShape(gId).SetTime(1.5);
    pic.GetShape(gId).SetDuration(4.0);

    std::stringstream ss;
    SaveService::Save(pic, ss);
    auto restored = LoadService::Load(ss);

    ASSERT_EQ(restored.GetOrder().size(), 1u);
    const std::string& g2 = restored.GetOrder()[0];
    EXPECT_NEAR(restored.GetShape(g2).GetTime(),     1.5, 1e-9);
    EXPECT_NEAR(restored.GetShape(g2).GetDuration(), 4.0, 1e-9);

    auto children = restored.GetShape(g2).GetChildIds();
    ASSERT_EQ(children.size(), 2u);
    EXPECT_EQ(children[0], a);
    EXPECT_EQ(children[1], b);
}

TEST(GroupTests, ResizeGroupAndNestedUndoRedo)
{
    shapes::Picture pic; CommandHistory h;
    auto a = AddRect(pic, h, {0, 0, 10, 10});
    auto b = AddRect(pic, h, {20, 0, 10, 10});
    auto c = AddRect(pic, h, {50, 0, 10, 10});

    h.Execute(std::make_unique<GroupCommand>(pic, std::vector<std::string>{a, b}));
    std::string g1 = FindFirstGroupId(pic);
    h.Execute(std::make_unique<GroupCommand>(pic, std::vector<std::string>{g1, c}));

    std::string g2;
    for (const auto& id : pic.GetOrder())
    {
        if (pic.GetShape(id).GetType() == "group")
        {
            auto children = pic.GetShape(id).GetChildIds();
            if (std::find(children.begin(), children.end(), g1) != children.end())
            {
                g2 = id;
                break;
            }
        }
    }
    ASSERT_FALSE(g2.empty());

    shapes::Bounds oldB = pic.GetShape(g2).GetBounds();
    shapes::Bounds newB = {oldB.x, oldB.y, oldB.width * 2.0, oldB.height * 2.0};

    h.Execute(std::make_unique<ResizeCommand>(pic, g2, oldB, newB));

    auto g2B = pic.GetShape(g2).GetBounds();
    EXPECT_NEAR(g2B.width,  120.0, 1e-9);
    EXPECT_NEAR(g2B.height,  20.0, 1e-9);

    // g1 — ребёнок g2, достаём через FindChild
    const auto* g1Ptr = FindChild(pic.GetShape(g2), g1);
    ASSERT_NE(g1Ptr, nullptr);
    auto g1B = g1Ptr->GetBounds();
    EXPECT_NEAR(g1B.width,  60.0, 1e-9);
    EXPECT_NEAR(g1B.height, 20.0, 1e-9);

    h.Execute(std::make_unique<UngroupCommand>(pic, g2));
    h.Execute(std::make_unique<UngroupCommand>(pic, g1));

    auto bA = pic.GetShape(a).GetBounds();
    auto bB = pic.GetShape(b).GetBounds();
    auto bC = pic.GetShape(c).GetBounds();

    EXPECT_NEAR(bA.width, 20.0, 1e-9);
    EXPECT_NEAR(bA.height, 20.0, 1e-9);
    EXPECT_NEAR(bB.x,     40.0, 1e-9);
    EXPECT_NEAR(bB.width, 20.0, 1e-9);
    EXPECT_NEAR(bC.x,    100.0, 1e-9);

    double gap = bB.x - (bA.x + bA.width);
    EXPECT_NEAR(gap, 20.0, 1e-9);

    h.Undo();
    h.Undo();
    h.Undo();

    auto g2AfterUndo = pic.GetShape(g2).GetBounds();
    EXPECT_NEAR(g2AfterUndo.width, 60.0, 1e-9);
    const auto* g1AfterUndo = FindChild(pic.GetShape(g2), g1);
    ASSERT_NE(g1AfterUndo, nullptr);
    EXPECT_NEAR(g1AfterUndo->GetBounds().width, 30.0, 1e-9);

    h.Redo();
    EXPECT_NEAR(pic.GetShape(g2).GetBounds().width, 120.0, 1e-9);
    const auto* g1AfterRedo = FindChild(pic.GetShape(g2), g1);
    ASSERT_NE(g1AfterRedo, nullptr);
    EXPECT_NEAR(g1AfterRedo->GetBounds().width, 60.0, 1e-9);
}

TEST(GroupTests, SaveLoadNestedHierarchy)
{
    shapes::Picture pic; CommandHistory h;
    auto a = AddRect(pic, h,    {0,  0, 10, 10});
    auto b = AddRect(pic, h,    {20, 0, 10, 10});
    auto c = AddEllipse(pic, h, {50, 0, 10, 10});

    auto bA_before = pic.GetShape(a).GetBounds();

    h.Execute(std::make_unique<GroupCommand>(pic, std::vector<std::string>{a, b}));
    std::string g1 = FindFirstGroupId(pic);
    h.Execute(std::make_unique<GroupCommand>(pic, std::vector<std::string>{g1, c}));
    std::string g2 = FindFirstGroupId(pic);

    std::stringstream ss;
    SaveService::Save(pic, ss);
    shapes::Picture restored = LoadService::Load(ss);

    ASSERT_EQ(restored.GetOrder().size(), 1u);
    const std::string& g2r = restored.GetOrder()[0];
    EXPECT_EQ(g2r, g2);
    EXPECT_EQ(restored.GetShape(g2r).GetType(), "group");

    auto outer = restored.GetShape(g2r).GetChildIds();
    ASSERT_EQ(outer.size(), 2u);

    std::string g1r;
    std::string ellipseId;
    for (const auto& id : outer)
    {
        const auto* child = FindChild(restored.GetShape(g2r), id);
        ASSERT_NE(child, nullptr);
        if (child->GetType() == "group") g1r = id;
        else ellipseId = id;
    }
    ASSERT_FALSE(g1r.empty());
    ASSERT_FALSE(ellipseId.empty());
    EXPECT_EQ(g1r, g1);
    EXPECT_EQ(ellipseId, c);

    const auto* g1Child = FindChild(restored.GetShape(g2r), g1r);
    ASSERT_NE(g1Child, nullptr);
    auto inner = g1Child->GetChildIds();
    ASSERT_EQ(inner.size(), 2u);
    EXPECT_EQ(inner[0], a);
    EXPECT_EQ(inner[1], b);

    const auto* childA = FindChild(*g1Child, a);
    ASSERT_NE(childA, nullptr);
    auto bA_after = childA->GetBounds();
    EXPECT_NEAR(bA_before.x,     bA_after.x,     1e-6);
    EXPECT_NEAR(bA_before.width, bA_after.width, 1e-6);
}

TEST(GroupTests, DeleteNestedGroupUndoRedo)
{
    shapes::Picture pic; CommandHistory h;
    auto a = AddRect(pic, h);
    auto b = AddRect(pic, h);
    auto c = AddRect(pic, h);

    h.Execute(std::make_unique<GroupCommand>(pic, std::vector<std::string>{a, b}));
    std::string g1 = FindFirstGroupId(pic);
    h.Execute(std::make_unique<GroupCommand>(pic, std::vector<std::string>{g1, c}));
    std::string g2 = FindFirstGroupId(pic);

    h.Execute(std::make_unique<DeleteCommand>(pic, std::vector<std::string>{g2}));
    EXPECT_EQ(pic.GetOrder().size(), 0u);

    h.Undo();
    ASSERT_EQ(pic.GetOrder().size(), 1u);
    EXPECT_TRUE(pic.HasShape(g2));

    auto outer = pic.GetShape(g2).GetChildIds();
    ASSERT_EQ(outer.size(), 2u);
    ASSERT_NE(std::find(outer.begin(), outer.end(), g1), outer.end());
    ASSERT_NE(std::find(outer.begin(), outer.end(), c),  outer.end());

    const auto* g1Child = FindChild(pic.GetShape(g2), g1);
    ASSERT_NE(g1Child, nullptr);
    auto inner = g1Child->GetChildIds();
    ASSERT_EQ(inner.size(), 2u);
    EXPECT_EQ(inner[0], a);
    EXPECT_EQ(inner[1], b);

    h.Redo();
    EXPECT_EQ(pic.GetOrder().size(), 0u);
}

TEST(ColorMacro, GroupColorAffectsAllChildren)
{
    shapes::Picture pic; CommandHistory h;
    auto a = AddRect(pic, h, {0, 0, 10, 10});
    auto b = AddRect(pic, h, {20, 0, 10, 10});
    h.Execute(std::make_unique<GroupCommand>(pic, std::vector<std::string>{a, b}));
    std::string gId = FindFirstGroupId(pic);

    gfx::Color blue(0, 0, 255, 255);
    h.Execute(std::make_unique<ColorCommand>(
        pic, std::vector<std::string>{gId}, blue, shapes::ColorTarget::Fill));

    const auto& group = pic.GetShape(gId);
    EXPECT_EQ(FindChild(group, a)->GetFillColor(), blue);
    EXPECT_EQ(FindChild(group, b)->GetFillColor(), blue);
}

TEST(ColorMacro, UndoRestoresGroupColor)
{
    shapes::Picture pic; CommandHistory h;
    auto a = AddRect(pic, h, {0, 0, 10, 10});
    auto b = AddRect(pic, h, {20, 0, 10, 10});
    h.Execute(std::make_unique<GroupCommand>(pic, std::vector<std::string>{a, b}));
    std::string gId = FindFirstGroupId(pic);

    gfx::Color oldGroupColor = pic.GetShape(gId).GetFillColor();
    gfx::Color blue(0, 0, 255, 255);

    h.Execute(std::make_unique<ColorCommand>(
        pic, std::vector<std::string>{gId}, blue, shapes::ColorTarget::Fill));
    h.Undo();

    const auto& group = pic.GetShape(gId);
    EXPECT_EQ(FindChild(group, a)->GetFillColor(), oldGroupColor);
    EXPECT_EQ(FindChild(group, b)->GetFillColor(), oldGroupColor);
}

TEST(ColorMacro, RedoReappliesGroupColor)
{
    shapes::Picture pic; CommandHistory h;
    auto a = AddRect(pic, h, {0, 0, 10, 10});
    auto b = AddRect(pic, h, {20, 0, 10, 10});
    h.Execute(std::make_unique<GroupCommand>(pic, std::vector<std::string>{a, b}));
    std::string gId = FindFirstGroupId(pic);

    gfx::Color blue(0, 0, 255, 255);
    h.Execute(std::make_unique<ColorCommand>(
        pic, std::vector<std::string>{gId}, blue, shapes::ColorTarget::Fill));
    h.Undo();
    h.Redo();

    const auto& group = pic.GetShape(gId);
    EXPECT_EQ(FindChild(group, a)->GetFillColor(), blue);
    EXPECT_EQ(FindChild(group, b)->GetFillColor(), blue);
}

TEST(ColorMacro, NestedGroupRecursive)
{
    shapes::Picture pic; CommandHistory h;
    auto a = AddRect(pic, h, {0, 0, 10, 10});
    auto b = AddRect(pic, h, {20, 0, 10, 10});
    auto c = AddRect(pic, h, {50, 0, 10, 10});
    h.Execute(std::make_unique<GroupCommand>(pic, std::vector<std::string>{a, b}));
    std::string g1 = FindFirstGroupId(pic);
    h.Execute(std::make_unique<GroupCommand>(pic, std::vector<std::string>{g1, c}));
    std::string g2 = FindFirstGroupId(pic);

    gfx::Color blue(0, 0, 255, 255);
    h.Execute(std::make_unique<ColorCommand>(
        pic, std::vector<std::string>{g2}, blue, shapes::ColorTarget::Fill));

    const auto& g2ref = pic.GetShape(g2);
    const auto* g1child = FindChild(g2ref, g1);
    ASSERT_NE(g1child, nullptr);
    EXPECT_EQ(FindChild(*g1child, a)->GetFillColor(), blue);
    EXPECT_EQ(FindChild(*g1child, b)->GetFillColor(), blue);
    EXPECT_EQ(FindChild(g2ref, c)->GetFillColor(), blue);

    h.Undo();
    const auto& g2after = pic.GetShape(g2);
    EXPECT_NE(FindChild(*FindChild(g2after, g1), a)->GetFillColor(), blue);
}

TEST(ColorMacro, SaveLoadPreservesChildColors)
{
    shapes::Picture pic; CommandHistory h;
    auto a = AddRect(pic, h, {0, 0, 10, 10});
    auto b = AddRect(pic, h, {20, 0, 10, 10});
    h.Execute(std::make_unique<GroupCommand>(pic, std::vector<std::string>{a, b}));
    std::string gId = FindFirstGroupId(pic);

    gfx::Color blue(0, 0, 255, 255);
    h.Execute(std::make_unique<ColorCommand>(
        pic, std::vector<std::string>{gId}, blue, shapes::ColorTarget::Fill));

    std::stringstream ss;
    SaveService::Save(pic, ss);
    auto restored = LoadService::Load(ss);

    const auto& g = restored.GetShape(gId);
    EXPECT_EQ(FindChild(g, a)->GetFillColor(), blue);
    EXPECT_EQ(FindChild(g, b)->GetFillColor(), blue);
}