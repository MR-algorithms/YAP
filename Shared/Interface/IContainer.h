#pragma once
#ifndef IIterator_h__20160827
#define IIterator_h__20160827

namespace Yap
{
	template <typename ELEMENT_TYPE>
	struct IIterator
	{
		virtual ELEMENT_TYPE * GetFirst() = 0;
		virtual ELEMENT_TYPE * GetNext() = 0;
	};

	template <typename ELEMENT_TYPE>
	struct IContainer
	{
		typedef IIterator<ELEMENT_TYPE> iterator;

		virtual ELEMENT_TYPE * Find(const wchar_t * name) = 0;
		virtual iterator * GetIterator() = 0;
		virtual bool Add(const wchar_t * name, ELEMENT_TYPE * element) = 0;
	};

	struct IPort;
	struct IProcessor;

	typedef IIterator<IPort> IPortIter;
	typedef IIterator<IProcessor> IProcessorIter;
};

#endif // IIterator_h__
