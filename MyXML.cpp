#include "StdAfx.h"
#include "MyXML.h"

MyXML::MyXML(void)
{
	xmlFilePath=_T(".\\config.xml");
}

MyXML::~MyXML(void)
{
}

BOOL MyXML::findVoiceNameByNodeId(int NodeId,LPTSTR VoiceNode,LPTSTR &VoicePath)
{
	LPTSTR strName;
	LPTSTR strAttrValue;

	CoInitialize(NULL);
	MSXML2::IXMLDOMDocument2Ptr pXMLDom;
	HRESULT hr = pXMLDom.CreateInstance(__uuidof(MSXML2::DOMDocument30));
	if(FAILED(hr))
	{
		CoUninitialize();
		return FALSE;
	}
	//
	if(pXMLDom->load(_variant_t(xmlFilePath)) != VARIANT_TRUE)
	{
		CoUninitialize();
		return FALSE;
	}
	//
	MSXML2::IXMLDOMNodePtr pRootNode = pXMLDom->selectSingleNode(_T("//CTI"));
	if(pRootNode == NULL)
	{
		CoUninitialize();
		return FALSE;
	}

	MSXML2::IXMLDOMNodePtr pNode = NULL;
	MSXML2::IXMLDOMNodePtr pNode2 = NULL;
	MSXML2::IXMLDOMNodeListPtr pNodeList = NULL;
	MSXML2::IXMLDOMElementPtr pElement = NULL;
	
	pNodeList = pRootNode->selectNodes(_T("//Node"));
	if( pNodeList == NULL )
	{
		CoUninitialize();
		return FALSE;	//????
	}

	for(int i=0; i<pNodeList->length; i++)
	{
		pNode = pNodeList->item[i];
		pElement = pNode;
		strAttrValue = _com_util::ConvertBSTRToString( _bstr_t(pElement->getAttribute(_T("NodeId"))));
		if(::atoi(strAttrValue) == NodeId)
			break;
	}
	
	pNode2 = pNode->selectSingleNode(_T("Content"))->selectSingleNode(_T("Voice"));
	pNodeList = pNode2->GetchildNodes();
	if( pNodeList == NULL )
	{
		CoUninitialize();
		return FALSE;	//????
	}

	for(int i=0; i<pNodeList->length; i++)
	{
		pNode = pNodeList->item[i];
		strName = _com_util::ConvertBSTRToString(pNode->GetnodeName());
		if(strcmp(strName,VoiceNode) == 0)
		{
			pElement = pNode;
			VoicePath = _com_util::ConvertBSTRToString(_bstr_t(pElement->getAttribute(_T("VoicePath"))));
			CoUninitialize();
			return TRUE;
		}
	}	
	
	pNode = NULL;
	pNode2 = NULL;
	pNodeList = NULL;
	pElement = NULL;
	pXMLDom = NULL;
	CoUninitialize();
	return FALSE;
}
//根据给定的content获取语音文件路径
BOOL MyXML::findVoiceNameByContent(int NodeId,CString content,LPTSTR &VoicePath)
{

	CoInitialize(NULL);
	MSXML2::IXMLDOMDocument2Ptr pXMLDom;
	MSXML2::IXMLDOMElementPtr p_elem;
	MSXML2::IXMLDOMNodeListPtr match_nodes;
	HRESULT hr = pXMLDom.CreateInstance(__uuidof(MSXML2::DOMDocument30));
	if(FAILED(hr))
	{
		CoUninitialize();
		return FALSE;
	}
	//
	if(pXMLDom->load(_variant_t(xmlFilePath)) != VARIANT_TRUE)
	{
		CoUninitialize();
		return FALSE;
	}
	//获取符合条件的节点
	CString node_path;
	node_path.Format(_T("//Node[@NodeId='%d']//*[@Content='%s']"),NodeId,_T(content));
	//node_path+=_T(content);
	//node_path+=_T("]");
	BSTR bstr_node_path = node_path.AllocSysString();
	match_nodes = pXMLDom->selectNodes(_T(bstr_node_path));
	
	SysFreeString(bstr_node_path);
	if(match_nodes == NULL)
	{
		CoUninitialize();
		return FALSE;
	}
	else
	{
		p_elem =match_nodes->item[0];
		VoicePath = _com_util::ConvertBSTRToString(_bstr_t(p_elem->getAttribute(_T("VoicePath"))));
	}
	return true;

}

int MyXML::getWaitTimeOrReplayNumByNodeId(int NodeId,LPTSTR NodeName)
{
	LPTSTR strAttrValue;
	int value = 0;

	CoInitialize(NULL);
	MSXML2::IXMLDOMDocument2Ptr pXMLDom;
	HRESULT hr = pXMLDom.CreateInstance(__uuidof(MSXML2::DOMDocument30));
	if(FAILED(hr))
	{
		CoUninitialize();
		return FALSE;
	}
	//
	if(pXMLDom->load(_variant_t(xmlFilePath)) != VARIANT_TRUE)
	{
		CoUninitialize();
		return FALSE;
	}
	//
	MSXML2::IXMLDOMNodePtr pRootNode = pXMLDom->selectSingleNode(_T("//CTI"));
	if(pRootNode == NULL)
	{
		CoUninitialize();
		return FALSE;
	}

	MSXML2::IXMLDOMNodePtr pNode = NULL;
	MSXML2::IXMLDOMNodeListPtr pNodeList = NULL;
	MSXML2::IXMLDOMElementPtr pElement = NULL;
	
	pNodeList = pRootNode->selectNodes(_T("//Node"));
	if( pNodeList == NULL )
	{
		CoUninitialize();
		return FALSE;	//????
	}

	for(int i=0; i<pNodeList->length; i++)
	{
		pNode = pNodeList->item[i];
		pElement = pNode;
		strAttrValue = _com_util::ConvertBSTRToString( _bstr_t(pElement->getAttribute(_T("NodeId"))));
		if(::atoi(strAttrValue) == NodeId)
			break;
	}

	pNodeList = pNode->selectSingleNode(_T("Content"))->GetchildNodes();
	
	for(int i=0; i<pNodeList->length; i++)
	{
		pNode = pNodeList->item[i];
		if(strcmp(pNode->GetnodeName(),NodeName) == 0)
		{
			pElement = pNode;
			value = ::atoi(_com_util::ConvertBSTRToString(_bstr_t(pElement->getAttribute(_T("Value")))));
			break;
		}
	}	

	pNode = NULL;
	pNodeList = NULL;
	pElement = NULL;
	pXMLDom = NULL;
	CoUninitialize();
	return value;
}

