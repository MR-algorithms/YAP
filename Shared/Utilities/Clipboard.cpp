/********************************************************************
	Filename: 	Clipboard.cpp
	Created:	2011/02/14
	Author:		Yang Guang, gyang@phy.ecnu.edu.cn
	Purpose:	

	All right reserved. 2009 -

	Shanghai Key Laboratory of Magnetic Resonance, 
	East China Normal University
*********************************************************************/

#include "StdAfx.h"
#include "Clipboard.h"
#include "macros.h"
#include <winbase.h>

using Utilities::CClipboard;

bool CClipboard::SetClipboardText(const TCHAR* str)
{
	size_t text_length = _tcslen(str);
	GLOBALHANDLE hGlobal = GlobalAlloc(GHND | GMEM_SHARE, (text_length + 1) * 2);
	if (hGlobal == 0)
	{
//		TRACEX(_T("Failed to allocate memory while copying text to clipboard."));
		return false;
	}

	char* p = reinterpret_cast<char*> (GlobalLock(hGlobal));
	if (p == NULL)
	{
//		TRACEX(_T("Failed to lock memory while copying text to clipboard."));
		return false;
	}

	strcpy_s(p, text_length * 2 + 2, CT2CA(str));
	GlobalUnlock(hGlobal);

	if (!OpenClipboard(NULL))
	{
//		TRACEX(_T("Failed to open clipboard while copying text to clipboard."));
		return false;
	}

	EmptyClipboard();
	SetClipboardData(CF_TEXT, hGlobal);
	CloseClipboard();

	return true;
}

CString CClipboard::GetClipboardText()
{
	if (!IsClipboardFormatAvailable(CF_TEXT) || !OpenClipboard(NULL))
		return _T("");

	GLOBALHANDLE hGlobal = GetClipboardData(CF_TEXT);
	char * pGlobal = reinterpret_cast<char *>(GlobalLock(hGlobal));

	CString str = CA2CT(pGlobal);

	GlobalUnlock(hGlobal);
	CloseClipboard();

	return str;
}