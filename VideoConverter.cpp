#include "VideoConverter.h"
#include "ProgramOptions.h"

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>  // Video write

#include <algorithm>
#include <iterator>
#include <vector>
#include <string>
#include <regex>
#include <iostream>

using namespace std;
namespace fs = boost::filesystem;

class VideoConverter::Impl {
public:
	explicit Impl(const ProgramOptions& po)
		: m_ProgramOptions{po}
	{
		extractPaths();
		filterPaths();
		generateFrames();
		getFrameInfo();
		chackFrames();
	}

	void generateVideo()
	{
		auto rgbVideoFilename = m_ProgramOptions.videoName() + string{'.'}
				+ m_ProgramOptions.videoExtension();
		auto alphaVideoFilename = m_ProgramOptions.videoName() + string{"_alpa"}
				+ string{'.'} + m_ProgramOptions.videoExtension();

		cv::VideoWriter videoWriterRGB{
					rgbVideoFilename,
					m_ProgramOptions.fourcc(),
					m_ProgramOptions.fps(),
					m_FrameSize
		};

		cv::VideoWriter videoWriterAlpha{
					alphaVideoFilename,
					m_ProgramOptions.fourcc(),
					m_ProgramOptions.fps(),
					m_FrameSize
		};

		for( const auto f : m_Frames)
		{
			try
			{
				const auto frame = cv::imread(f.absolutePath, cv::IMREAD_UNCHANGED);

//				videoWriterRGB << frame;

				vector<cv::Mat> spl;
				cv::split(frame, spl);

				cv::Mat alphaFrame;
				cv::cvtColor(spl[3], alphaFrame, cv::COLOR_GRAY2BGR);

				if (m_ProgramOptions.verbose() > 5)
				{
					cv::imshow("frame", frame);
					cv::imshow("alpha", alphaFrame);

					cv::waitKey(5);
				}
			}
			catch (const exception& exc)
			{
				cerr << "skipping " << f.absolutePath << ":" << exc.what() << endl;
				continue;
			}

//			try
//			{
//				const auto frame = cv::imread(f.absolutePath, cv::IMREAD_UNCHANGED );

//				vector<cv::Mat> spl;
//				cv::split(frame, spl);

//				cv::Mat alphaFrame;
//				cv::cvtColor(spl[3], alphaFrame, cv::COLOR_GRAY2BGR);

//				videoWriterAlpha << alphaFrame;

//				if (m_ProgramOptions.verbose() > 5)
//				{
//					cv::imshow("alphaFrame", alphaFrame);
//					cv::waitKey(15);
//				}
//			}
//			catch (const exception& exc)
//			{
//				cerr << "skipping " << f.absolutePath << ":" << exc.what() << endl;
//				continue;
//			}

		}
	}

	const vector<fs::path>& foundImages() const noexcept
	{
		return m_FilteredPaths;
	}

	const vector<Frame>& frames() const noexcept
	{
		return m_Frames;
	}

private:
	void extractPaths(fs::path p = fs::path{"."})
	{
		vector<fs::path> result;
		copy(fs::directory_iterator(p), fs::directory_iterator(),
				back_inserter(m_Paths));

		if (m_Paths.empty())
			throw invalid_argument{"no images found"};
	}

	void getFrameInfo()
	{
		const auto filename = fs::canonical(m_Paths.front()).string();
		cout << "opening " << filename << endl;

		const auto frame = cv::imread(m_Paths.front().string(), cv::IMREAD_UNCHANGED);
		m_FrameSize = cv::Size{frame.cols, frame.rows };

		if (m_FrameSize.width < 1 || m_FrameSize.height < 1)
			throw runtime_error{"frame size cannot be 0"};

		m_Channels = frame.channels();

		if (m_Channels != 4)
		{
			throw runtime_error{"frame must have 4 channels"};
		}
	}

	void filterPaths()
	{
		copy_if(begin(m_Paths), end(m_Paths),
				  back_inserter(m_FilteredPaths),
				  [this](const auto& p) -> bool
		{
			static regex re{R"((\w+)_(\d+)\.(\w+))"};
			smatch stringMatch;
			auto filename = p.filename().string();

			if (regex_match(filename, stringMatch, re) )
			{
				auto name = stringMatch.str(1);
				return name == m_ProgramOptions.prefix();
			}

			return false;
		});
	}

	void generateFrames()
	{
		transform(begin(m_FilteredPaths), end(m_FilteredPaths),
				  back_inserter(m_Frames),
				  [](const auto& filteredPath)
		{
			static regex re{R"((\w+)_(\d+)\.(\w+))"};
			smatch stringMatch;
			auto fullFilename = filteredPath.filename().string();
			regex_match(fullFilename, stringMatch, re);

			auto filename = stringMatch.str(1) ;
			auto index = stoi(stringMatch.str(2));
			auto ext = stringMatch.str(3);

			const auto absolutePath = fs::canonical(filteredPath).string();

			return Frame{ absolutePath, filename, ext, index };
		});

		sort(begin(m_Frames), end(m_Frames));
	}

	void chackFrames() const
	{
		if (m_Frames.size() < 2)
			return;

		for_each(begin(m_Frames), end(m_Frames),
				[this](const auto& frame)
		{
			cv::Mat f = cv::imread(frame.absolutePath, cv::IMREAD_UNCHANGED);
			cv::Size currSize{ f.cols, f.rows };

			if ( currSize != m_FrameSize || f.channels() != m_Channels )
			{
				ostringstream os;
				os << frame.absolutePath << ' ' << currSize.width << 'x' << currSize.height
				   << 'x' << f.channels() << " has invalid size or channels";
				throw runtime_error{os.str()};
			}
		});
	}

private:
	const ProgramOptions& m_ProgramOptions;

	vector<fs::path> m_Paths;
	vector<fs::path> m_FilteredPaths;
	vector<Frame> m_Frames;

	cv::Size m_FrameSize;
	int m_Channels;
};

VideoConverter::VideoConverter(const ProgramOptions& po)
	: m_Impl{make_unique<VideoConverter::Impl>(po)}
{
}

VideoConverter::~VideoConverter()
{
}

void VideoConverter::generateVideo()
{
	m_Impl->generateVideo();
}

const vector<fs::path>& VideoConverter::foundImages() const noexcept
{
	return m_Impl->foundImages();
}

const vector<Frame>& VideoConverter::frames() const noexcept
{
	return m_Impl->frames();
}

ostream& operator << (ostream& os, const Frame& f)
{
	os << f.index << ": " << f.name << " " << f.ext << " \"" << f.absolutePath << '"';
}
