/********************************************************************
Created:	2008/11/28
File:	xml.cpp
Author:	Yang Guang, gyang@phy.ecnu.edu.cn

Purpose:
Copyright: Magnetic Resonance Lab., East China Normal University
*********************************************************************/

#include "xml.h"

#include <cassert>
#include <sstream>

// #define NO_BOOST

#ifndef NO_BOOST
#include <boost/tokenizer.hpp>
using namespace boost;
#endif

using Utilities::CXml;
using Utilities::CXmlElement;

const CString CXmlElement::s_empty_string;

CString TimeToString(const CTime& time)
{
	return time.Format(_T("%Y-%m-%d %H:%M:%S"));
}

CTime StringToTime(const CString& time_string)
{
	// format: yyyy-mm-dd hh:mm:ss
	CString str = time_string;
	str.Replace('-', ' ');
	str.Replace(':', ' ');

	int year, month, day, hour, minute, second;
	std::istringstream input(std::string(CT2CA(str.GetString())));
	input >> year >> month >> day >> hour >> minute >> second;
	if (input)
	{
		return CTime(year, month, day, hour, minute, second);
	}
	else
	{
		return CTime();
	}
}

#ifndef NO_MFC
CString TimeToString(const COleDateTime& time)
{
	return time.Format(_T("%Y-%m-%d %H:%M:%S"));
}

COleDateTime StringToOleDateTime(const CString& time_string)
{
	// format: yyyy-mm-dd hh:mm:ss
	CString str = time_string;
	str.Replace('-', ' ');
	str.Replace(':', ' ');

	int year, month, day, hour, minute, second;
#ifdef UNICODE
	USES_CONVERSION;
	std::istringstream input(std::string(W2A(str.GetString())));
#else
	std::istringstream input(std::string(str.GetString()));
#endif
	input >> year >> month >> day >> hour >> minute >> second;
	return COleDateTime(year, month, day, hour, minute, second);
}

COleDateTime CXmlElement::GetOleDateTimeAttrib(const TCHAR* name) const
{
	return StringToOleDateTime(GetAttrib(name));
}

void CXmlElement::SetOleDateTimeAttrib(const TCHAR* name, COleDateTime time)
{
	SetAttrib(name, TimeToString(time));
}

COleDateTime CXmlElement::GetOleDateTime() const
{
	return StringToOleDateTime(GetData());
}

void CXmlElement::SetOleDateTime(COleDateTime time)
{
	assert(_children.empty());
	_value = TimeToString(time);
}

#endif

CXmlElement::CXmlElement(const TCHAR* name, const TCHAR* data)
	: _current_attrib(-1)
	, _current_child(-1)
	, _name(name)
	, _value(data)
{
}

CXmlElement::~CXmlElement(void)
{
	ClearChildren();
}

CXml::CXml(const TCHAR* name, const TCHAR* data)
	: CXmlElement(name, data)
{
}

void CXmlElement::ClearChildren()
{
	for (unsigned int i = 0; i < _children.size(); ++i)
	{
		delete _children[i];
	}
	_children.clear();
}

void CXmlElement::Save(CMarkup& xml)
{
	xml.AddElem(_name, _value);
	for (unsigned int i = 0; i < _attributes.size(); ++i)
	{
		xml.AddAttrib(_attributes[i].first, _attributes[i].second);
	}
	xml.IntoElem();
	for (unsigned int i = 0; i < _children.size(); ++i)
	{
		_children[i]->Save(xml);
	}
	xml.OutOfElem();
}

/**
调用此函数前需保证xml中已经通过FindElem()定位到当前元素。
*/
void CXmlElement::Load(CMarkup& xml)
{
	_name = xml.GetTagName();
	CString attrib_name, attrib_value;
	int i = 0;
	while(xml.GetNthAttrib(i, attrib_name, attrib_value))
	{
		_attributes.push_back(std::make_pair(attrib_name, attrib_value));
		++i;
	}
	_value = xml.GetData();
	if (_value.IsEmpty())
	{
		xml.IntoElem();
		while (xml.FindElem())
		{
			CXmlElement* element = new CXmlElement;
			element->Load(xml);
			_children.push_back(element);
		}
		xml.OutOfElem();
	}
}

const std::vector<CXmlElement*>& CXmlElement::GetChildElements() const
{
	return _children;
}

std::vector<CXmlElement*>& CXmlElement::GetChildElements()
{
	return _children;
}

