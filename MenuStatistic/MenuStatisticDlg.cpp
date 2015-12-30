
// MenuStatisticDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MenuStatistic.h"
#include "MenuStatisticDlg.h"
#include "afxdialogex.h"
#include <iostream>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

struct Record
{
	int times;
	vector<double> time;  //seconds
	Record(){
		times=0;
	}
};
Record recordStatics[1000];
Rect RoiImgDeal;
vector<Rect> g_ListRect;   //记录所画的矩形
vector<Rect> g_ReadinRect; //从记录的文本读取进来的矩形

Mat OnMouse_img;
vector<Mat> g_SaveOld_OnMouse_img;
double xscaled = 1.0;
double yscaled = 1.0;
int g_MouseStaus = 0, g_PauseStatus = 0;
static Point pre_pt = (-1,-1);//初始坐标  
static Point cur_pt = (-1,-1);//实时坐标 
static Point pre_pt_dlg = (-1, -1), cur_pt_dlg = (-1, -1);
int g_piclength, g_picwidth;

void LimitRect(Rect &inputRect, int widthlim, int heightlim)
{
	int l_x = inputRect.x;
	int t_y = inputRect.y;
	int r_x = l_x + inputRect.width;
	int b_y = t_y + inputRect.height;
	l_x = max(0, l_x);
	r_x = min(r_x, widthlim);
	t_y = max(0, t_y);
	b_y = min(b_y, heightlim);
	inputRect.x = l_x;
	inputRect.y = t_y;
	inputRect.width = r_x - l_x;
	inputRect.height = b_y - t_y;
}

void on_mouse(int event,int x,int y,int flags,void* usrdata)
{
	CMenuStatisticDlg *dlg = (CMenuStatisticDlg*)usrdata;
	if(event == CV_EVENT_LBUTTONDOWN){
		cur_pt_dlg.x = pre_pt_dlg.x = x;
		cur_pt_dlg.y = pre_pt_dlg.y = y;
		pre_pt.x = x * xscaled;
		pre_pt.y = y * yscaled;
		cur_pt.x = x * xscaled;
		cur_pt.y = y * yscaled;
		g_MouseStaus = 1;
	}else if ((event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON)) )
	{
		cur_pt_dlg.x = x;
		cur_pt_dlg.y = y;
		cur_pt.x = x * xscaled;
		cur_pt.y = y * yscaled;
		//rectangle(OnMouse_img, pre_pt, cur_pt, Scalar(0,255,0,0),2,8,0);
		g_MouseStaus = 1;

	}else if (event == CV_EVENT_LBUTTONUP)
	{
		//OnMouse_img.copyTo(newimg);
		cur_pt_dlg.x = x;
		cur_pt_dlg.y = y;
		cur_pt.x = x * xscaled;
		cur_pt.y = y * yscaled;
		if (abs(cur_pt_dlg.x - pre_pt_dlg.x) <= 1 || abs(cur_pt_dlg.y - pre_pt_dlg.y) <= 1)
			return;
		RoiImgDeal.x = min(pre_pt.x, cur_pt.x);
		RoiImgDeal.width = abs(pre_pt.x - cur_pt.x);
		RoiImgDeal.y = min(pre_pt.y, cur_pt.y);
		RoiImgDeal.height = abs(pre_pt.y - cur_pt.y);
		LimitRect(RoiImgDeal, g_picwidth, g_piclength);
		g_ListRect.push_back(RoiImgDeal);
		dlg->m_itCurrRect = g_ListRect[dlg->indexRect];


		Mat tempOnMouse;
		OnMouse_img.copyTo(tempOnMouse);
		rectangle(tempOnMouse, pre_pt, cur_pt, Scalar(0,255,0,0),2,8,0);
		g_SaveOld_OnMouse_img.push_back(tempOnMouse);
		g_MouseStaus = 0;
		CClientDC dc(dlg->GetDlgItem(IDC_STATIC_SHOW_SOURCEPIC));
		CBrush *pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
		CBrush *pOldBrush = dc.SelectObject(pBrush);
		dc.Rectangle(pre_pt_dlg.x, pre_pt_dlg.y, cur_pt_dlg.x, cur_pt_dlg.y);
//		pOldBrush = pBrush;
	}
}


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMenuStatisticDlg 对话框



