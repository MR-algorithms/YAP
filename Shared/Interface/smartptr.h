#pragma once

#ifndef yapMemory_h__20160817
#define yapMemory_h__20160817

#include <cassert>
#include <memory>

namespace Yap {

	/**
	@remark In YAP framework, objects are often created in one module and used in another. Since different
	modules can be developed by different developers and built with different compilers (or different
	versions of compiler), objects can only be passed from on module to another in form of interface
	pointers.

	There are three typical scenarios:
	1. The object are created in free store in the original module, and its ownership is transfered to
	one client module. In this case the client should not delete the interface pointer directly, since the
	client module might be using a different C runtime. Instead, the object should implement IDynamicObject
	interface and the client can call IDynamicObject::DeleteThis() to delete the object when it's finished
	with the object.

	2. The object is created in free store in the original module. However, different from case 1, the object
	may be shared by different
	client modules. In this case the object should implement ISharedObject and use a reference count to manage
	the life cycle of the object. Client module should Lock() the IShareObject as long as the object is used, and
	should call Release() when it's finished with the object. An ISharedObject should delete itself when no one
	is using it.

	3. The object is created on the stack of the original module and the address of the object is pass to
	client module. In this case, the object SHOULD NOT implement either IDynamicObject or ISharedObject.
	The client module can only use the interface pointer before the function call returns. It CAN NOT store
	the interface pointer for late use. Since it is the most probable scenario to cause problem. We strongly
	suggest not to use this.
	*/

	/**
	If an object is created dynamically in one module and then its ownership is transferred to another module, then
	the object should implement this interface, so that the client module can call DeleteThis() to safely deallocate
	the object.

	Normally, the object should use 'delete this' to delete itself when implementing DeleteThis().
	*/
	struct IDynamicObject
	{
		/// Client of the object should call this function to delete the object, instead of delete the interface pointer directly.
		virtual void DeleteThis() = 0;
	};

	/**
	If an object is created dynamically in one module, and then passed to several different users, it should
	implement this interface. The object should also implement the Clone() method, so that the object can be
	cloned.
	*/
	struct ISharedObject
	{
		///	Client of the object should call this function to keep the object in memory.
		virtual void Lock() = 0;

		/// Client of the object should call this function when it's finished with the object.
		virtual void Release() = 0;

		/// Return Cloned object.
		virtual ISharedObject * Clone() const = 0;

	};

/**
	SmartPtr is used to wrap ISharedObject object. It's similar to std::smart_ptr, however, it 
	calles ISharedObject::Lock() to add reference, and calles ISharedObject::Release() when SmartPtr
	itself is destroyed.

	The pointer SmartPtr wraps need not to be a pointer to SmartPtr. Instead, it 
	only need to point to an object which implements ISharedObject.

	@note You must use YapShared() function to create a SmartPtr wrapping a raw pointer.

	@remarks TYPE class should implement ISharedObject or the pointer should pointer to an object of 
	a class derived from both ISharedObject and TYPE. 
*/
template <typename TYPE>
class SmartPtr
{
public:
	SmartPtr() : _pointer(nullptr) {}

	SmartPtr(const SmartPtr<TYPE>& source) : _pointer(source._pointer) 
	{
		auto shared_object = dynamic_cast<ISharedObject*>(_pointer);
		if (shared_object != nullptr)
		{
			shared_object->Lock();
		}
	}

	template <typename SOURCE_TYPE>
		SmartPtr(const SmartPtr<SOURCE_TYPE>& source) : _pointer(source.get())
	{
			if (source.get() == nullptr)
				return;

			if (dynamic_cast<TYPE*>(source.get()) == nullptr)
				throw std::bad_cast();

			auto shared_object = dynamic_cast<ISharedObject*>(_pointer);
			if (shared_object != nullptr)
			{
				 shared_object->Lock();
			}
	}

		SmartPtr(SmartPtr<TYPE>&& source) : _pointer(source._pointer)
	{
			source._pointer = nullptr;
	}

	template <typename SOURCE_TYPE>
		SmartPtr(SmartPtr<SOURCE_TYPE>&& source) : _pointer(dynamic_cast<TYPE*>(source.get()))
	{
				if (_pointer == nullptr && source.get() != nullptr)
			throw std::bad_cast();

				auto shared_pointer = dynamic_cast<ISharedObject*>(_pointer);
				if (shared_pointer != nullptr)
				{
					shared_pointer->Lock();
				}
	}

	~SmartPtr()
	{
		auto shared_object = dynamic_cast<ISharedObject*>(_pointer);

		if (shared_object != nullptr)
		{
			shared_object->Release();
			_pointer = nullptr;
		}
	}