BOOL MyXML::findKeyByCode(int NodeId,char code, int &nextNodeId)
{
	LPTSTR strAttrValue;
	
	CoInitialize(NULL);
	MSXML2::IXMLDOMDocument2Ptr pXMLDom;
	HRESULT hr = pXMLDom.CreateInstance(__uuidof(MSXML2::DOMDocument30));
	if(FAILED(hr))
	{
		CoUninitialize();
		return FALSE;
	}
	//
	if(pXMLDom->load(_variant_t(xmlFilePath)) != VARIANT_TRUE)
	{
		CoUninitialize();
		return FALSE;
	}
	//
	MSXML2::IXMLDOMNodePtr pRootNode = pXMLDom->selectSingleNode(_T("//CTI"));
	if(pRootNode == NULL)
	{
		CoUninitialize();
		return FALSE;
	}

	MSXML2::IXMLDOMNodePtr pNode = NULL;
	MSXML2::IXMLDOMNodePtr pNode2 = NULL;
	MSXML2::IXMLDOMNodeListPtr pNodeList = NULL;
	MSXML2::IXMLDOMElementPtr pElement = NULL;
	
	pNodeList = pRootNode->selectNodes(_T("//Node"));
	if( pNodeList == NULL )
	{
		CoUninitialize();
		return FALSE;	//????
	}

	for(int i=0; i<pNodeList->length; i++)
	{
		pNode = pNodeList->item[i];
		pElement = pNode;
		strAttrValue = _com_util::ConvertBSTRToString( _bstr_t(pElement->getAttribute(_T("NodeId"))));
		if(::atoi(strAttrValue) == NodeId)
			break;
	}

	pNodeList = pNode->selectSingleNode(_T("Content"))->GetchildNodes();
	for(int i=0; i<pNodeList->length; i++)
	{
		pNode2 = pNodeList->item[i];
		if(strcmp(pNode2->GetnodeName(),_T("NextNode")) == 0)
		{
			break;
		}

	}

	pNodeList = pNode2->GetchildNodes();
	for(int i=0; i<pNodeList->length; i++)
	{
		pNode = pNodeList->item[i];
		if(strcmp(pNode->GetnodeName(),_T("Key")) == 0)
		{
			pElement = pNode;
			strAttrValue=::_com_util::ConvertBSTRToString(_bstr_t(pElement->getAttribute(_T("Code"))));
			if(strAttrValue[0] == code)
			{
				nextNodeId = ::atoi(::_com_util::ConvertBSTRToString(_bstr_t(pElement->getAttribute(_T("NodeId")))));
				CoUninitialize();
				return TRUE;
			}			
		}
	}

	pNode = NULL;
	pNode2 = NULL;
	pNodeList = NULL;
	pElement = NULL;
	pXMLDom = NULL;
	CoUninitialize();
	return FALSE;
}

BOOL MyXML::findKey0ByCode(int NodeId,char code, int &nextNodeId)
{
	LPTSTR strAttrValue;
	
	CoInitialize(NULL);
	MSXML2::IXMLDOMDocument2Ptr pXMLDom;
	HRESULT hr = pXMLDom.CreateInstance(__uuidof(MSXML2::DOMDocument30));
	if(FAILED(hr))
	{
		CoUninitialize();
		return FALSE;
	}
	//
	if(pXMLDom->load(_variant_t(xmlFilePath)) != VARIANT_TRUE)
	{
		CoUninitialize();
		return FALSE;
	}
	//
	MSXML2::IXMLDOMNodePtr pRootNode = pXMLDom->selectSingleNode(_T("//CTI"));
	if(pRootNode == NULL)
	{
		CoUninitialize();
		return FALSE;
	}

	MSXML2::IXMLDOMNodePtr pNode = NULL;
	MSXML2::IXMLDOMNodePtr pNode2 = NULL;
	MSXML2::IXMLDOMNodeListPtr pNodeList = NULL;
	MSXML2::IXMLDOMElementPtr pElement = NULL;
	
	pNodeList = pRootNode->selectNodes(_T("//Node"));
	if( pNodeList == NULL )
	{
		CoUninitialize();
		return FALSE;	//????
	}

	for(int i=0; i<pNodeList->length; i++)
	{
		pNode = pNodeList->item[i];
		pElement = pNode;
		strAttrValue = _com_util::ConvertBSTRToString( _bstr_t(pElement->getAttribute(_T("NodeId"))));
		if(::atoi(strAttrValue) == NodeId)
			break;
	}

	pNodeList = pNode->selectSingleNode(_T("Content"))->GetchildNodes();
	for(int i=0; i<pNodeList->length; i++)
	{
		pNode2 = pNodeList->item[i];
		if(strcmp(pNode2->GetnodeName(),_T("NextNode")) == 0)
		{
			break;
		}

	}

	pNodeList = pNode2->GetchildNodes();
	for(int i=0; i<pNodeList->length; i++)
	{
		pNode = pNodeList->item[i];
		if(strcmp(pNode->GetnodeName(),_T("Key0")) == 0)
		{
			pElement = pNode;
			strAttrValue=::_com_util::ConvertBSTRToString(_bstr_t(pElement->getAttribute(_T("Code"))));
			if(strAttrValue[0] == code)
			{
				nextNodeId = ::atoi(::_com_util::ConvertBSTRToString(_bstr_t(pElement->getAttribute(_T("NodeId")))));
				CoUninitialize();
				return TRUE;
			}			
		}
	}

	pRootNode = NULL;
	pNode = NULL;
	pNode2 = NULL;
	pNodeList = NULL;
	pElement = NULL;
	pXMLDom = NULL;
	CoUninitialize();
	return FALSE;
}

