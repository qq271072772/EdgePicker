#include "EdgePicker.h"

namespace EP{
	EdgePicker* EdgePicker::m_instance = NULL;
	bool EdgePicker::EnsureSrc(){
		if (m_src == NULL)
			cout << "Error: src is null!" << endl;
		return m_src != NULL;
	}
	void EdgePicker::LoadSrcImage(char* filename){
		m_src = ImageHelper::LoadImage(filename);
	}
	void EdgePicker::InitHistogram(){
		if (!EnsureSrc())
			return;
		m_pixelCnt = 0;
		m_histogram.Clear();
		m_gray = ImageHelper::Rgb2Gray(m_src);
		for (int i = 0; i < m_gray->height; i++)
			for (int j = 0; j < m_gray->width; j++){
				uchar bright = ImageHelper::SampleElem(m_gray, i, j);
				if (!m_histogram.ContainsKey(bright))
					m_histogram.Add(bright, 0);
				m_histogram[bright]++;
				m_pixelCnt++;
			}
	}
	void EdgePicker::DealBottom(){
		if (!EnsureSrc())
			return;
		
		//Find peak and average pixel cnt;
		int averagePixelCnt = m_pixelCnt / GRAY_PIXEL;
		int peakCnt = 0, peakBright = -1;
		List<int> brights = m_histogram.Keys();
		for (int i = 0; i < brights.Count(); i++){
			if (m_histogram[brights[i]]>peakCnt){
				peakCnt = m_histogram[brights[i]];
				peakBright = brights[i];
			}
		}

		//Find brights around peak
		List<int> botBrights;
		for (int i = 0; i < brights.Count(); i++){
			if (m_histogram[brights[i]]>averagePixelCnt && Math::Abs(brights[i] - peakBright) < PEAK_RANGE)
				botBrights.Add(brights[i]);
		}


		//Mark the bottom part of gray image
		CvSize size;
		size.width = m_gray->width;
		size.height = m_gray->height;
		IplImage* dst = cvCreateImage(size, IPL_DEPTH_8U, 1);
		cvCopy(m_gray, dst);
		for (int i = 0; i < dst->height; i++)
			for (int j = 0; j < dst->width; j++){
				uchar bright = ImageHelper::SampleElem(dst, i, j);
				if (botBrights.Contains(bright))
					ImageHelper::SetElem(dst, i, j, 255);
			}

		//Show Image
		double scale = 0.5f;
		size.width = m_gray->width*scale;
		size.height = m_gray->height*scale;
		IplImage* grayImg = cvCreateImage(size, IPL_DEPTH_8U, 1);
		IplImage* dstImg = cvCreateImage(size, IPL_DEPTH_8U, 1);
		cvResize(m_gray, grayImg, CV_INTER_LINEAR);
		cvResize(dst, dstImg, CV_INTER_LINEAR);
		cvShowImage("Gray", grayImg);
		cvShowImage("Dst", dstImg);
		cvWaitKey();
		cvDestroyWindow("Gray");
		cvDestroyWindow("Dst");
		ImageHelper::ReleaseImage(&dst);
		ImageHelper::ReleaseImage(&grayImg);
		ImageHelper::ReleaseImage(&dstImg);
	}
}