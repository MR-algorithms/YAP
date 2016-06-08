/********************************************************************
Filename: 	EasySizeDialog.cpp
Created:	2010/02/03
Author:		Yang Guang, gyang@phy.ecnu.edu.cn
Zhang Chengxiu, angcx@126.com
Purpose:

All right reserved. 2009 -

Shanghai Key Laboratory of Magnetic Resonance,
East China Normal University
*********************************************************************/

#include "stdafx.h"
#include "EasySizeDialog.h"

IMPLEMENT_DYNAMIC(CEasySizeDialog, CDialogEx)

	CEasySizeDialog::CEasySizeDialog(UINT template_id, const CString& name, CWnd* parent, bool resizable)
	: CDialogEx(template_id, parent)
	, _name(name)
	, _resizable(resizable)
	, _rect(0, 0, 0, 0)
	, _display_in_second_monitor(false)
	, _modal(true)
{
}

CEasySizeDialog::~CEasySizeDialog()
{
}

BEGIN_MESSAGE_MAP(CEasySizeDialog, CDialogEx)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_CREATE()
END_MESSAGE_MAP()

int CEasySizeDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CEasySizeDialog::OnDestroy()
{
	SaveDialogPos();
	CDialogEx::OnDestroy();
}

BOOL CEasySizeDialog::Create(UINT nIDTemplate, CWnd* pParentWnd)
{
	_modal = false;
	return __super::Create(nIDTemplate, pParentWnd);
}

BOOL CEasySizeDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (_resizable)
	{
		INIT_EASYSIZE;	// 与LoadDialogPos()配合使用，必须在前面调用。
	}
	LoadDialogPos();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

bool CEasySizeDialog::LoadDialogPos()
{
// 	CBCGPRegistry reg(FALSE, TRUE);
// 	CString registry_key = _T("SoftWare\\SKMR\\PsiAnswerviewer");
// 	if (reg.Open(registry_key + _name) == 0)
// 	{
// 		return false;
// 	}
// 
// 	int left, top, width, height;
// 
// 	if (!reg.Read(_T("Left"), left) || !reg.Read(_T("Top"), top))
// 	{
// 		reg.Close();
// 		return false;
// 	}
// #ifdef _DEBUG
// 	LoadFirstMonitorRect();
// 	if (_display_in_second_monitor)
// 	{
// 		left += _rect.Width();
// 	}
// #endif // _DEBUG
// 
// 	int resizable;
// 	if (!reg.Read(_T("Resizable"), resizable) || resizable == 0 || !reg.Read(_T("Width"), width) || !reg.Read(_T("Height"), height))
// 	{
// 		SetWindowPos(&wndTop, left, top, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
// 	}
// 	else
// 	{
// 		SetWindowPos(&wndTop, left, top, width, height, SWP_SHOWWINDOW);
// 	}
// 	reg.Close();
	return true;
}

bool CEasySizeDialog::SaveDialogPos()
{
// 	CRect rc;
// 	GetWindowRect(rc);
// 	CBCGPRegistry reg(FALSE, FALSE);
// 	CString registry_key = _T("SoftWare\\SKMR\\PsiAnswerViewer\\");
// 	if (reg.CreateKey(registry_key + _name) == 0)
// 	{
// 		return false;
// 	}
// #ifdef _DEBUG
// 	LoadFirstMonitorRect();
// 	if (_display_in_second_monitor)
// 	{
// 		rc.left = rc.left - _rect.Width();
// 		rc.right = rc.right - _rect.Width();
// 	}
// 
// #endif // _DEBUG
// 
// 	reg.Write(_T("Resizable"), _resizable ? 1 : 0);
// 	reg.Write(_T("Left"), rc.left);
// 	reg.Write(_T("Top"), rc.top);
// 	reg.Write(_T("Width"), rc.Width());
// 	reg.Write(_T("Height"), rc.Height());
// 
// 	reg.Close();
	return true;
}

void CEasySizeDialog::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialogEx::OnSizing(fwSide, pRect);
	if (_resizable)
	{
		CSize min_size = OnGetMinSize();
		EASYSIZE_MINSIZE(min_size.cx, min_size.cy, fwSide, pRect);
	}
}

void CEasySizeDialog::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}

CSize CEasySizeDialog::OnGetMinSize()
{
	return CSize(0, 0);
}

void CEasySizeDialog::OnCancel()
{
	if (_modal)
	{
		__super::OnCancel();
	}
	else
	{
		ShowWindow(SW_HIDE);
	}
}

void CEasySizeDialog::OnOK()
{
	if (_modal)
	{
		__super::OnOK();
	}
	else
	{
		ShowWindow(SW_HIDE);
	}
}


