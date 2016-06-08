/********************************************************************
Filename: 	EasySizeDialog.h
Created:	2010/02/03
Author:		Yang Guang, gyang@phy.ecnu.edu.cn
Zhang Chengxiu, angcx@126.com
Purpose:

All right reserved. 2009 -

Shanghai Key Laboratory of Magnetic Resonance,
East China Normal University
*********************************************************************/

#ifndef EasySizeDialog_h__20100203
#define EasySizeDialog_h__20100203

#pragma once

#include "easysize.h"

class CEasySizeDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CEasySizeDialog)

public:
	CEasySizeDialog(UINT template_id, const CString& name, CWnd* parent = NULL, bool resizable = false);
	virtual ~CEasySizeDialog();
	BOOL Create(UINT nIDTemplate, CWnd* pParentWnd = NULL);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);

	virtual BOOL OnInitDialog();
	bool LoadDialogPos();
	bool SaveDialogPos();
	virtual CSize OnGetMinSize();
	virtual void OnCancel();
	virtual void OnOK();

	CString _name;
	bool _resizable;
	CRect _rect;
	bool _display_in_second_monitor;

	bool _modal;

	DECLARE_EASYSIZE_2;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

#endif // CmrDialog_h__20100203