int MyXML::getKeyTypeByCode(int NodeId, char code,int &nextNodeId)
{
	int type = -1;
	LPTSTR strAttrValue;
	
	CoInitialize(NULL);
	MSXML2::IXMLDOMDocument2Ptr pXMLDom;
	HRESULT hr = pXMLDom.CreateInstance(__uuidof(MSXML2::DOMDocument30));
	if(FAILED(hr))
	{
		CoUninitialize();
		return FALSE;
	}
	//
	if(pXMLDom->load(_variant_t(xmlFilePath)) != VARIANT_TRUE)
	{
		CoUninitialize();
		return FALSE;
	}
	//
	MSXML2::IXMLDOMNodePtr pRootNode = pXMLDom->selectSingleNode(_T("//CTI"));
	if(pRootNode == NULL)
	{
		CoUninitialize();
		return FALSE;
	}

	MSXML2::IXMLDOMNodePtr pNode = NULL;
	MSXML2::IXMLDOMNodePtr pNode2 = NULL;
	MSXML2::IXMLDOMNodeListPtr pNodeList = NULL;
	MSXML2::IXMLDOMElementPtr pElement = NULL;
	
	pNodeList = pRootNode->selectNodes(_T("//Node"));
	if( pNodeList == NULL )
	{
		CoUninitialize();
		return FALSE;	//????
	}

	for(int i=0; i<pNodeList->length; i++)
	{
		pNode = pNodeList->item[i];
		pElement = pNode;
		strAttrValue = _com_util::ConvertBSTRToString( _bstr_t(pElement->getAttribute(_T("NodeId"))));
		if(::atoi(strAttrValue) == NodeId)
			break;
	}

	pNodeList = pNode->selectSingleNode(_T("Content"))->GetchildNodes();
	for(int i=0; i<pNodeList->length; i++)
	{
		pNode2 = pNodeList->item[i];
		if(strcmp(pNode2->GetnodeName(),_T("NextNode")) == 0)
		{
			break;
		}

	}

	pNodeList = pNode2->GetchildNodes();
	for(int i=0; i<pNodeList->length; i++)
	{
		pNode = pNodeList->item[i];
		if(strcmp(pNode->GetnodeName(),_T("Key")) == 0)
		{
			pElement = pNode;
			strAttrValue=::_com_util::ConvertBSTRToString(_bstr_t(pElement->getAttribute(_T("Code"))));
			if(strAttrValue[0] == code)
			{
				nextNodeId = ::atoi(::_com_util::ConvertBSTRToString(_bstr_t(pElement->getAttribute(_T("NodeId")))));
				type = ::atoi(::_com_util::ConvertBSTRToString(_bstr_t(pElement->getAttribute(_T("Type")))));				
			}			
		}
	}

	pRootNode = NULL;
	pNode = NULL;
	pNode2 = NULL;
	pNodeList = NULL;
	pElement = NULL;
	pXMLDom = NULL;
	CoUninitialize();	
	return type;
}

int MyXML::getKey0TypeByCode(int NodeId, char code,int &nextNodeId)
{
	int type = -1;
	LPTSTR strAttrValue;
	
	CoInitialize(NULL);
	MSXML2::IXMLDOMDocument2Ptr pXMLDom;
	HRESULT hr = pXMLDom.CreateInstance(__uuidof(MSXML2::DOMDocument30));
	if(FAILED(hr))
	{
		CoUninitialize();
		return FALSE;
	}
	//
	if(pXMLDom->load(_variant_t(xmlFilePath)) != VARIANT_TRUE)
	{
		CoUninitialize();
		return FALSE;
	}
	//
	MSXML2::IXMLDOMNodePtr pRootNode = pXMLDom->selectSingleNode(_T("//CTI"));
	if(pRootNode == NULL)
	{
		CoUninitialize();
		return FALSE;
	}

	MSXML2::IXMLDOMNodePtr pNode = NULL;
	MSXML2::IXMLDOMNodePtr pNode2 = NULL;
	MSXML2::IXMLDOMNodeListPtr pNodeList = NULL;
	MSXML2::IXMLDOMElementPtr pElement = NULL;
	
	pNodeList = pRootNode->selectNodes(_T("//Node"));
	if( pNodeList == NULL )
	{
		CoUninitialize();
		return FALSE;	//????
	}

	for(int i=0; i<pNodeList->length; i++)
	{
		pNode = pNodeList->item[i];
		pElement = pNode;
		strAttrValue = _com_util::ConvertBSTRToString( _bstr_t(pElement->getAttribute(_T("NodeId"))));
		if(::atoi(strAttrValue) == NodeId)
			break;
	}

	pNodeList = pNode->selectSingleNode(_T("Content"))->GetchildNodes();
	for(int i=0; i<pNodeList->length; i++)
	{
		pNode2 = pNodeList->item[i];
		if(strcmp(pNode2->GetnodeName(),_T("NextNode")) == 0)
		{
			break;
		}

	}

	pNodeList = pNode2->GetchildNodes();
	for(int i=0; i<pNodeList->length; i++)
	{
		pNode = pNodeList->item[i];
		if(strcmp(pNode->GetnodeName(),_T("Key0")) == 0)
		{
			pElement = pNode;
			strAttrValue=::_com_util::ConvertBSTRToString(_bstr_t(pElement->getAttribute(_T("Code"))));
			if(strAttrValue[0] == code)
			{
				nextNodeId = ::atoi(::_com_util::ConvertBSTRToString(_bstr_t(pElement->getAttribute(_T("NodeId")))));
				type = ::atoi(::_com_util::ConvertBSTRToString(_bstr_t(pElement->getAttribute(_T("Type")))));				
			}			
		}
	}

	pRootNode = NULL;
	pNode = NULL;
	pNode2 = NULL;
	pNodeList = NULL;
	pElement = NULL;
	pXMLDom = NULL;
	CoUninitialize();	
	return type;
}

