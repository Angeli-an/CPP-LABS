#include "../include/ass.h"
#include "../include/sami.h"
#include "../include/subrip.h"
#include "../include/vtt.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

class SubtitleTest : public ::testing::Test
{
  protected:
	void SetUp() override { createTestFiles(); }

	void TearDown() override
	{
		for (const auto& file : testFiles)
		{
			if (fs::exists(file))
			{
				fs::remove(file);
			}
		}
	}

	void createTestFiles()
	{
		writeTempFile("test1.srt", R"(1
00:00:01,000 --> 00:00:03,000
Hello world!

2
00:00:04,000 --> 00:00:06,000
This is a <b>test</b>)");

		writeTempFile("test2.srt", R"(1
00:01:00,000 --> 00:01:05,000
Line 1

2
00:01:03,000 --> 00:01:07,000
Line 2

3
00:01:08,000 --> 00:01:10,000
Line 3)");

		writeTempFile("empty.srt", "");
		writeTempFile("test1.smi", R"(<SAMI>
<BODY>
<SYNC Start=1000 End=3000><P>Hello world!</P></SYNC>
<SYNC Start=4000 End=6000><P>This is a <b>test</b></P></SYNC>
</BODY>
</SAMI>)");

		writeTempFile("test2.smi", R"(<SAMI>
<BODY>
<SYNC Start=60000 End=65000><P>Line 1</P></SYNC>
<SYNC Start=63000 End=67000><P>Line 2</P></SYNC>
<SYNC Start=68000 End=70000><P>Line 3</P></SYNC>
</BODY>
</SAMI>)");

		writeTempFile("empty.smi", "<SAMI></SAMI>");
		writeTempFile("test1.ass", R"([Script Info]
Title: Test

[Events]
Format: Layer, Start, End, Style, Name, MarginL, MarginR, MarginV, Effect, Text
Dialogue: 0,0:00:01.00,0:00:03.00,Default,,0,0,0,,Hello world!
Dialogue: 0,0:00:04.00,0:00:06.00,Default,,0,0,0,,This is a {b}test{/b})");

		writeTempFile("test2.ass", R"([Script Info]
Title: Test

[Events]
Format: Layer, Start, End, Style, Name, MarginL, MarginR, MarginV, Effect, Text
Dialogue: 0,0:01:00.00,0:01:05.00,Default,,0,0,0,,Line 1
Dialogue: 0,0:01:03.00,0:01:07.00,Default,,0,0,0,,Line 2
Dialogue: 0,0:01:08.00,0:01:10.00,Default,,0,0,0,,Line 3)");

		writeTempFile("empty.ass", "[Script Info]");
		writeTempFile("test1.vtt", R"(WEBVTT

00:00:01.000 --> 00:00:03.000
Hello world!

00:00:04.000 --> 00:00:06.000
This is a <b>test</b>)");

		writeTempFile("test2.vtt", R"(WEBVTT

00:01:00.000 --> 00:01:05.000
Line 1

00:01:03.000 --> 00:01:07.000
Line 2

00:01:08.000 --> 00:01:10.000
Line 3)");

		writeTempFile("empty.vtt", "WEBVTT");
	}

	void writeTempFile(const std::string& filename, const std::string& content)
	{
		std::ofstream file(filename);
		file << content;
		file.close();
		testFiles.push_back(filename);
	}

	std::vector< std::string > testFiles;
};

bool compareFiles(const std::string& p1, const std::string& p2)
{
	std::ifstream f1(p1, std::ifstream::binary | std::ifstream::ate);
	std::ifstream f2(p2, std::ifstream::binary | std::ifstream::ate);

	if (f1.fail() || f2.fail() || f1.tellg() != f2.tellg())
	{
		return false;
	}

	f1.seekg(0, std::ifstream::beg);
	f2.seekg(0, std::ifstream::beg);
	return std::equal(
		std::istreambuf_iterator< char >(f1.rdbuf()),
		std::istreambuf_iterator< char >(),
		std::istreambuf_iterator< char >(f2.rdbuf()));
}

