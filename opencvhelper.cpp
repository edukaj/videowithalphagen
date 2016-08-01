#include "opencvhelper.h"
#include <opencv2/imgproc.hpp>
#include <FreeImage.h>

using namespace cv;
using namespace std;

void FI2MAT(FIBITMAP* src, Mat& dst)
{
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

Mat loadImage(const string& filename)
{
    auto type = FreeImage_GetFileType(filename.c_str());
    auto bitmap = FreeImage_Load(type, filename.c_str());

    cv::Mat mat;
    FI2MAT(bitmap, mat);
    return mat;
}
