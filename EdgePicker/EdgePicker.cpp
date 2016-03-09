#include "EdgePicker.h"

namespace EP{
	EdgePicker* EdgePicker::m_instance = NULL;
	bool EdgePicker::EnsureSrc(){
		if (m_src == NULL)
			cout << "Error: src is null!" << endl;
		return m_src != NULL;
	}
	bool EdgePicker::LoadSrcImage(char* filename){
		m_src = ImageHelper::LoadImage(filename);
		return m_src != NULL;
	}
	void EdgePicker::SaveDstImage(char* filename){
		ImageHelper::SaveImage(filename, m_src);
	}
	bool EdgePicker::LoadGrabCutImage(char* filename){
		m_grabcut = ImageHelper::LoadImage(filename);
		return m_grabcut != NULL;
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
				m_edges.Add(List<Vector2>());
				edgeIndex++;
			}
			if (token1 != NULL && token2 != NULL){
				if (Tools::IsDigit(token1[0]) && Tools::IsDigit(token2[0])){
					Vector2 p(atoi((const char*)token1), atoi((const char*)token2));
					m_edges[edgeIndex].Add(p);
				}
			}
		}

		//Clear edges that have no points
		for (int i = m_edges.Count() - 1; i >= 0; i--){
			if (m_edges[i].Count() == 0)
				m_edges.RemoveAt(i);
			else
				break;
		}
	}
	void EdgePicker::LoadConfig(char* filename){
		char buffer[256];
		const char* split = " ";
		std::ifstream file(filename);
		if (!file.is_open()){
			cout << "File:" << filename << " not opened!" << endl;
			return;
		}
		while (!file.eof()){
			file.getline(buffer, 100);
			char* token1, *token2, *nextToken;
			token1 = strtok_s(buffer, split, &nextToken);
			token2 = strtok_s(NULL, split, &nextToken);
			if (token1 == NULL || token2==NULL)
				continue;
			if (!Tools::IsDigit(token2[0]))
				continue;
			if (strcmp(token1, "Erosion")==0)
				EROSION_CNT = atoi((const char*)token2);
			if (strcmp(token1, "Dilation")==0)
				DILATION_CNT = atoi((const char*)token2);
		}
	}

	void EdgePicker::PickEdge(){
		//---------------------------------------------------------------------------------------------------Method edgeBased
		//IplImage* edgeImg = ImageHelper::CreateImage(m_src->width, m_src->height, IPL_DEPTH_8U, 1);
		//cvZero(edgeImg);
		//DrawEdges(edgeImg, m_edges, RGB(255, 255, 255));
		//ImageHelper::SaveImage("edge.jpg", edgeImg);

		//IplImage* figure = FillEdges(m_edges);
		//ImageHelper::SaveImage("figure.jpg", figure);
		//CoordinateFigure(figure, 6, 11);
		//ImageHelper::SaveImage("figure_dilation.jpg", figure);
		//List<List<Vector2>> edges = GenerateEdgeData(figure);

		//DrawEdges(m_src, edges, RGB(0, 0, 255));
		//ImageHelper::SaveImage("output.jpg", m_src);

		//ImageHelper::ReleaseImage(&figure);
		//ImageHelper::ReleaseImage(&edgeImg);
		//---------------------------------------------------------------------------------------------------Method edgeBased

		//---------------------------------------------------------------------------------------------------Method grabcut
		IplImage* figure = m_grabcut;
		CoordinateFigure(figure, EROSION_CNT, DILATION_CNT);

		m_edges = GenerateEdgeData(figure);

		DrawEdges(m_src, m_edges, U_RGB(255, 255, 255));
		//---------------------------------------------------------------------------------------------------Method grabcut
	}

	void EdgePicker::CoordinateFigure(IplImage* figure, int erosion, int dilation){
		if (figure == NULL || figure->nChannels != 1)
			return;
		cvErode(figure, figure, NULL, erosion);
		cvDilate(figure, figure, NULL, dilation);
	}
	List<List<Vector2>> EdgePicker::GenerateEdgeData(IplImage* figure){
		List<List<Vector2>> edges;
		if (figure == NULL || figure->nChannels != 1)
			return edges;
	
		CvMemStorage* edgeMem = cvCreateMemStorage();
		CvSeq* edgeSeq = NULL;
		cvFindContours(figure, edgeMem, &edgeSeq, sizeof(CvContour), 1, CV_CHAIN_APPROX_SIMPLE);

		while (edgeSeq != NULL){
			edges.Add(List<Vector2>());
			for (int i = 0; i < edgeSeq->total; i++){
				CvPoint* p = (CvPoint*)cvGetSeqElem(edgeSeq, i);
				edges[edges.Count() - 1].Add(Vector2(p->x, p->y));
			}
			edgeSeq = edgeSeq->h_next;
		}

		cvReleaseMemStorage(&edgeMem);

		return edges;
	}

	void EdgePicker::CoordinateEdge(List<List<Vector2>>& edges1, List<List<Vector2>>& edges2){
		struct EdgePair{
			int index1, index2;
			double boxDiff;
			List<int> points1;
			List<int> points2;
			Dictionary<int, int>histogram;
			int peakBrightness;
		};

		List<EdgePair> edgePairs;
		List<Box2D> boxes1, boxes2;

		//generate edge box
		boxes1.Clear();
		boxes2.Clear();
		for (int i = 0; i < edges1.Count(); i++)
			boxes1.Add(GenerateEdgeBox(edges1[i]));
		for (int i = 0; i < edges2.Count(); i++)
			boxes2.Add(GenerateEdgeBox(edges2[i]));

		//find edgepairs
		for (int i = 0; i < edges1.Count(); i++){
			double minDiff = DBL_MAX;
			int target = -1;
			for (int j = 0; j < edges2.Count(); j++){
				double diff = BoxDiff(boxes1[i], boxes2[j]);
				if (diff < minDiff){
					minDiff = diff;
					target = j;
				}
			}
			EdgePair pair;
			pair.index1 = i;
			pair.index2 = target;
			pair.boxDiff = minDiff;
			edgePairs.Add(pair);
		}

		//Sort by boxDiff
		for (int i = 1; i < edgePairs.Count(); i++){
			for (int j = i ; j >= 1; j--){
				if (edgePairs[j].boxDiff < edgePairs[j-1].boxDiff){
					EdgePair tmp = edgePairs[j-1];
					edgePairs[j-1] = edgePairs[j];
					edgePairs[j] = tmp;
				}
			}
		}

		//Remove unvalid pair
		for (int i = edgePairs.Count() - 1; i > 0; i--){
			bool flag = false;
			for (int j = 0; j < i; j++)
				if (edgePairs[i].index2 == edgePairs[j].index2){
					flag = true;
					break;
				}
			if (flag)
				edgePairs.RemoveAt(i);
		}

		//Make point pair
		for (int i = 0; i < edgePairs.Count(); i++){
			int e1 = edgePairs[i].index1;
			int e2 = edgePairs[i].index2;
			for (int j = 0; j < edges1[e1].Count(); j++){
				edgePairs[i].points1.Add(j);
				double minDis = DBL_MAX;
				int p2 = -1;
				for (int k = 0; k < edges2[e2].Count(); k++){
					double dis = Vector2::Distance(edges1[e1][j], edges2[e2][k]);
					if (dis < minDis){
						minDis = dis;
						p2 = k;
					}
				}
				edgePairs[i].points2.Add(p2);
			}
		}

		//Generate historgram
		IplImage* gray = ImageHelper::Rgb2Gray(m_src);
		//for (int i = 0; i < edgePairs.Count(); i++){
		//	int e1 = edgePairs[i].index1;
		//	int e2 = edgePairs[i].index2;
		//	edgePairs[i].histogram.Clear();
		//	for (int j = 0; j < edgePairs[i].points1.Count(); j++){
		//		Vector2 p1 = edges1[e1][edgePairs[i].points1[j]];
		//		Vector2 p2 = edges2[e2][edgePairs[i].points2[j]];
		//		Box2D box(p1, p2);
		//		Line2D line(p1, p2);
		//		if (line.Perpendicular()){
		//			for (int k = box.Bottom(); k <= box.Top(); k++){
		//				uchar value = ImageHelper::SampleElem(gray, p1.X(), k);
		//				if (!edgePairs[i].histogram.ContainsKey(value))
		//					edgePairs[i].histogram.Add(value, 0);
		//				edgePairs[i].histogram[value]++;
		//			}
		//		}
		//		else{
		//			for (int k = box.Left(); k <= box.Right(); k++){
		//				int y = Line2D::Sample(line, k);
		//				uchar value = ImageHelper::SampleElem(gray, k, y);
		//				if (!edgePairs[i].histogram.ContainsKey(value))
		//					edgePairs[i].histogram.Add(value, 0);
		//				edgePairs[i].histogram[value]++;
		//			}
		//		}
		//	}
		//}

		//Find histogram's Peak
		//for (int i = 0; i < edgePairs.Count(); i++){
		//	List<int> keys = edgePairs[i].histogram.Keys();
		//	edgePairs[i].peakBrightness = -1;
		//	int maxCnt = 0;
		//	for (int j = 0; j < keys.Count(); j++){
		//		if (edgePairs[i].histogram[keys[j]]>maxCnt){
		//			maxCnt = edgePairs[i].histogram[keys[j]];
		//			edgePairs[i].peakBrightness = keys[j];
		//		}
		//	}
		//}

		//Coordinate edges
		m_edges.Clear();
		for (int i = 0; i < edgePairs.Count(); i++){
			m_edges.Add(List<Vector2>());
			int e1 = edgePairs[i].index1;
			int e2 = edgePairs[i].index2;
			for (int j = 0; j < edgePairs[i].points1.Count();j++){
				Vector2 p1 = edges1[e1][edgePairs[i].points1[j]];
				Vector2 p2 = edges2[e2][edgePairs[i].points2[j]];
				Line2D line(p1, p2);
				U_RGB baseValue = ImageHelper::SampleElemRGB(m_src, p1.X(), p1.Y());
				if (line.Vertical()){
					int inc = p1.Y() < p2.Y() ? 1 : -1;
					int targetY = -1;
					int maxbrightness = 0;
					Vector2 baseP(p1.X(),p1.Y());
					for (int k = p1.Y(); k != p2.Y(); k += inc){
						U_RGB value = ImageHelper::SampleElemRGB(m_src, p1.X(), k);
						if (ImageHelper::RGB2GRAY(value) > maxbrightness){
							maxbrightness = ImageHelper::RGB2GRAY(value);
							baseP = Vector2(p1.X(), k);
						}
					}
					//RGB baseValue = ImageHelper::SampleElemRGB(m_src, baseP.X(), baseP.Y());
					for (int k = p1.Y(); k != p2.Y(); k+=inc){
						U_RGB value = ImageHelper::SampleElemRGB(m_src, p1.X(), k);
						if (ImageHelper::RGBDiff(value, baseValue)>COLOR_THRESHOLD){
							targetY = k;
							break;
						}
					}
					if (targetY < 0)
						targetY = p2.Y();
					m_edges[i].Add(Vector2(p1.X(), targetY));
				}
				else{

					U_RGB baseValue = ImageHelper::SampleElemRGB(m_src, p1.X(), p1.Y());
					int xDiff = Math::Abs(p1.X() - p2.X());	
					int yDiff = Math::Abs(p1.Y() - p2.Y());
					double step;
					if (yDiff != 0)
						step = (double)xDiff / (double)yDiff;
					else
						step = 1;
					if (step > 1)
						step = 1;

					double realX = p1.X();
					int maxbrightness = 0;
					Vector2 baseP;
					while (true){
						int x = Math::Round(realX);
						int y = Math::Round(Line2D::Sample(line, realX));

						U_RGB value = ImageHelper::SampleElemRGB(m_src, x, y);
						if (ImageHelper::RGB2GRAY(value) > maxbrightness){
							maxbrightness = ImageHelper::RGB2GRAY(value);
							baseP = Vector2(x, y);
						}

						if (p1.X() < p2.X()){
							realX += step;
							if (realX > (p2.X()+0.0001f))
								break;
						}
						if (p1.X() > p2.X()){
							realX -= step;
							if (realX <  (p2.X() + 0.0001f))
								break;
						}
					}
					//RGB baseValue = ImageHelper::SampleElemRGB(m_src, baseP.X(), baseP.Y());

					//if (p1.X()==1130 && p1.Y()==241){
					//	int x = 1;
					//}
					int targetX = p1.X(), targetY = p1.Y();
					realX = p1.X();
					while (true){
						int x = Math::Round(realX);
						int y = Math::Round(Line2D::Sample(line, realX));

						U_RGB value = ImageHelper::SampleElemRGB(m_src, x, y);
						if (ImageHelper::RGBDiff(value, baseValue) > COLOR_THRESHOLD){
							break;
						}
						targetX = x;
						targetY = y;

						if (p1.X() < p2.X()){
							realX += step;
							if (realX >  (p2.X() + 0.0001f))
								break;
						}
						if (p1.X() > p2.X()){
							realX -= step;
							if (realX <  (p2.X() + 0.0001f))
								break;
						}
					}
					m_edges[i].Add(Vector2(targetX, targetY));
				}
			}
		}

 		ImageHelper::ReleaseImage(&gray);
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

	IplImage* EdgePicker::FillEdges(List<List<Vector2>> &edges){
		IplImage* figure = ImageHelper::CreateImage(m_src->width, m_src->height, IPL_DEPTH_8U, 1);
		cvZero(figure);
		DrawEdges(figure, edges, U_RGB(255, 255, 255));
		for (int i = 0; i < edges.Count(); i++){
			for (int j = 0; j < edges[i].Count(); j++){
				bool complete = false;
				Vector2 p = edges[i][j];
				for (int x = -1; x < 2; x++){
					for (int y = -1; y < 2; y++){
						Vector2 sampleP = Vector2(p.X() + x, p.Y() + y);
						if (IsInterior(edges[i], sampleP) && ImageHelper::SampleElem(figure, sampleP.X(), sampleP.Y()) != 255){
							cvFloodFill(figure, CvPoint(sampleP.X(), sampleP.Y()), CvScalar(255, 255, 255));
							complete = true;
							break;
						}
					}
					if (complete)
						break;
				}
				if (complete)
					break;
			}
		}
		return figure;
	}
	bool EdgePicker::IsInterior(List<Vector2> edge, Vector2 p){
		int interPointCnt = 0;
		Ray2D ray(p, Vector2(p.X() - 1, p.Y()));
		for (int i = 1; i != 0; i++){
			int index = i;
			Vector2 p1 = edge[index - 1];
			if (index == edge.Count()){
				index = 0;
				i = -1;
			}
			Vector2 p2 = edge[index];
			if (Ray2D::Intersection(ray, p1, p2))
				interPointCnt++;
		}
		return interPointCnt % 2 != 0;
	}

	void EdgePicker::DrawEdges(IplImage* src, List<List<Vector2>> &edges, U_RGB color){
		for (int i = 0; i < edges.Count(); i++){
			for (int j = 1; j < edges[i].Count(); j++){
				cvLine(src, CvPoint(edges[i][j - 1].X(), edges[i][j - 1].Y()), CvPoint(edges[i][j].X(), edges[i][j].Y()), CvScalar(color.b, color.g, color.r));
			}
			cvLine(src, CvPoint(edges[i][edges[i].Count() - 1].X(), edges[i][edges[i].Count() - 1].Y()),
				CvPoint(edges[i][0].X(), edges[i][0].Y()), CvScalar(color.b, color.g, color.r));
		}
	}
	void EdgePicker::OutputEdges(char* filename, IplImage* src,List<List<Vector2>>& edges){
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