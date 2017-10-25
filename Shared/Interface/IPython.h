#ifndef _IPython_h__
#define _IPython_h__
#pragma once

#include <Python.h>
#include "pyport.h"

typedef PyObject CPyObject;
typedef PyVarObject CPyVarObject;
typedef PyTypeObject CPyTypeObject;

typedef PyCFunction CPyCFunction;
typedef PyCFunctionWithKeywords CPyCFunctionWithKeywords;
typedef PyMethodDef CPyMethodDef;
typedef PyNoArgsFunction CPyNoArgsFunction;

typedef Py_ssize_t CPy_ssize_t;

typedef PyListObject CPyListObject;

PyObject* li = PyList_New(2);
PyObject * m;
auto llist = PyList_Type;

namespace Yap
{
	
	class IPython 
	{
		virtual void InitPython() = 0;
		virtual bool CheckScriptInfo(const wchar_t* module_file_path, const wchar_t* class_nam=NULL,
			const wchar_t* method_name=NULL) = 0;
		virtual CPyObject* RunScript(const wchar_t * script_name, CPyObject* data) = 0;

		/*About List*/
		virtual int CPyList_Append(CPyObject* a, CPyObject* b) = 0;
		virtual CPyObject* CPyList_AsTuple(CPyObject* a) = 0;
		virtual bool CPyList_Check(CPyObject* ob) = 0;
		virtual bool CPyList_CheckExact(CPyObject* ob) = 0;
		virtual int CPyList_ClearFreeList(void) = 0;
		virtual void CPyList_Fini(void) = 0;
		virtual CPyObject* CPyList_GET_ITEM(CPyObject* ob, CPy_ssize_t _size_t) = 0;
		virtual CPy_ssize_t CPyList_GET_SIZE(CPyObject* ob) = 0;
		virtual CPyObject* CPyList_GetItem(CPyObject* ob, CPy_ssize_t _size_t) = 0;
		virtual CPyObject* CPyList_GetSlice(CPyObject* ob, CPy_ssize_t _size_t1, CPy_ssize_t _size_t2) = 0;
		virtual int CPyList_Insert(CPyObject* ob, CPy_ssize_t _size_t1, CPy_ssize_t _size_t2) = 0;
		virtual CPyObject* CPyList_New(CPy_ssize_t _size_t) = 0; 
		virtual int CPyList_Reverse(CPyObject* ob) = 0;
		virtual CPyObject* CPyList_SET_ITEM(CPyObject* ob, CPy_ssize_t _size_t, CPyObject* val) = 0;
		virtual int CPyList_SetItem(CPyObject * ob, CPy_ssize_t _size_t, CPyObject* pyArgs) = 0;

		virtual int CPyList_SetSlice(CPyObject* ob1, CPy_ssize_t _size_t1, CPy_ssize_t _size_t2, CPyObject* ob2) = 0;
		virtual CPy_ssize_t CPyList_Size(CPyObject *ob) = 0;
		virtual int CPyList_Sort(CPyObject* ob) = 0;
		//PyAPI_DATA(PyTypeObject) PyList_Type;		//mark
		//PyAPI_DATA(PyTypeObject) PyListIter_Type	//mark
		//PyAPI_DATA(PyTypeObject) PyListRevIter_Type	//mark

		/*About Tuple*/
		virtual CPyObject* CPyTuple_New(CPy_ssize_t _size_t) = 0;
		virtual void CPyTuple_SetItem(CPyObject * cpyArgs, CPy_ssize_t _size_t, CPyObject* pyArgs) = 0;
		virtual CPyObject* CPyTuple_GetItem(CPyObject *object, CPy_ssize_t _size_t) = 0;

		//#define CPyObject_VAR_HEAD PyObject_VAR_HEAD
		//#define CPy_REFCNTY(ob) Py_REFCNT(ob)
		//#define CPy_TYPE(ob) Py_TYPE(ob)
		//#define CPy_SIZE(ob) Py_SIZE(ob)
		//#define CPyObject_HEAD_INIT PyObject_HEAD_INIT
		//#define CPyVarObject_HEAD_INIT PyVarObject_HEAD_INIT
		//#define CPy_INVALID_SIZE Py_INVALID_SIZE

		virtual void ReleasePython() = 0;
	};

	struct IPythonUser
	{
		virtual void SetPython(IPython* python) = 0;
		virtual IPython* GetPython() = 0;
	};
	
}

#endif