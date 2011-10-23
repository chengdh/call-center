// WavePlayer.cpp : 实现文件
//

#include "stdafx.h"
#include "CallCenter.h"
#include "WavePlayer.h"
#include ".\waveplayer.h"


// CWavePlayer 对话框

IMPLEMENT_DYNAMIC(CWavePlayer, CDialog)
CWavePlayer::CWavePlayer(CWnd* pParent /*=NULL*/)
	: CDialog(CWavePlayer::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	m_nSlider1 = 0;
	bPaused = FALSE;
	bClosing = FALSE;
	bPlaying = FALSE;
	bEnd = FALSE;
}

CWavePlayer::~CWavePlayer()
{
}

void CWavePlayer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWavePlayer, CDialog)
	ON_BN_CLICKED(IDC_PLAY, OnBnClickedPlay)
	ON_BN_CLICKED(IDC_PAUSE, OnBnClickedPause)
	ON_BN_CLICKED(IDC_STOP, OnBnClickedStop)
	ON_BN_CLICKED(IDC_START, OnBnClickedStart)
	ON_BN_CLICKED(IDC_END, OnBnClickedEnd)
ON_WM_DESTROY()
ON_WM_TIMER()
END_MESSAGE_MAP()


// CWavePlayer 消息处理程序

void CWavePlayer::OnBnClickedPlay()
{
	// TODO: 在此添加控件通知处理程序代码
	bPlaying = TRUE;
	bEnd = FALSE;	
	if( bPaused )
	{
		bPaused = FALSE;
		waveOutRestart(hWaveOut);
		((CButton*)GetDlgItem(IDC_PLAY))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_PAUSE))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_STOP))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_START))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_END))->EnableWindow(TRUE);
		return;
	}
	else
	{
		m_nSlider1 = 0;
		pSlide1->SetPos(m_nSlider1);
	}
	pWaveHdr1 = reinterpret_cast<PWAVEHDR>( malloc (sizeof (WAVEHDR)) );
    pWaveHdr2 = reinterpret_cast<PWAVEHDR>( malloc (sizeof (WAVEHDR)) );
	pBuffer1  = (PBYTE)malloc (dwFileSize) ;
	pBuffer2  = (PBYTE)malloc (dwFileSize) ;
	if (!pWaveHdr1 || !pWaveHdr2 || !pBuffer1 || !pBuffer2)
    {
        if (!pWaveHdr1) free (pWaveHdr1) ;
        if (!pWaveHdr2) free (pWaveHdr2) ;
        if (!pBuffer1)  free (pBuffer1) ;
        if (!pBuffer2)  free (pBuffer2) ;

        MessageBeep (MB_ICONEXCLAMATION) ;
		AfxMessageBox(_T("Error allocating memory!"));
        return ;
	}
	waveformat.wFormatTag      = WAVE_FORMAT_ALAW ;
    waveformat.nChannels       = 1 ;
    waveformat.nSamplesPerSec  = 8000 ;
    waveformat.nAvgBytesPerSec = 8000 ;
    waveformat.nBlockAlign     = 1 ;
    waveformat.wBitsPerSample  = 8 ;
    waveformat.cbSize          = 0 ;
	
	if (waveOutOpen (&hWaveOut, WAVE_MAPPER, &waveformat, (DWORD)this->m_hWnd, 0, CALLBACK_WINDOW)
                    != MMSYSERR_NOERROR)
    {
            free (pWaveHdr1) ;
            free (pWaveHdr2) ;
            free (pBuffer1) ;
            free (pBuffer2) ;

            hWaveOut = NULL ;
            MessageBeep (MB_ICONEXCLAMATION) ;
            AfxMessageBox(_T("Error allocating memory!"));
            return ;
    }

	// Set up headers and prepare them
	pWaveHdr1->lpData          = (LPTSTR)pBuffer1 ;
    pWaveHdr1->dwBufferLength  = dwFileSize ;
    pWaveHdr1->dwBytesRecorded = 0 ;
    pWaveHdr1->dwUser          = 0 ;
    pWaveHdr1->dwFlags         = 0 ;
    pWaveHdr1->dwLoops         = 1 ;
    pWaveHdr1->lpNext          = NULL ;
    pWaveHdr1->reserved        = 0 ;
    
    waveOutPrepareHeader (hWaveOut, pWaveHdr1, sizeof (WAVEHDR)) ;
	((CButton*)GetDlgItem(IDC_PLAY))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_PAUSE))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_STOP))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_START))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_END))->EnableWindow(TRUE);

}

void CWavePlayer::OnBnClickedPause()
{
	// TODO: 在此添加控件通知处理程序代码

	waveOutPause(hWaveOut);
	bPlaying = FALSE;
	bPaused = TRUE;
	((CButton*)GetDlgItem(IDC_PLAY))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_PAUSE))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_STOP))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_START))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_END))->EnableWindow(TRUE);
}

void CWavePlayer::OnBnClickedStop()
{
	// TODO: 在此添加控件通知处理程序代码
	bPlaying = FALSE;
	bPaused = FALSE;	
	if( bPaused )
	{
		bPaused = FALSE;
	}
	waveOutReset(hWaveOut);		
}

void CWavePlayer::OnBnClickedStart()
{
	// TODO: 在此添加控件通知处理程序代码

	bPlaying = FALSE;
	bPaused = FALSE;	
	bEnd = FALSE;
	m_nSlider1 = 0;
	pSlide1->SetPos(m_nSlider1);
	waveOutReset(hWaveOut);
	((CButton*)GetDlgItem(IDC_PLAY))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_PAUSE))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_STOP))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_START))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_END))->EnableWindow(TRUE);		
}

