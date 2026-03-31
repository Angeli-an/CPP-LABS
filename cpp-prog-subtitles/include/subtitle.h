#ifndef SUBTITLE_H
#define SUBTITLE_H

#include <string>

class Subtitle
{
  public:
	struct SubtitleLine
	{
		long long startTime;
		long long endTime;
		std::string text;

		SubtitleLine() : startTime(0), endTime(0) {}
		SubtitleLine(long long start, long long end, const std::string& txt) : startTime(start), endTime(end), text(txt)
		{
		}
	};

	virtual ~Subtitle() {}

	virtual bool readFromFile(const std::string& filename) = 0;
	virtual bool writeToFile(const std::string& filename) const = 0;

	virtual void removeFormatting() = 0;
	virtual void addDefaultStyle() = 0;
	virtual void shiftTime(long long deltaMs, bool shiftStart = true, bool shiftEnd = true) = 0;
	virtual Subtitle* findCollisions() const = 0;

	virtual int size() const = 0;
	virtual const SubtitleLine& get(int index) const = 0;
	virtual void addLine(const SubtitleLine& line) = 0;
	virtual void clear() = 0;
};

#endif
