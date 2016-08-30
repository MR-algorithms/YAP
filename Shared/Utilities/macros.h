/********************************************************************
	Filename: 	macros.h
	Created:	2009/06/15
	Author:		Yang Guang, gyang@phy.ecnu.edu.cn

	Purpose:	
	
	All right reserved. 2009 -
*********************************************************************/
#ifndef macros_h__
#define macros_h__

#include <locale.h>



#ifdef _UNICODE
#define TRACEX(A) setlocale(LC_ALL, "chs"); TRACE(_T("%S: ")A _T("\n"), __FUNCTION__ )
#define TRACEX1(A, B) setlocale(LC_ALL, "chs"); TRACE(_T("%S: ") A _T("\n"), __FUNCTION__, B)
#define TRACEX2(A, B, C) setlocale(LC_ALL, "chs"); TRACE(_T("%S: ") A _T("\n"), __FUNCTION__, B, C)
#define TRACEX3(A, B, C, D) setlocale(LC_ALL, "chs"); TRACE(_T("%S: ") A _T("\n"), __FUNCTION__, B, C, D)
#else
#define TRACEX(A) setlocale(LC_ALL, "chs"); TRACE(_T("%s: ")A _T("\n"), __FUNCTION__ )
#define TRACEX1(A, B) setlocale(LC_ALL, "chs"); TRACE(_T("%s: ") A _T("\n"), __FUNCTION__, B)
#define TRACEX2(A, B, C) setlocale(LC_ALL, "chs"); TRACE(_T("%s: ") A _T("\n"), __FUNCTION__, B, C)
#define TRACEX3(A, B, C, D) setlocale(LC_ALL, "chs"); TRACE(_T("%s: ") A _T("\n"), __FUNCTION__, B, C, D)
#endif

#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __LOC2__ __FILE__ "("__STR1__(__LINE__)") : "

// #define BUG(x) __pragma(message(__LOC2__  "Known bug in " __FUNCTION__ "() - [" __STR1__(x) "]"))
#define BUG(x) __pragma(message(__LOC2__  "[BUG] " __STR1__(x) ))
#define TODO(x) __pragma(message(__LOC2__  "[TODO] " __STR1__(x) ))
#define NOT_IMPLEMENTED TODO(Not implemented.)

// ;TRACEX(_T("Error, function called has not been implemented."))

#define TRACE_CALL_COUNT static int count = 0; TRACEX1(_T("%d"), count++);

#endif // macros_h__
