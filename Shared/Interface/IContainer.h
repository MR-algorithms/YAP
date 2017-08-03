#pragma once
#ifndef IContainer_h__20170803
#define IContainer_h__20170803

namespace Yap
{
	template <typename ELEMENT_TYPE>
	struct IIterator
	{
		virtual ELEMENT_TYPE * GetFirst() = 0;
		virtual ELEMENT_TYPE * GetNext() = 0;
	};

	template <typename ELEMENT_TYPE>
	struct IPtrContainer : public ISharedObject
	{
		typedef IIterator<ELEMENT_TYPE> iterator;

		virtual ELEMENT_TYPE * Find(const wchar_t * name) = 0;
		virtual iterator * GetIterator() = 0;
		virtual bool Add(const wchar_t * name, ELEMENT_TYPE * element) = 0;
		virtual bool Delete(const wchar_t * name) = 0;
		virtual void Clear() = 0;
	};

}

#endif // IContainer_h__