#ifndef PythonImpl_H_20171020
#define PythonImpl_H_20171020

#pragma once

#include <Interface/IPython.h>
#include <vector>
#include <list>
#include <boost/python/list.hpp>
#include <minwinbase.h>

namespace Yap
{
	namespace boostpy = boost::python;
	class PythonImpl : public IPython {

	public:
		~PythonImpl();

		virtual void* Process2D(const wchar_t* module_name, const wchar_t* method_name, int data_type,
			void * data, size_t width, size_t height, size_t& out_width, size_t& out_height) override;
		virtual void* Process3D(const wchar_t* module_name, const wchar_t* method_name, int data_type,
			void * data, size_t width, size_t height, size_t slice,
			size_t& out_width, size_t& out_height, size_t& out_slice) override;

	protected:
		template<typename T>
		void* Process2D(const wchar_t* module_name, const wchar_t* method_name,
			T* data, size_t width, size_t height,
			size_t& out_width, size_t& out_height);

		template<typename T>
		void* Process3D(const wchar_t* module_name, const wchar_t* method_name,
			T* data, size_t width, size_t height, size_t slice,
			size_t& out_width, size_t& out_height, size_t& out_slice);

		template< typename T>
		std::vector<T> Pylist2Vector(const boostpy::object& iterable);

		template< typename T>
		std::list<T> Pylist2list(const boostpy::object& iterable);

		template<typename T>
		boostpy::list Vector2Pylist(const std::vector<T>& v);

		template<typename T>
		boostpy::list List2Pylist(const std::list<T>& v);

		template<typename T>
		boostpy::list CArrary2Pylist(T* data, size_t width, size_t height, size_t slice = 1);

		template<typename T>
		void Pylist2CArray(boostpy::list li, T* out_data, size_t out_width, size_t out_height, size_t out_slice = 1);

	public:
		static PythonImpl& GetInstance();

	protected:
		PythonImpl();
		static std::shared_ptr<PythonImpl> s_instance;
		CRITICAL_SECTION g_cs;
	};
}

#endif