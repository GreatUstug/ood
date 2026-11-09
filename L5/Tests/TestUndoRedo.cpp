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
	//EXPECT_FALSE(h.Redo());
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

    h.Execute(std::make_unique<ResizeCommand>(
        pic, a, oldB, newB));

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
	EXPECT_EQ(pic.GetShape(c).GetFillColor(), oldA); // c не тронута

	h.Undo();
	EXPECT_EQ(pic.GetShape(a).GetFillColor(), oldA);
	EXPECT_EQ(pic.GetShape(b).GetFillColor(), oldB);
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

	ASSERT_EQ(pic.GetOrder().size(), 3);
	EXPECT_EQ(pic.GetOrder()[0], a);
	EXPECT_EQ(pic.GetOrder()[1], gId); // Группа заняла место B (самого нижнего)
	EXPECT_EQ(pic.GetOrder()[2], c);

	auto children = pic.GetShape(gId).GetChildIds();
	ASSERT_EQ(children.size(), 2);
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

	ASSERT_EQ(pic.GetOrder().size(), 3);
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

	h.Execute(std::make_unique<UngroupCommand>(pic, gId));
	ASSERT_EQ(pic.GetOrder().size(), 3);
	EXPECT_EQ(pic.GetOrder()[0], a);
	EXPECT_EQ(pic.GetOrder()[1], b);
	EXPECT_EQ(pic.GetOrder()[2], c);

	h.Undo();
	ASSERT_EQ(pic.GetOrder().size(), 2);
	EXPECT_TRUE(pic.HasShape(gId));

	h.Redo();
	ASSERT_EQ(pic.GetOrder().size(), 3);
	EXPECT_FALSE(pic.HasShape(gId));
}

TEST(GroupTests, MoveGroupMovesChildren)
{
	shapes::Picture pic; CommandHistory h;
	auto a = AddRect(pic, h, {10, 10, 10, 10});
	auto b = AddRect(pic, h, {50, 50, 10, 10});

	h.Execute(std::make_unique<GroupCommand>(pic, std::vector<std::string>{a, b}));
	std::string gId = FindFirstGroupId(pic);

	auto beforeA = pic.GetShape(a).GetBounds();
	auto beforeB = pic.GetShape(b).GetBounds();

	h.Execute(std::make_unique<MoveShapesCommand>(
		pic, std::vector<std::string>{gId}, 100.0, 50.0));

	auto afterA = pic.GetShape(a).GetBounds();
	auto afterB = pic.GetShape(b).GetBounds();

	EXPECT_NEAR(afterA.x, beforeA.x + 100.0, 1e-9);
	EXPECT_NEAR(afterA.y, beforeA.y + 50.0, 1e-9);
	EXPECT_NEAR(afterB.x, beforeB.x + 100.0, 1e-9);
	EXPECT_NEAR(afterB.y, beforeB.y + 50.0, 1e-9);
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

	std::string g2 = "";
	for (const auto& id : pic.GetOrder()) {
		if (pic.GetShape(id).GetType() == "group") {
			auto children = pic.GetShape(id).GetChildIds();
			if (std::find(children.begin(), children.end(), g1) != children.end()) {
				g2 = id;
				break;
			}
		}
	}
	ASSERT_FALSE(g2.empty());

	shapes::Bounds oldB = pic.GetShape(g2).GetBounds();
	shapes::Bounds newB = {oldB.x, oldB.y, oldB.width * 2.0, oldB.height * 2.0};

	h.Execute(std::make_unique<ResizeCommand>(pic, g2, oldB, newB));

	auto bA = pic.GetShape(a).GetBounds();
	EXPECT_NEAR(bA.width, 20.0, 1e-9);

	h.Undo();
	EXPECT_NEAR(pic.GetShape(a).GetBounds().width, 10.0, 1e-9);
}

TEST(GroupTests, DeleteGroupAndUndoRedo)
{
	shapes::Picture pic; CommandHistory h;
	auto a = AddRect(pic, h);
	auto b = AddRect(pic, h);

	h.Execute(std::make_unique<GroupCommand>(pic, std::vector<std::string>{a, b}));
	std::string gId = FindFirstGroupId(pic);

	h.Execute(std::make_unique<DeleteCommand>(pic, std::vector<std::string>{gId}));
	EXPECT_FALSE(pic.HasShape(gId));
	EXPECT_FALSE(pic.HasShape(a));
	EXPECT_EQ(pic.GetOrder().size(), 0);

	h.Undo();
	EXPECT_TRUE(pic.HasShape(gId));
	EXPECT_TRUE(pic.HasShape(a));
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
	shapes::Bounds newB = { bGroup.x, bGroup.y, bGroup.width + 100.0, bGroup.height + 100.0 };
	h.Execute(std::make_unique<ResizeCommand>(pic, gId, bGroup, newB));

	h.Undo();
	h.Undo();
	h.Undo();
	ASSERT_FALSE(pic.HasShape(gId));
	ASSERT_EQ(pic.GetOrder().size(), 2);

	h.Redo();
	h.Redo();
	h.Redo();
	ASSERT_TRUE(pic.HasShape(gId));
}
#endif //L4_TESTUNDOREDO_H