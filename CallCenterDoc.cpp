// CallCenterDoc.cpp : implementation of the CCallCenterDoc class
//

#include "stdafx.h"
#include "Callcenter.h"

#include "CallCenterDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCallCenterDoc

IMPLEMENT_DYNCREATE(CCallCenterDoc, CDocument)

BEGIN_MESSAGE_MAP(CCallCenterDoc, CDocument)
END_MESSAGE_MAP()


// CCallCenterDoc construction/destruction

CCallCenterDoc::CCallCenterDoc()
{
	// TODO: add one-time construction code here

}

CCallCenterDoc::~CCallCenterDoc()
{
}

BOOL CCallCenterDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	//

	return TRUE;
}




// CCallCenterDoc serialization

void CCallCenterDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CCallCenterDoc diagnostics

#ifdef _DEBUG
void CCallCenterDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCallCenterDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CCallCenterDoc commands
