#pragma once

/********************************************************************
Created:	2008/11/28
File:	Xml.h
Author:	Yang Guang, gyang@phy.ecnu.edu.cn

Purpose:
Copyright: Magnetic Resonance Lab., East China Normal University
*********************************************************************/
#ifndef XmlElement_h__2008_11_28
#define XmlElement_h__2008_11_28

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501	// Windows XP
#endif

#include <vector>
#ifndef NO_MFC
#include <afx.h>
#endif

#include <atltime.h>
#include <atlcomtime.h>

#include "markup.h"



namespace Utilities
{
	class CXmlElement
	{
	public:
		explicit CXmlElement(const TCHAR* name = NULL, const TCHAR* data = NULL);
		virtual ~CXmlElement(void);

		CXmlElement* GetElement(const TCHAR* name, bool create_if_not_exist = false);
		const CXmlElement* GetElement(const TCHAR* name) const;
		bool DeleteElement(const TCHAR* name);
		void ClearChildren();

		CXmlElement* SetElement(const TCHAR* name, const TCHAR* value, bool create_if_not_exist = false);
		const CString& GetElementData(const TCHAR* name) const;
		int GetElementInteger(const TCHAR* name) const;
		double GetElementFloat(const TCHAR* name) const;
		bool GetElementBool(const TCHAR* name) const;

		const std::vector<CXmlElement*>& GetChildElements() const;
		std::vector<CXmlElement*>& GetChildElements();

		CXmlElement* AddElement(const TCHAR* name, const TCHAR* value = NULL);
		CXmlElement* AddIntegerElement(const TCHAR* name, int data);

		const CString& GetAttrib(const TCHAR* name) const;
		int GetIntegerAttrib(const TCHAR* name, int default_value = 0) const;
		bool GetBoolAttrib(const TCHAR* name, bool default_value = false) const;
		double GetFloatAttrib(const TCHAR* name, double default_value = 0.0) const;
		ATL::CTime GetTimeAttrib(const TCHAR* name) const;

#ifndef NO_MFC
		COleDateTime GetOleDateTimeAttrib(const TCHAR* name) const;
		void SetOleDateTimeAttrib(const TCHAR* name, COleDateTime time);
#endif
		void SetAttrib(const TCHAR* name, const TCHAR* value);
		void SetIntegerAttrib(const TCHAR* name, int value);
		void SetBoolAttrib(const TCHAR* name, bool value);
		void SetFloatAttrib(const TCHAR* name, double value);
		void SetTimeAttrib(const TCHAR* name, const ATL::CTime& time);

		const CString& GetName() const;
		const CString& GetData() const;
		int GetInteger(int default_value = 0) const;
		bool GetBool(bool default = false) const;
		double GetFloat(double default_value = 0.0) const;
		ATL::CTime GetTime() const;

		void GetIntegers(std::vector<int>& values) const;
		std::vector<double> GetFloats() const;

		void SetData(const TCHAR* value);
		void SetInteger(int value);
		void SetBool(bool value);
		void SetFloat(double value);
		void SetTime(const ATL::CTime& time);

#ifndef NO_MFC
		COleDateTime GetOleDateTime() const;
		void SetOleDateTime(COleDateTime time);
#endif

		void SetIntegers(const std::vector<int>& values);
		void SetFloats(const std::vector<double>& values);

	protected:
		void Save(CMarkup& markup);
		void Load(CMarkup& markup);

		std::vector<std::pair<CString, CString>> _attributes;
		std::vector<CXmlElement*> _children;

		CString _name;
		CString _value;

		int _current_child;
		int _current_attrib;
		static const CString s_empty_string;
	};

	class CXml : public CXmlElement
	{
	public:
		explicit CXml(const TCHAR* name = NULL, const TCHAR* data = NULL);
		bool Load(LPCTSTR path);
		bool Save(LPCTSTR path);
		const CString& GetDoc();
		bool SetDoc(const CString& doc);
	protected:
		CMarkup _markup;
	};
}

#endif // XmlElement_h__2008_11_28