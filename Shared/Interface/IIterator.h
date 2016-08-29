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

	struct IPort;
	struct IProcessor;

	typedef IIterator<IPort> IPortIter;
	typedef IIterator<IProcessor> IProcessorIter;
};

#endif // IIterator_h__
