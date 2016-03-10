#include <iostream>
#include "EdgePicker.h"

using namespace std;
using namespace EP;

int main(int argCnt,char** args){
	char* srcFile, *edgeFile, *configFile, *outputFile;
	srcFile = edgeFile = configFile = outputFile = NULL;
	for (int i = 1; i < argCnt - 1; i++){
		if (strcmp(args[i], "-in") == 0)
			srcFile = args[i + 1];
		if (strcmp(args[i], "-pt") == 0)
			edgeFile = args[i + 1];
		if (strcmp(args[i], "-cnt") == 0)
			configFile = args[i + 1];
		if (strcmp(args[i], "-out") == 0)
			outputFile = args[i + 1];
	}
	EdgePicker* edgePicker = EdgePicker::Instance();
	edgePicker->PickEdge(srcFile, edgeFile, configFile, outputFile);
	//edgePicker->PickEdge("E:\\�ձ��Z�ƥ���\\Image1.jpg",
	//	"E:\\�ձ��Z�ƥ���\\Image1.txt",
	//	"E:\\�ձ��Z�ƥ���\\Config.txt",
	//	"E:\\�ձ��Z�ƥ���\\Image3Out.txt");
	edgePicker->Destroy();
}