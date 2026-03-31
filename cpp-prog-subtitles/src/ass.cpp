#include "../include/ass.h"

#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>

static long long parseAssTime(const std::string& timeStr)
{
	int h, m, s, cs;
	if (sscanf(timeStr.c_str(), "%d:%d:%d.%d", &h, &m, &s, &cs) == 4)
	{
		return (h * 3600 + m * 60 + s) * 1000 + cs * 10;
	}
	return 0;
}

static std::string formatAssTime(long long timeMs)
{
	long long totalCs = timeMs / 10;
	long long totalSeconds = totalCs / 100;
	long long cs = totalCs % 100;
	long long hours = totalSeconds / 3600;
	long long minutes = (totalSeconds % 3600) / 60;
	long long seconds = totalSeconds % 60;

	char buffer[20];
	snprintf(buffer, sizeof(buffer), "%lld:%02lld:%02lld.%02lld", hours, minutes, seconds, cs);
	return std::string(buffer);
}

bool ASS::readFromFile(const std::string& filename)
{
	clear();
	std::ifstream file(filename);
	if (!file.is_open())
		return false;

	std::string line;
	bool inEvents = false;

	while (std::getline(file, line))
	{
		if (line.find("[Events]") != std::string::npos)
		{
			inEvents = true;
			continue;
		}
		if (line.find("[") != std::string::npos && line.find("]") != std::string::npos && inEvents)
		{
			inEvents = false;
			continue;
		}

		if (inEvents && line.find("Dialogue:") != std::string::npos)
		{
			SubtitleLine subtitleLine;
			std::vector< std::string > parts;
			size_t start = 9;
			size_t end = line.find(',', start);

			while (end != std::string::npos)
			{
				std::string part = line.substr(start, end - start);
				parts.push_back(part);
				start = end + 1;
				end = line.find(',', start);
			}
			if (start < line.length())
			{
				parts.push_back(line.substr(start));
			}

			if (parts.size() >= 10)
			{
				subtitleLine.startTime = parseAssTime(parts[1]);
				subtitleLine.endTime = parseAssTime(parts[2]);
				subtitleLine.text = parts[9];
				lines.push_back(subtitleLine);
			}
		}
	}

	return true;
}

bool ASS::writeToFile(const std::string& filename) const
{
	std::ofstream file(filename);
	if (!file.is_open())
		return false;

	file << "[Script Info]\nTitle: Converted Subtitles\n\n";
	file << "[V4+ Styles]\nFormat: Name, Fontname, Fontsize, PrimaryColour, SecondaryColour, OutlineColour, "
			"BackColour, Bold, Italic, Underline, StrikeOut, ScaleX, ScaleY, Spacing, Angle, BorderStyle, Outline, "
			"Shadow, Alignment, MarginL, MarginR, MarginV, Encoding\n";
	file << "Style: "
			"Default,Arial,20,&H00FFFFFF,&H000000FF,&H00000000,&H00000000,0,0,0,0,100,100,0,0,1,2,0,2,10,10,10,1\n\n";
	file << "[Events]\nFormat: Layer, Start, End, Style, Name, MarginL, MarginR, MarginV, Effect, Text\n";

	for (int i = 0; i < lines.size(); i++)
	{
		const auto& line = lines[i];
		file << "Dialogue: 0," << formatAssTime(line.startTime) << "," << formatAssTime(line.endTime)
			 << ",Default,,0,0,0,," << line.text << "\n";
	}

	return true;
}

void ASS::removeFormatting()
{
	std::regex formattingRegex("\\{[^}]*\\}");
	for (int i = 0; i < lines.size(); i++)
	{
		lines[i].text = std::regex_replace(lines[i].text, formattingRegex, "");
	}
}

void ASS::addDefaultStyle()
{
	removeFormatting();
}

void ASS::shiftTime(long long deltaMs, bool shiftStart, bool shiftEnd)
{
	for (int i = 0; i < lines.size(); i++)
	{
		if (shiftStart)
			lines[i].startTime += deltaMs;
		if (shiftEnd)
			lines[i].endTime += deltaMs;
	}
}

Subtitle* ASS::findCollisions() const
{
	ASS* result = new ASS();

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
