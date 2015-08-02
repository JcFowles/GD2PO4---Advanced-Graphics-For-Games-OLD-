#include "stdafx.h"
#include "Cxml.h"
#include "string.h"
#include "malloc.h"
#include "utils.h"
Cxml::Cxml(void)
: m_s64(NULL)
, m_iCursor(0)
, m_iLength(0)
,m_iencoding(0)
, m_dversion(0.0)
{
	m_RootNode = new CNode();
}

Cxml::~Cxml(void)
{
	if(m_s64!=NULL)
		free(m_s64);
	delete(m_RootNode);
}

// parse a string containg xml code
bool Cxml::ParseString(_TCHAR* szXML)
{
	_TCHAR* wide;
	if(szXML == NULL)
		return false;
	GetEncoding(szXML);
	if(m_iencoding == ASCII)
	{
		int allocation = MultiByteToWideChar(CP_UTF8,0,(char*) szXML, -1, NULL, 0);
		wide = (_TCHAR*)calloc(allocation ,sizeof(_TCHAR));
		MultiByteToWideChar(CP_UTF8,0,(char*) szXML, -1, wide, allocation);
		m_iLength = lstrlen(wide);
		GetNode(wide);
		free(wide);
	}
	else
	{
		m_iLength = lstrlen(szXML);
		GetNode(szXML);;
	}
	return true;
}

_TCHAR Cxml::decode_64(_TCHAR c)
{
	if (c >= _T('A') && c <= 'Z')
	{
		return c - _T('A');
	}
	if (c >= _T('a') && c <= 'z')
	{
		return c - _T('a') + 26;
	}
	if (c >= _T('0') && c <= '9')
	{
		return c - _T('0') + 52;
	}
	if (c == _T('+'))
	{
		return 62;
	};
	return 63;
}

_TCHAR Cxml::encode_64(_TCHAR c)
{
	if (c < 26)
	{
		return _T('A')+c;
	}
	if (c < 52)
	{
		return _T('a')+(c-26);
	}
	if (c < 62)
	{
		return _T('0')+(c-52);
	}
	if (c == 62)
	{
		return _T('+');
	}
	return _T('/');
}

_TCHAR* Cxml::sencode_64(_TCHAR* s, int len)
{
	int input_len = (len != 0 ? len :lstrlen(s));
	int output_len = ((2 + input_len - ((input_len + 2) % 3)) * 4 / 3) + 1;// + 1 for null terminated string
	int oDataLen = (input_len * 4 + 2) / 3; // output length without padding
	if(m_s64 != NULL)
		free(m_s64);
	m_s64 = (_TCHAR*)malloc(output_len * sizeof(_TCHAR));
    int ip = 0;
    int op = 0;
	while(ip<input_len)
	{
		int i0 = s[ip++] & 0xff;
		int i1 = ip < input_len ? s[ip++] & 0xff : 0;
		int i2 = ip < input_len ? s[ip++] & 0xff : 0;
		int o0 = i0 >> 2;
		int o1 = ((i0 & 3) << 4) | (i1 >> 4);
		int o2 = ((i1 & 0xf) << 2) | (i2 >> 6);
		int o3 = i2 & 0x3F;
		m_s64[op++] = encode_64(o0);
		m_s64[op++] = encode_64(o1);
		m_s64[op] = op < oDataLen ? encode_64(o2) : '=';
		op++;
		m_s64[op] = op < oDataLen ? encode_64(o3) : '=';
		op++;
	}
	m_s64[op] = '\0';
	return m_s64;
}

_TCHAR* Cxml::sdecode_64(_TCHAR* str, int *len)
{
	if(m_s64 != NULL)
		free(m_s64);
	m_s64 = (_TCHAR*)malloc(lstrlen(str) *sizeof(_TCHAR));
	int cursor = 0;
	for (int i=0;i<lstrlen(str);i+=4)
	{
		_TCHAR c1=_T('A'),c2=_T('A'),c3=_T('A'),c4=_T('A');
		c1 = str[i];
		if (i + 1 < lstrlen(str))
		{
			c2 = str[i+1];
		};
		if (i + 2 < lstrlen(str))
		{
			c3 = str[i+2];
		};
		if (i + 3 < lstrlen(str))
		{
			c4 = str[i+3];
		};
		_TCHAR by1=0,by2=0,by3=0,by4=0;
		by1 = decode_64(c1);
		by2 = decode_64(c2);
		by3 = decode_64(c3);
		by4 = decode_64(c4);
		m_s64[cursor] = (by1<<2)|(by2>>4);
		cursor++;
		if (c3 != '=')
		{
			m_s64[cursor] =( ((by2&0xf)<<4)|(by3>>2) );
			cursor ++;
		}
		else
		{
			m_s64[cursor] = '\0';
		}
		if (c4 != '=')
		{
			m_s64[cursor] =( ((by3&0x3)<<6)|by4 );
			cursor ++;
		}
		else
		{
			m_s64[cursor] = '\0';
		}
	}
	*len = cursor;
	return m_s64;
}

