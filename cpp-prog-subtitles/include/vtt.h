#ifndef VTT_H
#define VTT_H

#include "fixed_array.h"
#include "subtitle.h"

class VTT : public Subtitle
{
	FixedArray< SubtitleLine > lines;

  public:
	bool readFromFile(const std::string& filename) override;
	bool writeToFile(const std::string& filename) const override;

	void removeFormatting() override;
	void addDefaultStyle() override;
	void shiftTime(long long deltaMs, bool shiftStart, bool shiftEnd) override;
	Subtitle* findCollisions() const override;

	int size() const override { return lines.size(); }
	const SubtitleLine& get(int index) const override { return lines[index]; }
	void addLine(const SubtitleLine& line) override { lines.push_back(line); }
	void clear() override { lines.clear(); }
};

#endif
