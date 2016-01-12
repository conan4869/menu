
#include "windowController.h"


#define PROCESSSTART 1
#define PROCESSPAUSE 2	
#define PROCESSEND 3

const char* windowName[MAX_WINDOW] = { "sourceview", "detectview", "dbview" };


using namespace std;


static DWORD WINAPI processVideo(void *pArg);

windowController::windowController()
{
}

int windowController::initClass()
{
	int ret = 0;
	ret = mTessApi.Init("../", "test", tesseract::OEM_DEFAULT);
	if (ret < 0)
	{
		return ret;
	}
	mTessApi.SetPageSegMode(tesseract::PSM_SINGLE_WORD);
	return ret;
}

int windowController::openVideo(char* filename)
{
	if(vCapture.open(filename)<0)
		return -1;
	totalFrames = vCapture.get(CV_CAP_PROP_FRAME_COUNT);
	mvHeight = vCapture.get(CV_CAP_PROP_FRAME_HEIGHT);
	mvWidth = vCapture.get(CV_CAP_PROP_FRAME_WIDTH);
//	vCapture.set(CV_CAP_PROP_POS_FRAMES, 634);
	vCapture.set(CV_CAP_PROP_POS_FRAMES, 9050);

	return 0;
}

int windowController::getVideoHeight()
{
	return mvHeight;
}

int windowController::getVideoWidth()
{
	return mvWidth;
}

int windowController::readAnImage(Mat &image)
{
	if (vCapture.isOpened())
	{
		newimg.copyTo(oldimg);
		if (vCapture.read(newimg) < 0 || newimg.cols == 0 || newimg.rows == 0)
		{
			return -1;
		}
		if (oldimg.data == NULL)
			newimg.copyTo(oldimg);
		image = newimg;
		return 0;
	}
	return -1;
}

int windowController::videoJumpToSeqs(int frameCount)
{
	if (vCapture.isOpened())
	{
		return vCapture.set(CV_CAP_PROP_POS_FRAMES, frameCount);
	}
	return -1;
}

int windowController::readConfigs(char *filename)
{
	if( mConfig.openFile(filename) < 0)
	{
		return -1;
	}
	mConfig.readFileRect();
	mRectResultText = new char *[mConfig.listRect()->size()];
	mRectLabelText = new char *[mConfig.listRect()->size()];
	mRectState = new int[mConfig.listRect()->size()];
	for (int i = 0; i < mConfig.listRect()->size(); i++)
	{
		mRectResultText[i] = NULL;
		mRectLabelText[i] = NULL;
		mRectState[i] = 0;
	}

	return 0;
}

vector<Rect> * windowController::getListRect()
{
	return mConfig.listRect();
}

int windowController::addRectText(char* &text, int index, char* &labelText)
{
	mConfig.addRectTextIndex(text, index, labelText);
	return 0;
}

int windowController::addRectLabelText(char* &text, int index)
{
	mConfig.addRectLabelTextIndex(text, index);
	return 0;
}

int windowController::updateRect(vector<Rect> &mListRect)
{
	mConfig.updateRect(mListRect);
	return 0;
}

int windowController::writeConfigs(char *filename)
{
//	mConfig.mRectCount = mRectCount;
	return mConfig.writeFileRect(filename);
}

int windowController::image2Text(Mat &image, char * &str)
{
	Mat tmp;
	image.copyTo(tmp);
	int width = tmp.cols;
	int height = tmp.rows;
	int nBytes = tmp.channels();
	mTessApi.SetImage(tmp.data, width, height, nBytes, width*nBytes);
	char *outText = mTessApi.GetUTF8Text();
	int i = MultiByteToWideChar(CP_UTF8, 0, outText, -1, NULL, 0);
	WCHAR *strUnicode = new WCHAR[i];
	MultiByteToWideChar(CP_UTF8, 0, outText, -1, strUnicode, i);
	i = WideCharToMultiByte(CP_ACP, 0, strUnicode, -1, NULL, 0, NULL, NULL);
	str = new char[i];
	WideCharToMultiByte(CP_ACP, 0, strUnicode, -1, str, i, NULL, NULL);
	int k;
	int j = 0;
	for (k = 0; k < i; k++)
	{
		if (str[k] == ' ' || str[k] == '\n')
			str[k] = '_';
	}
//	delete[] outText;
	delete[] strUnicode;
	return 0;
}

