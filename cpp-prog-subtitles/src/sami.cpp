#include "../include/sami.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <regex>

static std::string stripTags(const std::string& text)
{
	std::regex tagRegex("<[^>]*>");
	return std::regex_replace(text, tagRegex, "");
}

static long long parseTimeValue(const std::string& value)
{
	try
	{
		return std::stoll(value);
	} catch (...)
	{
		return 0;
	}
}

bool SAMI::readFromFile(const std::string& filename)
{
	clear();
	std::ifstream file(filename);
	if (!file.is_open())
		return false;

	std::string line;
	bool inBody = false;

	while (std::getline(file, line))
	{
		if (line.find("<BODY>") != std::string::npos)
		{
			inBody = true;
			continue;
		}
		if (line.find("</BODY>") != std::string::npos)
		{
			inBody = false;
			continue;
		}

		if (inBody && line.find("<SYNC") != std::string::npos)
		{
			SubtitleLine subtitleLine;
			std::string timeValue;

			size_t startPos = line.find("Start=");
			if (startPos != std::string::npos)
			{
				startPos += 6;
				size_t endPos = line.find_first_of(" >\"", startPos);
				if (endPos != std::string::npos)
				{
					timeValue = line.substr(startPos, endPos - startPos);
					subtitleLine.startTime = parseTimeValue(timeValue);
				}
			}

			size_t endTimePos = line.find("End=");
			if (endTimePos != std::string::npos)
			{
				endTimePos += 4;
				size_t endPos = line.find_first_of(" >\"", endTimePos);
				if (endPos != std::string::npos)
				{
					timeValue = line.substr(endTimePos, endPos - endTimePos);
					subtitleLine.endTime = parseTimeValue(timeValue);
				}
			}
			else
			{
				subtitleLine.endTime = subtitleLine.startTime + 3000;
			}

			size_t textStart = line.find("<P>");
			if (textStart != std::string::npos)
			{
				size_t textEnd = line.find("</P>", textStart);
				if (textEnd != std::string::npos)
				{
					subtitleLine.text = line.substr(textStart + 3, textEnd - textStart - 3);
				}
				else
				{
					subtitleLine.text = line.substr(textStart + 3);
				}
				subtitleLine.text = stripTags(subtitleLine.text);
			}

			if (subtitleLine.startTime > 0)
			{
				lines.push_back(subtitleLine);
			}
		}
	}

	return true;
}

bool SAMI::writeToFile(const std::string& filename) const
{
	std::ofstream file(filename);
	if (!file.is_open())
		return false;

	file << "<SAMI>\n<HEAD>\n<TITLE>Converted Subtitles</TITLE>\n</HEAD>\n<BODY>\n";

	for (int i = 0; i < lines.size(); i++)
	{
		const auto& line = lines[i];
		file << "<SYNC Start=" << line.startTime << " End=" << line.endTime << ">";
		file << "<P>" << line.text << "</P>";
		file << "</SYNC>\n";
	}

	file << "</BODY>\n</SAMI>\n";
	return true;
}

void SAMI::removeFormatting()
{
	for (int i = 0; i < lines.size(); i++)
	{
		lines[i].text = stripTags(lines[i].text);
	}
}

void SAMI::addDefaultStyle()
{
	removeFormatting();
}

void SAMI::shiftTime(long long deltaMs, bool shiftStart, bool shiftEnd)
{
	for (int i = 0; i < lines.size(); i++)
	{
		if (shiftStart)
			lines[i].startTime += deltaMs;
		if (shiftEnd)
			lines[i].endTime += deltaMs;
	}
}

Subtitle* SAMI::findCollisions() const
{
	SAMI* result = new SAMI();

	for (int i = 0; i < lines.size(); i++)
	{
		for (int j = i + 1; j < lines.size(); j++)
		{
			const auto& line1 = lines[i];
			const auto& line2 = lines[j];

			bool collision =
				(line1.startTime < line2.endTime && line1.endTime > line2.startTime) ||
				(line2.startTime < line1.endTime && line2.endTime > line1.startTime);

			if (collision)
			{
				result->addLine(line1);
				result->addLine(line2);
			}
		}
	}

	return result;
}
