
// ZSTTEyeProgramDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "ZSTTEyeProgram.h"
#include "ZSTTEyeProgramDlg.h"
#include "afxdialogex.h"
#include "BlobLabeling.h"
#include <omp.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define M_PI 3.141592

#define ORIGINMODE 0
#define SPRINTMODE 1
#define OBSTACLE 2
#define BASKETBALL 3
#define MARATONE 4
#define FOOTBALL 5

#define STAGEONE 0
#define STAGETWO 1
#define STAGETHREE 2

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	// 구현입니다.
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


// CZSTTEyeProgramDlg 대화 상자

struct CZSTTEyeProgramDlg:: Msg{

	unsigned char dir;
	unsigned char str;
	unsigned char left;
	unsigned char right;
	unsigned char heady;
	unsigned char headx;
	unsigned char LShift;
	unsigned char RShift;

}Msg;

TTYSTRUCT CZSTTEyeProgramDlg::Int2TTY()
{
	TTYSTRUCT tty;
	ZERO_MEMORY(tty);

	tty.byCommPort = (BYTE)m_nSettingPort+1;
	tty.byXonXoff  = FALSE;
	tty.byByteSize = (BYTE)_nDataValues[m_nSettingData];
	tty.byFlowCtrl = (BYTE)_nFlow[m_nSettingFlow];
	tty.byParity   = (BYTE)m_nSettingParity;
	tty.byStopBits = (BYTE)_nStopBits[m_nSettingStop];
	tty.dwBaudRate = (DWORD)_nBaudRates[m_nSettingBaud];

	return tty;
}

TTYSTRUCT CZSTTEyeProgramDlg::LoadSettings()
{
	CWinApp *pApp = AfxGetApp();

	m_nSettingPort = pApp->GetProfileInt(CS_REGKEY_SETTINGS, CS_REGENTRY_PORT, 1);
	m_nSettingParity = 0;
	m_nSettingBaud = pApp->GetProfileInt(CS_REGKEY_SETTINGS, CS_REGENTRY_BAUD, 1);
	m_nSettingData = 1;
	m_nSettingStop = 0;
	m_nSettingFlow = 0;
	return Int2TTY();
}

CZSTTEyeProgramDlg::CZSTTEyeProgramDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CZSTTEyeProgramDlg::IDD, pParent)
	, m_nSettingBaud(0)
	, m_nSettingPort(0)
	, m_EditData(_T(""))
	, m_nSettingStop(0)
	, eStr(_T(""))
	, eRShift(_T(""))
	, eRight(_T(""))
	, eLeft(_T(""))
	, eHeadY(_T(""))
	, eHeadX(_T(""))
	, eLShift(_T(""))
	, eDir(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CZSTTEyeProgramDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_BAUDRATE, m_nSettingBaud);
	DDX_CBIndex(pDX, IDC_PORT, m_nSettingPort);
	DDX_Text(pDX, IDC_EDIT1, m_EditData);
	DDX_Control(pDX, IDC_ORIGIN_CAMERA, originCamera);
	DDX_Control(pDX, IDC_OPENCV_CAMERA, opencvCamera);
	DDX_Control(pDX, IDC_OC_MODE, cameramodeBox);
	DDX_Control(pDX, IDC_TEST, test);
	DDX_Control(pDX, IDC_HISTH, HistH);
	DDX_Control(pDX, IDC_HISTS, HistS);
	DDX_Control(pDX, IDC_HISTV, HistV);
	DDX_Control(pDX, IDC_MAXH, MaxH);
	DDX_Control(pDX, IDC_MAXS, MaxS);
	DDX_Control(pDX, IDC_MAXV, MaxV);
	DDX_Control(pDX, IDC_MINH, MinH);
	DDX_Control(pDX, IDC_MINS, MinS);
	DDX_Control(pDX, IDC_MINV, MinV);
	DDX_Text(pDX, IDC_ESTR, eStr);
	DDX_Text(pDX, IDC_ERSHIFT, eRShift);
	DDX_Text(pDX, IDC_ERIGHT, eRight);
	DDX_Text(pDX, IDC_ELEFT, eLeft);
	DDX_Text(pDX, IDC_EHEADY, eHeadY);
	DDX_Text(pDX, IDC_EHEADX, eHeadX);
	//  DDX_Control(pDX, IDC_ELSHIFT, eLShift);
	DDX_Text(pDX, IDC_ELSHIFT, eLShift);
	DDX_Text(pDX, IDC_EDIR, eDir);
}

BEGIN_MESSAGE_MAP(CZSTTEyeProgramDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START_CONNECTION, &CZSTTEyeProgramDlg::OnBnClickedStartConnection)
	ON_BN_CLICKED(IDCANCEL, &CZSTTEyeProgramDlg::OnBnClickedCancel)
	ON_MESSAGE(WM_RECEIVEDATA,OnReceiveData)
	ON_WM_TIMER()
	ON_CBN_CLOSEUP(IDC_OC_MODE, &CZSTTEyeProgramDlg::OnCbnCloseupOcMode)
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BUTTON1, &CZSTTEyeProgramDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CZSTTEyeProgramDlg::OnBnClickedButton2)
	ON_STN_CLICKED(IDC_TEST, &CZSTTEyeProgramDlg::OnStnClickedTest)
	ON_BN_CLICKED(IDC_BUTTON3, &CZSTTEyeProgramDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CZSTTEyeProgramDlg 메시지 처리기

BOOL CZSTTEyeProgramDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.
	m_Comm.SetHwnd(this->m_hWnd);
	LoadSettings();
	UpdateData(FALSE);
	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.
	//omp_set_num_threads(4);
	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	capture = 0;
	capture = cvCaptureFromCAM(0);
	if(!capture)
		exit(0);
	//CAMERAMODE = ORIGINMODE;
	STAGE = STAGEONE;
	sendData = false;
	SPRINTCOLOR = cvScalar(180,50,30);
	BASKETBALLCOLOR = cvScalar(194,239,134);
	BASKETBALLBASKETCOLOR = cvScalar(0,0,0);
	ObstacleCount = 0;
	FOOTBALLCOLOR = cvScalar(0,0,0);
	MouseColor = cvScalar(240,116,58);
	OBSTACLEBLOCKCOLOR[0] = cvScalar(55, 70, 40);	
	OBSTACLEBLOCKCOLOR[1] = cvScalar(25, 118, 218);
	ObstacleDirect = true; //false 왼쪽 true 오른쪽
	dir = 2;
	str = 0;
	left =0;
	right=0;
	heady=100;
	headx=100;
	LShift=0;
	RShift=0;
	MaxS.SetPos(100);
	MinS.SetPos(100);
	MaxV.SetPos(100);
	MinV.SetPos(100);
	MaxH.SetPos(15);
	MinH.SetPos(15);
	count = 0;
	SetTimer(2, 30, NULL);
	SendMsg = new Msg;
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CZSTTEyeProgramDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX) 
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CZSTTEyeProgramDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}

	CDC *pDC;
	CRect rect;
	COLORREF color;

	//영상처리 코드를 입력하시오
	if(CAMERAMODE == ORIGINMODE)
	{
		pDC = originCamera.GetDC();
		originCamera.GetClientRect(rect);

		CvvImage.CopyOf(mainImage);
		CvvImage.DrawToHDC(pDC->m_hDC,rect);

		color = pDC->GetPixel(MousePoint);

		MouseColor.val[0] = GetRValue(color);
		MouseColor.val[1] = GetGValue(color);
		MouseColor.val[2] = GetBValue(color);

		ReleaseDC(pDC);




		pDC = opencvCamera.GetDC();
		opencvCamera.GetClientRect(rect);

		CvvImage.CopyOf(opencvImage);
		CvvImage.DrawToHDC(pDC->m_hDC,rect);

		ReleaseDC(pDC);


		/*
		pDC = HistH.GetDC();
		HistH.GetClientRect(rect);

		CvvImage.CopyOf(histImgS);
		CvvImage.DrawToHDC(pDC->m_hDC,rect);

		ReleaseDC(pDC);
		*/

	}
	else if(CAMERAMODE == SPRINTMODE)
	{
		pDC = originCamera.GetDC();
		originCamera.GetClientRect(rect);

		CvvImage.CopyOf(mainImage);
		CvvImage.DrawToHDC(pDC->m_hDC,rect);

		color = pDC->GetPixel(MousePoint);
		
		MouseColor.val[0] = GetRValue(color);
		MouseColor.val[1] = GetGValue(color);
		MouseColor.val[2] = GetBValue(color);

		ReleaseDC(pDC);


		pDC = opencvCamera.GetDC();
		opencvCamera.GetClientRect(rect);

		CvvImage.CopyOf(opencvImage);
		CvvImage.DrawToHDC(pDC->m_hDC,rect);

		ReleaseDC(pDC);
	}
	else if(CAMERAMODE == OBSTACLE)
	{
		pDC = originCamera.GetDC();
		originCamera.GetClientRect(rect);

		CvvImage.CopyOf(mainImage);
		CvvImage.DrawToHDC(pDC->m_hDC,rect);

		color = pDC->GetPixel(MousePoint);

		MouseColor.val[0] = GetRValue(color);
		MouseColor.val[1] = GetGValue(color);
		MouseColor.val[2] = GetBValue(color);

		ReleaseDC(pDC);


		pDC = opencvCamera.GetDC();
		opencvCamera.GetClientRect(rect);

		CvvImage.CopyOf(opencvImage);
		CvvImage.DrawToHDC(pDC->m_hDC,rect);

		ReleaseDC(pDC);
	}
	else if(CAMERAMODE == BASKETBALL)
	{
		pDC = originCamera.GetDC();
		originCamera.GetClientRect(rect);

		CvvImage.CopyOf(mainImage);
		CvvImage.DrawToHDC(pDC->m_hDC,rect);

		color = pDC->GetPixel(MousePoint);

		MouseColor.val[0] = GetRValue(color);
		MouseColor.val[1] = GetGValue(color);
		MouseColor.val[2] = GetBValue(color);

		ReleaseDC(pDC);


		pDC = opencvCamera.GetDC();
		opencvCamera.GetClientRect(rect);

		CvvImage.CopyOf(opencvImage);
		CvvImage.DrawToHDC(pDC->m_hDC,rect);

		ReleaseDC(pDC);
	}
	else if(CAMERAMODE == FOOTBALL)
	{
		pDC = originCamera.GetDC();
		originCamera.GetClientRect(rect);

		CvvImage.CopyOf(mainImage);
		CvvImage.DrawToHDC(pDC->m_hDC,rect);

		color = pDC->GetPixel(MousePoint);

		MouseColor.val[0] = GetRValue(color);
		MouseColor.val[1] = GetGValue(color);
		MouseColor.val[2] = GetBValue(color);

		ReleaseDC(pDC);


		pDC = opencvCamera.GetDC();
		opencvCamera.GetClientRect(rect);

		CvvImage.CopyOf(opencvImage);
		CvvImage.DrawToHDC(pDC->m_hDC,rect);

		ReleaseDC(pDC);
	}
	else{
		pDC = originCamera.GetDC();
		originCamera.GetClientRect(rect);

		CvvImage.CopyOf(mainImage);
		CvvImage.DrawToHDC(pDC->m_hDC,rect);

		color = pDC->GetPixel(MousePoint);

	//	MouseColor.val[0] = GetRValue(color);
	//	MouseColor.val[1] = GetGValue(color);
	//	MouseColor.val[2] = GetBValue(color);

		ReleaseDC(pDC);


		pDC = opencvCamera.GetDC();
		opencvCamera.GetClientRect(rect);

		CvvImage.CopyOf(opencvImage);
		CvvImage.DrawToHDC(pDC->m_hDC,rect);

		ReleaseDC(pDC);
	}
	cvReleaseImage(&opencvImage);
	cvReleaseImage(&histImgH);
	cvReleaseImage(&histImgS);
	cvReleaseImage(&histImgV);
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CZSTTEyeProgramDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CZSTTEyeProgramDlg::OnBnClickedStartConnection()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	BOOL bCheck = ((CButton*)GetDlgItem(IDC_START_CONNECTION))->GetCheck();

	if(bCheck)
	{
		if(m_Comm.OpenCommPort(&Int2TTY()) !=TRUE)
		{
			CString str;
			str.Format("COM%d 이 이미 사용중인지 확인하세요",m_nSettingPort+1);
			AfxMessageBox(str);

			((CButton*)GetDlgItem(IDC_START_CONNECTION))->SetCheck(!bCheck);
			return;
		}
		GetDlgItem(IDC_START_CONNECTION)->SetWindowTextA("통신종료");
		GetDlgItem(IDC_PORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BAUDRATE)->EnableWindow(FALSE);
	}
	else
	{
		m_Comm.CloseConnection();
		GetDlgItem(IDC_START_CONNECTION)->SetWindowTextA("통신시작");
		GetDlgItem(IDC_PORT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BAUDRATE)->EnableWindow(TRUE);

	}
}