int MyXML::getTypeByNodeId(int NodeId)
{
	int type = -1;
	
	LPTSTR strAttrValue;
	
	CoInitialize(NULL);
	MSXML2::IXMLDOMDocument2Ptr pXMLDom;
	HRESULT hr = pXMLDom.CreateInstance(__uuidof(MSXML2::DOMDocument30));
	if(FAILED(hr))
	{
		CoUninitialize();
		return FALSE;
	}
	//
	if(pXMLDom->load(_variant_t(xmlFilePath)) != VARIANT_TRUE)
	{
		CoUninitialize();
		return FALSE;
	}
	//
	MSXML2::IXMLDOMNodePtr pRootNode = pXMLDom->selectSingleNode(_T("//CTI"));
	if(pRootNode == NULL)
	{
		CoUninitialize();
		return FALSE;
	}

	MSXML2::IXMLDOMNodePtr pNode = NULL;
	MSXML2::IXMLDOMNodePtr pNode2 = NULL;
	MSXML2::IXMLDOMNodeListPtr pNodeList = NULL;
	MSXML2::IXMLDOMElementPtr pElement = NULL;
	
	pNodeList = pRootNode->selectNodes(_T("//Node"));
	if( pNodeList == NULL )
	{
		CoUninitialize();
		return FALSE;	//????
	}

	for(int i=0; i<pNodeList->length; i++)
	{
		pNode = pNodeList->item[i];
		pElement = pNode;
		strAttrValue = _com_util::ConvertBSTRToString( _bstr_t(pElement->getAttribute(_T("NodeId"))));
		if(::atoi(strAttrValue) == NodeId)
		{
			type=::atoi(_com_util::ConvertBSTRToString(_bstr_t(pElement->getAttribute(_T("Type")))));
			break;
		}
	}
	pRootNode = NULL;
	pNode = NULL;
	pNode2 = NULL;
	pNodeList = NULL;
	pElement = NULL;
	pXMLDom = NULL;
	CoUninitialize();
	return type;
}

BOOL MyXML::getChannelNoByPhoneNum(int NodeId,int phoneNum,int &channelNo)
{
	LPTSTR strAttrValue;
	
	CoInitialize(NULL);
	MSXML2::IXMLDOMDocument2Ptr pXMLDom;
	HRESULT hr = pXMLDom.CreateInstance(__uuidof(MSXML2::DOMDocument30));
	if( FAILED(hr) )
	{
		CoUninitialize();
		return FALSE;
	}
	//
	if(pXMLDom->load(_variant_t(xmlFilePath)) != VARIANT_TRUE)
	{
		CoUninitialize();
		return FALSE;
	}
	//
	MSXML2::IXMLDOMNodePtr pRootNode = pXMLDom->selectSingleNode(_T("//CTI"));
	if(pRootNode == NULL)
	{
		CoUninitialize();
		return FALSE;
	}

	MSXML2::IXMLDOMNodePtr pNode = NULL;
	MSXML2::IXMLDOMNodePtr pNode2 = NULL;
	MSXML2::IXMLDOMNodeListPtr pNodeList = NULL;
	MSXML2::IXMLDOMElementPtr pElement = NULL;
	
	pNodeList = pRootNode->selectNodes(_T("//Node"));
	if( pNodeList == NULL )
	{
		CoUninitialize();
		return FALSE;	//????
	}

	for(int i=0; i<pNodeList->length; i++)
	{
		pNode = pNodeList->item[i];
		pElement = pNode;
		strAttrValue = _com_util::ConvertBSTRToString( _bstr_t(pElement->getAttribute(_T("NodeId"))));
		if(::atoi(strAttrValue) == NodeId)
			break;
	}

	pNodeList = pNode->selectSingleNode(_T("Content"))->GetchildNodes();
	for(int i=0; i<pNodeList->length; i++)
	{
		pNode2 = pNodeList->item[i];
		if(strcmp(pNode2->GetnodeName(),_T("Extension")) == 0)
		{
			break;
		}

	}

	pNodeList = pNode2->GetchildNodes();
	for(int i=0; i<pNodeList->length; i++)
	{
		pNode = pNodeList->item[i];
		if(strcmp(pNode->GetnodeName(),_T("Phone")) == 0)
		{
			pElement = pNode;
			strAttrValue=::_com_util::ConvertBSTRToString(_bstr_t(pElement->getAttribute(_T("PhoneNum"))));
			if( ::atoi(strAttrValue) == phoneNum )
			{
				channelNo = ::atoi(::_com_util::ConvertBSTRToString(_bstr_t(pElement->getAttribute(_T("ChannelNo")))));
				CoUninitialize();
				return TRUE;
			}			
		}
	}
	pRootNode = NULL;
	pNode = NULL;
	pNode2 = NULL;
	pNodeList = NULL;
	pElement = NULL;
	pXMLDom = NULL;
	CoUninitialize();
	
	return FALSE;
}

