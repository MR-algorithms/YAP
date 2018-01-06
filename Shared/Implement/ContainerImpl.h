#pragma once


#ifndef ProcessorImpl_h_20160830
#define ProcessorImpl_h_20160830

#include "Interface/interfaces.h"
#include "Interface/smartptr.h"

#include <map>
#include <string>
#include <vector>

namespace Yap
{
	template <typename TYPE>
	class PtrContainerImpl :
		public IPtrContainer<TYPE>
	{
		IMPLEMENT_LOCK_RELEASE

		template <typename TYPE>  
		class Iterator : 
			public IIterator<TYPE>
		{

			explicit Iterator(PtrContainerImpl& container) : _container(container)
			{
			}

			virtual ~Iterator()
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

			virtual void DeleteThis() override
			{
				delete this;
			}

			PtrContainerImpl& _container;
			typename std::map<std::wstring, SmartPtr<TYPE>>::iterator _current;

			friend class PtrContainerImpl;
		};

	public:
		PtrContainerImpl() {}

		IPtrContainer<TYPE> * Clone() const override
		{
			auto cloned = new (std::nothrow) PtrContainerImpl;
			if (cloned == nullptr)
				return nullptr;

			for (auto element : _elements)
			{
				if (element.second)
				{
					auto cloned_element = dynamic_cast<TYPE *>(element.second->Clone());
					if (cloned_element != nullptr)
					{
						cloned->Add(element.first.c_str(), cloned_element);
					}
				}
			}

			return cloned;
		}

		virtual TYPE * Find(const wchar_t * name) override
		{
			auto iter = _elements.find(name);
			return  (iter != _elements.end()) ? iter->second.get() : nullptr;

		}

		virtual IIterator<TYPE> * GetIterator() override
		{
			return new (std::nothrow) Iterator<TYPE>(*this);
		}

		virtual bool Add(const wchar_t * name, TYPE * element) override
		{
			if (element == nullptr)
				return false;

			_elements.emplace(std::wstring(name), YapShared(element));

			return true;
		}

		virtual bool Delete(const wchar_t * name) override
		{
			auto iter = _elements.find(name);
			if (iter != _elements.end())
			{
				_elements.erase(iter);
				return true;
			}

			return false;
		}
		
		virtual void Clear() override
		{
			_elements.clear();
		}

	private:
		// protect the destructor to ensure the object can only be dynamically allocated
		~PtrContainerImpl() 
		{
			_elements.clear();
		}

		std::map<std::wstring, SmartPtr<TYPE>> _elements;
	};

	template <typename TYPE>
	class PtrArrayImpl : public IPtrArray<TYPE>
	{
		IMPLEMENT_LOCK_RELEASE

		unsigned int GetSize() const override
		{

		}

		TYPE * Element(unsigned int index) override
		{

		}


	};

}

#endif // ProcessorImpl_h_20160830
