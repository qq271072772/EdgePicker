#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace EP{
	struct RGB{
		uchar r, g, b;
		RGB(){}
		RGB(uchar rv, uchar gv, uchar bv) :r(rv), g(gv), b(bv){}
	};
	class ImageHelper{
	private:
	public:
		static IplImage* LoadImage(char* filename);
		static IplImage* Rgb2Gray(IplImage* src);
		static uchar SampleElem(IplImage* src, int x, int y);
		static RGB SampleElemRGB(IplImage* src, int x, int y);
		static void SetElem(IplImage* src, int x, int y, uchar value);
		static void SetElemRGB(IplImage*src ,int x, int y, RGB value);
		static void ReleaseImage(IplImage** src);
	};
}