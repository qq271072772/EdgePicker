#include "ImageHelper.h"

namespace Utility{
	IplImage* ImageHelper::LoadImage(char* filename){
		return cvLoadImage(filename, CV_LOAD_IMAGE_UNCHANGED);
	}
	void ImageHelper::SaveImage(char* filename, IplImage* img){
		cvSaveImage(filename, img);
	}
	IplImage* ImageHelper::Rgb2Gray(IplImage* src){
		if (src == NULL)
			return NULL;
		IplImage* ret = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
		cvCvtColor(src, ret, CV_RGB2GRAY);
		return ret;
	}
	IplImage* ImageHelper::CreateImage(int width, int height, int depth, int channels){
		return cvCreateImage(CvSize(width, height), depth, channels);
	}
	IplImage* ImageHelper::CreateCopy(IplImage* src){
		if (src == NULL)
			return NULL;
		IplImage* ret = CreateImage(src->width, src->height, src->depth, src->nChannels);
		cvCopy(src, ret);
		return ret;
	}
	void ImageHelper::ReleaseImage(IplImage** img){
		if (img != NULL)
			cvReleaseImage(img);
	}

	uchar ImageHelper::SampleElem(IplImage* src, int x, int y){
		if (src == NULL || src->nChannels != 1)
			return -1;
		return CV_IMAGE_ELEM(src, uchar, y, x);
	}
	U_RGB ImageHelper::SampleElemRGB(IplImage* src, int x, int y){
		if (src == NULL || src->nChannels != 3)
			return U_RGB();
		U_RGB ret;
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
	void ImageHelper::SetElemRGB(IplImage* src, int x, int y, U_RGB value){
		if (src == NULL || src->nChannels != 3)
			return;
		CV_IMAGE_ELEM(src, uchar, y, x*src->nChannels + 0) = value.b;
		CV_IMAGE_ELEM(src, uchar, y, x*src->nChannels + 1) = value.g;
		CV_IMAGE_ELEM(src, uchar, y, x*src->nChannels + 2) = value.r;
	}

	int ImageHelper::RGBDiff(U_RGB v1, U_RGB v2){
		int diffR = Math::Abs(v1.r - v2.r);
		int diffG = Math::Abs(v1.g - v2.g);
		int diffB = Math::Abs(v1.b - v2.b); 

		return sqrt(diffR*diffR + diffG*diffG + diffB*diffB);

		//if (diffR > diffG && diffR > diffB)
		//	return diffR;
		//if (diffG > diffB)
		//	return diffG;
		//return diffB;
	}
	int ImageHelper::RGB2Hash(U_RGB v){
		return v.r * 256 * 256 + v.g * 256 + v.b;
	}
	U_RGB ImageHelper::Hash2RGB(int hash){
		U_RGB ret;
		ret.r = hash / 256 / 256;
		ret.g = hash / 256 % 256;
		ret.b = hash % 256;
		return ret;
	}
	int ImageHelper::RGB2GRAY(U_RGB rgb){
		return (rgb.r + rgb.g + rgb.b) / 3;
	}
}