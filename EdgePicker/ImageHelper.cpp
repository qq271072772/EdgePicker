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
		if (src == NULL || src->nChannels != 1)
			return -1;
		return CV_IMAGE_ELEM(src, uchar, y, x);
	}
	RGB ImageHelper::SampleElemRGB(IplImage* src, int x, int y){
		if (src == NULL || src->nChannels != 3)
			return RGB();
		RGB ret;
		ret.b = CV_IMAGE_ELEM(src, uchar, y, x*src->nChannels + 0);
		ret.g = CV_IMAGE_ELEM(src, uchar, y, x*src->nChannels + 1);
		ret.r = CV_IMAGE_ELEM(src, uchar, y, x*src->nChannels + 2);
		return ret;
	}
	void ImageHelper::SetElem(IplImage* src, int x, int y, uchar value){
		if (src == NULL || src->nChannels != 1)
			return;
		CV_IMAGE_ELEM(src, uchar, y, x) = value;
	}
	void ImageHelper::SetElemRGB(IplImage* src, int x, int y, RGB value){
		if (src == NULL || src->nChannels != 3)
			return;
		CV_IMAGE_ELEM(src, uchar, y, x*src->nChannels + 0) = value.b;
		CV_IMAGE_ELEM(src, uchar, y, x*src->nChannels + 1) = value.g;
		CV_IMAGE_ELEM(src, uchar, y, x*src->nChannels + 2) = value.r;
	}
	void ImageHelper::ReleaseImage(IplImage** src){
		if (src != NULL)
			cvReleaseImage(src);
	}
}