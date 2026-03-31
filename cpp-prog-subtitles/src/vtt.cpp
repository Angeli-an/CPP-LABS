#include "../include/vtt.h"

#include <fstream>
#include <regex>
#include <sstream>

static long long parseVttTime(const std::string& timeStr)
{
	int h, m, s, ms;
	if (sscanf(timeStr.c_str(), "%d:%d:%d.%d", &h, &m, &s, &ms) == 4)
	{
		return (h * 3600 + m * 60 + s) * 1000 + ms;
	}
	return 0;
}

static std::string formatVttTime(long long timeMs)
{
	int h = timeMs / 3600000;
	int m = (timeMs / 60000) % 60;
	int s = (timeMs / 1000) % 60;
	int ms = timeMs % 1000;

	char buffer[20];
	snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d.%03d", h, m, s, ms);
	return std::string(buffer);
}

bool VTT::readFromFile(const std::string& filename)
{
	clear();
	std::ifstream file(filename);
	if (!file.is_open())
		return false;

	std::string line;
	bool foundHeader = false;
	SubtitleLine currentLine;

	while (std::getline(file, line))
	{
		if (!foundHeader)
		{
			if (line.find("WEBVTT") != std::string::npos)
			{
				foundHeader = true;
			}
			continue;
		}

		if (line.empty())
		{
			if (!currentLine.text.empty())
			{
				lines.push_back(currentLine);
				currentLine = SubtitleLine();
			}
			continue;
		}

		if (line.find("-->") != std::string::npos)
		{
			size_t arrowPos = line.find("-->");
			std::string startTimeStr = line.substr(0, arrowPos - 1);
			std::string endTimeStr = line.substr(arrowPos + 3);

			currentLine.startTime = parseVttTime(startTimeStr);
			currentLine.endTime = parseVttTime(endTimeStr);
		}
		else
		{
			if (!currentLine.text.empty())
				currentLine.text += "\n";
			currentLine.text += line;
		}
	}

	if (!currentLine.text.empty())
	{
		lines.push_back(currentLine);
	}

	return true;
}

bool VTT::writeToFile(const std::string& filename) const
{
	std::ofstream file(filename);
	if (!file.is_open())
		return false;

	file << "WEBVTT\n\n";

	for (int i = 0; i < lines.size(); i++)
	{
		const auto& line = lines[i];
		file << formatVttTime(line.startTime) << " --> " << formatVttTime(line.endTime) << "\n";
		file << line.text << "\n\n";
	}

	return true;
}

void VTT::removeFormatting()
{
	std::regex formattingRegex("<[^>]*>|\\{[^}]*\\}");
	for (int i = 0; i < lines.size(); i++)
	{
		lines[i].text = std::regex_replace(lines[i].text, formattingRegex, "");
	}
}

void VTT::addDefaultStyle()
{
	removeFormatting();
}

void VTT::shiftTime(long long deltaMs, bool shiftStart, bool shiftEnd)
{
	for (int i = 0; i < lines.size(); i++)
	{
		if (shiftStart)
			lines[i].startTime += deltaMs;
		if (shiftEnd)
			lines[i].endTime += deltaMs;
	}
}

Subtitle* VTT::findCollisions() const
{
	VTT* result = new VTT();

	for (int i = 0; i < lines.size(); i++)
	{
		for (int j = i + 1; j < lines.size(); j++)
		{
			const auto& line1 = lines[i];
			const auto& line2 = lines[j];

			if (line1.startTime < line2.endTime && line2.startTime < line1.endTime)
			{
				result->addLine(line1);
				result->addLine(line2);
			}
		}
	}

	return result;
}
