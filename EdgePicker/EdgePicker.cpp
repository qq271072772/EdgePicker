#include "EdgePicker.h"

namespace EP{
	//----------------------------Public
	void EdgePicker::PickEdge(char* srcFile, char* edgeFile, char* configFile, char* outputFile, char* outlineFile){
		IplImage* src = ImageHelper::LoadImage(srcFile);
		if (src == NULL){
			cout << "Source file load failed!" << endl;
			return;
		}
		List<List<Vector2>> refEdges = LoadEdges(edgeFile);
		if (refEdges.Count() == 0){
			cout << "Edges file load failed!" << endl;
			return;
		}
		LoadConfig(configFile);
		if (outputFile == NULL){
			cout << "Output filename not defined!" << endl;
			return;
		}

		//Step 1
		ShearImage(&src, refEdges, TRASH_PIXELS);
		ImageHelper::SaveImage("shearedSrc.jpg", src);
		//Step 2
		IplImage* figure = CreateAutoGrabCut(src, refEdges, GC_DOWN_SAMPLE_CNT, GC_ITE_CNT, GC_BRUSH_RADIUS);
		//Step 3
		IplImage* refMask = CreateRefMask(src, figure, refEdges);
		ImageHelper::SaveImage("refMask.jpg", refMask);
		//Step 4
		AutoConfigure(figure, refMask);
		ImageHelper::SaveImage("figure.jpg", figure);
		//Step 5
		List<List<Vector2>> figureEdges = GenerateEdgeData(figure);
		DrawEdges(src, figureEdges, DRAE_EDGE_COLOR);
		if (outlineFile != NULL)
			ImageHelper::SaveImage(outlineFile, src);
		//Step 6
		OutputEdges(outputFile, src, figureEdges);
		
		ImageHelper::ReleaseImage(&src);
		ImageHelper::ReleaseImage(&figure);
		ImageHelper::ReleaseImage(&refMask);
	}

