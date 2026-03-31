#include "../include/ass.h"
#include "../include/sami.h"
#include "../include/subrip.h"
#include "../include/vtt.h"

#include <iostream>
#include <string>

Subtitle* createSubtitleFromExtension(const std::string& filename)
{
	size_t dotPos = filename.find_last_of(".");
	if (dotPos == std::string::npos)
		return nullptr;

	std::string ext = filename.substr(dotPos + 1);
	if (ext == "srt")
		return new SubRip();
	if (ext == "smi")
		return new SAMI();
	if (ext == "ass" || ext == "ssa")
		return new ASS();
	if (ext == "vtt")
		return new VTT();
	return nullptr;
}

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		std::cout << "Usage: " << argv[0] << " <input_file> <output_file>\n";
		return 1;
	}

	std::string inputFile = argv[1];
	std::string outputFile = argv[2];

	Subtitle* inputSubtitle = createSubtitleFromExtension(inputFile);
	Subtitle* outputSubtitle = createSubtitleFromExtension(outputFile);

	if (!inputSubtitle || !outputSubtitle)
	{
		std::cerr << "Unsupported file format\n";
		delete inputSubtitle;
		delete outputSubtitle;
		return 1;
	}

	if (!inputSubtitle->readFromFile(inputFile))
	{
		std::cerr << "Failed to read input file\n";
		delete inputSubtitle;
		delete outputSubtitle;
		return 1;
	}

	for (int i = 0; i < inputSubtitle->size(); i++)
	{
		outputSubtitle->addLine(inputSubtitle->get(i));
	}

	if (!outputSubtitle->writeToFile(outputFile))
	{
		std::cerr << "Failed to write output file\n";
		delete inputSubtitle;
		delete outputSubtitle;
		return 1;
	}

	delete inputSubtitle;
	delete outputSubtitle;
	return 0;
}
