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
	void EdgePicker::LoadEdges(char* filename){
		char buffer[256];
		const char* split = "	";
		std::ifstream file(filename);
		if (!file.is_open()){
			cout << "File:" << filename << " not opened!" << endl;
			return;
		}

		//Read edges
		int edgeIndex = -1;
		while (!file.eof()){
			file.getline(buffer, 100);
			char* token1,*token2, *nextToken;
			token1 = strtok_s(buffer, split, &nextToken);
			token2 = strtok_s(NULL, split, &nextToken);
			if (token1 == NULL){
				edges.Add(List<Vector2>());
				edgeIndex++;
			}
			if (token1 != NULL && token2 != NULL){
				if (Tools::IsDigit(token1[0]) && Tools::IsDigit(token2[0])){
					Vector2 p(atoi((const char*)token1), atoi((const char*)token2));
					edges[edgeIndex].Add(p);
				}
			}
		}

		//Clear edges that have no points
		for (int i = edges.Count() - 1; i >= 0; i--){
			if (edges[i].Count() == 0)
				edges.RemoveAt(i);
			else
				break;
		}

		IplImage* test = cvCreateImage(CvSize(m_src->width, m_src->height), IPL_DEPTH_8U, 1);
		cvZero(test);
		cv::Mat testMat = cv::cvarrToMat(test);
		for (int i = 0; i < edges.Count(); i++){
		//	if (edges[i].Count()>100)
				for (int j = 1; j < edges[i].Count(); j++){
					cvLine(test, cvPoint(edges[i][j - 1].X(), edges[i][j - 1].Y()), cvPoint(edges[i][j].X(), edges[i][j].Y()), CV_RGB(0, 0, 255));
					//cvCircle(test, cvPoint(edges[i][j].X(), edges[i][j].Y()), 3, CV_RGB(0, 0, 255), 3);
					//ImageHelper::SetElem(test, edges[i][j].X(), edges[i][j].Y(), 255);
				}
				cvLine(test, cvPoint(edges[i][edges[i].Count() - 1].X(), edges[i][edges[i].Count() - 1].Y()),
					cvPoint(edges[i][0].X(), edges[i][0].Y()), CV_RGB(0, 0, 255));
		}
		IplImage* test2 = cvCreateImage(CvSize(m_src->width*0.5, m_src->height*0.5), IPL_DEPTH_8U, 1);
		cvResize(test, test2,CV_INTER_LINEAR);
		cvShowImage("test", test2);
		cvWaitKey();
		cvDestroyAllWindows();
		cvReleaseImage(&test);
		cvReleaseImage(&test2);
	}
}