#pragma once
#include "utils.h"

#define UNKNOWN			0
#define UTF_8			1
#define UTF_16			2
#define UTF_32			3
#define UTF_EBCDIC		4
#define GB_18030		5
#define ASCII			6
#define UNICODE_UNKNOWN	7

class Cxml
{
public:
	Cxml(void);
	~Cxml(void);
	// parse a string containg xml code
	bool ParseString(_TCHAR* szXML);
	//base64 utility functions
	_TCHAR decode_64(_TCHAR c);
	_TCHAR encode_64(_TCHAR c);
	_TCHAR* sencode_64(_TCHAR* s, int len=0);
	_TCHAR* sdecode_64(_TCHAR* str, int *len);
private:
	// used for the base64 methods
	_TCHAR* m_s64;
	_TCHAR m_szEncoding[16];
	int m_iCursor;
	int m_iLength;
	int m_iencoding;
	// searches for the encoding statement
	double m_dversion;
	CNode* m_RootNode;
	bool GetNode(_TCHAR* szXML);
public:
	CNode* GetRootNode();

	void GetEncoding(_TCHAR* szUnknown);
};