BOOL MyXML::getPhoneNumByChannelNo(int NodeId, int &phoneNum, int channelNo)
{
	LPTSTR strAttrValue;
	
	CoInitialize(NULL);
	MSXML2::IXMLDOMDocument2Ptr pXMLDom;
	HRESULT hr = pXMLDom.CreateInstance(__uuidof(MSXML2::DOMDocument30));
	if( FAILED(hr) )
	{
		CoUninitialize();
		return FALSE;
	}
	//
	if(pXMLDom->load(_variant_t(xmlFilePath)) != VARIANT_TRUE)
	{
		CoUninitialize();
		return FALSE;
	}
	//
	MSXML2::IXMLDOMNodePtr pRootNode = pXMLDom->selectSingleNode(_T("//CTI"));
	if(pRootNode == NULL)
	{
		CoUninitialize();
		return FALSE;
	}

	MSXML2::IXMLDOMNodePtr pNode = NULL;
	MSXML2::IXMLDOMNodePtr pNode2 = NULL;
	MSXML2::IXMLDOMNodeListPtr pNodeList = NULL;
	MSXML2::IXMLDOMElementPtr pElement = NULL;
	
	pNodeList = pRootNode->selectNodes(_T("//Node"));
	if( pNodeList == NULL )
	{
		CoUninitialize();
		return FALSE;	//????
	}

	for(int i=0; i<pNodeList->length; i++)
	{
		pNode = pNodeList->item[i];
		pElement = pNode;
		strAttrValue = _com_util::ConvertBSTRToString( _bstr_t(pElement->getAttribute(_T("NodeId"))));
		if(::atoi(strAttrValue) == NodeId)
			break;
	}

	pNodeList = pNode->selectSingleNode(_T("Content"))->GetchildNodes();
	for(int i=0; i<pNodeList->length; i++)
	{
		pNode2 = pNodeList->item[i];
		if(strcmp(pNode2->GetnodeName(),_T("Extension")) == 0)
		{
			break;
		}
	}

	pNodeList = pNode2->GetchildNodes();
	for(int i=0; i<pNodeList->length; i++)
	{
		pNode = pNodeList->item[i];
		if(strcmp(pNode->GetnodeName(),_T("Phone")) == 0)
		{
			pElement = pNode;
			strAttrValue=::_com_util::ConvertBSTRToString(_bstr_t(pElement->getAttribute(_T("ChannelNo"))));
			if( ::atoi(strAttrValue) == channelNo )
			{
				phoneNum = ::atoi(::_com_util::ConvertBSTRToString(_bstr_t(pElement->getAttribute(_T("PhoneNum")))));
				CoUninitialize();
				return TRUE;
			}			
		}
	}

	pRootNode = NULL;
	pNode = NULL;
	pNode2 = NULL;
	pNodeList = NULL;
	pElement = NULL;
	pXMLDom = NULL;
	CoUninitialize();
	
	return FALSE;
}

BOOL MyXML::getStateByChannelNo(int NodeId, int channelNo, int &state)
{
	LPTSTR strAttrValue;
	
	CoInitialize(NULL);
	MSXML2::IXMLDOMDocument2Ptr pXMLDom;
	HRESULT hr = pXMLDom.CreateInstance(__uuidof(MSXML2::DOMDocument30));
	if( FAILED(hr) )
	{
		CoUninitialize();
		return FALSE;
	}
	//
	if(pXMLDom->load(_variant_t(xmlFilePath)) != VARIANT_TRUE)
	{
		CoUninitialize();
		return FALSE;
	}
    //
	MSXML2::IXMLDOMNodePtr pRootNode = pXMLDom->selectSingleNode(_T("//CTI"));
	if(pRootNode == NULL)
	{
		CoUninitialize();
		return FALSE;
	}

	MSXML2::IXMLDOMNodePtr pNode = NULL;
	MSXML2::IXMLDOMNodePtr pNode2 = NULL;
	MSXML2::IXMLDOMNodeListPtr pNodeList = NULL;
	MSXML2::IXMLDOMElementPtr pElement = NULL;
	
	pNodeList = pRootNode->selectNodes(_T("//Node"));
	if( pNodeList == NULL )
	{
		CoUninitialize();
		return FALSE;	//????
	}

	for(int i=0; i<pNodeList->length; i++)
	{
		pNode = pNodeList->item[i];
		pElement = pNode;
		strAttrValue = _com_util::ConvertBSTRToString( _bstr_t(pElement->getAttribute(_T("NodeId"))));
		if(::atoi(strAttrValue) == NodeId)
			break;
	}

	pNodeList = pNode->selectSingleNode(_T("Content"))->GetchildNodes();
	for(int i=0; i<pNodeList->length; i++)
	{
		pNode2 = pNodeList->item[i];
		if(strcmp(pNode2->GetnodeName(),_T("Extension")) == 0)
		{
			break;
		}

	}

	pNodeList = pNode2->GetchildNodes();
	for(int i=0; i<pNodeList->length; i++)
	{
		pNode = pNodeList->item[i];
		if(strcmp(pNode->GetnodeName(),_T("Channel")) == 0)
		{
			pElement = pNode;
			strAttrValue=::_com_util::ConvertBSTRToString(_bstr_t(pElement->getAttribute(_T("ChannelNo"))));
			if( ::atoi(strAttrValue) == channelNo )
			{
				state = ::atoi(::_com_util::ConvertBSTRToString(_bstr_t(pElement->getAttribute(_T("State")))));
				CoUninitialize();
				return TRUE;
			}			
		}
	}

	pRootNode = NULL;
	pNode = NULL;
	pNode2 = NULL;
	pNodeList = NULL;
	pElement = NULL;
	pXMLDom = NULL;
	CoUninitialize();
	
	return FALSE;
}

