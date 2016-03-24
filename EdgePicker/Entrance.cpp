#include <iostream>
#include "EdgePicker.h"

using namespace std;
using namespace EP;

int main(int argCnt,char** args){
	char* srcFile, *edgeFile, *configFile, *outputFile, *outlineFile;
	srcFile = edgeFile = configFile = outputFile = outlineFile = NULL;
	for (int i = 1; i < argCnt - 1; i++){
		if (strcmp(args[i], "-in") == 0)
			srcFile = args[i + 1];
		if (strcmp(args[i], "-pt") == 0)
			edgeFile = args[i + 1];
		if (strcmp(args[i], "-cnt") == 0)
			configFile = args[i + 1];
		if (strcmp(args[i], "-out") == 0)
			outputFile = args[i + 1];
		if (strcmp(args[i], "-bmp") == 0)
			outlineFile = args[i + 1];
	}
	EdgePicker* edgePicker = EdgePicker::Instance();
	edgePicker->PickEdge(srcFile, edgeFile, configFile, outputFile,outlineFile);
	//edgePicker->PickEdge("E:\\Project\\EdgePicker\\Assets\\Image1.jpg", "E:\\Project\\EdgePicker\\Assets\\Image1.txt", 
	//	"E:\\Project\\EdgePicker\\Assets\\Config.txt", "output.txt", "outline.jpg");
	edgePicker->Destroy();
}