const CXmlElement* CXmlElement::GetElement(const TCHAR* name) const
{
	assert (name != NULL && name[0] != _T('\0'));

	for(unsigned int i = 0; i < _children.size(); ++i)
	{
		if (_children[i]->_name == name)
		{
			return _children[i];
		}
	}

	return NULL;
}

bool CXmlElement::DeleteElement(const TCHAR* name)
{
	assert (name != NULL && name[0] != _T('\0'));

	bool found = false;

	for (auto iter = _children.begin(); iter != _children.end(); )
	{
		if ((*iter)->_name == name)
		{
			iter = _children.erase(iter);
			found = true;
		}
		else
		{
			++iter;
		}
	}

	return found;
}

CXmlElement* CXmlElement::GetElement(const TCHAR* name, bool create_if_not_exist)
{
	assert (name != NULL && name[0] != _T('\0'));

	for(unsigned int i = 0; i < _children.size(); ++i)
	{
		if (_children[i]->_name == name)
		{
			return _children[i];
		}
	}

	if (create_if_not_exist)
	{
		return AddElement(name);
	}

	return NULL;
}

CXmlElement* CXmlElement::AddElement(const TCHAR* name, const TCHAR* value)
{
	CXmlElement* element = new CXmlElement(name, value);
	_children.push_back(element);

	return element;
}

CXmlElement* CXmlElement::AddIntegerElement(const TCHAR* name, int data)
{
	CString str;
	str.Format(TEXT("%d"), data);

	return AddElement(name, str);
}

CXmlElement* CXmlElement::SetElement(const TCHAR* name, const TCHAR* value, bool create_if_not_exist /* = false */)
{
	CXmlElement* element = GetElement(name, create_if_not_exist);
	if (element != NULL)
	{
		element->SetData(value);
	}

	return element;
}

void CXmlElement::SetData(const TCHAR* value)
{
	// 元素不可同时包含值和子元素
	assert(_children.size() == 0);
	_value = value;
}

const CString& CXmlElement::GetAttrib(const TCHAR* name) const
{
	static CString empty_string;
	for (unsigned int i = 0; i < _attributes.size(); ++i)
	{
		if (_attributes[i].first == name)
		{
			return _attributes[i].second;
		}
	}
	return empty_string;
}

int CXmlElement::GetIntegerAttrib(const TCHAR* name, int default_value) const
{
	CString attrib = GetAttrib(name);
	if (attrib.IsEmpty())
		return default_value;
	errno = EDOM;
	int value = _tstoi(GetAttrib(name).GetString());

	if ((value == 0 && attrib != _T("0")) || errno == ERANGE || errno == EINVAL)
		return default_value;

	return value;
}

bool CXmlElement::GetBoolAttrib(const TCHAR* name, bool default_value) const
{
	return GetIntegerAttrib(name, default_value ? 1 : 0) != 0;
}

double CXmlElement::GetFloatAttrib(const TCHAR* name, double default_value) const
{
	CString attrib = GetAttrib(name);
	if (attrib.IsEmpty())
		return default_value;

	double value = _tstof(GetAttrib(name).GetString());
	if ((value == 0.0 && (attrib != _T("0") && attrib != _T("0.0"))) || errno == ERANGE || errno == EINVAL)
		return default_value;

	return value;
}

ATL::CTime CXmlElement::GetTimeAttrib(const TCHAR* name) const
{
	return StringToTime(GetAttrib(name));
}

void CXmlElement::SetAttrib(const TCHAR* name, const TCHAR* value)
{
	// don't save empty attributes.
	if (CString(value).IsEmpty())
		return;

	for (unsigned int i =0; i < _attributes.size(); ++i)
	{
		if (_attributes[i].first == name)
		{
			_attributes[i].second = value;
			return;
		}
	}
	_attributes.push_back(std::make_pair(name, value));
}

void CXmlElement::SetIntegerAttrib(const TCHAR* name, int value)
{
	CString str;
	str.Format(_T("%d"), value);
	SetAttrib(name, str);
}

void CXmlElement::SetBoolAttrib(const TCHAR* name, bool value)
{
	SetIntegerAttrib(name, value ? 1 : 0);
}

void CXmlElement::SetFloatAttrib(const TCHAR* name, double value)
{
	CString str;
	str.Format(_T("%g"), value);
	SetAttrib(name, str);
}

void CXmlElement::SetTimeAttrib(const TCHAR* name, const ATL::CTime& time)
{
	SetAttrib(name, TimeToString(time));
}

const CString& CXmlElement::GetName() const
{
	return _name;
}

