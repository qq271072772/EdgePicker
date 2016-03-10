#ifndef SEGMENT_MANAGER
#define SEGMENT_MANAGER

#include<iostream>
#include<math.h>
#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "Utility.h"
#include "ImageHelper.h"

using namespace std;
using namespace Utility;

namespace EP {
	class SegmentManager {
		private:

			const int LABEL_TOP = 255;
			const int LABEL_BOTTOM = 0;

			IplImage* m_srcImg = NULL;
			
			cv::Mat m_mask;
			bool m_maskInited;

			static SegmentManager* m_instance;

		public:

			SegmentManager();
			~SegmentManager();

			static SegmentManager* Instance();

			void SetSrcImage(IplImage* src);
			void DrawMaskPoint(Vector2 p, int radius, cv::GrabCutClasses type);
			void ClearMask();
			IplImage* GenerateGrabCut(int iteCnt, int downSampleCnt);
	};
}

#endif