	const SmartPtr<TYPE>& operator = (const SmartPtr<TYPE>& source) 
	{
		if (_pointer != source._pointer)
		{
			auto shared_object = dynamic_cast<ISharedObject*>(_pointer);
			if (shared_object != nullptr) 
			{
				shared_object->Release();
			}

			_pointer = source._pointer;
			auto shared_object_new = dynamic_cast<ISharedObject*>(_pointer);
			if (shared_object_new != nullptr)
			{
				shared_object_new->Lock();
			}
		}

		return *this;
	}

	const SmartPtr<TYPE>& operator = (SmartPtr<TYPE>&& source) 
	{
		if (_pointer != source._pointer)
		{
			auto shared_object = dynamic_cast<ISharedObject*>(_pointer);
			if (shared_object != nullptr)
			{
				shared_object->Release();
			}

			_pointer = source._pointer;
			source._pointer = nullptr;
		}

		return *this;
	}

	void reset()
	{
		auto shared_object = dynamic_cast<ISharedObject*>(_pointer);
		if (shared_object != nullptr)
		{
			shared_object->Release();
		}
		_pointer = nullptr;
	}

	TYPE * get()
	{
		return _pointer;
	}

	const TYPE * get() const
	{
		return _pointer;
	}

	TYPE & operator * ()
	{
		return *_pointer;
	}

	const TYPE & operator * () const
	{
		return *_pointer;
	}

	const TYPE * operator -> () const 
	{
		assert(_pointer != nullptr);
		return _pointer;
	}

	TYPE * operator -> ()
	{
		assert(_pointer != nullptr);
		return _pointer;
	}

	operator bool() const 
	{
		return _pointer != nullptr;
	}
private:
	TYPE * _pointer;

	explicit SmartPtr(TYPE * pointer) : _pointer(pointer)
	{
		auto shared_object = dynamic_cast<ISharedObject*>(pointer);
		if (shared_object != nullptr) 
		{
			shared_object->Lock();
		}
	}

	template<typename TYPE> friend SmartPtr<TYPE> YapShared(TYPE * object);
	template<typename TYPE> friend SmartPtr<TYPE> YapShared(ISharedObject* object);
};

/**
	This is the factory function to create an SmartPtr to wrap a pointer to an ISharedObject object.
*/
template <typename TYPE>
SmartPtr<TYPE> YapShared(TYPE * object)
{
	assert(object == nullptr || dynamic_cast<ISharedObject*>(object) != nullptr &&
		   "Only pointers to object implementing ISharedObject can be wrapped using YapShared().");

	return Yap::SmartPtr<TYPE>(object);
}

/**
	This function should be used to help to wrap an interface pointer which is dynamically 
	created in another module and the ownership of the object is transfered to current module.

	TYPE class should implement IDynamicObject or the pointer should pointer to an object of
	a class derived from both IDynamicObject and TYPE.

	Returned shared_ptr can be copied and used freely in the current module.
	However, the shared_ptr SHALL NOT be passed to other modules.
*/

template <typename TYPE>
std::shared_ptr<TYPE> YapDynamic(TYPE * object)
{
	assert((object == nullptr || dynamic_cast<IDynamicObject*>(object) != nullptr) &&
		   "Only pointers to object implementing IDynamicObject can be wrapped using YapDynamicObject().");

	return std::shared_ptr<TYPE>(object, [](TYPE* pointer) {
		auto dynamic_object = dynamic_cast<IDynamicObject*>(pointer);
		assert(dynamic_object != nullptr);

		dynamic_object->DeleteThis(); });
}


template <typename TYPE>
SmartPtr<TYPE> YapShared(ISharedObject * clonable)
{
	assert(clonable == nullptr || dynamic_cast<TYPE*>(clonable) != nullptr &&
		   "Only pointer to classes implemented TYPE interface can be wrapped using YapShared().");
	assert(clonable == nullptr || dynamic_cast<ISharedObject*>(clonable) != nullptr &&
		   "Only pointers to object implementing ISharedObject can be wrapped using YapShared().");

	return Yap::SmartPtr<TYPE>(dynamic_cast<TYPE*>(clonable));
}

/**
	This function clones an object and wraps it with SmartPtr.
	It requires that the object implement IClonable and ISharedObject.
*/
template <typename TYPE>
SmartPtr<TYPE> YapClone(TYPE * object)
{
	assert(object != nullptr);
	assert(dynamic<IClonable*>(object) != nullptr && "The object must implement IClonable.");
	assert(dynamic<ISharedObject*>(object) != nullptr && "The object must implement ISharedObject.");
}

}; // namespace YAP

#define IMPLEMENT_CLONE(class_name) private:\
virtual class_name * Clone() const override {\
	return new (std::nothrow) class_name(*this);}

#define IMPLEMENT_LOCK_RELEASE private:\
virtual void Release() override{\
		if (_use_count == 0 || --_use_count == 0) \
		{ delete this; } \
}\
virtual void Lock() override{\
	++_use_count;}\
unsigned int _use_count = 0;

#define IMPLEMENT_SHARED(class_name) IMPLEMENT_LOCK_RELEASE \
IMPLEMENT_CLONE(class_name)

#endif
