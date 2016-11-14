#include "ProcessorImpl.h"

#include <iostream>
#include <algorithm>
#include <assert.h>
#include "PropertyImpl.h"
#include "Interface/IMemory.h"

using namespace std;
using namespace Yap;

namespace Yap
{
namespace details
{
	class Port : 
		public IPort,
		public SharedObjectImpl,
		public IClonable
	{
	public:
		Port::Port(const wchar_t * name, unsigned int dimension_count, int data_type) :
			_name(name),
			_dimension_count(dimension_count),
			_data_type(data_type)
		{
		}
	
		virtual const wchar_t * GetName() override
		{
			return _name.c_str();
		}

		virtual unsigned int GetDimensionCount() override
		{
			return _dimension_count;
		}

		virtual int GetDataType() override
		{
			return _data_type;
		}

	protected:
		std::wstring _name;
		unsigned int _dimension_count;
		int _data_type;


		// Inherited via IClonable
		virtual IClonable * Clone() const override
		{
			return new Port(*this);
		}

	};

	typedef ContainerImpl<IPort> PortContainer;
};	// namespace Yap::details

	using namespace details;

	ProcessorImpl::ProcessorImpl(const wchar_t * class_id) :
		_system_variables(nullptr),
		_class_id(class_id),
		_properties(YapShared(new ContainerImpl<IProperty>)),
		_input(YapShared(new ContainerImpl<IPort>)),
		_output(YapShared(new ContainerImpl<IPort>))
	{
		
	}

	Yap::ProcessorImpl::ProcessorImpl(const ProcessorImpl& rhs) :
		_input(YapShared<ContainerImpl<IPort>>(rhs._input->Clone())),
		_output(YapShared<ContainerImpl<IPort>>(rhs._output->Clone())),
		_properties(YapShared<ContainerImpl<IProperty>>(rhs._properties->Clone())),
		_instance_id(rhs._instance_id),
		_class_id(rhs._class_id),
		_system_variables(nullptr)
	{
		_links.clear();
		_property_links.clear();
	}

	ProcessorImpl::~ProcessorImpl()
	{
	}

	IPortContainer * ProcessorImpl::Inputs()
	{
		return _input.get();
	}

	IPortContainer * ProcessorImpl::Outputs()
	{
		return _output.get();
	}

	bool ProcessorImpl::Link(const wchar_t * out_port,
							 IProcessor* next_processor,
							 const wchar_t * in_port)
	{
		assert(wcslen(out_port) != 0);
		assert(next_processor != nullptr);
		assert(wcslen(in_port) != 0);

		if (!CanLink(out_port, next_processor, in_port))
			return false;

		_links.insert(std::make_pair(out_port, Anchor(next_processor, in_port)));
		return true;
	}

	bool ProcessorImpl::AddInput(const wchar_t * name, 
								 unsigned int dimensions,
								 int data_type)
	{
		return _input->Add(name, new Port(name, dimensions, data_type));
	}

	bool ProcessorImpl::AddOutput(const wchar_t * name,
								  unsigned int dimensions, 
								  int data_type)
	{
		return _output->Add(name, new Port(name, dimensions, data_type));
	}

	bool ProcessorImpl::Feed(const wchar_t * out_port, IData * data)
	{
		assert(wcslen(out_port) != 0);
		assert(data != nullptr);

		auto range = _links.equal_range(out_port);
		for (auto iter = range.first; iter != range.second; ++iter)
		{
			auto link = iter->second;
			assert(link.processor != nullptr);
			if (link.processor == nullptr || !link.processor->Input(link.port.c_str(), data))
				return false;
		}

		return true;
	}

