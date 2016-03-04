#ifndef IMAGE_HELPER
#define IMAGE_HELPER
#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "Utility.h"

namespace Utility{
	struct RGB{
		uchar r, g, b;
		RGB(){}
		RGB(uchar rv, uchar gv, uchar bv) :r(rv), g(gv), b(bv){}
	};
	class ImageHelper{
	private:
	public:
		static IplImage* LoadImage(char* filename);
		static void SaveImage(char* filename, IplImage* img);
		static IplImage* Rgb2Gray(IplImage* src);
		static IplImage* CreateImage(int width, int height, int depth, int channels);
		static IplImage* CreateCopy(IplImage* src);
		static void ReleaseImage(IplImage** img);

		static uchar SampleElem(IplImage* src, int x, int y);
		static RGB SampleElemRGB(IplImage* src, int x, int y);
		static void SetElem(IplImage* src, int x, int y, uchar value);
		static void SetElemRGB(IplImage*src ,int x, int y, RGB value);

		static int RGBDiff(RGB v1, RGB v2);
		static int RGB2Hash(RGB v);
		static RGB Hash2RGB(int hash);
		static int RGB2GRAY(RGB rgb);
	};
}

#endif