	//----------------------------Private
	EdgePicker* EdgePicker::m_instance = NULL;
	List<List<Vector2>> EdgePicker::LoadEdges(char* filename){
		List<List<Vector2>> edges;
		edges.Clear();
		char buffer[256];
		const char* split = "	";
		std::ifstream file(filename);
		if (!file.is_open())
			return edges;

		//Read edges
		int edgeIndex = -1;
		while (!file.eof()){
			file.getline(buffer, 100);
			char* token1, *token2, *nextToken;
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

		return edges;
	}
	void EdgePicker::LoadConfig(char* filename){
		char buffer[256];
		const char* split = " ";
		std::ifstream file(filename);
		if (!file.is_open())
			return;

		while (!file.eof()){
			file.getline(buffer, 100);
			char* token1, *token2, *nextToken;
			token1 = strtok_s(buffer, split, &nextToken);
			token2 = strtok_s(NULL, split, &nextToken);
			if (token1 == NULL || token2 == NULL)
				continue;
			if (!Tools::IsDigit(token2[0]))
				continue;
			if (strcmp(token1, "Extraction ") == 0)
				strcpy_s(EXTRACTION, token2);
			if (strcmp(token1, "TrashPixels") == 0)
				TRASH_PIXELS = atoi((const char*)token2);
			if (strcmp(token1, "DownSampleCnt") == 0)
				GC_DOWN_SAMPLE_CNT = atoi((const char*)token2);
			if (strcmp(token1, "ErosionCnt") == 0)
				EROSION_CNT = atoi((const char*)token2);
			if (strcmp(token1, "DilationCnt") == 0)
				DILATION_CNT = atoi((const char*)token2);
			if (strcmp(token1, "EdgeColor")==0){
				DRAE_EDGE_COLOR.r = atoi((const char*)token2);
				token2 = strtok_s(NULL, split, &nextToken); 
				if (token2!=NULL)
					DRAE_EDGE_COLOR.g = atoi((const char*)token2);
				token2 = strtok_s(NULL, split, &nextToken);
				if (token2 != NULL)
					DRAE_EDGE_COLOR.b = atoi((const char*)token2);
			}
		}
	}
	
	//Step Function
	void EdgePicker::ShearImage(IplImage** srcAddress, List<List<Vector2>>&refEdges, int trashPixels){
		IplImage* src = *srcAddress;
		if (src->height <= 2 * trashPixels || src->width <= 2 * trashPixels || trashPixels==0)
			return;
		cvSetImageROI(src, CvRect(trashPixels, trashPixels, src->width - 2*trashPixels, src->height - 2*trashPixels));
		IplImage* sheared = ImageHelper::CreateImage(src->width - 2 * trashPixels, src->height - 2 * trashPixels, 
			src->depth, src->nChannels);
		cvCopy(src, sheared);
		cvResetImageROI(src);
		ImageHelper::ReleaseImage(srcAddress);
		*srcAddress = sheared;

		for (int i = 0; i < refEdges.Count(); i++){
			for (int j = 0; j < refEdges[i].Count(); j++){
				Vector2 p = refEdges[i][j];
				refEdges[i][j] = Vector2(p.X() - trashPixels, p.Y() - trashPixels);
				if (refEdges[i][j].X() < 0 || refEdges[i][j].Y() < 0 || 
					refEdges[i][j].X() >= sheared->width || refEdges[i][j].Y() >= sheared->height ){
					refEdges[i].RemoveAt(j);
					j--;
				}
			}
		}
	}
	IplImage* EdgePicker::CreateAutoGrabCut(IplImage* src, List<List<Vector2>>& refEdges, int downSampleCnt, int iteCnt, int brushRadius){
		if (src == NULL)
			return NULL;
		SegmentManager* segMgr = SegmentManager::Instance();
		segMgr->SetSrcImage(src);
		segMgr->ClearMask();

		//Find Polygon with max points
		int maxPointsCnt = 0;
		int index = -1;
		for (int i = 0; i < refEdges.Count(); i++){
			if (refEdges[i].Count()>maxPointsCnt){
				maxPointsCnt = refEdges[i].Count();
				index = i;
			}
		}
		if (index < 0)
			return NULL;

		//Find Center
		Vector2 center(0, 0);
		for (int i = 0; i < refEdges[index].Count(); i++)
			center = center + refEdges[index][i];
		center = center / refEdges[index].Count();

		//Foreach point, find a proper place to draw mask
		for (int i = 0; i < refEdges[index].Count(); i++){
			Vector2 p = refEdges[index][i];
			Vector2 dir = center - p;
			p = p + dir.Normalized() * 2 * brushRadius;
			p = Vector2((int)p.X(), (int)p.Y());
			segMgr->DrawMaskPoint(p, brushRadius, cv::GC_FGD);
			p = p + dir.Normalized() * 3 * brushRadius;
			p = Vector2((int)p.X(), (int)p.Y());
			segMgr->DrawMaskPoint(p, brushRadius, cv::GC_FGD);
		}

		return segMgr->GenerateGrabCut(iteCnt, downSampleCnt);
	}
	IplImage* EdgePicker::CreateRefMask(IplImage* src, IplImage* figure, List<List<Vector2>>& refEdges){
		if (src == NULL || figure == NULL)
			return NULL;
		List<List<Vector2>> figureEdges = GenerateEdgeData(figure);
		IplImage* mask = ImageHelper::CreateImage(src->width, src->height, src->depth, src->nChannels);
		cvZero(mask);
		
		List<Box2D> refBoxes;
		for (int i = 0; i < refEdges.Count(); i++)
			refBoxes.Add(GenerateEdgeBox(refEdges[i]));

		List<List<Vector2>> pairEdges;
		for (int i = 0; i < figureEdges.Count(); i++){
			Box2D box = GenerateEdgeBox(figureEdges[i]);
			double minDiff = DBL_MAX;
			int index = -1;
			for (int j = 0; j < refEdges.Count(); j++){
				if (refEdges[j].Count() <= 0)
					continue;
				double boxDiff = BoxDiff(box, refBoxes[j]);
				if (boxDiff < minDiff){
					minDiff = boxDiff;
					index = j;
				}
			}
			if (index < 0){
				cout << "Warning:Reference edge not found!" << endl;
				return NULL;
			}
			pairEdges.Add(refEdges[index]);
		}
		DrawEdges(mask, pairEdges, U_RGB(255, 255, 255));
		return mask;
	}
	void EdgePicker::AutoConfigure(IplImage* figure, IplImage* refMask){
		if (figure == NULL || refMask == NULL)
			return;
		CoordinateFigure(figure, EROSION_CNT, 0);
		IplImage* tmp_figure = ImageHelper::CreateCopy(figure);
		List<List<Vector2>> figureEdges;
		int maxMatchCnt = 0;
		int dilationCnt = 0;
		for (int i = 0; i < EROSION_CNT + 5; i++){
			figureEdges = GenerateEdgeData(tmp_figure);
			int matchCnt = PointMatchCnt(figureEdges, refMask);
			if (matchCnt > maxMatchCnt){
				maxMatchCnt = matchCnt;
				dilationCnt = i + EXTRA_DILATION_CNT;
			}
			CoordinateFigure(tmp_figure, 0, 1);
		}
		CoordinateFigure(figure, 0, dilationCnt);

		ImageHelper::ReleaseImage(&tmp_figure);
	}

	List<List<Vector2>> EdgePicker::GenerateEdgeData(IplImage* figure){
		List<List<Vector2>> edges;
		if (figure == NULL || figure->nChannels != 1)
			return edges;

		IplImage* _figure = ImageHelper::CreateCopy(figure);
		CvMemStorage* edgeMem = cvCreateMemStorage();
		CvSeq* edgeSeq = NULL;
		cvFindContours(_figure, edgeMem, &edgeSeq, sizeof(CvContour), 1, CV_CHAIN_APPROX_SIMPLE);

		while (edgeSeq != NULL){
			edges.Add(List<Vector2>());
			for (int i = 0; i < edgeSeq->total; i++){
				CvPoint* p = (CvPoint*)cvGetSeqElem(edgeSeq, i);
				edges[edges.Count() - 1].Add(Vector2(p->x, p->y));
			}
			edgeSeq = edgeSeq->h_next;
		}

		cvReleaseMemStorage(&edgeMem);
		ImageHelper::ReleaseImage(&_figure);

		return edges;
	}

	//Internal Helper Function
	void EdgePicker::CoordinateFigure(IplImage* figure, int erosion, int dilation){
		if (figure == NULL || figure->nChannels != 1)
			return;
		if (erosion > 0)
			cvErode(figure, figure, NULL, erosion);
		if (dilation > 0)
			cvDilate(figure, figure, NULL, dilation);
	}
	Box2D EdgePicker::GenerateEdgeBox(List<Vector2>& edge){
		int maxX = 0, maxY = 0;
		int minX = INT_MAX, minY = INT_MAX;
		for (int i = 0; i < edge.Count(); i++){
			if (edge[i].X()>maxX)
				maxX = edge[i].X();
			if (edge[i].X() < minX)
				minX = edge[i].X();
			if (edge[i].Y() > maxY)
				maxY = edge[i].Y();
			if (edge[i].Y() < minY)
				minY = edge[i].Y();
		}
		return Box2D(Vector2(minX, maxY), Vector2(maxX, minY));
	}
	double EdgePicker::BoxDiff(Box2D& box1, Box2D& box2){
		double diffLeft = box1.Left() - box2.Left();
		double diffRight = box1.Right() - box2.Right();
		double diffTop = box1.Top() - box2.Top();
		double diffBot = box1.Bottom() - box2.Bottom();
		return sqrt(diffLeft*diffLeft + diffRight*diffRight + diffTop*diffTop + diffBot*diffBot);
	}
	int EdgePicker::PointMatchCnt(List<List<Vector2>>& edges, IplImage* mask){
		int ret = 0;
		for (int i = 0; i < edges.Count(); i++){
			for (int j = 0; j < edges[i].Count(); j++){
				U_RGB value = ImageHelper::SampleElemRGB(mask, edges[i][j].X(), edges[i][j].Y());
				if (value.r == 255 && value.g == 255 && value.b == 255)
					ret++;
			}
		}
		return ret;
	}

	//Tools
	void EdgePicker::DrawEdges(IplImage* src, List<List<Vector2>> &edges, U_RGB color){
		if (src == NULL)
			return;
		for (int i = 0; i < edges.Count(); i++){
			for (int j = 1; j < edges[i].Count(); j++){
				cvLine(src, CvPoint(edges[i][j - 1].X(), edges[i][j - 1].Y()), CvPoint(edges[i][j].X(), edges[i][j].Y()), CvScalar(color.b, color.g, color.r));
			}
			cvLine(src, CvPoint(edges[i][edges[i].Count() - 1].X(), edges[i][edges[i].Count() - 1].Y()),
				CvPoint(edges[i][0].X(), edges[i][0].Y()), CvScalar(color.b, color.g, color.r));
		}
	}
	void EdgePicker::OutputEdges(char* filename, IplImage* src, List<List<Vector2>>& edges){
		if (src == NULL)
			return;
		char buffer[256];
		const char* split = "	";
		std::ofstream file(filename, ios_base::trunc);
		file << "width " << src->width << endl;
		file << "height " << src->height << endl;
		for (int i = 0; i < edges.Count(); i++){
			file << endl;
			for (int j = 0; j < edges[i].Count(); j++)
				file << edges[i][j].X() << "	" << edges[i][j].Y() << endl;
		}
	}

}