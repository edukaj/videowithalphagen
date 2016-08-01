#include "ProgramOptions.h"
#include <boost/program_options.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <stdexcept>
#include <sstream>


using namespace std;
namespace po = boost::program_options;

enum class VerboseLevel : int {
	NONE, LOW, MEDIUM, HIGH
};

struct ProgramOptions::Impl {
public:
	Impl(int argc, char* argv[])
		: m_CommandLineArgCount{argc}
	{
		initializeOptions();

		po::store(po::parse_command_line(argc, argv, m_Desc), m_OptionsMap);
		po::notify(m_OptionsMap);

		if (!isFourCCValid())
			throw invalid_argument{"unknow fourcc code"};

		if (shouldDisplayOnlyHelp())
			cout << *this << endl;
	}


	bool shouldDisplayOnlyHelp() const noexcept
	{
		return m_OptionsMap.count("help") != 0 || m_CommandLineArgCount == 1;
	}

	int verbose() const noexcept
	{
		return m_Verbose;
	}

	const string& prefix() const noexcept
	{
		return m_Prefix;
	}

	const string& videoName() const noexcept
	{
		return m_VideoName;
	}


	const std::string& videoExtension() const noexcept
	{
		return m_VideoExtension;
	}

	double fps() const noexcept
	{
		return m_FPS;
	}

	int fourcc() const noexcept
	{
        return cv::VideoWriter::fourcc(
                toupper(m_FourCC[0]),
                toupper(m_FourCC[1]),
                toupper(m_FourCC[2]),
                toupper(m_FourCC[3])
                );
	}

    int videoMode() const noexcept
    {
        return m_VideoMode;
    }

	friend ostream& operator << (ostream& os, const Impl& imp)
	{
		if (imp.shouldDisplayOnlyHelp())
			os << imp.description() << "\n\n"
			   << imp.usageDescription() << '\n'
			   << imp.m_Desc << endl;

		if (imp.verbose() > (int)VerboseLevel::MEDIUM)
		{
			os << imp.printParameters() << endl;
		}

		return os;
	}

private:
	bool exist(const string& str) const noexcept
	{
		return m_OptionsMap.find(str) != end(m_OptionsMap);
	}

	void initializeOptions()
	{
		using namespace string_literals;

		m_Desc.add_options()
				("help,h", "produce this message")
				("prefix,p",
				 po::value<string>(&m_Prefix)->default_value("image"),
				 "prefix of files")
				("out,o", po::value<string>(&m_VideoName)->default_value("video"),
				 "destination video filename without extension")
				("extension,e", po::value<string>(&m_VideoExtension)->default_value("avi"),
				 "destination video extension")
				("fps,f", po::value<double>(&m_FPS)->default_value(15),
				 "frame per seconds")
				("fourcc,c", po::value<std::string>(&m_FourCC)->default_value("x264"s),
				 "fourcc code do use for encoding see: http://www.fourcc.org/codecs.php for other codecs")
				("verbose,v", po::value<int>(&m_Verbose)->default_value(0),
                 "verbose level")
                ("video-mode,m", po::value<int>(&m_VideoMode)->default_value(1),
                 "Video generation mode:\n"
                 "1 -> two videos: one with rgb and the other with alpha\n"
                 "2 -> a video with double height: on top rgb on bottom alpha\n"
				 "3 -> a video with alpha channel transformed as green\n");
	}

	bool isFourCCValid() const
	{
		return m_FourCC.length() == 4;
	}

	string printParameters() const noexcept
	{
		ostringstream os;

        os << "prefix:     " << m_Prefix << '\n'
           << "out:        " << m_VideoName << '\n'
           << "extension:  " << m_VideoExtension << '\n'
           << "fps:        " << m_FPS << '\n'
           << "fourcc:     " << m_FourCC << '\n'
           << "video-mode: " << m_VideoMode << endl
           << "verbose:    " << m_Verbose << endl;

		return os.str();
	}

	const std::string& description() const noexcept
	{
		using namespace string_literals;

		static auto desc = "This program allow to convert a sequence of images into a video\n"
			   "The images must have the following signature name_xxx where xxx\n"
			   "is a progressive number starting from one"s;
		return desc;
	}

	const std::string& usageDescription() const noexcept
	{
		using namespace string_literals;

		static auto usageDescription = "Simple usage:\n\tvideowithalphagen -p image"s;
		return usageDescription;
	}


private:
	const int m_CommandLineArgCount;

	bool m_ShouldDisplayOnlyHelp;
	int m_Verbose;
    int m_VideoMode;

	double m_FPS;
	string m_FourCC;

	string m_Prefix;
	string m_VideoName;
	string m_VideoExtension;

	po::options_description m_Desc{"Options"};
	po::variables_map m_OptionsMap;
};

ProgramOptions::ProgramOptions(int argc, char* argv[])
	: m_Impl{make_unique<ProgramOptions::Impl>(argc, argv)}
{
}

ProgramOptions::~ProgramOptions()
{
}

bool ProgramOptions::shouldDisplayOnlyHelp() const noexcept
{
	return m_Impl->shouldDisplayOnlyHelp();
}

const string&ProgramOptions::prefix() const noexcept
{
	return m_Impl->prefix();
}

double ProgramOptions::fps() const noexcept
{
	return m_Impl->fps();
}

int ProgramOptions::fourcc() const noexcept
{
	return m_Impl->fourcc();
}

const string&ProgramOptions::videoName() const noexcept
{
	return m_Impl->videoName();
}

const string&ProgramOptions::videoExtension() const noexcept
{
    return m_Impl->videoExtension();
}

int ProgramOptions::videoMode() const noexcept
{
    return m_Impl->videoMode();
}

ostream& operator <<(ostream& os, const ProgramOptions& options)
{
	return os << *options.m_Impl << endl;
}
int ProgramOptions::verbose() const noexcept
{
	return m_Impl->verbose();
}