int windowController::findText()
{
	Mat imgDiff;
	absdiff(oldimg, newimg, imgDiff);
	vector<Rect>::iterator rectIt;
	char *tmpText = NULL;
	int i = 0;
	if ((mConfig.listRect())->empty())
		return -1;
	for (rectIt = (mConfig.listRect())->begin(); rectIt != (mConfig.listRect())->end(); rectIt++)
	{
		Mat tmpMat = imgDiff(*rectIt);
		Mat tmpMatGray;
		cvtColor(tmpMat, tmpMatGray, CV_BGR2GRAY);
		int nonZeros = countNonZero(tmpMatGray > 20);
		if (nonZeros <= 10 && mRectResultText[i] != NULL)
		{
			i++;
			continue;
		}

 		image2Text(newimg(*rectIt), tmpText);
 		int classLabel = findTextClass(tmpText, (* mConfig.getRectClassPointer(i)));
 		mRectResultText[i] = tmpText;
		if (classLabel < 0)
		{
			char * tmp = new char[1];
			tmp[0] = '\0';
			mRectLabelText[i] = tmp;
			mRectState[i] = 0;
		}
		else
		{
			mRectLabelText[i] = mConfig.mRectLabelTextClass[i][classLabel];
			mRectState[i] = 1;
		}

		i++;
	}

	return 0;
}

int windowController::findTextClass(char* text, vector<char*> &allClass)
{
	vector<char*>::iterator tmpClass;
	int classLabel = -1;
	int i = 0;
	for (tmpClass = allClass.begin(); tmpClass != allClass.end(); tmpClass++)
	{
		if (strcmp(text, *tmpClass) == 0)
		{
			classLabel = i;
		}
		i++;
	}
	return classLabel;
}

int windowController::startProcess()
{
	int i = 0;
	Mat tmp;
	while (mProcessState == PROCESSSTART && readAnImage(tmp) == 0)
	{
		showImage(newimg, 0);
 		findText();
		i++;
		frameCount = i;
	}
	return 0;
}

int windowController::stopProcess()
{
	mProcessState = PROCESSEND;
	return 0;
}

int windowController::pauseProcess()
{
	mProcessState = PROCESSPAUSE;
	return 0;
}

int windowController::setWindowHwnd(HWND hwnd, int number, Rect &rect)
{
	if (number < 0 || number >= MAX_WINDOW)
		return -1;
	mMainWindow[number] = hwnd;
	namedWindow(windowName[number], WINDOW_AUTOSIZE);
	HWND hWnd = (HWND)cvGetWindowHandle(windowName[number]);
	HWND hParent = ::GetParent(hWnd);
	::SetParent(hWnd,hwnd);
	::ShowWindow(hParent, SW_HIDE);
	mWindowRect[number] = rect;
	return 0;
}

int windowController::showImage(Mat &image, int number)
{
	if (number < 0 || number >= MAX_WINDOW)
		return -1;
	Mat resizeFrame;
	resize(image, resizeFrame, Size(mWindowRect[number].width,mWindowRect[number].height));
	imshow(windowName[number],resizeFrame);	
	waitKey(1);
}

int windowController::getVideoPosNow()
{
	if (vCapture.isOpened())
	{
		return vCapture.get(CV_CAP_PROP_POS_FRAMES);
	}
	return -1;
}

int windowController::getTextNumber(char* &text, int number, char* &labelText)	//得到第number个矩形框检测结果文本，显示测试用
{
	if (number < 0 || number >= (mConfig.listRect()->size()))
		return -1;
	text = mRectResultText[number];
	labelText = mRectLabelText[number];
	return 0;
}

int windowController::getActiveLabelNum()
{
	int index = -1, max = 0;
	if (mConfig.listRect()->size() != 0)
	{
		int i = 0;
		int *tmpState = new int[mConfig.listRect()->size()];
		for (i = 0; i < mConfig.listRect()->size(); i++)
		{
			tmpState[i] = 0;
		}
		for (i = 0; i < mConfig.listRect()->size(); i++)
		{

			if (mRectState[i] == 1 && mConfig.mRectClassLabelRank[i] != 0)
			{
				tmpState[mConfig.mRectClassParentLabel[i]]++;
			}
		}
		for (i = 0; i < mConfig.listRect()->size(); i++)
		{
			if (tmpState[i] > max)
			{
				index = i;
				max = tmpState[i];
			}

		}
		delete[] tmpState;
	}

	return index;
}



int windowController::startProcessThread()
{
	dwThreadID = 0;
	mProcessState = PROCESSSTART;
	hThread = CreateThread(NULL, 0, processVideo, this, NULL, &dwThreadID);
	return 0;
}


static DWORD WINAPI processVideo(void *pArg)
{
	windowController* pWinCon = (windowController*) pArg;
	pWinCon->startProcess();

	return 0;
}