	bool ProcessorImpl::AddProperty(PropertyType type,
		const wchar_t * name,
		const wchar_t * description)
	{
		try
		{
			switch (type)
			{
			case PropertyBool:
				return _properties->Add(name, new BoolProperty(name, description));
			case PropertyInt:
				return _properties->Add(name, new IntProperty(name, description));
			case PropertyFloat:
				return _properties->Add(name, new DoubleProperty(name, description));
			case PropertyString:
				return _properties->Add(name, new StringProperty(name, description));
			default:
				return false;
			}
		}
		catch (std::bad_alloc&)
		{
			return false;
		}
	}

	IPropertyContainer * ProcessorImpl::GetProperties()
	{
		return _properties.get();
	}

	void ProcessorImpl::SetInt(const wchar_t * name, int value)
	{
		assert(name != nullptr && name[0] != 0);

		auto property = _properties->Find(name);
		if (property == nullptr)
			throw PropertyException(name, PropertyException::PropertyNotFound);

		if (property->GetType() != PropertyInt)
			throw PropertyException(name, PropertyException::TypeNotMatch);

		auto int_value = dynamic_cast<IInt*>(property);
		assert(int_value != nullptr);
		int_value->SetInt(value);
	}

	int ProcessorImpl::GetInt(const wchar_t * name)
	{
		assert(name != nullptr && name[0] != 0);

		auto property = _properties->Find(name);
		if (property == nullptr)
			throw PropertyException(name, PropertyException::PropertyNotFound);

		if (property->GetType() != PropertyInt)
			throw PropertyException(name, PropertyException::TypeNotMatch);

		auto int_value = dynamic_cast<IInt*>(property);
		assert(int_value != nullptr);

		return int_value->GetInt();
	}

	void ProcessorImpl::SetFloat(const wchar_t * name, double value)
	{
		assert(name != nullptr && name[0] != 0);

		auto property = _properties->Find(name);
		if (property == nullptr)
			throw PropertyException(name, PropertyException::PropertyNotFound);

		if (property->GetType() != PropertyFloat)
			throw PropertyException(name, PropertyException::TypeNotMatch);

		auto float_value = dynamic_cast<IDouble*>(property);
		assert(float_value != nullptr);
		float_value->SetDouble(value);
	}

	double ProcessorImpl::GetFloat(const wchar_t * name)
	{
		assert(name != nullptr && name[0] != 0);

		auto property = _properties->Find(name);
		if (property == nullptr)
			throw PropertyException(name, PropertyException::PropertyNotFound);

		if (property->GetType() != PropertyFloat)
			throw PropertyException(name, PropertyException::TypeNotMatch);

		auto float_value = dynamic_cast<IDouble*>(property);
		assert(float_value != nullptr);

		return float_value->GetDouble();
	}

	void ProcessorImpl::SetBool(const wchar_t * name, bool value)
	{
		assert(name != nullptr && name[0] != 0);

		auto property = _properties->Find(name);
		if (property == nullptr)
			throw PropertyException(name, PropertyException::PropertyNotFound);

		if (property->GetType() != PropertyBool)
			throw PropertyException(name, PropertyException::TypeNotMatch);

		auto bool_value = dynamic_cast<IBoolean*>(property);
		assert(bool_value != nullptr);
		bool_value->SetBool(value);
	}


	void ProcessorImpl::SetString(const wchar_t * name, const wchar_t * value)
	{
		assert(name != nullptr && name[0] != 0);

		auto property = _properties->Find(name);
		if (property == nullptr)
			throw PropertyException(name, PropertyException::PropertyNotFound);

		if (property->GetType() != PropertyString)
			throw PropertyException(name, PropertyException::TypeNotMatch);

		auto string_value = dynamic_cast<IString*>(property);
		assert(string_value != nullptr);
		string_value->SetString(value);
	}


	const wchar_t * ProcessorImpl::GetString(const wchar_t * name)
	{
		assert(name != nullptr && name[0] != 0);

		auto property = _properties->Find(name);
		if (property == nullptr)
			throw PropertyException(name, PropertyException::PropertyNotFound);

		if (property->GetType() != PropertyString)
			throw PropertyException(name, PropertyException::TypeNotMatch);

		auto string_value = dynamic_cast<IString*>(property);
		assert(string_value != nullptr);
		return string_value->GetString();
	}

