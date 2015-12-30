// ssim.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "cv.h"
#include "highgui.h"
using namespace std;

void ssim(IplImage *srcPic, IplImage *DBPic, double& r)
{
	// default settings
	double C1 = 6.5025, C2 = 58.5225;

	IplImage
		*img1 = NULL, *img2 = NULL, *img1_img2 = NULL,
		*img1_temp = NULL, *img2_temp = NULL,
		*img1_sq = NULL, *img2_sq = NULL,
		*mu1 = NULL, *mu2 = NULL,
		*mu1_sq = NULL, *mu2_sq = NULL, *mu1_mu2 = NULL,
		*sigma1_sq = NULL, *sigma2_sq = NULL, *sigma12 = NULL,
		*ssim_map = NULL, *temp1 = NULL, *temp2 = NULL, *temp3 = NULL;


	/***************************** INITS **********************************/
	img1_temp = srcPic;
	img2_temp = DBPic;

	if (img1_temp == NULL || img2_temp == NULL)
		return ;

	int x = img1_temp->width, y = img1_temp->height;
	int nChan = img1_temp->nChannels, d = IPL_DEPTH_32F;
	CvSize size = cvSize(x, y);

	img1 = cvCreateImage(size, d, nChan);
	img2 = cvCreateImage(size, d, nChan);

	cvConvert(img1_temp, img1);
	cvConvert(img2_temp, img2);
	cvReleaseImage(&img1_temp);
	cvReleaseImage(&img2_temp);


	img1_sq = cvCreateImage(size, d, nChan);
	img2_sq = cvCreateImage(size, d, nChan);
	img1_img2 = cvCreateImage(size, d, nChan);

	cvPow(img1, img1_sq, 2);
	cvPow(img2, img2_sq, 2);
	cvMul(img1, img2, img1_img2, 1);

	mu1 = cvCreateImage(size, d, nChan);
	mu2 = cvCreateImage(size, d, nChan);

	mu1_sq = cvCreateImage(size, d, nChan);
	mu2_sq = cvCreateImage(size, d, nChan);
	mu1_mu2 = cvCreateImage(size, d, nChan);


	sigma1_sq = cvCreateImage(size, d, nChan);
	sigma2_sq = cvCreateImage(size, d, nChan);
	sigma12 = cvCreateImage(size, d, nChan);

	temp1 = cvCreateImage(size, d, nChan);
	temp2 = cvCreateImage(size, d, nChan);
	temp3 = cvCreateImage(size, d, nChan);

	ssim_map = cvCreateImage(size, d, nChan);
	/*************************** END INITS **********************************/


	//////////////////////////////////////////////////////////////////////////
	// PRELIMINARY COMPUTING
	cvSmooth(img1, mu1, CV_GAUSSIAN, 11, 11, 1.5);
	cvSmooth(img2, mu2, CV_GAUSSIAN, 11, 11, 1.5);

	cvPow(mu1, mu1_sq, 2);
	cvPow(mu2, mu2_sq, 2);
	cvMul(mu1, mu2, mu1_mu2, 1);


	cvSmooth(img1_sq, sigma1_sq, CV_GAUSSIAN, 11, 11, 1.5);
	cvAddWeighted(sigma1_sq, 1, mu1_sq, -1, 0, sigma1_sq);

	cvSmooth(img2_sq, sigma2_sq, CV_GAUSSIAN, 11, 11, 1.5);
	cvAddWeighted(sigma2_sq, 1, mu2_sq, -1, 0, sigma2_sq);

	cvSmooth(img1_img2, sigma12, CV_GAUSSIAN, 11, 11, 1.5);
	cvAddWeighted(sigma12, 1, mu1_mu2, -1, 0, sigma12);


	//////////////////////////////////////////////////////////////////////////
	// FORMULA

	// (2*mu1_mu2 + C1)
	cvScale(mu1_mu2, temp1, 2);
	cvAddS(temp1, cvScalarAll(C1), temp1);

	// (2*sigma12 + C2)
	cvScale(sigma12, temp2, 2);
	cvAddS(temp2, cvScalarAll(C2), temp2);

	// ((2*mu1_mu2 + C1).*(2*sigma12 + C2))
	cvMul(temp1, temp2, temp3, 1);

	// (mu1_sq + mu2_sq + C1)
	cvAdd(mu1_sq, mu2_sq, temp1);
	cvAddS(temp1, cvScalarAll(C1), temp1);

	// (sigma1_sq + sigma2_sq + C2)
	cvAdd(sigma1_sq, sigma2_sq, temp2);
	cvAddS(temp2, cvScalarAll(C2), temp2);

	// ((mu1_sq + mu2_sq + C1).*(sigma1_sq + sigma2_sq + C2))
	cvMul(temp1, temp2, temp1, 1);

	// ((2*mu1_mu2 + C1).*(2*sigma12 + C2))./((mu1_sq + mu2_sq + C1).*(sigma1_sq + sigma2_sq + C2))
	cvDiv(temp3, temp1, ssim_map, 1);


	CvScalar index_scalar = cvAvg(ssim_map);

	// through observation, there is approximately 
	// 1% error max with the original matlab program
	r=index_scalar.val[0];
	//cout << "(R, G & B SSIM index)" << endl;
	//cout << index_scalar.val[2] << endl;
	//cout << index_scalar.val[1] << endl;
	//cout << index_scalar.val[0] << endl;

	cvReleaseImage(&img1_sq);
	cvReleaseImage(&img2_sq);
	cvReleaseImage(&img1_img2);
	cvReleaseImage(&mu1);
	cvReleaseImage(&mu2);
	cvReleaseImage(&mu1_sq);
	cvReleaseImage(&mu2_sq);
	cvReleaseImage(&mu1_mu2);
	cvReleaseImage(&sigma1_sq);
	cvReleaseImage(&sigma2_sq);
	cvReleaseImage(&sigma12);
	cvReleaseImage(&temp1);
	cvReleaseImage(&temp2);
	cvReleaseImage(&temp3);
	cvReleaseImage(&ssim_map);
}

