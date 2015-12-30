
// MenuStatisticDlg.h : 头文件
//

#pragma once


#include "opencv2/core/core.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "io.h"
#include "afxcmn.h"

#include "windowController.h"
#include "configController.h"
#include "afxwin.h"

#define DLL_IMPLEMENT

using namespace cv;
// CMenuStatisticDlg 对话框
class CMenuStatisticDlg : public CDialogEx
{
// 构造
public:
	CMenuStatisticDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MENUSTATISTIC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickedButtonBegin();
	afx_msg void OnClickedButtonFinish();
	afx_msg void OnClickedButtonPause();

public:
	void display_pic(Mat& frame, int ID,string view);
	void menu_detect(Mat _img, Mat _oldimg);
	void rect_detect(Mat srcPic, double& r, int& resultPic);
	bool JudgeIfSame(Mat srcPic, Mat DBPic, double& r);
	double getMSSIM(Mat i1,Mat i2);
	double getPSNR(Mat i1, Mat i2);
	Rect findRect(Mat &frame, Mat &lastframe);
	Mat DrawRect(Mat _img, vector<Rect> ListRect);

	Mat img, tmpimg;
	Mat oldimg, tmpoldimg;
	Mat drawedimg,newimg;
	Mat bg;
	VideoCapture vCapture;
	CRect showvideo_rect;
	double m_totalFrameNumber;
	int m_slider_pos;
	int m_savePic;
	double m_fps;

	vector<Rect> *m_listRect;
	Rect m_itCurrRect;
	Rect currRect;

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CSliderCtrl m_sourec_slider;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedStartSave();
	afx_msg void OnBnClickedFinishSave();
	afx_msg void OnBnClickedButtonPausevedio();
	afx_msg void OnBnClickedButtonBegindraw();
	afx_msg void OnBnClickedButtonSavedraw();
	afx_msg void OnBnClickedButtonThrowdraw();
	afx_msg void OnBnClickedButtonReadrect();

	windowController m_windowCon;

	afx_msg void OnBnClickedButtonRoianalysis();
	afx_msg void OnBnClickedButtonNextrect();
	afx_msg void OnBnClickedButtonSaverecttext();

	char *m_showText;
	int indexRect;
	afx_msg void OnBnClickedButtonSaveall();
	afx_msg void OnBnClickedButtonSaveroilabeltext();
	CEdit m_editRoi;
	afx_msg void OnBnClickedButtonSaveimage();
};
