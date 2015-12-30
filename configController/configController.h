
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
	vector<char*> *mRectClass;		// ÿһ���ο�ĺ�ѡ��
	vector<char*> *mRectLabelTextClass;		// ÿһ��ѡ���ʵ�ʱ�ǩ��
	vector<int> *mRectClassSubLabel;	// ÿһ�����ο��Ӧ���¼��˵�����

	vector<vector<char*>> *mRectClassVector;	//ÿһ���ο�ĺ�ѡ��ɱ䳤��
	vector<vector<char*>> *mRectLabelTextClassVector;		//ÿһ��ѡ���ʵ�ʱ�ǩ���ɱ䳤��



	int mRectCount;

	int openFile(char *rectFile);
	int readFileRect();
	int writeFileRect(char* rectFile);
	int readRectCount();
	vector<Rect> * listRect();
	vector<char*> *getRectClassPointer(int index);
	int updateRect(vector<Rect> &mList);
	int addRectTextIndex(char* text, int index, char* labelText);	//���Ӿ��η����ı�
	int addRectLabelTextIndex(char* text, int index);	//���Ӿ�������ı�
	

};