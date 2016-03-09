#include "EdgePicker.h"
#include "IOHelper.h"
#include<windows.h>
#include <strsafe.h>

using namespace std;
using namespace EP;
using namespace Utility;

int main(int argCnt,char** args){
	EdgePicker* edgePicker = EdgePicker::Instance();

	TCHAR buffer[MAX_PATH],sourcePath[MAX_PATH], grabcutPath[MAX_PATH], outputPath[MAX_PATH],configFile[MAX_PATH];
	IOHelper::CurrentDirectory(buffer);
	StringCchCopy(sourcePath, MAX_PATH, buffer);
	StringCchCopy(grabcutPath, MAX_PATH, buffer);
	StringCchCopy(outputPath, MAX_PATH, buffer);
	StringCchCopy(configFile, MAX_PATH, buffer);

	StringCchCat(sourcePath, MAX_PATH, TEXT("\\Source\\"));
	StringCchCat(grabcutPath, MAX_PATH, TEXT("\\GrabCut\\"));
	StringCchCat(outputPath, MAX_PATH, TEXT("\\Output\\"));
	StringCchCat(configFile, MAX_PATH, TEXT("\\Config.txt"));

	List < TCHAR*> srcs = IOHelper::ListDirectoryFiles(sourcePath);

	char config[MAX_PATH];
	wcstombs_s(NULL, config, configFile, wcsnlen_s(configFile, MAX_PATH) + 1);
	edgePicker->LoadConfig(config);

	for (int i = 0; i < srcs.Count(); i++){
		TCHAR sourceFile[MAX_PATH], grabcutFILE[MAX_PATH], outputFile[MAX_PATH];
		StringCchCopy(sourceFile, MAX_PATH, sourcePath);
		StringCchCopy(grabcutFILE, MAX_PATH, grabcutPath);
		StringCchCopy(outputFile, MAX_PATH, outputPath);
		StringCchCat(sourceFile, MAX_PATH, srcs[i]);
		StringCchCat(grabcutFILE, MAX_PATH, srcs[i]);
		StringCchCat(outputFile, MAX_PATH, srcs[i]);

		char filename[MAX_PATH];
		char grabfilename[MAX_PATH];
		char outputfilename[MAX_PATH];

		wcstombs_s(NULL, filename, sourceFile, wcsnlen_s(sourceFile, MAX_PATH) + 1);
		wcstombs_s(NULL, grabfilename, grabcutFILE, wcsnlen_s(grabcutFILE, MAX_PATH) + 1);
		wcstombs_s(NULL, outputfilename, outputFile, wcsnlen_s(outputFile, MAX_PATH) + 1);

		cout << filename << "(start)" << endl;
		if (!edgePicker->LoadSrcImage(filename)){
			cout << filename << "(could not open source)" << endl;
			continue;
		}
		if (!edgePicker->LoadGrabCutImage(grabfilename)){
			cout << filename << "could not open grabcut" << endl;
			continue;
		}
		//edgePicker->LoadEdges("E:\\Project\\EdgePicker\\Assets\\Image3.txt");
		edgePicker->PickEdge();
		edgePicker->SaveDstImage(outputfilename);
		cout << filename << "(complete)" << endl;
	}

	IOHelper::ReleaseDirectoryList(srcs);

	edgePicker->Destroy();

	system("pause");
}