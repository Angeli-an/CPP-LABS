#include "../include/cellbutton.h"
#include "../include/minesweeper.h"
#include "../include/newgamedialog.h"

#include <gtest/gtest.h>

#include <QApplication>
#include <QSignalSpy>
#include <QTest>

class CellButtonTest : public ::testing::Test
{
  protected:
	CellButton *btn;
	void SetUp() override { btn = new CellButton(); }
	void TearDown() override { delete btn; }
};

TEST_F(CellButtonTest, DefaultState_NotFlaggedAndDisabled)
{
	EXPECT_FALSE(btn->isFlagged());
	EXPECT_FALSE(btn->isEnabledCustom());
}

TEST_F(CellButtonTest, RightClick_TogglesFlag)
{
	QTest::mouseClick(btn, Qt::RightButton);
	EXPECT_TRUE(btn->isFlagged());
	QTest::mouseClick(btn, Qt::RightButton);
	EXPECT_FALSE(btn->isFlagged());
}

TEST_F(CellButtonTest, MiddleClick_EmitsSignal)
{
	QSignalSpy spy(btn, &CellButton::middleClicked);
	QTest::mouseClick(btn, Qt::MiddleButton);
	EXPECT_EQ(spy.count(), 1);
}

TEST_F(CellButtonTest, LeftClick_NotFlagged_PassesThrough)
{
	btn->setText("X");
	QTest::mouseClick(btn, Qt::LeftButton);
	EXPECT_EQ(btn->text(), "X");
}

TEST_F(CellButtonTest, RightClick_FlagSetsText)
{
	QTest::mouseClick(btn, Qt::RightButton);
	EXPECT_EQ(btn->text(), "|=");
}

TEST_F(CellButtonTest, GetState_WhenFlagged_ReturnsF)
{
	QTest::mouseClick(btn, Qt::RightButton);
	EXPECT_EQ(btn->getState(), "F");
}

TEST_F(CellButtonTest, GetState_WhenEnabledEmpty_ReturnsN)
{
	btn->enabled = true;
	btn->setText("");
	EXPECT_EQ(btn->getState(), "N");
}

TEST_F(CellButtonTest, GetState_Default_ReturnsNO)
{
	EXPECT_EQ(btn->getState(), "NO");
}

TEST_F(CellButtonTest, DoubleRightClick_RestoresOldText)
{
	btn->setText("X");
	QTest::mouseClick(btn, Qt::RightButton);
	QTest::mouseClick(btn, Qt::RightButton);
	EXPECT_EQ(btn->text(), "X");
}

TEST_F(CellButtonTest, Flagged_MiddleClick_DoesNotEmit)
{
	QTest::mouseClick(btn, Qt::RightButton);
	QSignalSpy spy(btn, &CellButton::middleClicked);
	QTest::mouseClick(btn, Qt::MiddleButton);
	EXPECT_EQ(spy.count(), 0);
}

TEST_F(CellButtonTest, LeftClick_WhenFlagged_Ignored)
{
	QTest::mouseClick(btn, Qt::RightButton);
	QString prev = btn->text();
	QTest::mouseClick(btn, Qt::LeftButton);
	EXPECT_EQ(btn->text(), prev);
}

TEST_F(CellButtonTest, Flagged_TogglesCorrectlyMultipleTimes)
{
	for (int i = 0; i < 10; i++)
	{
		QTest::mouseClick(btn, Qt::RightButton);
	}
	EXPECT_FALSE(btn->isFlagged());
}

TEST_F(CellButtonTest, InitialStyleSheetSet)
{
	EXPECT_TRUE(btn->styleSheet().contains("font-size: 20px"));
}

TEST_F(CellButtonTest, MinSizeIsSet)
{
	EXPECT_EQ(btn->minimumSize(), QSize(31, 31));
}

class NewGameDialogTest : public ::testing::Test
{
  protected:
	NewGameDialog *dlg;
	void SetUp() override { dlg = new NewGameDialog(); }
	void TearDown() override { delete dlg; }
};

TEST_F(NewGameDialogTest, DefaultValues)
{
	EXPECT_EQ(dlg->rows(), 10);
	EXPECT_EQ(dlg->cols(), 10);
	EXPECT_EQ(dlg->mines(), 10);
}

TEST_F(NewGameDialogTest, SetRussianTexts)
{
	dlg->setT(true);
	EXPECT_EQ(dlg->rowsSpinBox->value(), 10);
}

TEST_F(NewGameDialogTest, SetEnglishTexts)
{
	dlg->setT(false);
	EXPECT_EQ(dlg->colsSpinBox->value(), 10);
}

TEST_F(NewGameDialogTest, TooManyMines_DisablesOk)
{
	dlg->rowsSpinBox->setValue(2);
	dlg->colsSpinBox->setValue(2);
	dlg->minesSpinBox->setValue(10);
	auto okBtn = dlg->findChild< QDialogButtonBox * >()->button(QDialogButtonBox::Ok);
	EXPECT_FALSE(okBtn->isEnabled());
}

