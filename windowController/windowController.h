
#pragma once 
#define DLL_IMPLEMENT
#ifdef DLL_IMPLEMENT
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#include "opencv2/core/core.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "leptonica/allheaders.h"
#include "tesseract/baseapi.h"
//#include "tesseract/renderer.h"	//version 3.03
#include "configController.h"

#define MAX_WINDOW 3 //suppose there are 3 windows to show images


using namespace cv;

class DLL_API windowController
{
private:
	Mat oldimg, newimg;
	VideoCapture vCapture;
	int mvHeight, mvWidth;
	HWND mMainWindow[MAX_WINDOW];
	Rect mWindowRect[MAX_WINDOW];

	int mProcessState;

	int totalFrames;
	configController mConfig;

	char ** mRectResultText;	//���ο��ڷ�������ı�
	char ** mRectLabelText;		//���ο��ڽ������ı�
	
	int frameCount;

	tesseract::TessBaseAPI mTessApi;

	DWORD dwThreadID;
	HANDLE hThread;

public:
	windowController();
	int openVideo(char* filename);
	int getVideoHeight();
	int getVideoWidth();
	int getVideoPosNow();
	int readAnImage(Mat &image);
	int videoJumpToSeqs(int frameCount);
	int image2Text(Mat &image, char * &str);
	int initClass();
	int findText();
	int readConfigs(char *filename);
	int writeConfigs(char *filename);
	int findTextClass(char* text, vector<char*> &allClass);
	int startProcess();
	int stopProcess();
	int pauseProcess();
	int setWindowHwnd(HWND hwnd, int number, Rect &rect);
	int showImage(Mat &image, int number);
	int startProcessThread();
	int getTextNumber(char* &text, int number, char* &labelText);
	vector<Rect> *getListRect();
	int addRectText(char* &text, int index, char* &labelText);	//���Ӿ��η����ı�������ı�
	int updateRect(vector<Rect> &mListRect);
	int addRectLabelText(char* &text, int index);	//���Ӿ���ʵ������ı�
};