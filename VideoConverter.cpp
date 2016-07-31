#include "VideoConverter.h"
#include "ProgramOptions.h"

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>  // Video write

#include <FreeImage.h>

#include <algorithm>
#include <iterator>
#include <vector>
#include <string>
#include <regex>
#include <iostream>

using namespace std;
namespace fs = boost::filesystem;

namespace internal {

void FI2MAT(FIBITMAP* src, cv::Mat& dst)
{
    using namespace cv;

    //FIT_BITMAP    //standard image : 1 - , 4 - , 8 - , 16 - , 24 - , 32 - bit
    //FIT_UINT16    //array of unsigned short : unsigned 16 - bit
    //FIT_INT16     //array of short : signed 16 - bit
    //FIT_UINT32    //array of unsigned long : unsigned 32 - bit
    //FIT_INT32     //array of long : signed 32 - bit
    //FIT_FLOAT     //array of float : 32 - bit IEEE floating point
    //FIT_DOUBLE    //array of double : 64 - bit IEEE floating point
    //FIT_COMPLEX   //array of FICOMPLEX : 2 x 64 - bit IEEE floating point
    //FIT_RGB16     //48 - bit RGB image : 3 x 16 - bit
    //FIT_RGBA16    //64 - bit RGBA image : 4 x 16 - bit
    //FIT_RGBF      //96 - bit RGB float image : 3 x 32 - bit IEEE floating point
    //FIT_RGBAF     //128 - bit RGBA float image : 4 x 32 - bit IEEE floating point

    int bpp = FreeImage_GetBPP(src);
    FREE_IMAGE_TYPE fit = FreeImage_GetImageType(src);

    int cv_type = -1;
    int cv_cvt = -1;

    switch (fit)
    {
    case FIT_UINT16: cv_type = DataType<ushort>::type; break;
    case FIT_INT16: cv_type = DataType<short>::type; break;
    case FIT_UINT32: cv_type = DataType<unsigned>::type; break;
    case FIT_INT32: cv_type = DataType<int>::type; break;
    case FIT_FLOAT: cv_type = DataType<float>::type; break;
    case FIT_DOUBLE: cv_type = DataType<double>::type; break;
    case FIT_COMPLEX: cv_type = DataType<Complex<double>>::type; break;
    case FIT_RGB16: cv_type = DataType<Vec<ushort, 3>>::type; cv_cvt = COLOR_RGB2BGR; break;
    case FIT_RGBA16: cv_type = DataType<Vec<ushort, 4>>::type; cv_cvt = COLOR_RGBA2BGRA; break;
    case FIT_RGBF: cv_type = DataType<Vec<float, 3>>::type; cv_cvt = COLOR_RGB2BGR; break;
    case FIT_RGBAF: cv_type = DataType<Vec<float, 4>>::type; cv_cvt = COLOR_RGBA2BGRA; break;
    case FIT_BITMAP:
        switch (bpp) {
        case 8: cv_type = DataType<Vec<uchar, 1>>::type; break;
        case 16: cv_type = DataType<Vec<uchar, 2>>::type; break;
        case 24: cv_type = DataType<Vec<uchar, 3>>::type; break;
        case 32: cv_type = DataType<Vec<uchar, 4>>::type; break;
        default:
            // 1, 4 // Unsupported natively
            cv_type = -1;
        }
        break;
    default:
        // FIT_UNKNOWN // unknown type
        dst = Mat(); // return empty Mat
        return;
    }

    int width = FreeImage_GetWidth(src);
    int height = FreeImage_GetHeight(src);
    int step = FreeImage_GetPitch(src);

    if (cv_type >= 0) {
        dst = Mat(height, width, cv_type, FreeImage_GetBits(src), step);
        if (cv_cvt > 0)
        {
            cvtColor(dst, dst, cv_cvt);
        }
    }
    else {

        vector<uchar> lut;
        int n = cvRound(pow(2.0, bpp));
        for (int i = 0; i < n; ++i)
        {
            lut.push_back(static_cast<uchar>((255 / (n - 1))*i));
        }

        FIBITMAP* palletized = FreeImage_ConvertTo8Bits(src);
        BYTE* data = FreeImage_GetBits(src);
        for (int r = 0; r < height; ++r) {
            for (int c = 0; c < width; ++c) {
                dst.at<uchar>(r, c) = saturate_cast<uchar>(lut[data[r*step + c]]);
            }
        }
    }

    flip(dst, dst, 0);
}

cv::Mat loadImage(const std::string& filename)
{
    auto type = FreeImage_GetFileType(filename.c_str());
    auto bitmap = FreeImage_Load(type, filename.c_str());

    cv::Mat mat;
    FI2MAT(bitmap, mat);
    return mat;
}

}

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
		auto rgbVideoFilename = m_ProgramOptions.videoName() + string{'.'}
				+ m_ProgramOptions.videoExtension();
//		auto alphaVideoFilename = m_ProgramOptions.videoName() + string{"_alpa"}
//				+ string{'.'} + m_ProgramOptions.videoExtension();

		cv::VideoWriter videoWriterRGB{
					rgbVideoFilename,
					m_ProgramOptions.fourcc(),
					m_ProgramOptions.fps(),
					m_FrameSize
		};

//		cv::VideoWriter videoWriterAlpha{
//					alphaVideoFilename,
//					m_ProgramOptions.fourcc(),
//					m_ProgramOptions.fps(),
//					m_FrameSize
//		};

		for( const auto f : m_Frames)
        {
			try
			{
                const auto frame = internal::loadImage(f.absolutePath);
                cv::Mat rgbFrame;
                cv::cvtColor(frame, rgbFrame, cv::COLOR_BGRA2BGR);

                if (m_ProgramOptions.verbose() > 5)
                {
                  cv::imshow("rgbFrame", rgbFrame);
                  cv::waitKey(5);
                }

                videoWriterRGB << frame;

			}
			catch (const exception& exc)
			{
				cerr << "skipping " << f.absolutePath << ":" << exc.what() << endl;
				continue;
			}
        }

//        for( const auto f : m_Frames)
//        {
//            try
//            {
//                const auto frame = internal::loadImage(f.absolutePath);

//                vector<cv::Mat> spl;
//                cv::split(frame, spl);

//                cv::Mat alphaFrame;
//                cv::cvtColor(spl[3], alphaFrame, cv::COLOR_GRAY2BGR);

//                if (m_ProgramOptions.verbose() > 5)
//                {
//                  cv::imshow("alpha", spl[3]);
//                  cv::waitKey(5);
//                }

//                videoWriterAlpha << spl[3];

//            }
//            catch (const exception& exc)
//            {
//                cerr << "skipping " << f.absolutePath << ":" << exc.what() << endl;
//                continue;
//            }
//        }
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

        if (m_ProgramOptions.verbose() > 4)
            cout << "opening " << filename << endl;

        auto frame = internal::loadImage(filename);

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
            cv::Mat f = internal::loadImage(frame.absolutePath);
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
	return os;
}