const CString& CXmlElement::GetData() const
{
	return _value;
}

int CXmlElement::GetInteger(int default_value) const
{
	CString data = GetData();
	if (data.IsEmpty())
		return default_value;

	_set_errno(0);
	int value = _tstoi(data);
	int error_number = errno;
	if ((value == 0 && data != _T("0")) || error_number == ERANGE || error_number == EINVAL)
	{
		return default_value;
	}
	return value;
}

bool CXmlElement::GetBool(bool default_value) const
{
	return GetInteger(default_value ? 1 : 0) != 0;
}

double CXmlElement::GetFloat(double default_value) const
{
	CString data = GetData();
	if (data.IsEmpty())
		return default_value;

	_set_errno(0);
	double value = _tstof(data.GetString());
	if ((value == 0.0 && (data != _T("0") && data != _T("0.0"))) || errno == ERANGE || errno == EINVAL)
		return default_value;

	return value;
}

ATL::CTime CXmlElement::GetTime() const
{
	return StringToTime(GetData());
}

#ifndef NO_BOOST
void CXmlElement::GetIntegers(std::vector<int>& values) const
{
	CString data = GetData();
	if (data.IsEmpty())
	{
		return;
	}
	std::string data_string = CT2CA(data.GetString());
	tokenizer<> tokens(data_string);

	for (tokenizer<>::iterator it = tokens.begin(); it != tokens.end(); ++it)
	{
		values.push_back(atoi((*it).c_str()));
	}
}

std::vector<double> CXmlElement::GetFloats() const
{
	CString data = GetData();
	std::vector<double> values;

	if (data.IsEmpty())
	{
		return values;
	}

	std::string data_string = CT2CA(data.GetString());
	tokenizer<> tokens(data_string);

	for (tokenizer<>::iterator it = tokens.begin(); it != tokens.end(); ++it)
	{
		values.push_back(atof((*it).c_str()));
	}

	return values;
}
#endif
void CXmlElement::SetInteger(int value)
{
	assert(_children.empty());
	_value.Format(_T("%d"), value);
}

void CXmlElement::SetFloat(double value)
{
	assert(_children.empty());
	_value.Format(_T("%f"), value);
}

void CXmlElement::SetBool(bool value)
{
	SetInteger(value ? 1 : 0);
}

void CXmlElement::SetTime(const ATL::CTime& time)
{
	assert(_children.empty());
	_value = TimeToString(time);
}

void CXmlElement::SetIntegers(const std::vector<int>& values)
{
	assert(_children.empty());

	CString format_string = _T("%d");
	for (unsigned int i = 0; i < values.size(); ++i)
	{
		_value.Format(format_string, values[i]);
		format_string = _value + _T(" %d");
	}
}

void CXmlElement::SetFloats(const std::vector<double>& values)
{
	assert(_children.empty());

	CString format_string = _T("%lf");
	for (unsigned int i = 0; i < values.size(); ++i)
	{
		_value.Format(format_string, values[i]);
		format_string = _value + _T(" %lf");
	}
}

const CString& CXmlElement::GetElementData(const TCHAR* name) const
{
	const CXmlElement* element = GetElement(name);

	return (element == NULL) ? s_empty_string : element->GetData();
}

int CXmlElement::GetElementInteger(const TCHAR* name) const
{
	const CXmlElement* element = GetElement(name);

	if (element == NULL)
	{
		throw false;
	}
	return element->GetInteger();
}

bool CXmlElement::GetElementBool(const TCHAR* name) const
{
	return GetElementInteger(name) != 0;
}

double CXmlElement::GetElementFloat(const TCHAR* name) const
{
	const CXmlElement* element = GetElement(name);

	if (element == NULL)
	{
		throw false;
	}

	return element->GetFloat();
}

bool CXml::Load(LPCTSTR path)
{
	if (!_markup.Load(path))
		return false;

	if (!_markup.FindElem())
		return false;

	CXmlElement::Load(_markup);

	return true;
}

bool CXml::Save(LPCTSTR path)
{
	_markup.SetDoc(NULL);

	CXmlElement::Save(_markup);

	return _markup.Save(path);
}

const CString& CXml::GetDoc()
{
	CXmlElement::Save(_markup);
	return _markup.GetDoc();
}

bool CXml::SetDoc(const CString& doc)
{
	if (!_markup.SetDoc(doc) || !_markup.FindElem())
	{
		return false;
	}

	CXmlElement::Load(_markup);
	return true;
}