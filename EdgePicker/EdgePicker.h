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

		List<List<Vector2>> m_edges;

		static EdgePicker* m_instance;

		const int CHANNEL_1_WHITE = 255;
		const int COLOR_THRESHOLD = 60;

	public:

		static EdgePicker* Instance(){
			if (m_instance == NULL)
				m_instance = new EdgePicker();
			return m_instance;
		}

		EdgePicker(){
			m_edges.Clear();
		}
		~EdgePicker(){
			if (m_src != NULL)
				ImageHelper::ReleaseImage(&m_src);
		}

		bool EnsureSrc();
		void LoadSrcImage(char* filename);
		void LoadGrabCutImage(char* filename);
		void LoadEdges(char* filename);

		void PickEdge();

		IplImage* GenerateFigure(IplImage* src, IplImage* grabcut,int erosion, int dilation, RGB value);
		List<List<Vector2>> GenerateEdgeData(IplImage* edgeImg);
		
		void CoordinateEdge(List<List<Vector2>>& edge1,List<List<Vector2>>& edge2);

		Box2D GenerateEdgeBox(List<Vector2>& edge);
		double BoxDiff(Box2D& box1, Box2D& box2);

		void DebugDrawEdges(List<List<Vector2>> edges, RGB color);
		
		void Destroy(){
			delete this;
		}
	};
}