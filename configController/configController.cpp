
#include "configController.h"



#define _CRT_SECURE_NO_WARNINGS

int configController::openFile(char *rectFile)
{
	mFile.open(rectFile);
	return mFile.is_open();
}

int configController::readRectCount()
{
	int mRectCount;
	mFile >> mRectCount;
	return mRectCount;
}

int configController::writeFileRect(char* rectFile)
{
	int flag;
	int i;
	ofstream oFile;
	oFile.open(rectFile);
	oFile << mListRect.size()<<endl;
	
	for (i = 0; i < mListRect.size(); i++)
	{
		oFile << mListRect[i].x << " " << mListRect[i].y << " " << mListRect[i].width << " " << mListRect[i].height << " ";
		oFile << mRectClass[i].size() << " ";
		for (int j = 0; j < mRectClass[i].size(); j++)
		{
			oFile << mRectClass[i][j] << " " << mRectLabelTextClass[i][j] << " ";
		}
		oFile << mRectClassSubLabel[i].size() << " ";
		for (int j = 0; j < mRectClassSubLabel[i].size(); j++)
		{
			oFile << mRectClassSubLabel[i][j] << " ";
		}
		oFile << endl;
	}
	oFile.close();
	return 0;
}

int configController::readFileRect()
{
	mListRect.clear();

	int index = 0;
	vector<int> onerect;
	Rect temprect;
	int tempnum = 0;
	mFile >> mRectCount;
	mRectClass = new vector<char*>[mRectCount];
	mRectLabelTextClass = new vector<char*>[mRectCount];
	mRectClassSubLabel = new vector<int>[mRectCount];
	mFile >> tempnum;
	char tmp[256];
	while (!mFile.eof())
	{
		onerect.push_back(tempnum);
		for (int i = 0; i < 3; i++)
		{
			mFile >> tempnum;
			onerect.push_back(tempnum);
		}
		onerect.push_back(tempnum);
		temprect.x = onerect[0];
		temprect.y = onerect[1];
		temprect.width = abs(onerect[2]);
		temprect.height = abs(onerect[3]);
		mListRect.push_back(temprect);
		onerect.clear();

		int rectClass = 0;
		mFile >> rectClass;
		if (rectClass != 0)
		{
			for (int j = 0; j < rectClass; j++)
			{
				mFile >> tmp;
				int n = strlen(tmp);
				char* tmpText = new char[n];
				strcpy(tmpText, tmp);
				mRectClass[index].push_back(tmpText);
				mFile >> tmp;
				n = strlen(tmp);
				tmpText = new char[n];
				strcpy(tmpText, tmp);
				mRectLabelTextClass[index].push_back(tmpText);
			}
		}
		int rectSubLabelCount = 0;
		mFile >> rectSubLabelCount;
		if (rectSubLabelCount != 0)
		{
			for (int j = 0; j < rectSubLabelCount; j++)
			{
				int labeltmp = -1;
				mFile >> labeltmp;
				mRectClassSubLabel[index].push_back(labeltmp);
			}
		}
		
		index++;
		mFile >> tempnum;
	}


//	fclose(fp);
	return 0;
}

vector<Rect> * configController::listRect()
{
	return &mListRect;
}

int configController::updateRect(vector<Rect> &mList)
{
	mListRect.clear();
	vector<Rect>::iterator it;
	for (it = mList.begin(); it != mList.end(); it++)
	{
		mListRect.push_back(*it);
	}
	return 0;
}

int configController::addRectTextIndex(char* text, int index, char* labelText)
{
	vector<char*>::iterator it;
	vector<char*>::iterator itLabel;
	it = mRectClass[index].begin();
	for (it = mRectClass[index].begin(), itLabel = mRectLabelTextClass[index].begin(); it != mRectClass[index].end(); it++, itLabel++)
	{
		if (strcmp(*it, text) == 0)
		{
			*itLabel = labelText;
			return -1;
		}
			
	}
	mRectClass[index].push_back(text);
	mRectLabelTextClass[index].push_back(labelText);
	
	return 0;
}

int configController::addRectLabelTextIndex(char* text, int index)
{
	return 0;
}

vector<char*> * configController::getRectClassPointer(int index)
{
	return &mRectClass[index];
}