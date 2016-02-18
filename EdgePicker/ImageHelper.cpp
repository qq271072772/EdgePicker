#include "ImageHelper.h"

namespace EP{
	IplImage* ImageHelper::LoadImage(char* filename){
		return cvLoadImage(filename, CV_LOAD_IMAGE_UNCHANGED);
	}
	IplImage* ImageHelper::Rgb2Gray(IplImage* src){
		if (src == NULL)
			return NULL;
		IplImage* ret = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
		cvCvtColor(src, ret, CV_RGB2GRAY);
		return ret;
	}
	uchar ImageHelper::SampleElem(IplImage* src, int x, int y){
		if (src == NULL)
			return -1;
		return CV_IMAGE_ELEM(src, uchar, x, y);
	}
	void ImageHelper::SetElem(IplImage* src, int x, int y, uchar value){
		if (src == NULL)
			return;
		CV_IMAGE_ELEM(src, uchar, x, y) = value;
	}
	void ImageHelper::ReleaseImage(IplImage** src){
		if (src != NULL)
			cvReleaseImage(src);
	}
}