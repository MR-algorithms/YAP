#pragma once


#ifndef ProcessorImpl_h_20160830
#define ProcessorImpl_h_20160830

#include "Interface/IContainer.h"
#include "Interface/IMemory.h"

#include <map>
#include <string>

namespace Yap
{
	template <typename TYPE>
	class ContainerImpl :
		public IContainer<TYPE>, 
		public ISharedObject
	{
		template <typename TYPE>
		class Iterator : public IIterator<TYPE>
		{
			explicit Iterator(ContainerImpl& container) : _container(container)
			{
			}

			// Inherited via IIterator
			virtual TYPE * GetFirst() override
			{
				_current = _container._elements.begin();

				return  (_current != _container._elements.end()) ? _current->second.get() : nullptr;

			}

			virtual TYPE * GetNext() override
			{
				return  (_current != _container._elements.end() && 
						 ++_current != _container._elements.end()) ?
					_current->second.get() : nullptr;

			}

			ContainerImpl& _container;
			typename std::map<std::wstring, SmartPtr<TYPE>>::iterator _current;

			friend class ContainerImpl;
		};

	public:
		ContainerImpl()	: _use_count(0){}

		virtual IProcessor * Find(const wchar_t * name) override
		{
			auto iter = _elements.find(name);
			return  (iter != _elements.end()) ? iter->second.get() : nullptr;

		}

		virtual IIterator<TYPE> * GetIterator() override
		{
			try
			{
				return new Iterator<TYPE>(*this);
			}
			catch (std::bad_alloc&)
			{
				return nullptr;
			}
		}

		virtual void Lock() override
		{
			++_use_count;
		}

		virtual void Release() override
		{
			if (_use_count == 0 || --_use_count == 0)
			{
				delete this;
			}
		}

		bool Add(const wchar_t * name, TYPE * element)
		{
			if (element == nullptr)
				return false;

			_elements.insert(std::make_pair(std::wstring(name), YapShared(element)));

			return true;
		}

	private:
		// protect the destructor to ensure the object can only be dynamically allocated
		~ContainerImpl() {}

		std::map<std::wstring, SmartPtr<TYPE>> _elements;
		unsigned int _use_count;
	};
}

#endif // ProcessorImpl_h_20160830
