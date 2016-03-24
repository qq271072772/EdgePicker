#ifndef EDGE_PICKER
#define EDGE_PICKER
#include <iostream>
#include <fstream>
#include "Utility.h"
#include "ImageHelper.h"
#include "SegmentManager.h"

using namespace std;
using namespace Utility;

namespace EP{
	class EdgePicker{
	private:
		static EdgePicker* m_instance;

		char EXTRACTION[_MAX_PATH];
		int TRASH_PIXELS = 0;
		int EROSION_CNT = 3;
		int DILATION_CNT = 3;						//Removed
		int GC_DOWN_SAMPLE_CNT = 2;

		U_RGB DRAE_EDGE_COLOR = U_RGB(255, 255, 255);

		const int EXTRA_DILATION_CNT = 2;
		const int GC_ITE_CNT = 10;
		const int GC_BRUSH_RADIUS = 3;

		List<List<Vector2>> LoadEdges(char* filename);
		void LoadConfig(char* filename);

		void ShearImage(IplImage** src, List<List<Vector2>>&refEdges, int trashPixels);
		IplImage* CreateAutoGrabCut(IplImage* src, List<List<Vector2>>& refEdges, int downSampleCnt, int iteCnt, int brushRadius);
		IplImage* CreateRefMask(IplImage* src, IplImage* figure, List<List<Vector2>>& refEdges);
		void AutoConfigure(IplImage* figure, IplImage* refMask);

		List<List<Vector2>> GenerateEdgeData(IplImage* figure);

		void CoordinateFigure(IplImage* figure, int erosion, int dilation);
		Box2D GenerateEdgeBox(List<Vector2>& edge);
		double BoxDiff(Box2D& box1, Box2D& box2);
		int PointMatchCnt(List<List<Vector2>>& edges, IplImage* mask);

		void DrawEdges(IplImage* src, List<List<Vector2>> &edges, U_RGB color);
		void OutputEdges(char* filename, IplImage* src, List<List<Vector2>>& edges);

	public:

		static EdgePicker* Instance(){
			if (m_instance == NULL)
				m_instance = new EdgePicker();
			return m_instance;
		}

		EdgePicker(){
		}
		~EdgePicker(){
		}

		void PickEdge(char* srcFile, char* edgeFile, char* configFile, char* outputFile, char* outlineFile);
		
		void Destroy(){
			m_instance = NULL;
			delete this;
		}
	};
}
#endif