TEST_F(SubtitleTest, SubRipReadBasic)
{
	SubRip sub;
	ASSERT_TRUE(sub.readFromFile("test1.srt"));
	ASSERT_EQ(sub.size(), 2);
	EXPECT_EQ(sub.get(0).startTime, 1000);
	EXPECT_EQ(sub.get(0).endTime, 3000);
	EXPECT_EQ(sub.get(0).text, "Hello world!");
	EXPECT_EQ(sub.get(1).text, "This is a <b>test</b>");
}

TEST_F(SubtitleTest, SubRipReadEmpty)
{
	SubRip sub;
	ASSERT_TRUE(sub.readFromFile("empty.srt"));
	EXPECT_EQ(sub.size(), 0);
}

TEST_F(SubtitleTest, SubRipWriteBasic)
{
	SubRip sub;
	ASSERT_TRUE(sub.readFromFile("test1.srt"));
	ASSERT_TRUE(sub.writeToFile("output.srt"));
	testFiles.push_back("output.srt");

	SubRip sub2;
	ASSERT_TRUE(sub2.readFromFile("output.srt"));
	ASSERT_EQ(sub2.size(), 2);
	EXPECT_EQ(sub2.get(0).startTime, 1000);
	EXPECT_EQ(sub2.get(0).endTime, 3000);
	EXPECT_EQ(sub2.get(0).text, "Hello world!");
}

TEST_F(SubtitleTest, SubRipRemoveFormatting)
{
	SubRip sub;
	ASSERT_TRUE(sub.readFromFile("test1.srt"));
	sub.removeFormatting();
	EXPECT_EQ(sub.get(1).text, "This is a test");
}

TEST_F(SubtitleTest, SubRipShiftTime)
{
	SubRip sub;
	ASSERT_TRUE(sub.readFromFile("test1.srt"));
	sub.shiftTime(1000, true, true);
	EXPECT_EQ(sub.get(0).startTime, 2000);
	EXPECT_EQ(sub.get(0).endTime, 4000);
}

TEST_F(SubtitleTest, SubRipShiftOnlyStart)
{
	SubRip sub;
	ASSERT_TRUE(sub.readFromFile("test1.srt"));
	sub.shiftTime(1000, true, false);
	EXPECT_EQ(sub.get(0).startTime, 2000);
	EXPECT_EQ(sub.get(0).endTime, 3000);
}

TEST_F(SubtitleTest, SubRipShiftOnlyEnd)
{
	SubRip sub;
	ASSERT_TRUE(sub.readFromFile("test1.srt"));
	sub.shiftTime(1000, false, true);
	EXPECT_EQ(sub.get(0).startTime, 1000);
	EXPECT_EQ(sub.get(0).endTime, 4000);
}

TEST_F(SubtitleTest, SubRipShiftNegativeTime)
{
	SubRip sub;
	ASSERT_TRUE(sub.readFromFile("test1.srt"));
	sub.shiftTime(-500, true, true);
	EXPECT_EQ(sub.get(0).startTime, 500);
	EXPECT_EQ(sub.get(0).endTime, 2500);
}

TEST_F(SubtitleTest, SubRipFindCollisionsNone)
{
	SubRip sub;
	ASSERT_TRUE(sub.readFromFile("test1.srt"));
	std::unique_ptr< Subtitle > collisions(sub.findCollisions());
	EXPECT_EQ(collisions->size(), 0);
}

TEST_F(SubtitleTest, SubRipFindCollisions)
{
	SubRip sub;
	ASSERT_TRUE(sub.readFromFile("test2.srt"));
	std::unique_ptr< Subtitle > collisions(sub.findCollisions());
	EXPECT_GT(collisions->size(), 0);
}

TEST_F(SubtitleTest, SubRipAddDefaultStyle)
{
	SubRip sub;
	ASSERT_TRUE(sub.readFromFile("test1.srt"));
	sub.addDefaultStyle();
	EXPECT_EQ(sub.get(1).text, "This is a test");
}

