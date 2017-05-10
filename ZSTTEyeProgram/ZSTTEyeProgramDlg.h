
// ZSTTEyeProgramDlg.h : 헤더 파일
//

#pragma once

#include "afxwin.h"
#include "comm.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "CvvImage.h"
#include <vector>
#include <math.h>
#include "afxcmn.h"



using namespace cv;
using namespace std;

// CZSTTEyeProgramDlg 대화 상자
class CZSTTEyeProgramDlg : public CDialog
{
// 생성입니다.
public:
	CZSTTEyeProgramDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
	int m_nSettingStop;
	TTYSTRUCT Int2TTY();
	TTYSTRUCT LoadSettings();
	struct Msg;
// 대화 상자 데이터입니다.
	enum { IDD = IDD_ZSTTEYEPROGRAM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;
	CComm m_Comm;
	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LONG OnReceiveData(UINT,LONG);
	DECLARE_MESSAGE_MAP()
public:
	int m_nSettingBaud;
	int m_nSettingPort;
	CString m_EditData;
	afx_msg void OnBnClickedStartConnection();
	int m_nSettingData;
	int m_nSettingParity;
	int m_nSettingFlow;
	unsigned char TX_MotionS[17];
	Msg *SendMsg;
	void SaveSetting(void);
	afx_msg void OnBnClickedCancel();
	bool sendData;
	bool ObstacleDirect;
	void OnSendData(unsigned char dir,unsigned char str,unsigned char left,unsigned char right,unsigned char heady,unsigned char headx,unsigned char LShift,unsigned char RShift);
	CStatic originCamera;
	CStatic opencvCamera;
	int count;
	CvCapture *capture; // 영상 불러오기
	IplImage *mainImage;
	IplImage *opencvImage;
	CvvImage CvvImage;
	CvHistogram * hist;
	IplImage *histImgH;
	IplImage *histImgS;
	IplImage *histImgV;
	CvScalar SPRINTCOLOR;
	CvScalar BASKETBALLCOLOR;
	CvScalar BASKETBALLBASKETCOLOR;
	int ObstacleCount;
	CvScalar OBSTACLECOLOR[100];

	CvScalar OBSTACLEBLOCKCOLOR[3];
	CvScalar OBSTACLELINECOLRO;
	CvScalar CollorBuffer[100];
	CvScalar FOOTBALLCOLOR;
	CvScalar FOOTBALLBLOCKCOLOR;
	CPoint MousePoint;
	CvScalar MouseColor;
	bool Kick;
	int CAMERAMODE;
	
	
	vector<CvPoint> arrowPoint;
	vector<CvPoint> rectPoint;
	
	CvPoint rectCenter;



	CStatic text_Theta;
	CStatic preCenterX;
	CStatic preCenterY;
	CStatic CenterX;
	CStatic CenterY;

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnCbnCloseupOcMode();
	CComboBox cameramodeBox;
	IplImage* GetThresholdImage(IplImage* img, CvScalar from, CvScalar to);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	CvScalar R2H(CvScalar in);
	CvScalar H2R(CvScalar in);
	IplImage* SprintFunc(IplImage* img , CvScalar from, CvScalar to);
	CStatic testeeeee;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	IplImage* ObstacleFunc(IplImage* img, CvScalar from, CvScalar to);
	CStatic test;
	CStatic HistH;
	CStatic HistS;
	CStatic HistV;
	CSliderCtrl MaxH;
	CSliderCtrl MaxS;
	CSliderCtrl MaxV;
	CSliderCtrl MinH;
	CSliderCtrl MinS;
	CSliderCtrl MinV;
	IplImage* BasketBallFunc(IplImage* img, CvScalar from, CvScalar in);
	IplImage* makeHisto(int c);
	IplImage* MaratoneFunc(IplImage* img, CvScalar from, CvScalar to);
	IplImage* FootballFunc(IplImage* img, CvScalar from, CvScalar to);
	IplImage* FootBallBlockFunc(IplImage* img, CvScalar from, CvScalar to);
	IplImage* BasketBallBasketFunc(IplImage* img, CvScalar from, CvScalar to);

	int STAGE;
	unsigned char dir;
	unsigned char str;
	unsigned char left;
	unsigned char right;
	unsigned char heady;
	unsigned char headx;
	unsigned char LShift;
	unsigned char RShift;

	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnStnClickedTest();
	afx_msg void OnEnChangeEdit3();
	CString eStr;
	CString eRShift;
	CString eRight;
	CString eLeft;
	CString eHeadY;
	CString eHeadX;
//	CEdit eLShift;
	CString eLShift;
	CString eDir;
	afx_msg void OnBnClickedButton3();
	IplImage* FindContours(IplImage* img);
	CvPoint getMinXPos(vector<CvPoint> a);
	CvPoint getMaxXPos(vector<CvPoint> a);
	CvPoint getMinYPos(vector<CvPoint> a);
	CvPoint getMaxYPos(vector<CvPoint> a);
	CvPoint rectArrow;
	double getTheta(vector<CvPoint> a);
	CvPoint getTransPos(CvPoint sPos, CvPoint prePos, double theta,int mode);
	double getGradient(CvPoint & p1, CvPoint & p2);
};
