#include "EdgePicker.h"

using namespace EP;

int main(int argCnt,char** args){
	EdgePicker* edgePicker = EdgePicker::Instance();
	edgePicker->LoadSrcImage("E:\\Project\\EdgePicker\\Assets\\Image4.jpg");
	edgePicker->LoadGrabCutImage("E:\\Project\\EdgePicker\\Assets\\GrabCut4.jpg");
	edgePicker->PickEdge();
	//edgePicker->LoadEdges("E:\\Project\\EdgePicker\\Assets\\Image2.txt");
	edgePicker->Destroy();

	//IplImage* test = cvLoadImage(args[1],CV_LOAD_IMAGE_UNCHANGED);
	//cout << args[1] << endl;
	//if (test == NULL)
	//	cout << "test is null" << endl;
	//int x;
	//cin >> x;
}