TEST_F(SubtitleTest, SAMIReadBasic)
{
	SAMI sub;
	ASSERT_TRUE(sub.readFromFile("test1.smi"));
	ASSERT_EQ(sub.size(), 2);
	EXPECT_EQ(sub.get(0).startTime, 1000);
	EXPECT_EQ(sub.get(0).endTime, 3000);
	EXPECT_EQ(sub.get(0).text, "Hello world!");
	EXPECT_EQ(sub.get(1).text, "This is a test");
}

TEST_F(SubtitleTest, SAMIReadEmpty)
{
	SAMI sub;
	ASSERT_TRUE(sub.readFromFile("empty.smi"));
	EXPECT_EQ(sub.size(), 0);
}

TEST_F(SubtitleTest, SAMIWriteBasic)
{
	SAMI sub;
	ASSERT_TRUE(sub.readFromFile("test1.smi"));
	ASSERT_TRUE(sub.writeToFile("output.smi"));
	testFiles.push_back("output.smi");

	SAMI sub2;
	ASSERT_TRUE(sub2.readFromFile("output.smi"));
	ASSERT_EQ(sub2.size(), 2);
	EXPECT_EQ(sub2.get(0).startTime, 1000);
	EXPECT_EQ(sub2.get(0).endTime, 3000);
	EXPECT_EQ(sub2.get(0).text, "Hello world!");
}

TEST_F(SubtitleTest, SAMIRemoveFormatting)
{
	SAMI sub;
	ASSERT_TRUE(sub.readFromFile("test1.smi"));
	sub.removeFormatting();
	EXPECT_EQ(sub.get(1).text, "This is a test");
}

TEST_F(SubtitleTest, SAMIShiftTime)
{
	SAMI sub;
	ASSERT_TRUE(sub.readFromFile("test1.smi"));
	sub.shiftTime(1000, true, true);
	EXPECT_EQ(sub.get(0).startTime, 2000);
	EXPECT_EQ(sub.get(0).endTime, 4000);
}

TEST_F(SubtitleTest, SAMIShiftOnlyStart)
{
	SAMI sub;
	ASSERT_TRUE(sub.readFromFile("test1.smi"));
	sub.shiftTime(1000, true, false);
	EXPECT_EQ(sub.get(0).startTime, 2000);
	EXPECT_EQ(sub.get(0).endTime, 3000);
}

TEST_F(SubtitleTest, SAMIShiftOnlyEnd)
{
	SAMI sub;
	ASSERT_TRUE(sub.readFromFile("test1.smi"));
	sub.shiftTime(1000, false, true);
	EXPECT_EQ(sub.get(0).startTime, 1000);
	EXPECT_EQ(sub.get(0).endTime, 4000);
}

TEST_F(SubtitleTest, SAMIShiftNegativeTime)
{
	SAMI sub;
	ASSERT_TRUE(sub.readFromFile("test1.smi"));
	sub.shiftTime(-500, true, true);
	EXPECT_EQ(sub.get(0).startTime, 500);
	EXPECT_EQ(sub.get(0).endTime, 2500);
}

TEST_F(SubtitleTest, SAMIFindCollisionsNone)
{
	SAMI sub;
	ASSERT_TRUE(sub.readFromFile("test1.smi"));
	std::unique_ptr< Subtitle > collisions(sub.findCollisions());
	EXPECT_EQ(collisions->size(), 0);
}

TEST_F(SubtitleTest, SAMIFindCollisions)
{
	SAMI sub;
	ASSERT_TRUE(sub.readFromFile("test2.smi"));
	std::unique_ptr< Subtitle > collisions(sub.findCollisions());
	EXPECT_GT(collisions->size(), 0);
}

TEST_F(SubtitleTest, SAMIAddDefaultStyle)
{
	SAMI sub;
	ASSERT_TRUE(sub.readFromFile("test1.smi"));
	sub.addDefaultStyle();
	EXPECT_EQ(sub.get(1).text, "This is a test");
}

