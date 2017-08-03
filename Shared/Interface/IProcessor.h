#pragma once
#ifndef IProcessor_h__20170803
#define IProcessor_h__20170803

#include "IContainer.h"
#include "smartptr.h"

namespace Yap
{
	struct IData;

	struct IPort : public ISharedObject
	{
		virtual const wchar_t * GetId() const = 0;
		virtual unsigned int GetDimensionCount() const = 0;
		virtual int GetDataType() const = 0;
	};

	typedef IPtrContainer<IPort> IPortContainer;
	typedef IPortContainer::Iterator IPortIter;

	struct IProcessor : public ISharedObject
	{
		virtual ISharedObject * Clone() const override = 0;
		virtual const wchar_t * GetClassId() const = 0;
		virtual void SetClassId(const wchar_t * id) = 0;

		virtual const wchar_t * GetInstanceId() const = 0;
		virtual void SetInstanceId(const wchar_t * instance_id) = 0;

		virtual IPortContainer * Inputs() = 0;
		virtual IPortContainer * Outputs() = 0;

		/// Return all properties of the processor.
		virtual IVariableContainer * GetProperties() = 0;

		/// Map a global variable to a processor property.
		virtual bool MapProperty(const wchar_t * property_id, const wchar_t * variable_id,
			bool input, bool output) = 0;

		/// Set the global variable space. 
		virtual bool SetGlobalVariables(IVariableContainer * params) = 0;

		/// Line the output port to an input port of another processor.
		virtual bool Link(const wchar_t * output, IProcessor * next, const wchar_t * next_input) = 0;

		/// Feed data into the processor via the specified port.
		virtual bool Input(const wchar_t * name, IData * data) = 0;

		/** @brief Specifies the module that hosts this processor. @p
		The module should be locked till the processor is no longer used. */
		virtual void SetModule(ISharedObject * module) = 0;
	};

	typedef IPtrContainer<IProcessor> IProcessorContainer;
	typedef IProcessorContainer::Iterator IProcessorIter;
}

#endif // IProcessor_h__