bool JudgeIfSame(IplImage *srcPic, IplImage *DBPic, double& r)
{

	int srcDim=srcPic->nChannels;
	IplImage* srcPicGray=cvCreateImage(cvGetSize(srcPic), IPL_DEPTH_8U, 1);
	if(srcDim>1)
		 cvCvtColor(srcPic, srcPicGray, CV_RGB2GRAY);
	else
		srcPicGray=srcPic;

	int dbDim=DBPic->nChannels;
	IplImage* dbPicGray=cvCreateImage(cvGetSize(DBPic), IPL_DEPTH_8U, 1);
	if(srcDim>1)
		cvCvtColor(DBPic, dbPicGray, CV_RGB2GRAY);
	else
		dbPicGray=DBPic;
	ssim(srcPicGray,dbPicGray,r);

	if(r>6)
		return true;
	else
		return false;
}

void rectDetect(IplImage *srcPic, double& r, int& resultPic)
{
	int srcHeight=srcPic->height;
	int srcWidth=srcPic->width;
	r=0;
	resultPic=0;
	int k=1;
	CString DBPath;
	DBPath.Format(_T("%s\\%d.png"),"rectDB",k);
	IplImage *dbPic;
	while(access(DBPath, 0) != -1)
	{
		dbPic=cvLoadImage(DBPath);
		int dbHeight=dbPic->height;
		int dbWidth=dbPic->width;
		if(abs(srcHeight-dbHeight)<50&&abs(srcWidth-dbWidth)<50){
			double tmpr=0.0;
			bool tmpresult=JudgeIfSame(srcPic,dbPic,tmpr);
			if(tmpr>r){
				resultPic=k;
				r=tmpr;
			}
		}
		k=k+1;
		DBPath.Format(_T("%s\\%d.png"),"rectDB",k);
	}
}


