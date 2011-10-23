#pragma once

#include <mmsystem.h>
#include <mmreg.h>
// CWavePlayer 对话框

#pragma comment(lib,"winmm.lib")

class CWavePlayer : public CDialog
{
	DECLARE_DYNAMIC(CWavePlayer)

public:
	CWavePlayer(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWavePlayer();

// 对话框数据
	enum { IDD = IDD_DIALOGPLAYER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedPlay();
	afx_msg void OnBnClickedPause();
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedEnd();
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	HICON m_hIcon;
	int m_nSlider1;
	CSliderCtrl* pSlide1;
	CListCtrl *pList5;
	int nItem;		//listctrl的行号
	CString strFileName; //wave文件名
	CString strFileSize;	//wave文件字节长
	DWORD dwFileSize;

	BOOL         bPlaying,bPaused,bEnd,bClosing;
	HWAVEOUT     hWaveOut;
	PBYTE        pBuffer1, pBuffer2 ;
    PWAVEHDR     pWaveHdr1, pWaveHdr2 ;
	WAVEFORMATEX waveformat ;
	
	void setList(CListCtrl *listctrl,int nItem);
	void FillBuffer(PBYTE pBuffer,LPCTSTR cstrName);

public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
};