CMenuStatisticDlg::CMenuStatisticDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMenuStatisticDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMenuStatisticDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SOURCESLIDER, m_sourec_slider);
	DDX_Control(pDX, IDC_EDIT_ROILABELTEXT, m_editRoi);
}

BEGIN_MESSAGE_MAP(CMenuStatisticDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_BEGIN, &CMenuStatisticDlg::OnClickedButtonBegin)
	ON_BN_CLICKED(IDC_BUTTON_FINISH, &CMenuStatisticDlg::OnClickedButtonFinish)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, &CMenuStatisticDlg::OnClickedButtonPause)
	ON_WM_TIMER()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_START_SAVE, &CMenuStatisticDlg::OnBnClickedStartSave)
	ON_BN_CLICKED(IDC_FINISH_SAVE, &CMenuStatisticDlg::OnBnClickedFinishSave)
	ON_BN_CLICKED(IDC_BUTTON_PAUSEVEDIO, &CMenuStatisticDlg::OnBnClickedButtonPausevedio)
	ON_BN_CLICKED(IDC_BUTTON_BEGINDRAW, &CMenuStatisticDlg::OnBnClickedButtonBegindraw)
	ON_BN_CLICKED(IDC_BUTTON_SAVEDRAW, &CMenuStatisticDlg::OnBnClickedButtonSavedraw)
	ON_BN_CLICKED(IDC_BUTTON_THROWDRAW, &CMenuStatisticDlg::OnBnClickedButtonThrowdraw)
	ON_BN_CLICKED(IDC_BUTTON_READRECT, &CMenuStatisticDlg::OnBnClickedButtonReadrect)
	ON_BN_CLICKED(IDC_BUTTON_ROIANALYSIS, &CMenuStatisticDlg::OnBnClickedButtonRoianalysis)
	ON_BN_CLICKED(IDC_BUTTON_NEXTRECT, &CMenuStatisticDlg::OnBnClickedButtonNextrect)
	ON_BN_CLICKED(IDC_BUTTON_SAVERECTTEXT, &CMenuStatisticDlg::OnBnClickedButtonSaverecttext)
	ON_BN_CLICKED(IDC_BUTTON_SAVEALL, &CMenuStatisticDlg::OnBnClickedButtonSaveall)
	ON_BN_CLICKED(IDC_BUTTON_SAVEROILABELTEXT, &CMenuStatisticDlg::OnBnClickedButtonSaveroilabeltext)
	ON_BN_CLICKED(IDC_BUTTON_SAVEIMAGE, &CMenuStatisticDlg::OnBnClickedButtonSaveimage)
END_MESSAGE_MAP()


// CMenuStatisticDlg 消息处理程序

BOOL CMenuStatisticDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_windowCon.initClass();
	//vCapture.open("E:/mywork/船总工作/test1.mp4");
	m_windowCon.openVideo("test6.mp4");
	//	vCapture.open("E:/电影/The.Walking.Dead.S06E04.PROPER.720p.HDTV.x264-KILLERS.mkv");

	m_savePic = 0;
	m_slider_pos = 0;
	m_totalFrameNumber = vCapture.get(CV_CAP_PROP_FRAME_COUNT);
	m_fps = vCapture.get(CV_CAP_PROP_FPS);

	m_sourec_slider.SetRange(0,m_totalFrameNumber);
	m_sourec_slider.SetPos(m_slider_pos);
	m_sourec_slider.SetLineSize(1);
	m_sourec_slider.SetPageSize(10);

