#pragma once

#ifndef IProcessor_h__20160818
#define IProcessor_h__20160818

#include "IMemory.h"
#include "IContainer.h"
#include "IProperty.h"

namespace Yap
{
	struct IData;

	struct IPort
	{
		virtual const wchar_t * GetName() = 0;
		virtual unsigned int GetDimensions() = 0;
		virtual int GetDataType() = 0;
	};

	typedef IContainer<IPort> IPortContainer;
	typedef IPortContainer::iterator IPortIter;
	
	
	struct IProcessor 
	{
		/// Return a cloned copy of this object.
		virtual IProcessor * Clone() = 0;

		/// DeleteThis this object.
		virtual void DeleteThis() = 0;

		virtual const wchar_t * GetClassId() = 0;
		virtual void SetClassId(const wchar_t * id) = 0;

		virtual const wchar_t * GetInstanceId() = 0;
		virtual void SetInstanceId(const wchar_t * instance_id) = 0;

		virtual IPortContainer * GetInputPorts() = 0;
		virtual IPortContainer * GetOutputPorts() = 0;

		/// 获得属性访问接口。
		virtual IPropertyContainer * GetProperties() = 0;

		/// 将指定名称的属性与参数空间的参数相关联。
		virtual bool LinkProperty(const wchar_t * property_id, const wchar_t * param_id) = 0;

		/// 接口用户调用这个函数来通知模块利用参数空间中的参数更新属性。
		virtual bool UpdateProperties(IPropertyContainer * params) = 0;

		/// 将指定处理模块的输入端口链接到当前模块指定的输出端口上。
		virtual bool Link(const wchar_t * output, IProcessor * next, const wchar_t * next_input) = 0;

		/// 向当前处理模块馈送数据。
		virtual bool Input(const wchar_t * name, IData * data) = 0;
	};

	typedef IContainer<IProcessor> IProcessorContainer;
	typedef IProcessorContainer::iterator IProcessorIter;
}


#endif // IProcessor_h__