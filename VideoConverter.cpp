#include "VideoConverter.h"
#include "ProgramOptions.h"
#include "opencvhelper.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>  // Video write
#include <opencv2/highgui.hpp>

#include <FreeImage.h>

#include <algorithm>
#include <iterator>
#include <vector>
#include <string>
#include <regex>
#include <iostream>

using namespace std;
namespace fs = boost::filesystem;

struct VideoConverter::Impl {
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
        switch (m_ProgramOptions.videoMode()) {
        case 1:
            generateRGBandAlphaVideo();
            break;

        case 2:
            generateVideoWithAlphaChannelMergetAtBottom();
            break;

        case 3:
        default:
            generateVideoWithAlphaChannelAsGreen();
            break;
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
        copy_if(fs::directory_iterator(p), fs::directory_iterator(),
                back_inserter(m_Paths), [](const auto& p)
        {
            return fs::is_regular_file(p);
        });

		if (m_Paths.empty())
			throw invalid_argument{"no images found"};
	}

	void getFrameInfo()
	{
		const auto filename = fs::canonical(m_Paths.front()).string();

        if (m_ProgramOptions.verbose() > 4)
            cout << "opening " << filename << endl;

        auto frame = loadImage(filename);

        m_FrameSize = cv::Size{frame.cols, frame.rows};
        m_Channels = frame.channels();

        if (m_FrameSize.width < 1 || m_FrameSize.height < 1)
            throw runtime_error{"frame size cannot be 0"};

        if (m_Channels != 4)
            throw runtime_error{"frame must have 4 channels"};
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
            cv::Mat f = loadImage(frame.absolutePath);
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

    void generateRGBandAlphaVideo()
    {
		using namespace string_literals;

		auto rgbVideoFilename = m_ProgramOptions.videoName() + "."s
                + m_ProgramOptions.videoExtension();
		auto alphaVideoFilename = m_ProgramOptions.videoName() + "_alpa"s
				+ "."s + m_ProgramOptions.videoExtension();

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
                const auto frame = loadImage(f.absolutePath);
                cv::Mat rgbFrame;
                cv::cvtColor(frame, rgbFrame, cv::COLOR_BGRA2BGR);

                vector<cv::Mat> spl;
                cv::split(frame, spl);

				cv::Mat alphaFrame;
				cv::cvtColor(spl[3], alphaFrame, cv::COLOR_GRAY2BGR);

                videoWriterRGB << rgbFrame;
				videoWriterAlpha << alphaFrame;

				displayParsedFileIf(m_ProgramOptions.verbose() > 4, f.absolutePath);
				displayWindowsIf(m_ProgramOptions.verbose() > 5, rgbFrame, alphaFrame);
            }
            catch (const exception& exc)
            {
                cerr << "skipping " << f.absolutePath << ":" << exc.what() << endl;
                continue;
            }
        }
    }

    void generateVideoWithAlphaChannelMergetAtBottom()
    {
		auto rgbVideoFilename = m_ProgramOptions.videoName() + "."s
                + m_ProgramOptions.videoExtension();

        auto newFrameSize = cv::Size{m_FrameSize.width, m_FrameSize.height * 2};

        cv::VideoWriter videoWriterRGBWithAlphaAtBottom{
                    rgbVideoFilename,
                    m_ProgramOptions.fourcc(),
                    m_ProgramOptions.fps(),
                    newFrameSize
        };

        for( const auto f : m_Frames)
        {
            try
            {
                const auto frame = loadImage(f.absolutePath);
                cv::Mat rgbFrame;
                cv::cvtColor(frame, rgbFrame, cv::COLOR_BGRA2BGR);

                vector<cv::Mat> spl;
                cv::split(frame, spl);

                cv::Mat alphaFrame;
                cv::cvtColor(spl[3], alphaFrame, cv::COLOR_GRAY2BGR);

                const auto type = rgbFrame.type();

                cv::Mat newFrame(newFrameSize.height, newFrameSize.width, type, cv::Scalar{0});

                static const cv::Rect topRoi{0, 0, m_FrameSize.width, m_FrameSize.height};
                static const cv::Rect bottomRoi{0, m_FrameSize.height,
                            m_FrameSize.width, m_FrameSize.height};

                rgbFrame.copyTo(newFrame(topRoi));
				alphaFrame.copyTo(newFrame(bottomRoi));

				videoWriterRGBWithAlphaAtBottom << newFrame;
				displayParsedFileIf(m_ProgramOptions.verbose() > 4, f.absolutePath);
				displayWindowIf(m_ProgramOptions.verbose() > 5, newFrame);

            }
            catch (const exception& exc)
            {
                cerr << "skipping " << f.absolutePath << ":" << exc.what() << endl;
                continue;
            }
        }
    }

    void generateVideoWithAlphaChannelAsGreen()
    {
        throw std::runtime_error{"This mode is not still implemented"};
    }

	void displayParsedFileIf(bool condition, const string& filename )
	{
		if (condition)
			cout << "parsing " << filename << endl;
	}

	void displayWindowIf(bool condition, const cv::Mat& rgbFrame)
	{
		if (condition)
		{
			cv::imshow("video", rgbFrame);
			cv::waitKey(1);
		}
	}


	void displayWindowsIf(bool condition, const cv::Mat& rgbFrame, const cv::Mat& alphaFrame)
	{
		if (condition)
		{
			cv::imshow("video", rgbFrame);
			cv::imshow("video", alphaFrame);
			cv::waitKey(1);
		}
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
	return os;
}
