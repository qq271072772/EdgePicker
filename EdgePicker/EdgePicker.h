#include <iostream>
#include "Utility.h"
#include "ImageHelper.h"

using namespace std;
using namespace Utility;

namespace EP{
	class EdgePicker{
	private:
		IplImage* m_src;
		IplImage* m_gray;
		Dictionary<int, int> m_histogram;

		int m_pixelCnt;

		static EdgePicker* m_instance;

		const int GRAY_PIXEL = 256;
		const int PEAK_RANGE = 30;
	public:

		static EdgePicker* Instance(){
			if (m_instance == NULL)
				m_instance = new EdgePicker();
			return m_instance;
		}

		EdgePicker(){
		}
		~EdgePicker(){
			ImageHelper::ReleaseImage(&m_src);
			ImageHelper::ReleaseImage(&m_gray);
		}

		bool EnsureSrc();

		void LoadSrcImage(char* filename);
		void InitHistogram();
		void DealBottom();
		
		void Destroy(){
			delete this;
		}
	};
}