CString MyXML::getSQL(int NodeId)
{
	LPTSTR strAttrValue;
	LPTSTR NodeName = _T("SQL");
	CString value = _T("");
	
	CoInitialize(NULL);
	MSXML2::IXMLDOMDocument2Ptr pXMLDom;
	HRESULT hr = pXMLDom.CreateInstance(__uuidof(MSXML2::DOMDocument30));
	if(FAILED(hr))
	{
		CoUninitialize();
		return FALSE;
	}
	//
	if(pXMLDom->load(_variant_t(xmlFilePath)) != VARIANT_TRUE)
	{
		CoUninitialize();
		return FALSE;
	}
	//
	MSXML2::IXMLDOMNodePtr pRootNode = pXMLDom->selectSingleNode(_T("//CTI"));
	if(pRootNode == NULL)
	{
		CoUninitialize();
		return FALSE;
	}

	MSXML2::IXMLDOMNodePtr pNode = NULL;
	MSXML2::IXMLDOMNodeListPtr pNodeList = NULL;
	MSXML2::IXMLDOMElementPtr pElement = NULL;
	
	pNodeList = pRootNode->selectNodes(_T("//Node"));
	if( pNodeList == NULL )
	{
		CoUninitialize();
		return FALSE;	//????
	}

	for(int i=0; i<pNodeList->length; i++)
	{
		pNode = pNodeList->item[i];
		pElement = pNode;
		strAttrValue = _com_util::ConvertBSTRToString( _bstr_t(pElement->getAttribute(_T("NodeId"))));
		if(::atoi(strAttrValue) == NodeId)
			break;
	}

	pNodeList = pNode->selectSingleNode(_T("Content"))->GetchildNodes();
	
	for(int i=0; i<pNodeList->length; i++)
	{
		pNode = pNodeList->item[i];
		if(strcmp(pNode->GetnodeName(),NodeName) == 0)
		{
			pElement = pNode;
			value = (LPCTSTR)(LPTSTR)(_com_util::ConvertBSTRToString(_bstr_t(pElement->getAttribute(_T("Value")))));
			break;
		}
	}	

	pRootNode = NULL;
	pNode = NULL;
	pNodeList = NULL;
	pElement = NULL;
	pXMLDom = NULL;
	CoUninitialize();
	return value;
}

BOOL MyXML::createLogFile(CTime ct)
{
	//创建日志文件
	CString strTime;
	CString strFile;
	strTime = ct.Format(_T("%Y-%m-%d"));
	strFile = CString(_T(".\\Log"));	
	CFileFind fileFind;
	BOOL isFinded = (BOOL)fileFind.FindFile(strFile);
	if( !isFinded )
	{
       	::CreateDirectory(strFile,0);			//创建日志文件夹
	}
	
	strFile = CString(_T(".\\Log\\"))+strTime+CString(_T(".xml"));
	isFinded = (BOOL)fileFind.FindFile(strFile);
	if( !isFinded )
	{
		CoInitialize(NULL);
		MSXML2::IXMLDOMDocument2Ptr pDom;
		HRESULT hr = pDom.CreateInstance(__uuidof(MSXML2::DOMDocument30));
		if(FAILED(hr))
		{
			AfxMessageBox(_T("无法创建DOMDocument对象,请检查是否安装MS XML parser运行库。"));
			pDom = NULL;
			return FALSE;
		}
        IXMLDOMProcessingInstructionPtr pPI = NULL;
		pPI = pDom->createProcessingInstruction("xml","version='1.0' encoding='GB2312'");	//创建XML文件头部
		_variant_t vNullVal;
		vNullVal.vt = VT_NULL;
		MSXML2::IXMLDOMNodePtr pNode = pPI;
		pDom->insertBefore(pNode,vNullVal);

		MSXML2::IXMLDOMElementPtr pElement = NULL;
		pElement = pDom->createElement(_T("Log"));
		pDom->appendChild(pElement);
		pElement->setAttribute(_T("Date"),(LPTSTR)(LPCTSTR)strTime);
		
		pDom->save((LPTSTR)(LPCTSTR)strFile);

		pPI = NULL;
		pNode = NULL;
		pElement = NULL;
		pDom = NULL;
		CoUninitialize();
	}

	return TRUE;
}


