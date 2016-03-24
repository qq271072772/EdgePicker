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
		int DILATION_CNT = 3;
		int GC_DOWN_SAMPLE_CNT = 2;

		const int GC_ITE_CNT = 10;
		const int GC_BRUSH_RADIUS = 3;

		List<List<Vector2>> LoadEdges(char* filename);
		void LoadConfig(char* filename);

		void ShearImage(IplImage** src, List<List<Vector2>>&edges, int trashPixels);
		IplImage* AutoGrabCut(IplImage* src, List<List<Vector2>>& edges,int downSampleCnt,int iteCnt,int brushRadius);

		void CoordinateFigure(IplImage* figure, int erosion, int dilation);
		List<List<Vector2>> GenerateEdgeData(IplImage* figure);
		Box2D GenerateEdgeBox(List<Vector2>& edge);
		double BoxDiff(Box2D& box1, Box2D& box2);
		double BoxDiff(List < List<Vector2>> edges, List<List<Vector2>>refEdges,IplImage* src);
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