void CZSTTEyeProgramDlg::SaveSetting(void)
{
	CWinApp *pApp = AfxGetApp();

	pApp->WriteProfileInt(CS_REGKEY_SETTINGS, CS_REGENTRY_PORT, m_nSettingPort);
	pApp->WriteProfileInt(CS_REGKEY_SETTINGS, CS_REGENTRY_PARITY, m_nSettingParity);
	pApp->WriteProfileInt(CS_REGKEY_SETTINGS, CS_REGENTRY_BAUD, m_nSettingBaud);
	pApp->WriteProfileInt(CS_REGKEY_SETTINGS, CS_REGENTRY_DATABITS, m_nSettingData);
	pApp->WriteProfileInt(CS_REGKEY_SETTINGS, CS_REGENTRY_STOPBITS, m_nSettingStop);
	pApp->WriteProfileInt(CS_REGKEY_SETTINGS, CS_REGENTRY_FLOW, m_nSettingFlow);
}


void CZSTTEyeProgramDlg::OnBnClickedCancel()
{

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SaveSetting();
	DestroyWindow();
	cvReleaseCapture(&capture);
	CDialog::OnCancel();
}

LONG CZSTTEyeProgramDlg::OnReceiveData(UINT WParam, LONG a){
	int i;
	int nLen = WParam;
	CString str;

	UpdateData(TRUE);

	BYTE ch;

	for(i = 0; i < nLen; i++){
		ch = (int)m_Comm.abIn[i];
		str.Format("%x\t", ch);
		m_EditData += str;
	}
	UpdateData(FALSE);
	return TRUE;
}

void CZSTTEyeProgramDlg::OnSendData(unsigned char dir,unsigned char str,unsigned char left,unsigned char right,unsigned char heady,unsigned char headx,unsigned char LShift,unsigned char RShift)
{
	UpdateData(TRUE);

	TX_MotionS[0] = 1;

	TX_MotionS[1] = 8;//8

	TX_MotionS[2] = 254;//254
	TX_MotionS[3] = 17;//17

	TX_MotionS[4] = 2;

	TX_MotionS[5] = dir; // 방향 //왼 7 오 8

	TX_MotionS[6] = str; //보폭

	TX_MotionS[7] = left;//좌회전
	TX_MotionS[8] = right;//우회전

	TX_MotionS[9] = heady;//대구리 들고내리고
	TX_MotionS[10] = headx;//대구리 좌우

	TX_MotionS[11] = LShift;//대구리 왼쪽값
	TX_MotionS[12] = RShift;//대구리 오른쪽값

	TX_MotionS[13] = 0;   

	TX_MotionS[14] = 0;   //0
	TX_MotionS[15] = 254;  //254

	TX_MotionS[16] = 255;

	m_Comm.WriteCommBlock((LPSTR)TX_MotionS, 17);

	/*
	CString TempMsg;
	TempMsg.Format("Send Data"); 
	PrintMsg(TempMsg);   
	*/
	UpdateData(FALSE);
}


void CZSTTEyeProgramDlg::OnTimer(UINT_PTR nIDEvent)
{

	mainImage = cvQueryFrame(capture);
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(nIDEvent == 1){
		// ID = 1 : 원본영상을 불러와 출력해주는 타이머
		
	OnSendData(dir,str,left,right,heady,headx,LShift,RShift);
			
	}
	if(nIDEvent == 2){
		//영상처리 코드를 입력하시오
		mainImage = cvQueryFrame(capture);
		if(CAMERAMODE == ORIGINMODE)
		{
			CvScalar Hsv, from, to;

			Hsv = R2H(MouseColor);

			from = cvScalar(Hsv.val[0] - MinH.GetPos() , Hsv.val[1] - MinS.GetPos() , Hsv.val[2] - MinV.GetPos());
			to = cvScalar(Hsv.val[0] + MaxH.GetPos() , Hsv.val[1]+ MaxS.GetPos()  , Hsv.val[2]+ MaxV.GetPos() );

			opencvImage = GetThresholdImage(mainImage, from, to);
		}
		else if(CAMERAMODE == SPRINTMODE)
		{
			CvScalar Hsv, from, to;

			Hsv = R2H(SPRINTCOLOR);

			from = cvScalar(Hsv.val[0] - MinH.GetPos() , Hsv.val[1] - MinS.GetPos() , Hsv.val[2] - MinV.GetPos());
			to = cvScalar(Hsv.val[0] + MaxH.GetPos() , Hsv.val[1]+ MaxS.GetPos()  , Hsv.val[2]+ MaxV.GetPos() );

			opencvImage = SprintFunc(mainImage, from, to);
		}
		else if(CAMERAMODE == OBSTACLE)
		{

			
				CvScalar Hsv, from, to;

				Hsv = R2H(MouseColor);

				from = cvScalar(MouseColor.val[0] - MinH.GetPos() , MouseColor.val[1] - MinS.GetPos() , MouseColor.val[2] - MinV.GetPos());
				to = cvScalar(MouseColor.val[0] + MaxH.GetPos() , MouseColor.val[1]+ MaxS.GetPos()  , MouseColor.val[2]+ MaxV.GetPos() );

				opencvImage = ObstacleFunc(mainImage, from, to);
			

		}
		else if(CAMERAMODE == BASKETBALL)
		{
			
				CvScalar Hsv, from, to;

				Hsv = R2H(BASKETBALLCOLOR);

				from = cvScalar(Hsv.val[0] - MinH.GetPos() , Hsv.val[1] - MinS.GetPos() , Hsv.val[2] - MinV.GetPos());
				to = cvScalar(Hsv.val[0] + MaxH.GetPos() , Hsv.val[1]+ MaxS.GetPos()  , Hsv.val[2]+ MaxV.GetPos() );

				opencvImage = BasketBallFunc(mainImage, from, to);		
				
			
		}

		else if(CAMERAMODE == MARATONE)
		{
			CvScalar Hsv, from, to;

			Hsv = R2H(SPRINTCOLOR);

				from = cvScalar(Hsv.val[0] - MinH.GetPos() , Hsv.val[1] - MinS.GetPos() , Hsv.val[2] - MinV.GetPos());
				to = cvScalar(Hsv.val[0] + MaxH.GetPos() , Hsv.val[1]+ MaxS.GetPos()  , Hsv.val[2]+ MaxV.GetPos() );
				SprintFunc(mainImage,from, to);
				opencvImage = FindContours(mainImage);
				
		}
		else if(CAMERAMODE == FOOTBALL)
		{
			if(!FOOTBALLCOLOR.val[0]){
				CvScalar Hsv, from, to;

				Hsv = R2H(FOOTBALLCOLOR);

				from = cvScalar(Hsv.val[0] - MinH.GetPos() , Hsv.val[1] - MinS.GetPos() , Hsv.val[2] - MinV.GetPos());
				to = cvScalar(Hsv.val[0] + MaxH.GetPos() , Hsv.val[1]+ MaxS.GetPos()  , Hsv.val[2]+ MaxV.GetPos() );

				opencvImage = FootballFunc(mainImage, from, to);
			}
			else{
				CvScalar Hsv, from, to;

				Hsv = R2H(FOOTBALLCOLOR);

				from = cvScalar(Hsv.val[0] - MinH.GetPos() , Hsv.val[1] - MinS.GetPos() , Hsv.val[2] - MinV.GetPos());
				to = cvScalar(Hsv.val[0] + MaxH.GetPos() , Hsv.val[1]+ MaxS.GetPos()  , Hsv.val[2]+ MaxV.GetPos() );

				opencvImage = FootballFunc(mainImage, from, to);
			}
		}

		Invalidate(FALSE);
	}
	CDialog::OnTimer(nIDEvent);
}