	const wchar_t * Yap::ProcessorImpl::GetClassId()
	{
		return _class_id.c_str();
	}

	void Yap::ProcessorImpl::SetClassId(const wchar_t * id)
	{
		_class_id = id;
	}

	const wchar_t * Yap::ProcessorImpl::GetInstanceId()
	{
		return _instance_id.c_str();
	}

	void Yap::ProcessorImpl::SetInstanceId(const wchar_t * instance_id)
	{
		_instance_id = instance_id;
	}

	bool Yap::ProcessorImpl::LinkProperty(const wchar_t * property_id, const wchar_t * param_id)
	{
		if (_properties->Find(property_id) != nullptr)
		{
			return false;
		}

		_property_links.insert(make_pair(wstring(property_id), wstring(param_id)));

		return true;
	}

	bool Yap::ProcessorImpl::UpdateProperties(IPropertyContainer * params)
	{
		for (auto link : _property_links)
		{
			auto property = _properties->Find(link.first.c_str());
			if (property == nullptr)
				return false;

			IProperty * system_variable = params->Find(link.second.c_str());
			if (system_variable == nullptr)
				return false;

			if (property->GetType() != system_variable->GetType())
				return false;

			try
			{
				switch (property->GetType())
				{
				case PropertyBool:
					dynamic_cast<IBoolean&>(*property).SetBool(dynamic_cast<IBoolean&>(*system_variable).GetBool());
					break;
				case PropertyInt:
					dynamic_cast<IInt&>(*property).SetInt(dynamic_cast<IInt&>(*system_variable).GetInt());
					break;
				case PropertyFloat:
					dynamic_cast<IDouble&>(*property).SetDouble(dynamic_cast<IDouble&>(*system_variable).GetDouble());
					break;
				case PropertyString:
					dynamic_cast<IString&>(*property).SetString(dynamic_cast<IString&>(*system_variable).GetString());
					break;
				default:
					return false;
				}
			}
			catch (std::bad_cast&)
			{
				return false;
			}
		}

		return true;
	}

	bool Yap::ProcessorImpl::CanLink(const wchar_t * source_output_name,
		IProcessor * next,
		const wchar_t * next_input_name)
	{
		assert(next != nullptr);
		assert(wcslen(source_output_name) != 0);
		assert(wcslen(next_input_name) != 0);

		auto out_port = _output->Find(source_output_name);
		if (out_port == nullptr)
			return false;

		auto in_ports = next->Inputs();
		if (in_ports == nullptr)
			return false;

		auto in_port = in_ports->Find(next_input_name);
		if (in_port == nullptr)
			return false;

		if ((out_port->GetDataType() & in_port->GetDataType()) == 0)
			return false;

		TODO(Refine the processing of YAP_ANY_DIMENSION and support of multiple data type.);

		return (out_port->GetDimensionCount() == in_port->GetDimensionCount() ||
			out_port->GetDimensionCount() == YAP_ANY_DIMENSION ||
			in_port->GetDimensionCount() == YAP_ANY_DIMENSION);
	}

	bool Yap::ProcessorImpl::OutportLinked(const wchar_t * out_port_name) const
	{
		return (_links.find(out_port_name) != _links.end());
	}

	bool ProcessorImpl::GetBool(const wchar_t * name)
	{
		assert(name != nullptr && name[0] != 0);

		auto property = _properties->Find(name);
		if (property == nullptr)
			throw PropertyException(name, PropertyException::PropertyNotFound);

		if (property->GetType() != PropertyBool)
			throw PropertyException(name, PropertyException::TypeNotMatch);

		auto bool_value = dynamic_cast<IBoolean*>(property);
		assert(bool_value != nullptr);

		return bool_value->GetBool();
	}
	
};	// namepace Yap

