
#pragma once 
#define DLL_API __declspec(dllexport)
#include <iostream>
#include <vector>
#include <fstream>

#include "opencv2/core/core.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

using namespace std;

class DLL_API configController
{
private:
	vector<Rect> mListRect;
	ifstream mFile;



public:
	vector<char*> *mRectClass;		// 每一矩形框的候选类
	vector<char*> *mRectLabelTextClass;		// 每一候选类的实际标签名
	vector<int> *mRectClassSubLabel;	// 每一个矩形框对应的下级菜单类编号

	vector<vector<char*>> *mRectClassVector;	//每一矩形框的候选类可变长度
	vector<vector<char*>> *mRectLabelTextClassVector;		//每一候选类的实际标签名可变长度



	int mRectCount;

	int openFile(char *rectFile);
	int readFileRect();
	int writeFileRect(char* rectFile);
	int readRectCount();
	vector<Rect> * listRect();
	vector<char*> *getRectClassPointer(int index);
	int updateRect(vector<Rect> &mList);
	int addRectTextIndex(char* text, int index, char* labelText);	//增加矩形分析文本
	int addRectLabelTextIndex(char* text, int index);	//增加矩形类别文本
	

};