TEST_F(SubtitleTest, ASSReadBasic)
{
	ASS sub;
	ASSERT_TRUE(sub.readFromFile("test1.ass"));
	ASSERT_EQ(sub.size(), 2);
	EXPECT_EQ(sub.get(0).startTime, 1000);
	EXPECT_EQ(sub.get(0).endTime, 3000);
	EXPECT_EQ(sub.get(0).text, "Hello world!");
	EXPECT_EQ(sub.get(1).text, "This is a {b}test{/b}");
}

TEST_F(SubtitleTest, ASSReadEmpty)
{
	ASS sub;
	ASSERT_TRUE(sub.readFromFile("empty.ass"));
	EXPECT_EQ(sub.size(), 0);
}

TEST_F(SubtitleTest, ASSWriteBasic)
{
	ASS sub;
	ASSERT_TRUE(sub.readFromFile("test1.ass"));
	ASSERT_TRUE(sub.writeToFile("output.ass"));
	testFiles.push_back("output.ass");

	ASS sub2;
	ASSERT_TRUE(sub2.readFromFile("output.ass"));
	ASSERT_EQ(sub2.size(), 2);
	EXPECT_EQ(sub2.get(0).startTime, 1000);
	EXPECT_EQ(sub2.get(0).endTime, 3000);
	EXPECT_EQ(sub2.get(0).text, "Hello world!");
}

TEST_F(SubtitleTest, ASSRemoveFormatting)
{
	ASS sub;
	ASSERT_TRUE(sub.readFromFile("test1.ass"));
	sub.removeFormatting();
	EXPECT_EQ(sub.get(1).text, "This is a test");
}

TEST_F(SubtitleTest, ASSShiftTime)
{
	ASS sub;
	ASSERT_TRUE(sub.readFromFile("test1.ass"));
	sub.shiftTime(1000, true, true);
	EXPECT_EQ(sub.get(0).startTime, 2000);
	EXPECT_EQ(sub.get(0).endTime, 4000);
}

TEST_F(SubtitleTest, ASSShiftOnlyStart)
{
	ASS sub;
	ASSERT_TRUE(sub.readFromFile("test1.ass"));
	sub.shiftTime(1000, true, false);
	EXPECT_EQ(sub.get(0).startTime, 2000);
	EXPECT_EQ(sub.get(0).endTime, 3000);
}

TEST_F(SubtitleTest, ASSShiftOnlyEnd)
{
	ASS sub;
	ASSERT_TRUE(sub.readFromFile("test1.ass"));
	sub.shiftTime(1000, false, true);
	EXPECT_EQ(sub.get(0).startTime, 1000);
	EXPECT_EQ(sub.get(0).endTime, 4000);
}

TEST_F(SubtitleTest, ASSShiftNegativeTime)
{
	ASS sub;
	ASSERT_TRUE(sub.readFromFile("test1.ass"));
	sub.shiftTime(-500, true, true);
	EXPECT_EQ(sub.get(0).startTime, 500);
	EXPECT_EQ(sub.get(0).endTime, 2500);
}

TEST_F(SubtitleTest, ASSFindCollisionsNone)
{
	ASS sub;
	ASSERT_TRUE(sub.readFromFile("test1.ass"));
	std::unique_ptr< Subtitle > collisions(sub.findCollisions());
	EXPECT_EQ(collisions->size(), 0);
}

TEST_F(SubtitleTest, ASSFindCollisions)
{
	ASS sub;
	ASSERT_TRUE(sub.readFromFile("test2.ass"));
	std::unique_ptr< Subtitle > collisions(sub.findCollisions());
	EXPECT_GT(collisions->size(), 0);
}

TEST_F(SubtitleTest, ASSAddDefaultStyle)
{
	ASS sub;
	ASSERT_TRUE(sub.readFromFile("test1.ass"));
	sub.addDefaultStyle();
	EXPECT_EQ(sub.get(1).text, "This is a test");
}