bool Cxml::GetNode(_TCHAR* szXML)
{
	// revision of 29-10-2010
	//	*added in support for CDATA sections.
	//	The CDATA section will generate an error if it is found somplace else than a node value.
	//	Also '//<![CDATA[' is not valid and is beyound the purpose of this project.

	int k = m_iCursor;
	int j = 0;//second level cursor;
	bool bCDATA = false;
	bool bIsPI = false;//Set to true if the cursor is curently inside a processing instruction
	_TCHAR cDelim = 0;
	_TCHAR c = szXML[k];
	const _TCHAR COPEN = _T('<');
	const _TCHAR CCLOSE = _T('>');
	const _TCHAR CSLASH = _T('/');
	const _TCHAR CSPACE = _T(' ');
	const _TCHAR CQUOTE = _T('\'');
	const _TCHAR CDQUOTE = _T('\"');
	const _TCHAR CEQUAL = _T('=');
	const _TCHAR CNEW = _T('\n');
	const _TCHAR CTAB = _T('\t');
	const _TCHAR CEXCLAMATION = _T('!');
	const _TCHAR CMINUS = _T('-');
	const _TCHAR CSQRPO = _T('[');
	const _TCHAR CSQRPC = _T(']');
	const _TCHAR SZCDATA[9] = _T("![CDATA[");
	const _TCHAR CQM = _T('?');
	const _TCHAR CRET = 13;//carriage return
	_TCHAR *szNodeNameBuff = (_TCHAR *)calloc(256,sizeof(_TCHAR));
	_TCHAR *szNodeValBuff = (_TCHAR *)calloc(256,sizeof(_TCHAR));
	_TCHAR *szAttrNameBuff = (_TCHAR *)calloc(256,sizeof(_TCHAR));
	_TCHAR *szAttrValBuff = (_TCHAR *)calloc(256,sizeof(_TCHAR));
	if(k >= m_iLength)
		return false;
	m_RootNode->SetName(_T("XML_DOC"));
	CNode* Current = m_RootNode->AdChildNode();
	while(k<m_iLength)
	{
		c = szXML[k];
		if(c == CNEW || c == CTAB || c == CRET)
		{
			k++;
			continue;
		}
		if(c == COPEN)
		{
#pragma region This section looks for a comment
			if(szXML[k+1] == CEXCLAMATION && szXML[k+2] == CMINUS && szXML[k+3] == CMINUS)//it is a comment
			{//Comment section
				//memset(szAttrValBuff,0,80*sizeof(_TCHAR));
				voidstr(szAttrValBuff);
				k+=4;
				c = szXML[k];
				while(!(szXML[k] == CMINUS && szXML[k+1] == CMINUS && szXML[k+2] == CCLOSE))
				{
					szAttrNameBuff = concat(szAttrNameBuff, c);
					c = szXML[++k];
				}
				k+=3;
				if(Current->GetName() != NULL)//we have seted this node, navigate to a child of it
					Current = Current->AdChildNode();
				Current->SetComment(szAttrNameBuff);//set it as a comment node
				Current = Current->GetParent();//return to the previous level
				continue;
			}
#pragma endregion
#pragma region This section looks for a CDATA section
			while(k+10 < m_iLength && j < 9)
			{
				if (szXML[k+1+j] != SZCDATA[j])
					break;
				j++;
				if(j==8)
				{
					//definetly a CDATA section follows
					k = k + j;
					int start = k;
					while((k + 3) < m_iLength && szXML[k+1] != CSQRPC && szXML[k+2] != CSQRPC && szXML[k+3] != CCLOSE)
					{
						k++;
					}
					int stop = k;
					k+=5;
					_TCHAR* buffer = (_TCHAR*)calloc(stop - start + 2, sizeof(_TCHAR));
					copyx(buffer, szXML, start, stop);
					Current->SetValue(buffer);
					free(buffer);
					j = 0;
					bCDATA = true;
					break;
				}
			}
			if(bCDATA)
			{
				bCDATA = false;
				continue;
			}
#pragma endregion
			//memset(szNodeNameBuff,0,80*sizeof(_TCHAR));
			voidstr(szNodeNameBuff);
			if(szXML[k+1] == CSLASH)
			{//closing tag for the last opend node
				Current = Current->GetParent();
				k++;
				while(szXML[k] != CCLOSE)
				{
					k++;
				}
				k++;
				continue;
			}
			if(szXML[k+1] == CQM)
			{
				c = szXML[++k];
				bIsPI = true;
			}
			// open tag. It means we have a node so we create it
			c = szXML[++k];
			while(c != CSLASH && c != CSPACE && c != CCLOSE)
			{//loops until the node name has been entirely read
				if(c != CNEW && c != CTAB && c != CRET)
					szNodeNameBuff = concat(szNodeNameBuff,c);
				c = szXML[++k];
			}
			if(Current != NULL)//we have seted this node, navigate to a child of it
				if(Current->GetName() != NULL)//we have seted this node, navigate to a child of it
					Current = Current->AdChildNode();

			Current->SetName(szNodeNameBuff);
			while(c == CSPACE)
			{
				c = szXML[++k];
				if(c == CSLASH)
				{
					break;
				}
				if(c == CQM && bIsPI)
				{
					break;
				}
				//memset(szAttrNameBuff,0,80*sizeof(_TCHAR));
				//memset(szAttrValBuff,0,80*sizeof(_TCHAR));
				voidstr(szAttrNameBuff);
				voidstr(szAttrValBuff);
				CAttribute* pA = new CAttribute();
				while(c != CEQUAL)
				{//loops until the attribute name has been entirely read
					if(c != CNEW && c != CTAB && c != CRET)
						szAttrNameBuff = concat(szAttrNameBuff, c);
					c = szXML[++k];
				}
				c = szXML[++k];
				if(c == CQUOTE || c == CDQUOTE)
				{
					cDelim = c;
					c = szXML[++k];
				}
				while(c != cDelim && cDelim != 0)
				{//loops until the attribute value has been entirely read
					if(c != CNEW && c != CTAB && c != CRET)
						szAttrValBuff = concat(szAttrValBuff, c);
					c = szXML[++k];
				}
				cDelim = 0;
				c = szXML[++k];
				pA->SetName(szAttrNameBuff);
				pA->SetValue(szAttrValBuff);
				Current->AddAttribute(pA);
			}
			if(c == CSLASH)
			{
				Current = Current->GetParent();
				c=szXML[++k];
				while(c != CCLOSE)
				{
					c = szXML[++k];
				}
			}
			if(c == CQM && bIsPI)
			{
				Current->SetNodeAsPI();
				Current = Current->GetParent();
				c=szXML[++k];
				bIsPI = false;
				while(c != CCLOSE)
				{
					c = szXML[++k];
				}
			}
			if(c == CCLOSE)
			{
				;
			}
		}
		if(c != COPEN && c != CCLOSE && c != CSLASH/* && c != CSPACE*/)
		{
			//memset(szNodeValBuff,0,80*sizeof(_TCHAR));
			voidstr(szNodeValBuff);
			while(c != COPEN)
			{
				if(c != CNEW && c != CTAB && c != CRET/* && c != CSPACE*/)
					szNodeValBuff =concat(szNodeValBuff,c);
				c = szXML[++k];
			}
			Current->SetValue(szNodeValBuff);
			continue;
		}
		k++;
	}
	free(szNodeNameBuff);
	free(szNodeValBuff);
	free(szAttrNameBuff);
	free(szAttrValBuff);
	return true;
}
CNode* Cxml::GetRootNode()
{
	return m_RootNode;
}
void Cxml::GetEncoding(_TCHAR* szUnknown)
{
	unsigned char bom[5] = {0};
	char *buf = (char *)szUnknown;
	bom[0] = buf[0];
	bom[1] = buf[1];
	bom[2] = buf[2];
	bom[3] = buf[3];
	if (bom[0] == 0xef && bom[1] == 0xbb && bom[2] == 0xbf)
	{
		m_iencoding = UTF_8;
	}
	else
	{
		if ((bom[0] == 0xef && bom[1] == 0xbb && bom[2] == 0xbf) ||
			(bom[0] == 0xff && bom[1] == 0xfe) ||
			(bom[0] == 0xfe && bom[1] == 0xff) ||
			(bom[0] == 0 && bom[1] == 0 && bom[2] == 0xfe && bom[3] == 0xff))
		{ 
			m_iencoding = UNICODE_UNKNOWN;
		}
		else
		{ 
			m_iencoding = ASCII;
		}
	}
}
