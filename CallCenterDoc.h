// CallCenterDoc.h : interface of the CCallCenterDoc class
//


#pragma once


class CCallCenterDoc : public CDocument
{
protected: // create from serialization only
	CCallCenterDoc();
	DECLARE_DYNCREATE(CCallCenterDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CCallCenterDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