void CZSTTEyeProgramDlg::OnCbnCloseupOcMode()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 해당 코드는 명령어가 내려왔을때 CAMERAMODE를 변경하는 코드
	int i = cameramodeBox.GetCurSel();
	if(i == 0){
		CAMERAMODE = ORIGINMODE;
		SPRINTCOLOR = cvScalar(0,0,0);
	}
	else if(i == 1){
		CAMERAMODE = SPRINTMODE;
		dir =2;
	}
	else if(i == 2){
		CAMERAMODE = OBSTACLE;
		SPRINTCOLOR = cvScalar(0,0,0);
	}
	else if(i == 3){
		CAMERAMODE = BASKETBALL;
		SPRINTCOLOR = cvScalar(0,0,0);
	}
	else if(i == 4){
		CAMERAMODE = MARATONE;
		SPRINTCOLOR = cvScalar(0,0,0);
	}
	else if(i == 5){
		CAMERAMODE = FOOTBALL;
		SPRINTCOLOR = cvScalar(0,0,0);
	}
}




IplImage* CZSTTEyeProgramDlg::GetThresholdImage(IplImage* img, CvScalar from, CvScalar to)
{
	IplImage* imgDest = cvCreateImage(cvGetSize(img), 8, 1);
	IplImage* imgThreshed = cvCreateImage(cvGetSize(img), 8, 1);
	IplImage* imgHSV = cvCreateImage(cvGetSize(img),8,3);

	cvCvtColor(img, imgHSV, CV_BGR2HSV);


	cvInRangeS(imgHSV , from, to, imgThreshed);
	//



	cvMorphologyEx(imgThreshed, imgDest, NULL, NULL, CV_MOP_CLOSE, 1);
	cvMorphologyEx(imgThreshed, imgDest, NULL, NULL, CV_MOP_OPEN, 1);
	//
	cvReleaseImage(&imgThreshed);
	cvReleaseImage(&imgHSV);

	//return imgThreshed;
	return imgDest;
}


void CZSTTEyeProgramDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	MousePoint = point;
	MousePoint.x -= 10;
	MousePoint.y -= 10;
	CDialog::OnMouseMove(nFlags, point);
}


CvScalar CZSTTEyeProgramDlg::R2H(CvScalar in)
{
	CvScalar out;


	IplImage* chg = cvCreateImage(cvSize(1,1),8,3);
	IplImage* chg2 = cvCreateImage(cvSize(1,1),8,3);
	cvSet2D(chg,0,0,in);

	cvCvtColor(chg, chg2, CV_RGB2HSV);

	out = cvGet2D(chg2, 0, 0);
	cvReleaseImage(&chg);
	cvReleaseImage(&chg2);
	return out;
}


CvScalar CZSTTEyeProgramDlg::H2R(CvScalar in)
{
	CvScalar out;


	IplImage* chg = cvCreateImage(cvSize(1,1),8,3);
	IplImage* chg2 = cvCreateImage(cvSize(1,1),8,3);
	cvSet2D(chg,0,0,in);

	cvCvtColor(chg, chg2, CV_HSV2BGR);

	out = cvGet2D(chg2, 0, 0);
	cvReleaseImage(&chg);
	cvReleaseImage(&chg2);
	return out;
}