//	vCapture.read(img);
//	m_windowCon.readAnImage();
	img.copyTo(oldimg);

	g_piclength = m_windowCon.getVideoHeight();
	g_picwidth = m_windowCon.getVideoWidth();
	RoiImgDeal = Rect(0, 0, 0, 0);
	GetDlgItem(IDC_STATIC_SHOW_SOURCEPIC)->GetClientRect(&showvideo_rect);
	xscaled = 1.0 * m_windowCon.getVideoWidth() / showvideo_rect.Width();
	yscaled = 1.0 * m_windowCon.getVideoHeight() / showvideo_rect.Height();

	CRect windowRect;
	Rect rect;
	GetDlgItem(IDC_STATIC_SHOW_SOURCEPIC)->GetClientRect(&windowRect);
	rect.width = windowRect.right;
	rect.height = windowRect.bottom;
	m_windowCon.setWindowHwnd(GetDlgItem(IDC_STATIC_SHOW_SOURCEPIC)->GetSafeHwnd(), 0, rect);

	GetDlgItem(IDC_STATIC_SHOW_RESULTPIC)->GetClientRect(&windowRect);
	rect.width = windowRect.right;
	rect.height = windowRect.bottom;
	m_windowCon.setWindowHwnd(GetDlgItem(IDC_STATIC_SHOW_RESULTPIC)->GetSafeHwnd(), 1, rect);

	GetDlgItem(IDC_STATIC_SHOW_DBPIC)->GetClientRect(&windowRect);
	rect.width = windowRect.right;
	rect.height = windowRect.bottom;
	m_windowCon.setWindowHwnd(GetDlgItem(IDC_STATIC_SHOW_DBPIC)->GetSafeHwnd(), 2, rect);

	bg = imread("bg.png");

	SetTimer(1, 15,NULL);
	g_PauseStatus = 1;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMenuStatisticDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMenuStatisticDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMenuStatisticDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMenuStatisticDlg::OnClickedButtonBegin()
{
	// TODO: 在此添加控件通知处理程序代码
	g_PauseStatus = 0;	
	m_windowCon.startProcessThread();
}


void CMenuStatisticDlg::OnClickedButtonFinish()
{
	// TODO: 在此添加控件通知处理程序代码
	SendMessage(WM_CLOSE);
}


void CMenuStatisticDlg::OnClickedButtonPause()
{
	// TODO: 在此添加控件通知处理程序代码
	m_windowCon.pauseProcess();
	g_PauseStatus = 1;
}

void CMenuStatisticDlg::display_pic(Mat& frame, int ID, string view){
	GetDlgItem(ID)->GetClientRect(&showvideo_rect);
//	Mat resizeFrame;
//	resize(frame, resizeFrame, Size(showvideo_rect.right,showvideo_rect.bottom));
//	imshow(view,resizeFrame);	
}