TEST_F(NewGameDialogTest, ValidInput_EnablesOk)
{
	dlg->rowsSpinBox->setValue(5);
	dlg->colsSpinBox->setValue(5);
	dlg->minesSpinBox->setValue(10);
	auto okBtn = dlg->findChild< QDialogButtonBox * >()->button(QDialogButtonBox::Ok);
	EXPECT_TRUE(okBtn->isEnabled());
}

TEST_F(NewGameDialogTest, RowsSpinBoxRange)
{
	EXPECT_EQ(dlg->rowsSpinBox->minimum(), 2);
	EXPECT_EQ(dlg->rowsSpinBox->maximum(), 100);
}

TEST_F(NewGameDialogTest, ColsSpinBoxRange)
{
	EXPECT_EQ(dlg->colsSpinBox->minimum(), 2);
	EXPECT_EQ(dlg->colsSpinBox->maximum(), 100);
}

TEST_F(NewGameDialogTest, MinesSpinBoxRange)
{
	EXPECT_EQ(dlg->minesSpinBox->minimum(), 1);
	EXPECT_EQ(dlg->minesSpinBox->maximum(), 10000);
}

class MinesweeperTest : public ::testing::Test
{
  protected:
	minesweeper *game;
	void SetUp() override { game = new minesweeper(nullptr, true); }
	void TearDown() override { delete game; }
};

TEST_F(MinesweeperTest, InitialBoardIsAllocated)
{
	EXPECT_GT(game->rows, 0);
	EXPECT_GT(game->cols, 0);
}

TEST_F(MinesweeperTest, SwitchToEnglishSetsFlag)
{
	game->switchToEnglish();
	EXPECT_TRUE(game->r_ing);
}

TEST_F(MinesweeperTest, SwitchToRussianSetsFlag)
{
	game->switchToRussian();
	EXPECT_FALSE(game->r_ing);
}

TEST_F(MinesweeperTest, DebugModeToggle_RevealsMines)
{
	game->distributeMines(0, 0);
	game->toggleDebugMode(Qt::Checked);
	bool found = false;
	for (int i = 0; i < game->rows; i++)
		for (int j = 0; j < game->cols; j++)
		{
			auto btn = qobject_cast< CellButton * >(game->gridLayout->itemAtPosition(i, j)->widget());
			if (btn && btn->text() == "M")
				found = true;
		}
	EXPECT_TRUE(found);
}

TEST_F(MinesweeperTest, CntMineAroundReturnsCorrect)
{
	game->distributeMines(0, 0);
	int around = game->cntMineAround(0, 0);
	EXPECT_GE(around, 0);
	EXPECT_LE(around, 8);
}

TEST_F(MinesweeperTest, OpenLakeRevealsCell)
{
	game->distributeMines(0, 0);
	game->openLake(1, 1);
	auto btn = qobject_cast< CellButton * >(game->gridLayout->itemAtPosition(1, 1)->widget());
	EXPECT_TRUE(btn->isEnabledCustom());
}

TEST_F(MinesweeperTest, ClearBoardRemovesWidgets)
{
	game->clearBoard();
	EXPECT_EQ(game->gridLayout, nullptr);
}

TEST_F(MinesweeperTest, StartLastParamsGameCreatesBoard)
{
	game->startLastParamsGame();
	EXPECT_NE(game->gridLayout, nullptr);
}

TEST_F(MinesweeperTest, DistributeMinesPlacesCorrectCount)
{
	game->rows = 5;
	game->cols = 5;
	game->mines = 5;
	game->distributeMines(0, 0);
	int cnt = 0;
	for (auto &row : game->board)
		for (int c : row)
			if (c == -1)
				cnt++;
	EXPECT_EQ(cnt, 5);
}

TEST_F(MinesweeperTest, EndGameDisablesAllButtons)
{
	game->rows = 3;
	game->cols = 3;
	game->mines = 1;
	game->createNewBoard();
	game->distributeMines(0, 0);
	game->endGame(0, 0, 0);
	bool allDisabled = true;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		{
			auto btn = qobject_cast< QPushButton * >(game->gridLayout->itemAtPosition(i, j)->widget());
			if (btn->isEnabled())
				allDisabled = false;
		}
	EXPECT_TRUE(allDisabled);
}

TEST_F(MinesweeperTest, HandleButtonClickFirstClickGeneratesMines)
{
	auto btn = qobject_cast< QPushButton * >(game->gridLayout->itemAtPosition(0, 0)->widget());
	QTest::mouseClick(btn, Qt::LeftButton);
	EXPECT_FALSE(game->firstClick);
}

TEST_F(MinesweeperTest, FlaggedCellsCountedCorrectly)
{
	auto btn = qobject_cast< CellButton * >(game->gridLayout->itemAtPosition(0, 0)->widget());
	QTest::mouseClick(btn, Qt::RightButton);
	EXPECT_EQ(game->cntFlaggedAround(0, 0), 1);
}

TEST_F(MinesweeperTest, OpenCellsAroundRevealsNeighbors)
{
	game->distributeMines(0, 0);
	game->openLake(1, 1);
	game->openCellsAround(1, 1);
	auto btn = qobject_cast< CellButton * >(game->gridLayout->itemAtPosition(0, 0)->widget());
	EXPECT_TRUE(btn->isEnabledCustom() || btn->text() == "M" || !btn->isEnabled());
}

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
