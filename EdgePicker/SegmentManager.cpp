#include"SegmentManager.h"

namespace EP {

	//Constructor
	SegmentManager::SegmentManager(){
		m_maskInited = false;
	}
	SegmentManager::~SegmentManager(){

	}

	//Public

	SegmentManager* SegmentManager::m_instance = NULL;

	void SegmentManager::SetSrcImage(IplImage* src){
		m_srcImg = src;
	}
	IplImage* SegmentManager::GenerateGrabCut(int iteCnt, int downSampleCnt){
		if (!m_maskInited || m_srcImg==NULL)
			return NULL;
		using namespace cv;
		Mat fgdModel, bgdModel;

		IplImage* downSrcImg = ImageHelper::DownSample(m_srcImg, downSampleCnt);
		IplImage* downMaskImg = ImageHelper::DownSample(&(IplImage(m_mask)), downSampleCnt);
		Mat downMaskMat = cvarrToMat(downMaskImg);

		grabCut(cvarrToMat(downSrcImg), downMaskMat, cv::Rect(), bgdModel, fgdModel, iteCnt, cv::GC_INIT_WITH_MASK);

		for (int i = 0; i < downMaskImg->height; i++){
			for (int j = 0; j < downMaskImg->width; j++){
				uchar label = downMaskMat.at<uchar>(i, j);
				ImageHelper::SetElem(downMaskImg, j, i, (label == cv::GC_FGD || label == cv::GC_PR_FGD) ? LABEL_TOP : LABEL_BOTTOM);
			}
		}

		IplImage* grabcut = ImageHelper::UpSample(downMaskImg, downSampleCnt);

		cvReleaseImage(&downSrcImg);
		cvReleaseImage(&downMaskImg);

		return grabcut;
	}
	void SegmentManager::DrawMaskPoint(Vector2 p, int radius, cv::GrabCutClasses type){
		if (!m_maskInited){
			ClearMask();
			m_maskInited = true;
		}
		circle(m_mask, cv::Point(p.X(), p.Y()), radius, type, -1);
	}
	void SegmentManager::ClearMask(){
		if (m_srcImg == NULL)
			return;
		m_mask = cv::Mat(m_srcImg->height, m_srcImg->width, CV_8UC1);
		for (int i = 0; i < m_srcImg->height; i++){
			for (int j = 0; j < m_srcImg->width; j++){
				m_mask.at<uchar>(i, j) = cv::GC_PR_BGD;
			}
		}
		m_maskInited = false;
	}

	SegmentManager* SegmentManager::Instance() {
		if (m_instance == NULL) {
			m_instance = new SegmentManager();
		}
		return m_instance;
	}
}