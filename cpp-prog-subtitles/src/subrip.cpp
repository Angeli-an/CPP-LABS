#include "../include/subrip.h"

#include <fstream>
#include <regex>
#include <sstream>

static long long parseSrtTime(const std::string& timeStr)
{
	int h, m, s, ms;
	if (sscanf(timeStr.c_str(), "%d:%d:%d,%d", &h, &m, &s, &ms) == 4)
	{
		return (h * 3600 + m * 60 + s) * 1000 + ms;
	}
	return 0;
}

static std::string formatSrtTime(long long timeMs)
{
	int h = timeMs / 3600000;
	int m = (timeMs / 60000) % 60;
	int s = (timeMs / 1000) % 60;
	int ms = timeMs % 1000;

	char buffer[20];
	snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d,%03d", h, m, s, ms);
	return std::string(buffer);
}

bool SubRip::readFromFile(const std::string& filename)
{
	clear();
	std::ifstream file(filename);
	if (!file.is_open())
		return false;

	std::string line;
	SubtitleLine currentLine;
	int state = 0;

	while (std::getline(file, line))
	{
		if (line.empty())
		{
			if (state == 2)
			{
				lines.push_back(currentLine);
				currentLine = SubtitleLine();
			}
			state = 0;
			continue;
		}

		if (state == 0)
		{
			state = 1;
		}
		else if (state == 1)
		{
			size_t arrowPos = line.find("-->");
			if (arrowPos != std::string::npos)
			{
				std::string startTimeStr = line.substr(0, arrowPos - 1);
				std::string endTimeStr = line.substr(arrowPos + 3);

				startTimeStr.erase(0, startTimeStr.find_first_not_of(" \t"));
				startTimeStr.erase(startTimeStr.find_last_not_of(" \t") + 1);
				endTimeStr.erase(0, endTimeStr.find_first_not_of(" \t"));
				endTimeStr.erase(endTimeStr.find_last_not_of(" \t") + 1);

				currentLine.startTime = parseSrtTime(startTimeStr);
				currentLine.endTime = parseSrtTime(endTimeStr);
				state = 2;
			}
		}
		else if (state == 2)
		{
			if (!currentLine.text.empty())
				currentLine.text += "\n";
			currentLine.text += line;
		}
	}

	if (state == 2)
	{
		lines.push_back(currentLine);
	}

	return true;
}

bool SubRip::writeToFile(const std::string& filename) const
{
	std::ofstream file(filename);
	if (!file.is_open())
		return false;

	for (int i = 0; i < lines.size(); i++)
	{
		const auto& line = lines[i];
		file << (i + 1) << "\n";
		file << formatSrtTime(line.startTime) << " --> " << formatSrtTime(line.endTime) << "\n";
		file << line.text << "\n\n";
	}

	return true;
}

void SubRip::removeFormatting()
{
	std::regex formattingRegex("<[^>]*>|\\{[^}]*\\}");
	for (int i = 0; i < lines.size(); i++)
	{
		lines[i].text = std::regex_replace(lines[i].text, formattingRegex, "");
	}
}

void SubRip::addDefaultStyle()
{
	removeFormatting();
}

void SubRip::shiftTime(long long deltaMs, bool shiftStart, bool shiftEnd)
{
	for (int i = 0; i < lines.size(); i++)
	{
		if (shiftStart)
			lines[i].startTime += deltaMs;
		if (shiftEnd)
			lines[i].endTime += deltaMs;
	}
}

Subtitle* SubRip::findCollisions() const
{
	SubRip* result = new SubRip();

	for (int i = 0; i < lines.size(); i++)
	{
		for (int j = i + 1; j < lines.size(); j++)
		{
			const auto& line1 = lines[i];
			const auto& line2 = lines[j];

			bool collision = (line1.startTime < line2.endTime) && (line1.endTime > line2.startTime);

			if (collision)
			{
				bool line1Exists = false;
				bool line2Exists = false;

				for (int k = 0; k < result->size(); k++)
				{
					if (result->get(k).startTime == line1.startTime && result->get(k).endTime == line1.endTime &&
						result->get(k).text == line1.text)
					{
						line1Exists = true;
					}
					if (result->get(k).startTime == line2.startTime && result->get(k).endTime == line2.endTime &&
						result->get(k).text == line2.text)
					{
						line2Exists = true;
					}
				}

				if (!line1Exists)
					result->addLine(line1);
				if (!line2Exists)
					result->addLine(line2);
			}
		}
	}

	return result;
}
