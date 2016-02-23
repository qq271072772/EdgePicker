#include <iostream>
#include <fstream>
#include "Utility.h"
#include "ImageHelper.h"

using namespace std;
using namespace Utility;

namespace EP{
	class EdgePicker{
	private:
		IplImage* m_src;
		IplImage* m_grabcut;

		List<List<Vector2>> edges;

		static EdgePicker* m_instance;

	public:

		static EdgePicker* Instance(){
			if (m_instance == NULL)
				m_instance = new EdgePicker();
			return m_instance;
		}

		EdgePicker(){
			edges.Clear();
		}
		~EdgePicker(){
			ImageHelper::ReleaseImage(&m_src);
		}

		bool EnsureSrc();
		void LoadSrcImage(char* filename);
		void LoadGrabCutImage(char* filename);
		void LoadEdges(char* filename);
		
		void Destroy(){
			delete this;
		}
	};
}