/********************************************************************
	Filename: 	Clipboard.h
	Created:	2011/02/14
	Author:		Yang Guang, gyang@phy.ecnu.edu.cn
	Purpose:	

	All right reserved. 2009 -

	Shanghai Key Laboratory of Magnetic Resonance, 
	East China Normal University
*********************************************************************/

#ifndef Clipboard_h__20110214
#define Clipboard_h__20110214

#include <atlstr.h>

#pragma once
namespace Utilities
{
	class CClipboard
	{
	public:
		static CString GetClipboardText();
		static bool SetClipboardText(const TCHAR * text);
	};

}


#endif // Clipboard_h__20110214