IplImage* CZSTTEyeProgramDlg::SprintFunc(IplImage* img , CvScalar from, CvScalar to)
{
	//밝기 : -1.0
	IplImage * image = GetThresholdImage(img, from, to);


	IplImage* laberled = cvCreateImage(cvGetSize(image),8,3);

	//cvCopyImage(img, laberled); 
	CBlobLabeling blob;
	blob.SetParam(image, 100);
	blob.DoLabeling();

	bool Go = false;
	cvCvtColor (image, laberled, CV_GRAY2RGB);
	
	///if(blob.m_nBlobs>0) Go = true;
	//if(blob.m_nBlobs=0) Go = false;

	for(int i = 0; i < blob.m_nBlobs; i++){
		CvPoint pt1 = cvPoint(  blob.m_recBlobs[i].x,
			blob.m_recBlobs[i].y);
		CvPoint pt2 = cvPoint(  pt1.x + blob.m_recBlobs[i].width,
			pt1.y + blob.m_recBlobs[i].height);

		CvScalar color = cvScalar( 0, 0, 255);
		cvDrawRect ( laberled, pt1, pt2, color);
		int width = blob.m_recBlobs[i].width;
		int height = blob.m_recBlobs[i].height;

		CvPoint CenterPoint = cvPoint((pt1.x + pt2.x)/2, (pt1.y + pt2.y)/2);

		CString a;
		a.Format("%d %d %d %d %d %d %d %d", pt1.x, pt1.y, pt2.x, pt2.y, CenterPoint.x, CenterPoint.y, width, height);
		test.SetWindowTextA(a);
		//스프린트 시작
		dir = 2;
		
			if(CenterPoint.x < 240){
			test.SetWindowTextA("left");
			
			str = 9;
			left= 9;
			right=0;

			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(CenterPoint.x < 200){
			test.SetWindowTextA("Bigleft");
		
			str = 9;
			left= 10;
			right=0;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(CenterPoint.x < 180){
			test.SetWindowTextA("Bigleft");
		
			str = 9;
			left= 11;
			right=0;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(CenterPoint.x < 160){
			test.SetWindowTextA("Bigleft");
			
			str = 9;
			left= 12;
			right=0;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(CenterPoint.x < 140){
			test.SetWindowTextA("Bigleft");
		
			str = 9;
			left= 13;
			right=0;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(CenterPoint.x < 120){
			test.SetWindowTextA("Bigleft");
			
			str = 9;
			left= 14;
			right=0;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(CenterPoint.x < 100){
			test.SetWindowTextA("Bigleft");
			
			str = 9;
			left= 15;
			right= 0;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(CenterPoint.x < 80){
			test.SetWindowTextA("Bigleft");
			str = 9;
			left= 16;
			right=0;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(CenterPoint.x < 60){
			test.SetWindowTextA("Bigleft");
			
			str = 9;
			left= 17;
			right=0;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(CenterPoint.x < 40){
			test.SetWindowTextA("Bigleft");
			
			str = 9;
			left= 18;
			right=0;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(CenterPoint.x < 20){
			test.SetWindowTextA("Bigleft");
			
			str = 9;
			left= 19;
			right=0;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}

			else if(CenterPoint.x > 400){
			test.SetWindowTextA("right");

			str = 9;
			left= 0;
			right=10;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(CenterPoint.x > 420){
			test.SetWindowTextA("Bigright");
			
			str = 9;
			left= 0;
			right=11;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(CenterPoint.x > 440){
			test.SetWindowTextA("Bigright");
			
			str =9;
			left= 0;
			right=12;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(CenterPoint.x > 460){
			test.SetWindowTextA("Bigright");
			
			str = 9;
			left= 0;
			right=13;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(CenterPoint.x > 480){
			test.SetWindowTextA("Bigright");
			
			str = 9;
			left= 0;
			right=14;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(CenterPoint.x > 500){
			test.SetWindowTextA("Bigright");
			
			str = 9;
			left= 0;
			right=15;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(CenterPoint.x > 520){
			test.SetWindowTextA("Bigright");
			
			str = 9;
			left= 0;
			right=16;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(CenterPoint.x > 540){
			test.SetWindowTextA("Bigright");
		
			str = 9;
			left= 0;
			right=17;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(CenterPoint.x > 560){
			test.SetWindowTextA("Bigright");
			
			str = 9;
			left= 0;
			right=18;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(CenterPoint.x > 580){
			test.SetWindowTextA("Bigright");
		
			str = 9;
			left= 0;
			right=19;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(CenterPoint.x > 600){
			test.SetWindowTextA("Bigright");
			
			str = 9;
			left= 0;
			right=20;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(CenterPoint.x > 620){
			test.SetWindowTextA("Bigright");
			
			str = 9;
			left= 0;
			right=21;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(CenterPoint.x > 640){
			test.SetWindowTextA("Bigright");
			
			str = 9;
			left= 0;
			right=22;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else{
			test.SetWindowTextA("Go St");
			
			str = 9;
			left= 0;
			right=0;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
		
	
	
	}
	
	if(!Go)
	{
		//dir = 4;		
	}	
	cvReleaseImage(&image);
	return laberled;
}


BOOL CZSTTEyeProgramDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	switch (pMsg -> message)
	{
	case WM_KEYDOWN:
		if(pMsg -> wParam == VK_TAB){
			//                                       dir = 2;
			if(CAMERAMODE == SPRINTMODE) 
			SetTimer(1,200,NULL);
			else
				SetTimer(1,200,NULL);
		}
		if(pMsg -> wParam == VK_SPACE){
			//                                       dir = 2;
			UpdateData(TRUE);

	TX_MotionS[0] = 1;

	TX_MotionS[1] = 8;//8

	TX_MotionS[2] = 254;//254
	TX_MotionS[3] = 17;//17

	TX_MotionS[4] = 5;

	TX_MotionS[5] = 5; // 방향 //왼 7 오 8

	TX_MotionS[6] = 0; //보폭

	TX_MotionS[7] = 0;//좌회전
	TX_MotionS[8] = 0;//우회전

	TX_MotionS[9] = 100;//대구리 들고내리고
	TX_MotionS[10] = 100;//대구리 좌우

	TX_MotionS[11] = 0;//대구리 왼쪽값
	TX_MotionS[12] = 0;//대구리 오른쪽값

	TX_MotionS[13] = 0;   

	TX_MotionS[14] = 0;   //0
	TX_MotionS[15] = 254;  //254

	TX_MotionS[16] = 255;

	m_Comm.WriteCommBlock((LPSTR)TX_MotionS, 17);

	/*
	CString TempMsg;
	TempMsg.Format("Send Data"); 
	PrintMsg(TempMsg);   
	*/
	UpdateData(FALSE);
		}
		if(pMsg -> wParam == VK_SHIFT)
		{
			if(CAMERAMODE == SPRINTMODE){
				/*
				if(SPRINTCOLOR.val[0]&&SPRINTCOLOR.val[1]&&SPRINTCOLOR.val[2]){
					AfxMessageBox("색상이 제거되었습니다.");
					SPRINTCOLOR = cvScalar(0,0,0);
				}
				else{
					AfxMessageBox("색상이 추가되었습니다.");
					SPRINTCOLOR = MouseColor;
				}
				*/
				CString stt, stt2;
				//stt2.Format("%d %d %d", (int)OBSTACLEBLOCKCOLOR[0].val[0], (int)OBSTACLEBLOCKCOLOR[0].val[1],(int)OBSTACLEBLOCKCOLOR[0].val[2]);
				stt.Format("%d %d %d", (int)MouseColor.val[0], (int)MouseColor.val[1],(int)MouseColor.val[2]);
				//test.SetWindowTextA(stt);
				AfxMessageBox(stt);
			}
			if(CAMERAMODE == BASKETBALL){
				//OBSTACLECOLOR[ObstacleCount] = MouseColor;
				//ObstacleCount++;
				CString stt, stt2;
				//stt2.Format("%d %d %d", (int)OBSTACLEBLOCKCOLOR[0].val[0], (int)OBSTACLEBLOCKCOLOR[0].val[1],(int)OBSTACLEBLOCKCOLOR[0].val[2]);
				stt.Format("%d %d %d", (int)MouseColor.val[0], (int)MouseColor.val[1],(int)MouseColor.val[2]);
				//test.SetWindowTextA(stt);
				AfxMessageBox(stt);

			}
			if(CAMERAMODE == OBSTACLE){

				//OBSTACLECOLOR[ObstacleCount] = MouseColor;
				//ObstacleCount++;
				CString stt, stt2;
				//stt2.Format("%d %d %d", (int)OBSTACLEBLOCKCOLOR[0].val[0], (int)OBSTACLEBLOCKCOLOR[0].val[1],(int)OBSTACLEBLOCKCOLOR[0].val[2]);
				stt2.Format("%d %d %d", (int)MouseColor.val[0], (int)MouseColor.val[1],(int)MouseColor.val[2]);
				//test.SetWindowTextA(stt);
				AfxMessageBox(stt2);

			}
			if(CAMERAMODE == FOOTBALL){
					AfxMessageBox("색상이 추가되었습니다.");
					FOOTBALLCOLOR = MouseColor;
			
			}
		}
		break;
	default:
		break;
	}
	return CDialog::PreTranslateMessage(pMsg);
}


IplImage* CZSTTEyeProgramDlg::ObstacleFunc(IplImage* img, CvScalar from, CvScalar to)
{

	bool Go = true;
	
	CvScalar	Hsv = R2H(OBSTACLEBLOCKCOLOR[0]);

	CvScalar	from1 = cvScalar(Hsv.val[0] - MinH.GetPos() , Hsv.val[1] - MinS.GetPos() , Hsv.val[2] - MinV.GetPos());
	CvScalar	to1 = cvScalar(Hsv.val[0] + MaxH.GetPos() , Hsv.val[1]+ MaxS.GetPos()  , Hsv.val[2]+ MaxV.GetPos() );
	IplImage * image = GetThresholdImage(img, from1, to1);

	Mat img1(image);

	IplImage * side1,* side2,* side3;

	
	Mat img1dst = img1(Rect(0,0,160,480)).clone();
	Mat img2dst = img1(Rect(160,0,320,480)).clone();
	Mat img3dst = img1(Rect(480,0,160,480)).clone();

	side1 = &IplImage(img1dst);
	side2 = &IplImage(img2dst);
	side3 = &IplImage(img3dst);
	
	





	IplImage* laberled = cvCreateImage(cvGetSize(side2),8,3);

	cvCvtColor (side2, laberled, CV_GRAY2RGB);
	//cvCopyImage(img, laberled);

	CBlobLabeling blob, blobLeft, blobRight, blobBlock ;
	
	blob.SetParam(side2, 100);
	blob.DoLabeling();

	blobLeft.SetParam(side1, 100);
	blobLeft.DoLabeling();

	blobRight.SetParam(side3, 100);
	blobRight.DoLabeling();



	Hsv = R2H(OBSTACLEBLOCKCOLOR[1]);
	
	from1 = cvScalar(Hsv.val[0] - MinH.GetPos() , Hsv.val[1] - MinS.GetPos() , Hsv.val[2] - MinV.GetPos());
	to1 = cvScalar(Hsv.val[0] + MaxH.GetPos() , Hsv.val[1]+ MaxS.GetPos()  , Hsv.val[2]+ MaxV.GetPos() );

	IplImage * CenterBlock = GetThresholdImage(img, from1, to1);

	Mat CenterB(CenterBlock);

	Mat Ctb = CenterB(cvRect(160,0,320,480)).clone();
	IplImage *  CenterImg= &IplImage(Ctb);

	blobBlock.SetParam(CenterImg, 100);
	blobBlock.DoLabeling();







	int CenterExtent = 0;
	int LeftExtent =0;
	int RightExtent	= 0;
	
	for(int i = 0; i < blob.m_nBlobs; i++){
		CenterExtent += (blob.m_recBlobs[i].width * blob.m_recBlobs[i].height);
		/*
		int LowRight = 0;
		int LowLeft = 0;
		for(int i = 0 ; i < blobRight.m_nBlobs; i++){
			if(LowRight < blobRight.m_recBlobs[i].height)
				LowRight = blobRight.m_recBlobs[i].height;
		}

		for(int i = 0 ; i < blobLeft.m_nBlobs; i++){
			if(LowLeft < blobLeft.m_recBlobs[i].height)
				LowLeft = blobLeft.m_recBlobs[i].height;
		}
		
		if(LowRight > LowLeft){
			ObstacleDirect= false;
				
		}
		else if(LowLeft > LowRight){
			ObstacleDirect = true;
		}


		if(pt2.y < 240){
			dir = 2;
	str = 5;
	left =3;
	right=0;
	heady=100;
	headx=100;
	LShift=0;
	RShift=0;
		}

		else{
		if(ObstacleDirect == false )// 왼
		{
			dir = 4;
			str = 5;
			left =0;
			right=0;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
		}
		else{
			dir = 5;
			str = 5;
			left =0;
			right=0;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
		}
		}
		*/
	}
	for(int i = 0; i < blobLeft.m_nBlobs; i++){
		LeftExtent += blobLeft.m_recBlobs[i].width * blobLeft.m_recBlobs[i].height;
	}
	for(int i = 0; i< blobRight.m_nBlobs; i++){
		RightExtent += blobRight.m_recBlobs[i].width * blobRight.m_recBlobs[i].height;
	}
	for(int i = 0; i < blobBlock.m_nBlobs; i++){
		CenterExtent -= blobBlock.m_recBlobs[i].width * blobBlock.m_recBlobs[i].height;
	}	
	CString tt;
	tt.Format("%d %d %d" , LeftExtent, CenterExtent, RightExtent);
	test.SetWindowTextA(tt);
	cvReleaseImage(&image);

	if(CenterExtent < 125000){
		//스탑
		Go = false;
		//test.SetWindowTextA("");
		if(LeftExtent< 30000 && LeftExtent < RightExtent) ObstacleDirect = false;
		else if(RightExtent < 30000&& LeftExtent > RightExtent) ObstacleDirect = true;
	}
	else{
				
		Go = true;
		if(LeftExtent < 20000) Go = false;
		if(RightExtent < 20000) Go = false;
	
	}

	if(Go){
			test.SetWindowTextA("직진");
				dir = 2;
				str = 5;
				left =0;
				right=0;
				heady=100;
				headx=100;
				LShift=0;
				RShift=0;
	}
	else{
		if(ObstacleDirect){
			test.SetWindowTextA("왼쪽");
				dir = 4;
				str = 5;
				left =0;
				right=0;
				heady=100;
				headx=100;
				LShift=0;
				RShift=0;
		}
		else{
			
			test.SetWindowTextA("오른쪽");
				dir = 5;
				str = 5;
				left =0;
				right=0;
				heady=100;
				headx=100;
				LShift=0;
				RShift=0;
		}
	}


	return laberled;
}


IplImage* CZSTTEyeProgramDlg::BasketBallFunc(IplImage* img, CvScalar from, CvScalar to)
{ // 밝기 -2.25
	IplImage * image = GetThresholdImage(img, from, to);


	IplImage* laberled = cvCreateImage(cvGetSize(image),8,3);

	cvCvtColor (image, laberled, CV_GRAY2RGB);
	//cvCopyImage(img, laberled);
	CBlobLabeling blob;
	blob.SetParam(image, 100);


	blob.DoLabeling();

	for(int i = 0; i < blob.m_nBlobs; i++){
		CvPoint pt1 = cvPoint(  blob.m_recBlobs[i].x,
			blob.m_recBlobs[i].y);
		CvPoint pt2 = cvPoint(  pt1.x + blob.m_recBlobs[i].width,
			pt1.y + blob.m_recBlobs[i].height);

		CvScalar color = cvScalar( 0, 0, 255);
		cvDrawRect ( laberled, pt1, pt2, color);


		CvPoint CenterPoint = cvPoint((pt1.x + pt2.x)/2, (pt1.y + pt2.y)/2);

		CString str1;
		str1.Format("%d %d ", CenterPoint.x, CenterPoint.y);
		test.SetWindowTextA(str1);

		if(STAGE == STAGEONE){
			if(CenterPoint.x<565 && CenterPoint.x>575){
				if(CenterPoint.y>185){
					dir = 7;
					str = 0;
					left =0;
					right=0;
					heady=100;
					headx=100;
					LShift=0;
					RShift=0;
					STAGE = STAGETWO;   //49~70
				}
			}
			else if(CenterPoint.y <= 185){
				dir = 2;
				str = 3;
				left =0;
				right=0;
				heady=100;
				headx=100;
				LShift=0;
				RShift=0;
			}

			else if(CenterPoint.x<565){
				dir = 4;
				str = 0;
				left =0;
				right=0;
				heady=100;
				headx=100;
				LShift=60;
				RShift=0;
			}
			else if(CenterPoint.x>575){
				dir = 5;
				str = 0;
				left =0;
				right=0;
				heady=100;
				headx=100;
				LShift=0;
				RShift=60;
			}

		}
		else if(STAGE == STAGETWO){
			BASKETBALLCOLOR = cvScalar(0,0,0);
		}
	}
	cvReleaseImage(&image);
	return laberled;
}


IplImage* CZSTTEyeProgramDlg::makeHisto(int chn)
{

	int histH[256];

	int maxh = 0;

	IplImage* make = cvCreateImage(cvSize(256, maxh),8,3);

	for(int y = 0; y < mainImage->height; y++){
		for(int x = 0; x <mainImage->width; x++){
			int h = (int)cvGet2D(mainImage,y,x).val[chn];


			histH[h] += 1;


			if(maxh < histH[h]) maxh = histH[h];

		}
	}




	for(int i = 0 ; i < 256; i++){
		cvDrawLine(make, cvPoint(i, maxh), cvPoint(i, maxh-histH[i]),cvScalar(255,255,255),1,8,0);
	}


	test.SetWindowTextA("a");

	return make;
}


IplImage* CZSTTEyeProgramDlg::MaratoneFunc(IplImage* img, CvScalar from, CvScalar to)
{
	IplImage * image0 = cvCreateImage(cvGetSize(img),8,1);
	IplImage * image = cvCreateImage(cvGetSize(img),8,1);

	cvCvtColor(img, image0, CV_BGR2GRAY);

	cvThreshold(image0, image, 0.0,255.0, CV_THRESH_BINARY_INV|CV_THRESH_OTSU);

	IplImage* laberled = cvCreateImage(cvGetSize(image),8,3);

	cvCvtColor (image, laberled, CV_GRAY2RGB);
	//cvCopyImage(img, laberled);
	CBlobLabeling blob;
	blob.SetParam(image, 100);


	blob.DoLabeling();

	for(int i = 0; i < blob.m_nBlobs; i++){
		CvPoint pt1 = cvPoint(  blob.m_recBlobs[i].x,
			blob.m_recBlobs[i].y);
		CvPoint pt2 = cvPoint(  pt1.x + blob.m_recBlobs[i].width,
			pt1.y + blob.m_recBlobs[i].height);


		cvSetImageROI(image, blob.m_recBlobs[i]);

		IplImage* sub_gray = cvCreateImage(cvSize(blob.m_recBlobs[i].width, blob.m_recBlobs[i].height), 8, 1);

		cvThreshold(image, sub_gray, 1, 255, CV_THRESH_BINARY_INV);
		cvResetImageROI(image);

		//레이블링

		CBlobLabeling inner;
		inner.SetParam(sub_gray, 100);

		inner.DoLabeling();

		int nSubMinWidth	= sub_gray -> width * 4 / 10;
		int nSubMinHeight	= sub_gray -> height * 4 / 10;

		int nSubMaxWidth	= sub_gray -> width * 8 / 10;
		int nSubMaxHeight	= sub_gray -> height * 8 / 10;

		inner.BlobSmallSizeConstraint(nSubMinWidth, nSubMinHeight);
		inner.BlobBigSizeConstraint(nSubMaxWidth, nSubMaxHeight);

		if(inner.m_nBlobs){
			Mat img(image);
			Mat dst = img(Rect(inner.m_recBlobs[0].x,inner.m_recBlobs[0].y,inner.m_recBlobs[0].width,inner.m_recBlobs[0].height)).clone();
			
			Mat LSide, RSide;
		
			LSide = dst(Rect(0,0,dst.rows/2,dst.cols)).clone();
			RSide = dst(Rect(dst.rows/2,0,dst.rows/2,dst.cols)).clone();
			
			/*
			Mat img1(image);

	IplImage * side1,* side2,* side3;

	
	Mat img1dst = img1(Rect(0,0,160,480)).clone();
	Mat img2dst = img1(Rect(160,0,320,480)).clone();
	Mat img3dst = img1(Rect(480,0,160,480)).clone();

	side1 = &IplImage(img1dst);
	side2 = &IplImage(img2dst);
	side3 = &IplImage(img3dst);
	
	
			*/
			
			IplImage * dstImage = &IplImage(dst);
			IplImage * LImage, * RImage;
			LImage = &IplImage(LSide);
			RImage = &IplImage(RSide);
			if(dstImage->width < dstImage->height){
				test.SetWindowTextA("go");
			}
			else{
				CBlobLabeling left;
				CBlobLabeling right;
				left.SetParam(LImage,100);
				right.SetParam(RImage,100);
				left.DoLabeling();
				right.DoLabeling();

				if(left.m_recBlobs[0].height > right.m_recBlobs[0].height){
					test.SetWindowTextA("right");
				}
				else{
					test.SetWindowTextA("left");
				}
			}

		}



		CvScalar color = cvScalar( 255, 0, 0);
		cvDrawRect ( laberled, pt1, pt2, color);


		CvPoint CenterPoint = cvPoint((pt1.x + pt2.x)/2, (pt1.y + pt2.y)/2);
		//마라톤 코드



	}
	cvReleaseImage(&image);
	return laberled;
}


IplImage* CZSTTEyeProgramDlg::FootballFunc(IplImage* img, CvScalar from, CvScalar to)
{
	IplImage * image = GetThresholdImage(img, from, to);


	IplImage* laberled = cvCreateImage(cvGetSize(image),8,3);

	cvCvtColor (image, laberled, CV_GRAY2RGB);
	//cvCopyImage(img, laberled);
	CBlobLabeling blob;
	blob.SetParam(image, 100);


	blob.DoLabeling();
	if(blob.m_recBlobs){
		Kick = true;
	}
	else 
		Kick = false;

	for(int i = 0; i < blob.m_nBlobs; i++){
		CvPoint pt1 = cvPoint(  blob.m_recBlobs[i].x,
			blob.m_recBlobs[i].y);
		CvPoint pt2 = cvPoint(  pt1.x + blob.m_recBlobs[i].width,
			pt1.y + blob.m_recBlobs[i].height);

		
		CvScalar color = cvScalar( 0, 0, 255);
		cvDrawRect ( laberled, pt1, pt2, color);


		CvPoint CenterPoint = cvPoint((pt1.x + pt2.x)/2, (pt1.y + pt2.y)/2);
		

		//페널티킥 코드
		
		if(Kick){
			
			
			if(CenterPoint.x > 320){

				if(CenterPoint.x <=450 && CenterPoint.x >= 400){
				if(CenterPoint.y >= 425){
					dir = 8;//오른발슛
					str = 0;
					left =0;
					right=0;
					heady=100;
					headx=100;
					LShift=0;
					RShift=0;
				
				}
			}

			else if(CenterPoint.y < 425){
				dir = 2;
				str = 4;
				left =0;
				right=0;
				heady=100;
				headx=100;
				LShift=0;
				RShift=0;
			}
			else if(CenterPoint.x < 400){
				dir = 4;
				str = 0;
				left =0;
				right=0;
				heady=100;
				headx=100;
				LShift=60;
				RShift=0;
			}
			else if(CenterPoint.x > 450){
				dir = 5;
				str = 0;
				left =0;
				right=0;
				heady=100;
				headx=100;
				LShift=0;
				RShift=60;
			}
			}
			else if(CenterPoint.x <= 320){
				if(CenterPoint.x < 255 && CenterPoint.x > 205){
					if(CenterPoint.y >= 425){
					dir = 7; // 왼발
					str = 0;
					left =0;
					right=0;
					heady=100;
					headx=100;
					LShift=0;
					RShift=0;
				}
			}
				else if(CenterPoint.y < 425){
				dir = 2;
				str = 4;
				left =0;
				right=0;
				heady=100;
				headx=100;
				LShift=0;
				RShift=0;
			}
			else if(CenterPoint.x < 205){
				dir = 4;
				str = 0;
				left =0;
				right=0;
				heady=100;
				headx=100;
				LShift=60;
				RShift=0;
			}
			else if(CenterPoint.x > 255){
				dir = 5;
				str = 0;
				left =0;
				right=0;
				heady=100;
				headx=100;
				LShift=0;
				RShift=60;
			}
			}

		}

		CString a;
		a.Format("%d %d %d %d %d", (int)FOOTBALLCOLOR.val[0],(int)FOOTBALLCOLOR.val[1],(int)FOOTBALLCOLOR.val[2],CenterPoint.x, CenterPoint.y);
		test.SetWindowTextA(a);

	}
	cvReleaseImage(&image);
	return laberled;
}


IplImage* CZSTTEyeProgramDlg::FootBallBlockFunc(IplImage* img, CvScalar from, CvScalar to)
{
	IplImage * image = GetThresholdImage(img, from, to);


	IplImage* laberled = cvCreateImage(cvGetSize(image),8,3);

	cvCvtColor (image, laberled, CV_GRAY2RGB);
	//cvCopyImage(img, laberled);
	CBlobLabeling blob;
	blob.SetParam(image, 100);


	blob.DoLabeling();

	for(int i = 0; i < blob.m_nBlobs; i++){
		CvPoint pt1 = cvPoint(  blob.m_recBlobs[i].x,
			blob.m_recBlobs[i].y);
		CvPoint pt2 = cvPoint(  pt1.x + blob.m_recBlobs[i].width,
			pt1.y + blob.m_recBlobs[i].height);


		cvSetImageROI(image, blob.m_recBlobs[i]);

		IplImage* sub_gray = cvCreateImage(cvSize(blob.m_recBlobs[i].width, blob.m_recBlobs[i].height), 8, 1);

		cvThreshold(image, sub_gray, 1, 255, CV_THRESH_BINARY_INV);
		cvResetImageROI(image);

		//레이블링

		CBlobLabeling inner;
		inner.SetParam(sub_gray, 100);

		inner.DoLabeling();

		int nSubMinWidth	= sub_gray -> width * 4 / 10;
		int nSubMinHeight	= sub_gray -> height * 4 / 10;

		int nSubMaxWidth	= sub_gray -> width * 8 / 10;
		int nSubMaxHeight	= sub_gray -> height * 8 / 10;

		inner.BlobSmallSizeConstraint(nSubMinWidth, nSubMinHeight);
		inner.BlobBigSizeConstraint(nSubMaxWidth, nSubMaxHeight);

		for(int j=0; j < inner.m_nBlobs; j++)
		{
			int nThick = 20;

			if( inner.m_recBlobs[j].x < nThick ||
				inner.m_recBlobs[j].y < nThick ||
				(sub_gray->width - (inner.m_recBlobs[j].x + inner.m_recBlobs[j].width)) < nThick ||
				(sub_gray->height - (inner.m_recBlobs[j].y + inner.m_recBlobs[j].height)) < nThick ) continue;


			CvPoint s_pt1 = cvPoint(	pt1.x + inner.m_recBlobs[j].x,
				pt1.y + inner.m_recBlobs[j].y);
			CvPoint s_pt2 = cvPoint(	s_pt1.x + inner.m_recBlobs[j].width,
				s_pt1.y + inner.m_recBlobs[j].height);

			CvScalar green = cvScalar(0, 255, 0);

			cvDrawRect(laberled, s_pt1, s_pt2, green, 2);
			cvReleaseImage(&sub_gray);
		}



		CvScalar color = cvScalar( 0, 0, 255);
		cvDrawRect ( laberled, pt1, pt2, color);


		CvPoint CenterPoint = cvPoint((pt1.x + pt2.x)/2, (pt1.y + pt2.y)/2);


		//축구


	}
	cvReleaseImage(&image);
	return laberled;
}


IplImage* CZSTTEyeProgramDlg::BasketBallBasketFunc(IplImage* img, CvScalar from, CvScalar to)
{

	IplImage * image = GetThresholdImage(img, from, to);


	IplImage* laberled = cvCreateImage(cvGetSize(image),8,3);

	cvCvtColor (image, laberled, CV_GRAY2RGB);
	//cvCopyImage(img, laberled);
	CBlobLabeling blob;
	blob.SetParam(image, 100);


	blob.DoLabeling();

	for(int i = 0; i < blob.m_nBlobs; i++){
		CvPoint pt1 = cvPoint(  blob.m_recBlobs[i].x,
			blob.m_recBlobs[i].y);
		CvPoint pt2 = cvPoint(  pt1.x + blob.m_recBlobs[i].width,
			pt1.y + blob.m_recBlobs[i].height);


		cvSetImageROI(image, blob.m_recBlobs[i]);

		IplImage* sub_gray = cvCreateImage(cvSize(blob.m_recBlobs[i].width, blob.m_recBlobs[i].height), 8, 1);

		cvThreshold(image, sub_gray, 1, 255, CV_THRESH_BINARY_INV);
		cvResetImageROI(image);

		//레이블링

		CBlobLabeling inner;
		inner.SetParam(sub_gray, 100);

		inner.DoLabeling();

		int nSubMinWidth	= sub_gray -> width * 4 / 10;
		int nSubMinHeight	= sub_gray -> height * 4 / 10;

		int nSubMaxWidth	= sub_gray -> width * 8 / 10;
		int nSubMaxHeight	= sub_gray -> height * 8 / 10;

		inner.BlobSmallSizeConstraint(nSubMinWidth, nSubMinHeight);
		inner.BlobBigSizeConstraint(nSubMaxWidth, nSubMaxHeight);

		for(int j=0; j < inner.m_nBlobs; j++)
		{
			int nThick = 20;

			if( inner.m_recBlobs[j].x < nThick ||
				inner.m_recBlobs[j].y < nThick ||
				(sub_gray->width - (inner.m_recBlobs[j].x + inner.m_recBlobs[j].width)) < nThick ||
				(sub_gray->height - (inner.m_recBlobs[j].y + inner.m_recBlobs[j].height)) < nThick ) continue;


			CvPoint s_pt1 = cvPoint(	pt1.x + inner.m_recBlobs[j].x,
				pt1.y + inner.m_recBlobs[j].y);
			CvPoint s_pt2 = cvPoint(	s_pt1.x + inner.m_recBlobs[j].width,
				s_pt1.y + inner.m_recBlobs[j].height);


			CvScalar green = cvScalar(0, 255, 0);

			cvDrawRect(laberled, s_pt1, s_pt2, green, 2);
			cvReleaseImage(&sub_gray);
		}



		CvScalar color = cvScalar( 255, 0, 0);
		cvDrawRect ( laberled, pt1, pt2, color);


		CvPoint CenterPoint = cvPoint((pt1.x + pt2.x)/2, (pt1.y + pt2.y)/2);
		//베스킷볼 코드

		if(STAGE == STAGETHREE){
			if(CenterPoint.x >300  && CenterPoint.x < 340){
				if(pt1.x >120){
					dir = 2;
					str = 5;
					left =0;
					right=0;
					heady=180;
					headx=100;
					LShift=0;
					RShift=0;
				}
				else {
					dir = 6;
					str = 0;
					left =0;
					right=0;
					heady=180;
					headx=100;
					LShift=0;
					RShift=0;
				}

			}
			else if(CenterPoint.x <300){
				dir = 2;
				str = 0;
				left =10;
				right=0;
				heady=180;
				headx=100;
				LShift=0;
				RShift=0;
			}
			else if(CenterPoint.x >340){
				dir = 2;
				str = 0;
				left =0;
				right=10;
				heady=180;
				headx=100;
				LShift=0;
				RShift=0;
			}
		}


	}
	cvReleaseImage(&image);
	return laberled;
}


void CZSTTEyeProgramDlg::OnBnClickedButton1()
{
	SetTimer(1, 2000, NULL);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CZSTTEyeProgramDlg::OnBnClickedButton2()
{
	KillTimer(1);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CZSTTEyeProgramDlg::OnStnClickedTest()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}



void CZSTTEyeProgramDlg::OnBnClickedButton3()
{

	OnSendData((unsigned char)atoi(eDir),(unsigned char)atoi(eStr),(unsigned char)atoi(eLeft),(unsigned char)atoi(eRight),(unsigned char)atoi(eHeadY),(unsigned char)atoi(eHeadY),(unsigned char)atoi(eLShift),(unsigned char)atoi(eRShift));
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


IplImage* CZSTTEyeProgramDlg::FindContours(IplImage* img)
{
	//CvScalar fromBlack = cvScalar(0, 0, 0);
	//CvScalar toBlack = cvScalar(100, 100, 100);

	IplImage* binMark = cvCreateImage( cvGetSize(img), 8, 1 );

	// 원본 영상 그레이 변환
	cvCvtColor( img, binMark, CV_BGR2GRAY );

	cvThreshold( binMark, binMark, 0.0, 255.0, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);

	// 레이블링
	CBlobLabeling blob;
	blob.SetParam( binMark, 100 );	// 레이블링 할 이미지와 최소 픽셀수 등을 설정

	blob.DoLabeling();

	// 노이즈 제거
	int nMaxWidth	= binMark->width  * 9 / 10;	
	int nMaxHeight	= binMark->height * 9 / 10;	

	blob.BlobSmallSizeConstraint( 40, 40 );
	blob.BlobBigSizeConstraint( nMaxWidth, nMaxHeight );

	for( int i=0; i < blob.m_nBlobs; i++ )
	{

		CvPoint	pt1 = cvPoint(	blob.m_recBlobs[i].x,				
			blob.m_recBlobs[i].y );
		CvPoint pt2 = cvPoint(	pt1.x + blob.m_recBlobs[i].width,	
			pt1.y + blob.m_recBlobs[i].height );

		IplImage* sub_gray = cvCreateImage( cvSize( blob.m_recBlobs[i].width, blob.m_recBlobs[i].height ), 8, 1 );

		blob.GetBlobImage( sub_gray, i );

		// 이미지 반전
		cvThreshold( sub_gray, sub_gray, 1, 255, CV_THRESH_BINARY_INV );

		// 레이블링
		CBlobLabeling inner;
		inner.SetParam( sub_gray, 70 );

		inner.DoLabeling();


		CvScalar	Hsv = R2H(SPRINTCOLOR);

	CvScalar	from1 = cvScalar(Hsv.val[0] - MinH.GetPos() , Hsv.val[1] - MinS.GetPos() , Hsv.val[2] - MinV.GetPos());
	CvScalar	to1 = cvScalar(Hsv.val[0] + MaxH.GetPos() , Hsv.val[1] + MaxS.GetPos()  , Hsv.val[2]+ MaxV.GetPos() );
	IplImage * lineimage = GetThresholdImage(img, from1, to1);

		CBlobLabeling Lineblob;

		Lineblob.SetParam(lineimage , 50);

		Lineblob.DoLabeling();

		bool Go = true;


		dir = 2;
		// 노이즈 제거
		int nSubMinWidth	= sub_gray->width * 4 / 10;
		int nSubMinHeight	= sub_gray->height * 4 / 10;

		int nSubMaxWidth	= sub_gray->width * 9 / 10;
		int nSubMaxHeight	= sub_gray->height * 9 / 10;

		inner.BlobSmallSizeConstraint( nSubMinWidth, nSubMinHeight );
		inner.BlobBigSizeConstraint( nSubMaxWidth, nSubMaxHeight );

		for( int j=0; j < inner.m_nBlobs; j++ )
		{
			int nThick = 8;
			if(		inner.m_recBlobs[j].x < nThick
				||	inner.m_recBlobs[j].y < nThick 
				||	( sub_gray->width - (inner.m_recBlobs[j].x + inner.m_recBlobs[j].width) ) < nThick
				||	( sub_gray->height - (inner.m_recBlobs[j].y + inner.m_recBlobs[j].height) ) < nThick )	continue;

			// 마커를 찾은 경우
			CvPoint	s_pt1 = cvPoint(	pt1.x + inner.m_recBlobs[j].x,				
				pt1.y + inner.m_recBlobs[j].y );
			CvPoint s_pt2 = cvPoint(	s_pt1.x + inner.m_recBlobs[j].width,	
				s_pt1.y + inner.m_recBlobs[j].height );


			//--------------------------------------------------------------------------------------겉 사각형 꼭지점 출력
			if(inner.m_recBlobs){
				CvMemStorage* out_storage	= cvCreateMemStorage(0);
				CvSeq* out_contours			= 0;

				// 1. 윤곽 검출
				int mw	= blob.m_recBlobs[i].width;
				int mh	= blob.m_recBlobs[i].height;

				IplImage* out_marker = cvCreateImage( cvSize(mw, mh), 8, 1 );

				blob.GetBlobImage( out_marker, i );

				cvFindContours( out_marker, out_storage, &out_contours, sizeof (CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

				double out_fMaxDist;

				CvPoint	out_corner[4];

				for(int x=0; x < 4; x++ )		out_corner[x] = cvPoint(0, 0);

				// 초기 위치 설정
				CvPoint *out_st  = (CvPoint *)cvGetSeqElem( out_contours, 0 );

				// 첫 번 째 꼭지점 추출(최대 거리를 가지는 점 선택)
				out_fMaxDist = 0.0;

				for(int x = 1; x < out_contours->total; x++ )
				{
					CvPoint* pt = (CvPoint *)cvGetSeqElem( out_contours, x );

					double fDist = sqrt( (double)(( out_st->x - pt->x ) * ( out_st->x - pt->x ) + ( out_st->y - pt->y ) * ( out_st->y - pt->y ) ));

					if( fDist > out_fMaxDist )
					{
						out_corner[0] = *pt;

						out_fMaxDist = fDist;
					}
				}

				// 두 번 째 꼭지점 추출(첫 번 째 꼭지점에서 최대 거리를 가지는 점 선택)
				out_fMaxDist = 0.0;

				for(int x = 0; x < out_contours->total; x++ )
				{
					CvPoint* pt = (CvPoint *)cvGetSeqElem( out_contours, x );

					double fDist = sqrt( (double)(( out_corner[0].x - pt->x ) * ( out_corner[0].x - pt->x ) + ( out_corner[0].y - pt->y ) * ( out_corner[0].y - pt->y ) ));

					if( fDist > out_fMaxDist )
					{
						out_corner[1] = *pt;

						out_fMaxDist = fDist;
					}
				}

				// 세 번 째 꼭지점 추출(첫 번 째, 두 번 째 꼭지점에서 최대 거리를 가지는 점 선택)
				out_fMaxDist = 0.0;

				for(int x = 0; x < out_contours->total; x++ )
				{
					CvPoint* pt = (CvPoint *)cvGetSeqElem( out_contours, x );

					double fDist =		sqrt( (double)(( out_corner[0].x - pt->x ) * ( out_corner[0].x - pt->x ) + ( out_corner[0].y - pt->y ) * ( out_corner[0].y - pt->y ) ))
						+	sqrt( (double)(( out_corner[1].x - pt->x ) * ( out_corner[1].x - pt->x ) + ( out_corner[1].y - pt->y ) * ( out_corner[1].y - pt->y ) ));

					if( fDist > out_fMaxDist )
					{
						out_corner[2] = *pt;

						out_fMaxDist = fDist;
					}
				}

				// 네 번 째 꼭지점 추출
				// (벡터 내적을 이용하여 좌표평면에서 사각형의 너비의 최대 값을 구한다.)
				int x1 = out_corner[0].x;	int y1 = out_corner[0].y;			
				int x2 = out_corner[1].x;	int y2 = out_corner[1].y;
				int x3 = out_corner[2].x;	int y3 = out_corner[2].y;

				int nMaxDim = 0;

				for(int x = 0; x < out_contours->total; x++ )
				{
					CvPoint* pt = (CvPoint *)cvGetSeqElem( out_contours, x );

					int posX= pt->x;
					int posY = pt->y;

					int nDim =		abs( ( x1 * y2 + x2 * posY  + posX  * y1 ) - ( x2 * y1 + posX  * y2 + x1 * posY  ) )
						+	abs( ( x1 * posY  + posX  * y3 + x3 * y1 ) - ( posX  * y1 + x3 * posY  + x1 * y3 ) )
						+	abs( ( posX  * y2 + x2 * y3 + x3 * posY  ) - ( x2 * posY  + x3 * y2 + posX  * y3 ) );

					if( nDim > nMaxDim )
					{
						out_corner[3] = *pt;

						nMaxDim = nDim;
					}
				}


				//CvFont out_font;
				//cvInitFont(&out_font, CV_FONT_HERSHEY_SIMPLEX, .5, .5, 0, 1, 8);
				CString out_text;

				for( int m=0; m < 4; m++ )
				{
					int x = out_corner[m].x + blob.m_recBlobs[i].x;
					int y = out_corner[m].y + blob.m_recBlobs[i].y;

					cvCircle( img, cvPoint(x, y), 2, CV_RGB(0,255,255), 2 );
					rectPoint.push_back(cvPoint(x,y));

					out_text.Format("(%d, %d)", x, y );

					x += 5;	// 5픽셀 밀어서 출력
				//	cvPutText (img, (LPSTR)(LPCTSTR)out_text, cvPoint(x, y), &out_font, CV_RGB(250,250,100));
				}

				cvReleaseImage(&out_marker);
				cvReleaseMemStorage(&out_storage);

				//사각형 중심정 구하기
				rectCenter = cvPoint((pt1.x+pt2.x)/2, (pt1.y+pt2.y)/2);

			}//겉 사각형의 꼭지점 찾기 끝

			//내부 화살표-------------------------------------------------------
			if(inner.m_recBlobs){
				CvMemStorage* in_storage = cvCreateMemStorage(0);
				CvSeq* in_contours = 0;

				int in_mw	= inner.m_recBlobs[j].width;
				int in_mh	= inner.m_recBlobs[j].height;

				IplImage* in_marker = cvCreateImage(cvSize(in_mw, in_mh), 8,1);

				inner.GetBlobImage(in_marker,j);
				cvFindContours( in_marker, in_storage, &in_contours, sizeof (CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
				cvDrawContours( img, in_contours, CV_RGB (255, 100, 0), CV_RGB (0, 255, 0), 1, 2, 8, cvPoint (inner.m_recBlobs[j].x + blob.m_recBlobs[i].x, inner.m_recBlobs[j].y + blob.m_recBlobs[i].y));

				double in_fMaxDist;

				CvPoint		in_corner[4];

				for(int x=0; x < 4; x++ )		in_corner[x] = cvPoint(0, 0);

				// 초기 위치 설정
				CvPoint *in_st  = (CvPoint *)cvGetSeqElem( in_contours, 0 );

				// 첫 번 째 꼭지점 추출(최대 거리를 가지는 점 선택)
				in_fMaxDist = 0.0;

				for (int x = 1; x < in_contours->total; x++)
				{
					CvPoint* pt = (CvPoint *)cvGetSeqElem( in_contours, x );

					double fDist = sqrt( (double)(( in_st->x - pt->x ) * ( in_st->x - pt->x ) + ( in_st->y - pt->y ) * ( in_st->y - pt->y ) ));

					if( fDist > in_fMaxDist )
					{
						in_corner[0] = *pt;
						in_fMaxDist = fDist;
					}
				}

				// 두 번 째 꼭지점 추출(첫 번 째 꼭지점에서 최대 거리를 가지는 점 선택)
				in_fMaxDist = 0.0;

				for (int x = 0; x < in_contours->total; x++)
				{
					CvPoint* pt = (CvPoint *)cvGetSeqElem( in_contours, x );

					double fDist = sqrt( (double)(( in_corner[0].x - pt->x ) * ( in_corner[0].x - pt->x ) + ( in_corner[0].y - pt->y ) * ( in_corner[0].y - pt->y ) ));

					if( fDist > in_fMaxDist )
					{
						in_corner[1] = *pt;
						in_fMaxDist = fDist;
					}
				}

				// 세 번 째 꼭지점 추출(첫 번 째, 두 번 째 꼭지점에서 최대 거리를 가지는 점 선택)
				in_fMaxDist = 0.0;

				for(int x = 0; x < in_contours->total; x++ )
				{
					CvPoint* pt = (CvPoint *)cvGetSeqElem( in_contours, x );

					double fDist =		sqrt( (double)(( in_corner[0].x - pt->x ) * ( in_corner[0].x - pt->x ) + ( in_corner[0].y - pt->y ) * ( in_corner[0].y - pt->y ) ))
						+	sqrt( (double)(( in_corner[1].x - pt->x ) * ( in_corner[1].x - pt->x ) + ( in_corner[1].y - pt->y ) * ( in_corner[1].y - pt->y ) ));

					if( fDist > in_fMaxDist )
					{
						in_corner[2] = *pt;
						in_fMaxDist = fDist;
					}
				}


				// 네 번 째 꼭지점 추출(두 번째, 세 번째에서 최대거리)
				in_fMaxDist = 0.0;

				for(int x = 0; x < in_contours->total; x++ )
				{
					CvPoint* pt = (CvPoint *)cvGetSeqElem( in_contours, x );

					double fDist =		sqrt( (double)(( in_corner[1].x - pt->x ) * ( in_corner[1].x - pt->x ) + ( in_corner[1].y - pt->y ) * ( in_corner[1].y - pt->y ) ))
						+	sqrt( (double)(( in_corner[2].x - pt->x ) * ( in_corner[2].x - pt->x ) + ( in_corner[2].y - pt->y ) * ( in_corner[2].y - pt->y ) ));

					if( fDist > in_fMaxDist )
					{
						in_corner[3] = *pt;
						in_fMaxDist = fDist;
					}
				}


				////CvFont in_font;
				//cvInitFont(&in_font, CV_FONT_HERSHEY_SIMPLEX, .5, .5, 0, 1, 8);
				CString in_text;

				for( int m=0; m < 4; m++ )
				{
					int x = in_corner[m].x + inner.m_recBlobs[j].x + blob.m_recBlobs[i].x;
					int y = in_corner[m].y + inner.m_recBlobs[j].y + blob.m_recBlobs[i].y;

					cvCircle( img, cvPoint(x, y), 2, CV_RGB(0,0,255), 2 );
					arrowPoint.push_back(cvPoint(x,y));

					in_text.Format("(%d, %d)", x, y );

					x += 5;	// 5픽셀 밀어서 출력
					//cvPutText (img, (LPSTR)(LPCTSTR)in_text, cvPoint(x, y)  , &in_font, CV_RGB(255,0,255));//내부 꼭지점 좌표 출력
				}


				cvReleaseMemStorage(&in_storage);	
				cvReleaseImage(&in_marker);
			}	//내부 화살표 찾기 끝
		}

		cvReleaseImage(&sub_gray);


		if(!rectPoint.empty()){
			//중앙점 표시
		//	CvFont center_font;
		//	cvInitFont(&center_font, CV_FONT_HERSHEY_SIMPLEX, .5, .5, 0, 1, 8);
			CString center_text;

			center_text.Format("(%d, %d)", rectCenter.x, rectCenter.y );
		//	cvPutText (img, (LPSTR)(LPCTSTR)center_text, cvPoint((rectCenter.x)+5, (rectCenter.y)+5) , &center_font, CV_RGB(255,0,255));
			//중앙점 표시


			//theta 구함 -> theta/-theta
			double theta = getTheta(rectPoint);
			CvPoint centerPos, arrowPos;	//변환했을때의 center점과 목표점

			rectArrow = getMinYPos(arrowPoint);

			if( sin(theta) >= sqrt(2) / 2.0) {
				if(theta == 0) {
					centerPos = getTransPos(getMinYPos(rectPoint), rectCenter, theta, 2);
					arrowPos = getTransPos(getMinYPos(rectPoint), rectArrow,  theta, 2);
				} else {
					centerPos = getTransPos(getMinYPos(rectPoint), rectCenter, M_PI/2.0 - theta, 2);
					arrowPos = getTransPos(getMinYPos(rectPoint), rectArrow,  M_PI/2.0 - theta, 2);
				}
			} else if(sin(theta) < sqrt(2) / 2.0){
				centerPos = getTransPos(getMinXPos(rectPoint), rectCenter, theta, 1);
				arrowPos = getTransPos(getMinXPos(rectPoint), rectArrow,theta, 1);
			}

			double gradient = getGradient(centerPos, arrowPos);

			if(centerPos.x != arrowPos.x) {
				if(abs(gradient) > 2.0) {
				

				} else {
					rectArrow = getMaxYPos(arrowPoint);
					if( sin(theta) >= sqrt(2) / 2.0) {
						arrowPos = getTransPos(getMinYPos(rectPoint), rectArrow,  M_PI/2.0 - theta, 2);
					} else if(sin(theta) < sqrt(2) / 2.0){
						arrowPos = getTransPos(getMinXPos(rectPoint), rectArrow,theta, 1);
					}
					gradient = getGradient(centerPos, arrowPos);

					if(gradient > 0) {
						Go = false;
						ObstacleDirect = true;
					}

					if(gradient < 0) {
						Go = false;
						ObstacleDirect = false;
					}
				}
			} else {
				test.SetWindowTextA("straight");
			}

			/*
			if(theta == 0){
			if(abs(getGradient(rectCenter, getMinYPos(arrowPoint))) > 2.0)
			test.SetWindowTextA("straight");
			else{
			if(getGradient(rectCenter, getMaxYPos(arrowPoint)) > 0)
			test.SetWindowTextA("left");
			else if(getGradient(rectCenter, getMaxYPos(arrowPoint)) < 0)
			test.SetWindowTextA("right");
			}
			}
			*/

			
		}
		else {
			test.SetWindowTextA("none");
		}
		if(Go){
		for(int i = 0; i< Lineblob.m_nBlobs; i++){
								CvPoint pt1 = cvPoint(  blob.m_recBlobs[i].x,
			blob.m_recBlobs[i].y);
		CvPoint pt2 = cvPoint(  pt1.x + blob.m_recBlobs[i].width,
			pt1.y + blob.m_recBlobs[i].height);
		CvPoint CenterPoint = cvPoint((pt1.x + pt2.x)/2, (pt1.y + pt2.y)/2);
				str = 7;
		if(pt1.x < 240){
			test.SetWindowTextA("left");
			
			str = 5;
			left= 7;
			right=0;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(pt1.x < 200){
			test.SetWindowTextA("Bigleft");
		
			str = 5;
			left= 8;
			right=0;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(pt1.x < 180){
			test.SetWindowTextA("Bigleft");
		
			str = 5;
			left= 9;
			right=0;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(pt1.x < 160){
			test.SetWindowTextA("Bigleft");
			
			str = 5;
			left= 10;
			right=0;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(pt1.x < 140){
			test.SetWindowTextA("Bigleft");
		
			str = 5;
			left= 11;
			right=0;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(pt1.x < 120){
			test.SetWindowTextA("Bigleft");
			
			str = 5;
			left= 12;
			right=0;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(pt1.x < 100){
			test.SetWindowTextA("Bigleft");
			
			str = 5;
			left= 13;
			right= 0;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(pt1.x < 80){
			test.SetWindowTextA("Bigleft");
			str = 5;
			left= 14;
			right=0;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(pt1.x < 60){
			test.SetWindowTextA("Bigleft");
			
			str = 5;
			left= 15;
			right=0;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(pt1.x < 40){
			test.SetWindowTextA("Bigleft");
			
			str = 5;
			left= 16;
			right=0;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(pt1.x < 20){
			test.SetWindowTextA("Bigleft");
			
			str = 5;
			left= 17;
			right=0;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}

			else if(pt1.x > 400){
			test.SetWindowTextA("right");

			str = 5;
			left= 0;
			right=12;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(pt1.x > 420){
			test.SetWindowTextA("Bigright");
			
			str = 5;
			left= 0;
			right=13;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(pt1.x > 440){
			test.SetWindowTextA("Bigright");
			
			str = 5;
			left= 0;
			right=14;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(pt1.x > 460){
			test.SetWindowTextA("Bigright");
			
			str = 5;
			left= 0;
			right=15;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(pt1.x > 480){
			test.SetWindowTextA("Bigright");
			
			str = 5;
			left= 0;
			right=16;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(pt1.x > 500){
			test.SetWindowTextA("Bigright");
			
			str = 5;
			left= 0;
			right=17;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(pt1.x > 520){
			test.SetWindowTextA("Bigright");
			
			str = 5;
			left= 0;
			right=18;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(pt1.x > 540){
			test.SetWindowTextA("Bigright");
		
			str = 5;
			left= 0;
			right=19;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(pt1.x > 560){
			test.SetWindowTextA("Bigright");
			
			str = 5;
			left= 0;
			right=20;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(pt1.x > 580){
			test.SetWindowTextA("Bigright");
		
			str = 5;
			left= 0;
			right=21;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(pt1.x > 600){
			test.SetWindowTextA("Bigright");
			
			str = 5;
			left= 0;
			right=22;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(pt1.x > 620){
			test.SetWindowTextA("Bigright");
			
			str = 5;
			left= 0;
			right=23;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else if(pt1.x > 640){
			test.SetWindowTextA("Bigright");
			
			str = 5;
			left= 0;
			right=24;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
			else{
			test.SetWindowTextA("Go St");
			
			str = 5;
			left= 0;
			right=0;
			heady=100;
			headx=100;
			LShift=0;
			RShift=0;
			}
		}
		}
		else{
			if(ObstacleDirect){
				dir = 5;
			}
			else{
				dir = 4; 
					
			}
		}
	}
	
	return binMark;
}


CvPoint CZSTTEyeProgramDlg::getMinXPos(vector<CvPoint> a)
{
	CvPoint minXPoint = cvPoint(1000,1000);
	for(unsigned int i = 0; i<a.size(); i++){
		if(minXPoint.x > a[i].x){ 
			minXPoint = a[i];
		}
	}
	return minXPoint;
}


CvPoint CZSTTEyeProgramDlg::getMaxXPos(vector<CvPoint> a)
{
	CvPoint maxXPoint = cvPoint(0,0);
	for(unsigned int i =0; i<a.size(); i++){
		if(maxXPoint.x <a[i].x){
			maxXPoint = a[i];
		}
	}
	return maxXPoint;
}


CvPoint CZSTTEyeProgramDlg::getMinYPos(vector<CvPoint> a)
{
	CvPoint minXPoint = cvPoint(1000,1000);
	for(unsigned int i = 0; i<a.size(); i++){
		if(minXPoint.x > a[i].x){
			minXPoint = a[i];
		}
	}
	return minXPoint;
}


CvPoint CZSTTEyeProgramDlg::getMaxYPos(vector<CvPoint> a)
{
		CvPoint maxYPoint = cvPoint(0,0);
	for(unsigned int i =0; i<a.size(); i++){
		if(maxYPoint.y <a[i].y){
			maxYPoint = a[i];
		}
	}
	return maxYPoint;
}


double CZSTTEyeProgramDlg::getTheta(vector<CvPoint> a)
{
		//getTheta
	CvPoint minX = getMinXPos(a);
	CvPoint minY = getMinYPos(a);

	int thetaX = minY.x-minX.x;
	int thetaY = minY.y-minX.y;


	if( minY.x == minX.x || minY.y == minX.y || minY.x == minX.y)		// 사각형이 뒤틀리지 않고 정바를때
		return 0.0;

	else
		return atan(thetaY/thetaX);
}


CvPoint CZSTTEyeProgramDlg::getTransPos(CvPoint sPos, CvPoint prePos, double theta, int mode)
{
	CvPoint modPos;
	int disX, disY; 

	disX = prePos.x - sPos.x;
	disY = prePos.y - sPos.y; 

	modPos = cvPoint(disX, disY);		//이동변환

	if(mode == 1) {
		modPos.x = (double)modPos.x * cos(theta) - (double)modPos.y * sin(theta);
		modPos.y = (double)modPos.x * sin(theta) + (double)modPos.y * cos(theta);
	} else if(mode == 2) {
		modPos.x = (double)modPos.x * cos(theta) + (double)modPos.y * sin(theta);
		modPos.y = -(double)modPos.x * sin(theta) + (double)modPos.y * cos(theta);
	}

	return modPos;
}


double CZSTTEyeProgramDlg::getGradient(CvPoint & p1, CvPoint & p2)
{
	//p1과 p2의 기울기를 반환
	double gradient;
	if((p1.x-p2.x) == 0)
		gradient = 0;  
	else
		gradient =  (p1.y-p2.y)/(p1.x-p2.x);
	return gradient;
}
