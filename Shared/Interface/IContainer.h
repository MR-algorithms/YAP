#pragma once
#ifndef IContainer_h__20170803
#define IContainer_h__20170803

namespace Yap
{
	template <typename ELEMENT_TYPE>
	struct IIterator : public IDynamicObject
	{
		virtual ELEMENT_TYPE * GetFirst() = 0;
		virtual ELEMENT_TYPE * GetNext() = 0;
	};

	template <typename ELEMENT_TYPE>
	struct IPtrContainer : public ISharedObject
	{
		typedef IIterator<ELEMENT_TYPE> Iterator;

		virtual ELEMENT_TYPE * Find(const wchar_t * name) = 0;
		virtual Iterator * GetIterator() = 0;
		virtual bool Add(const wchar_t * name, ELEMENT_TYPE * element) = 0;
		virtual bool Delete(const wchar_t * name) = 0;
		virtual void Clear() = 0;
	};

	template <typename ELEMENT_TYPE>
	struct IPtrArray : public ISharedObject
	{
		virtual unsigned int GetSize() const = 0;
		virtual ELEMENT_TYPE *& Element(unsigned int index) = 0;
	};

}

#endif // IContainer_h__