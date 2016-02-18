#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace EP{
	class ImageHelper{
	private:
	public:
		static IplImage* LoadImage(char* filename);
		static IplImage* Rgb2Gray(IplImage* src);
		static uchar SampleElem(IplImage* src, int x, int y);
		static void SetElem(IplImage* src, int x, int y, uchar value);
		static void ReleaseImage(IplImage** src);
	};
}