BOOL MyXML::createLogItem(CString strId,CString strType,int channelNo,CString strStartTime,CString strEndTime,CString strDetails)
{
	CTime tm;
	CString strTime;
	CString strFile;
	tm=CTime::GetCurrentTime();
	strTime = tm.Format(_T("%Y-%m-%d"));
	strFile = CString(_T(".\\Log\\"))+strTime+CString(_T(".xml")); 
	CFileFind fileFind;
	BOOL isFinded = (BOOL)fileFind.FindFile(strFile);
	if( !isFinded )
	{
		if( !createLogFile(tm) )
			return FALSE;
	}

	CoInitialize(NULL);
	MSXML2::IXMLDOMDocument2Ptr pDom;
	HRESULT hr = pDom.CreateInstance(__uuidof(MSXML2::DOMDocument30));
	if(FAILED(hr))
		AfxMessageBox(_T("创建DOMDocument对象错误."));

	if( pDom->load((LPTSTR)(LPCTSTR)strFile) != VARIANT_TRUE )
	{
		AfxMessageBox("加载日志文件失败.");
		CoUninitialize();
		return FALSE;
	}

	MSXML2::IXMLDOMElementPtr pRootElement = NULL;
	MSXML2::IXMLDOMElementPtr pElement1 = NULL;
	MSXML2::IXMLDOMElementPtr pElement2 = NULL;

	pRootElement = pDom->documentElement;

	pElement1 = pDom->createElement(_T("Item"));
	pRootElement->appendChild(pElement1);
	pElement1->setAttribute(_T("ID"),(LPTSTR)(LPCTSTR)strId);

	pElement2 = pDom->createElement(_T("Type"));
	pElement2->Puttext((LPTSTR)(LPCTSTR)strType);
	pElement1->appendChild(pElement2);

	CString strChNo = _T("");
	strChNo.Format(_T("%d"),channelNo);
	pElement2 = pDom->createElement(_T("ChannelNo"));
	pElement2->Puttext((LPTSTR)(LPCTSTR)strChNo);
	pElement1->appendChild(pElement2);

	pElement2 = pDom->createElement(_T("StartTime"));
	pElement2->Puttext((LPTSTR)(LPCTSTR)strStartTime);
	pElement1->appendChild(pElement2);

	pElement2 = pDom->createElement(_T("EndTime"));
	pElement2->Puttext((LPTSTR)(LPCTSTR)strEndTime);
	pElement1->appendChild(pElement2);
	
	pElement2 = pDom->createElement(_T("Details"));
	pElement2->Puttext((LPTSTR)(LPCTSTR)strDetails);
	pElement1->appendChild(pElement2);

	pDom->save((LPTSTR)(LPCTSTR)strFile);
	
	pRootElement = NULL;
	pElement1 = NULL;
	pElement2 = NULL;
	pDom = NULL;
	CoUninitialize();
	return FALSE;
}

BOOL MyXML::insertTextToLogItem(CString strId,CString strText,CString nodeName,CTime ct)
{
	CoInitialize(NULL);
	MSXML2::IXMLDOMDocument2Ptr pDom;
	HRESULT hr = pDom.CreateInstance(__uuidof(MSXML2::DOMDocument30));
	if(FAILED(hr))
		AfxMessageBox(_T("创建DOMDocument对象错误."));

	CTime tm;
	CString strTime;
	CString strFile;
	tm=CTime::GetCurrentTime();
	strTime = ct.Format(_T("%Y-%m-%d"));
	strFile = CString(_T(".\\Log\\"))+strTime+CString(_T(".xml"));
	if( pDom->load((LPTSTR)(LPCTSTR)strFile) != VARIANT_TRUE )
	{
		AfxMessageBox("加载XML文件失败.");		
		pDom = NULL;
		CoUninitialize();		
		return FALSE;
	}

	MSXML2::IXMLDOMElementPtr pRootElement = NULL;
	MSXML2::IXMLDOMElementPtr pElement = NULL;
	MSXML2::IXMLDOMNodePtr pNode = NULL;
	MSXML2::IXMLDOMNodeListPtr pNodeList = NULL;

	pRootElement = pDom->documentElement;
	pNodeList = pRootElement->childNodes;

	if( pNodeList == NULL )
	{
		CoUninitialize();
		return FALSE;	//????
	}
	
	CString strTempId = _T("");
	for(int i=0; i<pNodeList->length; i++)
	{
		pNode = pNodeList->item[i];
		pElement = pNode;
		strTempId = (LPCTSTR)(_bstr_t)pElement->getAttribute(_T("ID"));
		if( strTempId == strId )
			break;
	}

	pNodeList = pNode->GetchildNodes();	
	for(int i=0; i<pNodeList->length; i++)
	{
		pNode = pNodeList->item[i];
		if( (LPCTSTR)pNode->GetnodeName() == nodeName )
		{
			if( nodeName == _T("Details") )
			{
				strText = (LPCTSTR)(LPTSTR)pNode->text + strText;
			}
			pNode->Puttext((LPTSTR)(LPCTSTR)strText);
			break;
		}
	}	

	pDom->save((LPTSTR)(LPCTSTR)strFile);

	pRootElement = NULL;
	pElement = NULL;
	pNode = NULL;
	pNodeList = NULL;
	pDom = NULL;
	CoUninitialize();

	return FALSE;
}