TEST_F(SubtitleTest, VTTReadBasic)
{
	VTT sub;
	ASSERT_TRUE(sub.readFromFile("test1.vtt"));
	ASSERT_EQ(sub.size(), 2);
	EXPECT_EQ(sub.get(0).startTime, 1000);
	EXPECT_EQ(sub.get(0).endTime, 3000);
	EXPECT_EQ(sub.get(0).text, "Hello world!");
	EXPECT_EQ(sub.get(1).text, "This is a <b>test</b>");
}

TEST_F(SubtitleTest, VTTReadEmpty)
{
	VTT sub;
	ASSERT_TRUE(sub.readFromFile("empty.vtt"));
	EXPECT_EQ(sub.size(), 0);
}

TEST_F(SubtitleTest, VTTWriteBasic)
{
	VTT sub;
	ASSERT_TRUE(sub.readFromFile("test1.vtt"));
	ASSERT_TRUE(sub.writeToFile("output.vtt"));
	testFiles.push_back("output.vtt");

	VTT sub2;
	ASSERT_TRUE(sub2.readFromFile("output.vtt"));
	ASSERT_EQ(sub2.size(), 2);
	EXPECT_EQ(sub2.get(0).startTime, 1000);
	EXPECT_EQ(sub2.get(0).endTime, 3000);
	EXPECT_EQ(sub2.get(0).text, "Hello world!");
}

TEST_F(SubtitleTest, VTTRemoveFormatting)
{
	VTT sub;
	ASSERT_TRUE(sub.readFromFile("test1.vtt"));
	sub.removeFormatting();
	EXPECT_EQ(sub.get(1).text, "This is a test");
}

TEST_F(SubtitleTest, VTTShiftTime)
{
	VTT sub;
	ASSERT_TRUE(sub.readFromFile("test1.vtt"));
	sub.shiftTime(1000, true, true);
	EXPECT_EQ(sub.get(0).startTime, 2000);
	EXPECT_EQ(sub.get(0).endTime, 4000);
}

TEST_F(SubtitleTest, VTTShiftOnlyStart)
{
	VTT sub;
	ASSERT_TRUE(sub.readFromFile("test1.vtt"));
	sub.shiftTime(1000, true, false);
	EXPECT_EQ(sub.get(0).startTime, 2000);
	EXPECT_EQ(sub.get(0).endTime, 3000);
}

TEST_F(SubtitleTest, VTTShiftOnlyEnd)
{
	VTT sub;
	ASSERT_TRUE(sub.readFromFile("test1.vtt"));
	sub.shiftTime(1000, false, true);
	EXPECT_EQ(sub.get(0).startTime, 1000);
	EXPECT_EQ(sub.get(0).endTime, 4000);
}

TEST_F(SubtitleTest, VTTShiftNegativeTime)
{
	VTT sub;
	ASSERT_TRUE(sub.readFromFile("test1.vtt"));
	sub.shiftTime(-500, true, true);
	EXPECT_EQ(sub.get(0).startTime, 500);
	EXPECT_EQ(sub.get(0).endTime, 2500);
}

TEST_F(SubtitleTest, VTTFindCollisionsNone)
{
	VTT sub;
	ASSERT_TRUE(sub.readFromFile("test1.vtt"));
	std::unique_ptr< Subtitle > collisions(sub.findCollisions());
	EXPECT_EQ(collisions->size(), 0);
}

TEST_F(SubtitleTest, VTTFindCollisions)
{
	VTT sub;
	ASSERT_TRUE(sub.readFromFile("test2.vtt"));
	std::unique_ptr< Subtitle > collisions(sub.findCollisions());
	EXPECT_GT(collisions->size(), 0);
}

TEST_F(SubtitleTest, VTTAddDefaultStyle)
{
	VTT sub;
	ASSERT_TRUE(sub.readFromFile("test1.vtt"));
	sub.addDefaultStyle();
	EXPECT_EQ(sub.get(1).text, "This is a test");
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
