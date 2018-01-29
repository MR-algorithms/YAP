#pragma once
#include "interface/interfaces.h"
#include "interface/smartptr.h"
#include "interface/IVariable.h"

#include <string>
#include <vector>

#define IMPLEMENT_VARIABLE_NO_ENABLE public: \
	virtual int GetType() const override { return _type;} \
	virtual const wchar_t * GetId() const override { return _id.c_str(); }\
	virtual void SetId(const wchar_t * id) override { _id = id; } \
	virtual const wchar_t * GetTitle() const override { return _title.c_str();}\
	virtual void SetTitle(const wchar_t * title) override { _title = title;}\
	virtual const wchar_t * GetDescription() const override	{ return _description.c_str();	} \
	virtual void SetDescription(const wchar_t * description) override {	_description = description;	}\
protected: \
	std::wstring _id; \
	std::wstring _description; \
	std::wstring _title;\
	int _type;

#define IMPLEMENT_VARIABLE   IMPLEMENT_VARIABLE_NO_ENABLE \
public: \
	virtual void Enable(bool enable) { _enabled = enable;} \
	virtual bool IsEnabled() const override { return _enabled;} \
protected: \
	bool _enabled;

namespace Yap {
	namespace _details {
		template<typename T> struct variable_store_type
		{
			typedef T type;
			typedef std::vector<T> vector_type;
		};

		template <> struct variable_store_type<bool>
		{
			typedef bool type;
			typedef std::vector<unsigned char> vector_type;
		};

		template <> struct variable_store_type<const wchar_t * const>
		{
			typedef std::wstring type;
			typedef std::vector<std::wstring> vector_type;
		};

		template <> struct variable_store_type<IVariable*>
		{
			typedef SmartPtr<IVariable> type;
			typedef std::vector<type> vector_type;
		};
	}
}