BOOL MyXML::showLogByDay(CTime ct,CListCtrl *pList)
{
	CoInitialize(NULL);
	MSXML2::IXMLDOMDocument2Ptr pDom;
	HRESULT hr = pDom.CreateInstance(__uuidof(MSXML2::DOMDocument30));
	if(FAILED(hr))
		AfxMessageBox(_T("创建DOMDocument对象错误."));

	CString strTime;
	CString strFile;
	strTime = ct.Format(_T("%Y-%m-%d"));
	strFile = CString(_T(".\\Log\\"))+strTime+CString(_T(".xml"));

	if( pDom->load((LPTSTR)(LPCTSTR)strFile) != VARIANT_TRUE )
	{
		AfxMessageBox("加载XML文件失败.");
		CoUninitialize();
		return FALSE;
	}

	MSXML2::IXMLDOMElementPtr pRootElement = NULL;
	MSXML2::IXMLDOMElementPtr pElement = NULL;
	MSXML2::IXMLDOMNodePtr pNode = NULL;
	MSXML2::IXMLDOMNodeListPtr pNodeList = NULL;
	MSXML2::IXMLDOMNodeListPtr pTempList=NULL;

	pRootElement = pDom->documentElement;
	pNodeList = pRootElement->childNodes;

	if( pNodeList == NULL )
	{
		CoUninitialize();
		return FALSE;	//????
	}

	CString iStr = _T("");
	for(int i=0; i<pNodeList->length; i++)
	{	
		iStr.Format(_T("%d"),i);
		pList->InsertItem(i,iStr);
		pNode = pNodeList->item[i];
		pTempList = pNode->GetchildNodes();

		for(int j=0; j<pTempList->length; j++)
		{			
			pNode = pTempList->item[j];
			if( strcmp(pNode->nodeName,_T("Type")) == 0 )
				pList->SetItemText(i,1,pNode->text);
			else if( strcmp(pNode->nodeName,_T("ChannelNo")) == 0 )
				pList->SetItemText(i,2,pNode->text);
			else if( strcmp(pNode->nodeName,_T("StartTime")) == 0 )
				pList->SetItemText(i,3,pNode->text);
			else if( strcmp(pNode->nodeName,_T("EndTime")) == 0 )
				pList->SetItemText(i,4,pNode->text);
			else if( strcmp(pNode->nodeName,_T("Details")) == 0 )
				pList->SetItemText(i,5,pNode->text);
		}		
	}

	pRootElement = NULL;
	pElement = NULL;
	pNode = NULL;
	pNodeList = NULL;
	pTempList=NULL;
	pDom = NULL;
	CoUninitialize();

	return TRUE;
}

BOOL MyXML::showLogByDistance(CTime ct1,CTime ct2,CListCtrl *pList)
{
	CoInitialize(NULL);
	MSXML2::IXMLDOMDocument2Ptr pDom;
	HRESULT hr = pDom.CreateInstance(__uuidof(MSXML2::DOMDocument30));
	if(FAILED(hr))
		AfxMessageBox(_T("创建DOMDocument对象错误."));

	if( ct1 > ct2 )
	{
		CTime tempTime;
		tempTime = ct1;
		ct1 = ct2;
		ct2 = tempTime;
	}

	CTimeSpan ts( 1, 0, 0, 0 );
	CString strTime;
	CString strFile;
	strTime = ct1.Format(_T("%Y-%m-%d"));
	strFile = CString(_T(".\\Log\\"))+strTime+CString(_T(".xml"));

	CFileFind fileFind;
	BOOL isFinded;

	MSXML2::IXMLDOMElementPtr pRootElement = NULL;
	MSXML2::IXMLDOMElementPtr pElement = NULL;
	MSXML2::IXMLDOMNodePtr pNode = NULL;
	MSXML2::IXMLDOMNodeListPtr pNodeList = NULL;
	MSXML2::IXMLDOMNodeListPtr pTempList=NULL;

	int k=0;
	while( ct1 <= ct2 )
	{
        isFinded = (BOOL)fileFind.FindFile(strFile);
		if( !isFinded )
		{
			ct1 += ts;
			strTime = ct1.Format(_T("%Y-%m-%d"));
			strFile = CString(_T(".\\Log\\"))+strTime+CString(_T(".xml"));
			continue;
		}
		if( pDom->load((LPTSTR)(LPCTSTR)strFile) != VARIANT_TRUE )
		{
			AfxMessageBox("加载XML文件失败.");
			CoUninitialize();
			return FALSE;
		}
		
		pRootElement = pDom->documentElement;
		pNodeList = pRootElement->childNodes;

		if( pNodeList == NULL )
		{
			CoUninitialize();
			return FALSE;	//????
		}

		CString iStr = _T("");
		for(int i=0; i<pNodeList->length; i++)
		{	
			iStr.Format(_T("%d"),k);
			pList->InsertItem(k,iStr);
			pNode = pNodeList->item[i];
			pTempList = pNode->GetchildNodes();

			for(int j=0; j<pTempList->length; j++)
			{			
				pNode = pTempList->item[j];
				if( strcmp(pNode->nodeName,_T("Type")) == 0 )
					pList->SetItemText(k,1,pNode->text);
				else if( strcmp(pNode->nodeName,_T("ChannelNo")) == 0 )
					pList->SetItemText(k,2,pNode->text);
				else if( strcmp(pNode->nodeName,_T("StartTime")) == 0 )
					pList->SetItemText(k,3,pNode->text);
				else if( strcmp(pNode->nodeName,_T("EndTime")) == 0 )
					pList->SetItemText(k,4,pNode->text);
				else if( strcmp(pNode->nodeName,_T("Details")) == 0 )
					pList->SetItemText(k,5,pNode->text);
			}
			k++;
		}
		ct1 += ts;
		strTime = ct1.Format(_T("%Y-%m-%d"));
		strFile = CString(_T(".\\Log\\"))+strTime+CString(_T(".xml"));
	}

	pRootElement = NULL;
	pElement = NULL;
	pNode = NULL;
	pNodeList = NULL;
	pTempList=NULL;
	pDom = NULL;
	CoUninitialize();

	return TRUE;
}

