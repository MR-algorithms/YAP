#ifndef dataptr_h_20140619
#define dataptr_h_20140619

#include "interface.h"
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
}

#endif