void CMenuStatisticDlg::menu_detect(Mat _img, Mat _oldimg)
{
	if (_img.empty())
	{
		return;
	}

	Mat difFrame, closeMat;
	absdiff(_img, _oldimg, difFrame);
	threshold(difFrame,difFrame,22,255,0);

	int difDim=difFrame.dims;
	if(difDim>1)
		cvtColor(difFrame, difFrame, CV_RGB2GRAY);

	Mat kernelMat = getStructuringElement(0, Size(3,3));
	morphologyEx(difFrame, difFrame,  CV_MOP_CLOSE,kernelMat);

	vector<vector<Point>> contours;
	findContours(difFrame, contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
	double r = 0.0;
	int resultPic=0;
	Mat srcStore;
	for (int i = 0; i < contours.size(); i++)
	{
		double aera=contourArea(contours[i]);
		if (aera>1000)
		{
			Rect detectRect = boundingRect(contours[i]);
			Mat srcRoi;
			_img(detectRect).copyTo(srcRoi);
			double tmpr=0.0;
			int tmpresultPic=0;
			rect_detect(srcRoi, tmpr, tmpresultPic);
			if(tmpr>r){
				resultPic=tmpresultPic;
				r=tmpr;
				srcRoi.copyTo(srcStore);
			}
		}
	}
	char name[40]={0};

	if(resultPic!=0){
		static int resultnum=0;
		sprintf_s(name,"detectresult/%d-%d-%.2f.png",resultnum,resultPic,r);
		imwrite(name,srcStore);
		resultnum++;

		double currentfram = vCapture.get(CV_CAP_PROP_POS_FRAMES);
		double currenttime = currentfram / m_fps;
		recordStatics[resultPic].times++;
		recordStatics[resultPic].time.push_back(currenttime);

		char t[50]={0};
		sprintf_s(t,"%s\\%d.png","rectDB",resultPic);

		display_pic(imread(t),IDC_STATIC_SHOW_DBPIC, "dbview");
		display_pic(srcStore, IDC_STATIC_SHOW_RESULTPIC,"detectview");

		CString simulation_r;
		simulation_r.Format(_T("%d : %f"),resultPic,r);
		GetDlgItem(IDC_STATIC_SIMULATION_VALUE)->SetWindowTextW(simulation_r);

	}

}

void CMenuStatisticDlg::rect_detect(Mat srcPic, double& r, int& resultPic)
{
	int srcRows=srcPic.rows;
	int srcCols=srcPic.cols;
	r=0;
	resultPic=0;
	int k=1;

	string DBPath;
	char t[256];
	sprintf_s(t,"%s\\%d.png","rectDB",k);
	DBPath=t;

	Mat dbPic;
	while(_access(DBPath.c_str(),00) != -1)
	{
		dbPic=imread(DBPath);

		int dbRows=dbPic.rows;
		int dbCols=dbPic.cols;
		if(abs(srcRows-dbRows)<50&&abs(srcCols-dbCols)<50){
			double tmpr=0.0;
			bool tmpresult=JudgeIfSame(srcPic,dbPic,tmpr);

			if(tmpresult&&tmpr>r){
				resultPic=k;
				r=tmpr;
			}
		}
		k = k+1;
		sprintf_s(t,"%s\\%d.png","rectDB",k);
		DBPath=t;
	}
}

bool CMenuStatisticDlg::JudgeIfSame(Mat srcPic, Mat DBPic, double& r)
{

	int srcDim=srcPic.dims;
	r=getPSNR(srcPic,DBPic);
	if(r>13)
		return true;
	else
		return false;
}

double CMenuStatisticDlg::getMSSIM(Mat i1,Mat i2)
{
	resize(i1, i1, i2.size());
	const double C1 = 6.5025, C2 = 58.5225;
	int d = CV_32F;
	Mat I1, I2;
	i1.convertTo(I1, d);
	i2.convertTo(I2, d);
	Mat I2_2   = I2.mul(I2);
	Mat I1_2   = I1.mul(I1);
	Mat I1_I2  = I1.mul(I2);
	Mat mu1, mu2;
	GaussianBlur(I1, mu1, Size(11, 11), 1.5);
	GaussianBlur(I2, mu2, Size(11, 11), 1.5);
	Mat mu1_2   =   mu1.mul(mu1);
	Mat mu2_2   =   mu2.mul(mu2);
	Mat mu1_mu2 =   mu1.mul(mu2);
	Mat sigma1_2, sigma2_2, sigma12;
	GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
	sigma1_2 -= mu1_2;
	GaussianBlur(I2_2, sigma2_2, Size(11, 11), 1.5);
	sigma2_2 -= mu2_2;
	GaussianBlur(I1_I2, sigma12, Size(11, 11), 1.5);
	sigma12 -= mu1_mu2;
	Mat t1, t2, t3;
	t1 = 2 * mu1_mu2 + C1;
	t2 = 2 * sigma12 + C2;
	t3 = t1.mul(t2);
	t1 = mu1_2 + mu2_2 + C1;
	t2 = sigma1_2 + sigma2_2 + C2;
	t1 = t1.mul(t2);
	Mat ssim_map;
	divide(t3, t1, ssim_map);
	Scalar mssim = mean( ssim_map );
	return (mssim.val[0]+mssim.val[1]+mssim.val[2])/3;
}

double CMenuStatisticDlg::getPSNR(Mat I1, Mat I2)
{
	resize(I1, I1, I2.size());
	Mat s1; 
	absdiff(I1, I2, s1);       // |I1 - I2|
	s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
	s1 = s1.mul(s1);           // |I1 - I2|^2

	Scalar s = sum(s1);         // sum elements per channel

	double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels

	if( sse <= 1e-10) // for small values return zero
		return 0;
	else
	{
		double  mse =sse /(double)(I1.channels() * I1.total());
		double psnr = 10.0*log10((255*255)/mse);
		return psnr;
	}
}

void CMenuStatisticDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(g_MouseStaus == 0 && g_PauseStatus == 0)
	{
//		img.copyTo(oldimg);
		int count=2;
		//char name[40]={0};
//		while (count--)
		{
			//vCapture.read(img);
//			m_windowCon.readAnImage(img);
//			if(img.empty())
//				return;
		}
//		img.copyTo(drawedimg);
	}
	if(m_savePic == 1)
	{
		char name[40]={0};
		static int num=1;
		sprintf_s(name,"rectDB11/%d.png",num);
//		imwrite(name,img);
		num++;
	}
	double r = 0.0;
	int resultPic = 0;
	char *tmp = NULL;
	char *tmpLabelText = NULL;
	r = m_windowCon.getTextNumber(tmp, indexRect, tmpLabelText);
	if (tmp != NULL)
	{
 		char *showPointer = tmpLabelText;
 		int tmplen = strlen(showPointer);
		int len = MultiByteToWideChar(CP_ACP, 0, showPointer, tmplen, NULL, 0);
		wchar_t *pWhar = new wchar_t[len + 1];
		MultiByteToWideChar(CP_ACP, 0, showPointer, tmplen, pWhar, len);
		pWhar[len] = '\0';
		CString simulation_r;
		simulation_r.Append(pWhar);
		GetDlgItem(IDC_STATIC_SIMULATION_VALUE)->SetWindowTextW(simulation_r);
		delete[] pWhar;
	}

	switch (nIDEvent)
	{
	case 1:
		{
			int nPos =m_windowCon.getVideoPosNow();
			m_sourec_slider.SetPos(nPos);
			CString strPos;
			strPos.Format(_T("Postion:%d"), nPos);
			GetDlgItem(IDC_SOURCE_SLIDER_POS)->SetWindowTextW(strPos);

//			if(g_MouseStaus == 0)
//			    drawedimg.copyTo(OnMouse_img);
			setMouseCallback("sourceview", on_mouse, (void*)this);

//			img.copyTo(tmpimg);
//			oldimg.copyTo(tmpoldimg);

			if(g_MouseStaus == 0){
//				img.copyTo(drawedimg);
//				drawedimg = DrawRect(drawedimg, g_ListRect);
				display_pic(drawedimg,IDC_STATIC_SHOW_SOURCEPIC, "sourceview");
			}
			else{
//				rectangle(OnMouse_img, pre_pt, cur_pt, Scalar(0,255,0,0),2,8,0);
				display_pic(OnMouse_img, IDC_STATIC_SHOW_SOURCEPIC, "sourceview");
				drawedimg.copyTo(OnMouse_img);
			}

//			m_windowCon.findText();
//			if(g_MouseStaus == 0)
//				menu_detect(tmpimg(RoiImgDeal), tmpoldimg(RoiImgDeal));

			break;
		}
	case 2:{
		break;
		   }
	default:
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CMenuStatisticDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(pScrollBar->GetDlgCtrlID() == IDC_SOURCESLIDER && nPos!=0)
	{
		vCapture.set(CV_CAP_PROP_POS_FRAMES, nPos);
		CString strPos;
		strPos.Format(_T("Postion:%d"), nPos);
		GetDlgItem(IDC_SOURCE_SLIDER_POS)->SetWindowTextW(strPos);
	}
}

Mat CMenuStatisticDlg::DrawRect(Mat _img, vector<Rect> ListRect)
{
	for (int i = 0; i < ListRect.size(); i++)
	{
		int l_x = ListRect[i].x;
		int r_x = ListRect[i].x + ListRect[i].width;
		int t_y = ListRect[i].y;
		int b_y = ListRect[i].y + ListRect[i].height;
		rectangle(_img, Point(l_x, t_y), Point(r_x, b_y), Scalar(0,255,0,0),2,8,0);
	}
	return _img;
}

void CMenuStatisticDlg::OnBnClickedStartSave()
{
	// TODO: 在此添加控件通知处理程序代码
	m_savePic = 1;
}


void CMenuStatisticDlg::OnBnClickedFinishSave()
{
	// TODO: 在此添加控件通知处理程序代码
	m_savePic = 0;
}


void CMenuStatisticDlg::OnBnClickedButtonPausevedio()
{
	// TODO: 在此添加控件通知处理程序代码
	m_windowCon.readAnImage(newimg);
	m_windowCon.showImage(newimg,0);

}


void CMenuStatisticDlg::OnBnClickedButtonBegindraw()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CMenuStatisticDlg::OnBnClickedButtonSavedraw()
{
	m_windowCon.updateRect(g_ListRect);
//	m_windowCon.writeConfigs("rect_record.txt");
}

void CMenuStatisticDlg::OnBnClickedButtonSaverecttext()
{
	// TODO:  在此添加控件通知处理程序代码
	int n = m_editRoi.GetWindowTextLengthW();
	CString tmp;
	m_editRoi.GetWindowText(tmp);
	int len = WideCharToMultiByte(CP_ACP, 0, tmp, n, NULL, 0, NULL, NULL); 
	char *pChar = new char[len + 1]; 
	WideCharToMultiByte(CP_ACP, 0, tmp, n, pChar, len, NULL, NULL); 
	pChar[len] = '\0'; 

	m_windowCon.addRectText(m_showText, indexRect, pChar);
}

void CMenuStatisticDlg::OnBnClickedButtonThrowdraw()
{
	// TODO: 在此添加控件通知处理程序代码
	if (g_ListRect.empty())
	{
		return;
	}
	g_ListRect.pop_back();
}


void CMenuStatisticDlg::OnBnClickedButtonReadrect()
{
	m_windowCon.readConfigs("rect_record1.txt");
	m_listRect = m_windowCon.getListRect();
	vector<Rect>::iterator tmpRect;
	for (tmpRect = m_listRect->begin(); tmpRect != m_listRect->end(); tmpRect++)
	{
		g_ListRect.push_back(*tmpRect);
	}
//	m_itCurrRect = g_ListRect.begin();
	indexRect = 0;
	m_itCurrRect = g_ListRect[indexRect];
}


void CMenuStatisticDlg::OnBnClickedButtonRoianalysis()
{
	// TODO:  在此添加控件通知处理程序代码

	if (g_ListRect.empty())
	{
		return;
	}
	currRect = m_itCurrRect;

	imwrite("roi.tif", newimg(currRect));
	
	m_windowCon.image2Text(newimg(currRect), m_showText);
	CString mCString;
	int i = MultiByteToWideChar(CP_ACP, 0, m_showText, -1, NULL, 0);
	WCHAR *strUnicode = new WCHAR[i];
	MultiByteToWideChar(CP_ACP, 0, m_showText, -1, strUnicode, i);

	mCString.Format(_T("%s"), m_showText);
	GetDlgItem(IDC_STATIC_ROITEXT)->SetWindowTextW(strUnicode);
	
	m_editRoi.SetWindowTextW(strUnicode);
	delete[] strUnicode;
}



void CMenuStatisticDlg::OnBnClickedButtonNextrect()
{
	// TODO:  在此添加控件通知处理程序代码
	m_windowCon.showImage(newimg, 0);
	if (indexRect == g_ListRect.size() - 1)
	{
		indexRect = 0;
		m_itCurrRect = g_ListRect[indexRect];
	}
	else
	{
		indexRect++;
		m_itCurrRect = g_ListRect[indexRect];
	}
// 	m_itCurrRect;
// 	indexRect++;
// 	if (m_itCurrRect == g_ListRect.end())
// 	{
// 		m_itCurrRect = g_ListRect.begin();
// 		indexRect = 0;
// 	}
	CClientDC dc(GetDlgItem(IDC_STATIC_SHOW_SOURCEPIC));
	CPen pPen;
	pPen.CreatePen(0, 1, RGB(255, 0, 0));
	dc.SelectObject(pPen);
	CBrush *pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
	CBrush *pOldBrush = dc.SelectObject(pBrush);
	dc.Rectangle(m_itCurrRect.x / xscaled, m_itCurrRect.y / yscaled, (m_itCurrRect.x + m_itCurrRect.width) / xscaled, (m_itCurrRect.y+m_itCurrRect.height) / yscaled);
}




void CMenuStatisticDlg::OnBnClickedButtonSaveall()
{
	// TODO:  在此添加控件通知处理程序代码
	m_windowCon.writeConfigs("rect_record1.txt");
}


void CMenuStatisticDlg::OnBnClickedButtonSaveroilabeltext()
{
	// TODO:  在此添加控件通知处理程序代码
//	m_windowCon.addRectLabelText(m_showText, indexRect);
}


void CMenuStatisticDlg::OnBnClickedButtonSaveimage()
{
	// TODO:  在此添加控件通知处理程序代码

	imwrite("test.jpg",newimg);
}