void CWavePlayer::OnBnClickedEnd()
{
	// TODO: 在此添加控件通知处理程序代码	
	bPlaying = FALSE;
	bPaused = FALSE;
	bEnd = TRUE;	
	m_nSlider1 = dwFileSize*20/8000;
	pSlide1->SetPos(m_nSlider1);
	waveOutReset(hWaveOut);	
	((CButton*)GetDlgItem(IDC_PLAY))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_PAUSE))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_STOP))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_START))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_END))->EnableWindow(FALSE);
}


void CWavePlayer::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	
	CDialog::OnOK();
}

void CWavePlayer::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	
	CDialog::OnCancel();
}

void CWavePlayer::setList(CListCtrl *listctrl, int nItem)
{
	this->nItem = nItem;
	pList5 = listctrl;
	strFileName = pList5->GetItemText(nItem,1).Trim();
	strFileSize = pList5->GetItemText(nItem,3).Trim();
	dwFileSize = atol((LPTSTR)(LPCTSTR)strFileSize);
}

BOOL CWavePlayer::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetIcon(m_hIcon, TRUE);			// Set big icon
//	SetIcon(m_hIcon, FALSE);		// Set small icon

	this->SetWindowText(strFileName);
	((CButton*)GetDlgItem(IDC_PLAY))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_PAUSE))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_STOP))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_START))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_END))->EnableWindow(FALSE);

	pSlide1 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER1);
	pSlide1->SetRange(0,dwFileSize*20/8000);
	pSlide1->SetPos(m_nSlider1);

	SetTimer(1001,250,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL CWavePlayer::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	switch(pMsg->message)
	{	
		case MM_WOM_OPEN:
			TRACE0(_T(" open "));
			FillBuffer(pBuffer1,strFileName);
			waveOutWrite(hWaveOut,pWaveHdr1,sizeof(WAVEHDR));
			break;
		case MM_WOM_DONE:
			TRACE0(_T(" done "));		
			bPlaying = FALSE;
			if( bEnd )
				m_nSlider1 = dwFileSize*20/8000;
			else
				m_nSlider1 = 0;
			pSlide1->SetPos(m_nSlider1);
			waveOutClose(hWaveOut);			
			((CButton*)GetDlgItem(IDC_PLAY))->EnableWindow(TRUE);
			((CButton*)GetDlgItem(IDC_PAUSE))->EnableWindow(FALSE);
			((CButton*)GetDlgItem(IDC_STOP))->EnableWindow(FALSE);
			if( bEnd )
				((CButton*)GetDlgItem(IDC_START))->EnableWindow(TRUE);
			else
				((CButton*)GetDlgItem(IDC_START))->EnableWindow(FALSE);
		//	((CButton*)GetDlgItem(IDC_END))->EnableWindow(TRUE);
			break;
		case MM_WOM_CLOSE:
			TRACE0(_T(" CLOSEd "));
			waveOutUnprepareHeader (hWaveOut, pWaveHdr1, sizeof (WAVEHDR)) ;
			free (pWaveHdr1) ;
			free (pWaveHdr2) ;
			free (pBuffer1) ;
		    free (pBuffer2) ;
			hWaveOut = NULL ;
			if( bClosing )
			{
				EndDialog (0) ;
		//?		AfxMessageBox(_T("closed"));
			}
			break;		
    }	

	return CDialog::PreTranslateMessage(pMsg);
}

void CWavePlayer::FillBuffer(PBYTE pBuffer,LPCTSTR cstrName)
{
	FILE *fp;
	CString strTemp = CString(_T(".\\Record\\")) + cstrName;
	
	if( (fp = fopen((LPCTSTR)strTemp,"rb")) == NULL )
	{
		AfxMessageBox("cannot open this file!");
		return;
	}
	
	int i = 0;
	rewind(fp);
	while( !feof(fp) && i< dwFileSize )
	{
		pBuffer[i++] = (BYTE)fgetc(fp);		
	}
}

void CWavePlayer::OnDestroy()
{
	//CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	if (hWaveOut != NULL)
	{				
		bClosing = TRUE ;
        waveOutReset(hWaveOut);
		TRACE0(_T("\nCLOSE "));
	
	}
	else
	{
		TRACE0(_T(" CLOSE1 "));
		EndDialog (0) ;//运行 不到此处？？？？？ 		
	}
}

void CWavePlayer::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	CString strTime = _T("");
	CString strCurrentTime = _T("");
	CString strTotalTime = _T("");
	int nTotalTime = dwFileSize / 8000;	//秒
	int nCurrentTime;
	if( nTotalTime == 0 )
		nCurrentTime = 0;
	else
		nCurrentTime = m_nSlider1 / 20;	//秒
	
	int nMinute,nSecond;
	nMinute = (int)nCurrentTime / 60;
	nSecond = nCurrentTime - nMinute * 60;
	strCurrentTime.Format(_T("%d:%02d"),nMinute,nSecond);
	nMinute = (int)nTotalTime / 60;
	nSecond = nTotalTime - nMinute * 60;
	strTotalTime.Format(_T("%d:%02d"),nMinute,nSecond);
	strTime = strCurrentTime + CString(_T(" / ")) + strTotalTime;	
	((CStatic*)GetDlgItem(IDC_STATIC_TIME))->SetWindowText(strTime);

	if( bPlaying )
	{
		m_nSlider1 += 5;
		pSlide1->SetPos(m_nSlider1);	
	}

	CDialog::OnTimer(nIDEvent);
}
