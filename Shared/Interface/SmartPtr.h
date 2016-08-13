#ifndef dataptr_h_20140619
#define dataptr_h_20140619

#include "YapInterfaces.h"
#include <assert.h>

namespace Yap
{
	/** 
		要求TYPE类有Lock()和Release()两个函数.
	*/
	template <typename TYPE>
	class CSmartPtr
	{
	public:
		CSmartPtr() : _pointer(nullptr) {}

		explicit CSmartPtr(TYPE * pointer) : _pointer(pointer){
			if (pointer != nullptr){
				_pointer->Lock();
			}
		}

		CSmartPtr(CSmartPtr<TYPE>& source) : _pointer(source._pointer) {
			if (_pointer != nullptr){
				_pointer->Lock();
			}
		}

		CSmartPtr(CSmartPtr<TYPE>&& source) : _pointer(source._pointer) {
			if (pointer != nullptr){
				_pointer->Lock();
			}
		}

		~CSmartPtr(){
			if (_pointer != nullptr){
				_pointer->Release();
			}
		}

		const CSmartPtr<TYPE>& operator = (const CSmartPtr<TYPE>& source) {
			if (_pointer != source._pointer)
			{
				if (_pointer != nullptr){
					_pointer->Release();
				}
				_pointer = source._pointer;
				if (_pointer != nullptr){
					_pointer->Lock();
				}
			}

			return *this;
		}

		const CSmartPtr<TYPE>& operator = (const CSmartPtr<TYPE>&& source) {
			if (_pointer != source._pointer)
			{
				if (_pointer != nullptr){
					_pointer->Release();
				}
				_pointer = source._pointer;
				if (_pointer != nullptr){
					_pointer->Lock();
				}
			}

			return *this;
		}

		TYPE * get(){
			return _pointer;
		}

		TYPE & operator * () const {
			return *_pointer;
		}

		TYPE * operator -> () const {
			assert(_pointer != nullptr);
			return _pointer;
		}

		TYPE * operator -> () {
			assert(_pointer != nullptr);
			return _pointer;
		}

		operator bool() const {
			return _pointer != nullptr;
		}
	private:
		TYPE * _pointer;
	};

	/**
	要求TYPE类有Lock()和Release()两个函数.
	*/
	template <typename TYPE, typename MEMORY_TYPE>
	class CSmartPtr2
	{
	public:
		CSmartPtr2() : _pointer(nullptr) {}

		explicit CSmartPtr2(TYPE * pointer) : _pointer(pointer) {
			if (pointer != nullptr) {
				auto mp = dynamic_cast<MEMORY_TYPE*>(_pointer);
				if (mp != nullptr) {
					mp->Lock();
				}
			}
		}

		CSmartPtr2(CSmartPtr2<TYPE, MEMORY_TYPE>& source) : _pointer(source._pointer) {
			if (_pointer != nullptr) {
				auto mp = dynamic_cast<MEMORY_TYPE*>(_pointer);
				if (mp != nullptr) {
					mp->Lock();
				}
			}
		}

		CSmartPtr2(CSmartPtr2<TYPE, MEMORY_TYPE>&& source) : _pointer(source._pointer) {
			if (_pointer != nullptr) {
				auto mp = dynamic_cast<MEMORY_TYPE*>(_pointer);
				if (mp != nullptr) {
					mp->Lock();
				}
			}
		}

		~CSmartPtr2() {
			if (_pointer != nullptr) {
				auto mp = dynamic_cast<MEMORY_TYPE*>(_pointer);
				if (mp != nullptr) {
					mp->Release();
				}
			}
		}

		const CSmartPtr2<TYPE, MEMORY_TYPE>& operator = (const CSmartPtr2<TYPE, MEMORY_TYPE>& source) {
			if (_pointer != source._pointer)
			{
				if (_pointer != nullptr) {
					auto mp = dynamic_cast<MEMORY_TYPE*>(_pointer);
					if (mp != nullptr) {
						mp->Release();
					}
				}
				_pointer = source._pointer;
				if (_pointer != nullptr) {
					auto mp = dynamic_cast<MEMORY_TYPE*>(_pointer);
					if (mp != nullptr) {
						mp->Lock();
					}
				}
			}

			return *this;
		}

		const CSmartPtr2<TYPE, MEMORY_TYPE>& operator = (const CSmartPtr2<TYPE, MEMORY_TYPE>&& source) {
			if (_pointer != source._pointer)
			{
				if (_pointer != nullptr) {
					auto mp = dynamic_cast<MEMORY_TYPE*>(_pointer);
					if (mp != nullptr) {
						mp->Release();
					}
				}
				_pointer = source._pointer;
				if (_pointer != nullptr) {
					auto mp = dynamic_cast<MEMORY_TYPE*>(_pointer);
					if (mp != nullptr) {
						mp->Lock();
					}
				}
			}

			return *this;
		}

		TYPE * get() {
			return _pointer;
		}

		TYPE & operator * () const {
			return *_pointer;
		}

		TYPE * operator -> () const {
			assert(_pointer != nullptr);
			return _pointer;
		}

		TYPE * operator -> () {
			assert(_pointer != nullptr);
			return _pointer;
		}

		operator bool() const {
			return _pointer != nullptr;
		}
	private:
		TYPE * _pointer;
	};
}

#endif