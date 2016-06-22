#pragma once
#include "../Interface/Interface.h"

#include <map>
#include <string>
#include <memory>
#include "../Utilities/macros.h"
#include "../interface/reconData.h"

class CPort : public IPort
{
public:
	CPort(const wchar_t * name, unsigned int dimensions, DataType data_type);
	virtual const wchar_t * GetName() override;
	virtual unsigned int GetDimensions() override;
	virtual DataType GetDataType() override;

protected:
	std::wstring _name;
	unsigned int _dimensions;
	DataType _data_type;

};

class CPortEnumerator : public IPortEnumerator
{
public:
	bool AddPort(IPort* port);;

	virtual IPort * GetFirstPort() override;
	virtual IPort * GetNextPort() override;
protected:
	std::map<std::wstring, std::shared_ptr<IPort>> _ports;
	std::map<std::wstring, std::shared_ptr<IPort>>::iterator _current_port;
};

struct Anchor
{
	IProcessor * processor;
	std::wstring in_port;
	Anchor(IProcessor* processor_, const wchar_t * in_port_) : processor(processor_), in_port(in_port_) {}
};

class CFloatProperty : public IProperty, public IFloatValue
{
public:
	CFloatProperty(const wchar_t * name, double value = 0.0) 
		: _name(name), _value(value){}
	virtual double GetValue() { return _value; }
	virtual void SetValue(double value) { _value = value; }
	virtual const wchar_t * GetName() override {
		return _name.c_str();
	}
	virtual PropertyType GetType() override {
		return PropertyFloat;
	}

protected:
	double _value;
	std::wstring _name;
};

class CIntProperty : public IProperty, public IIntValue
{
public:
	CIntProperty(const wchar_t * name, int value = 0)
		: _name(name), _value(value) {}
	virtual int GetValue() { return _value; }
	virtual void SetValue(int value) { _value = value; }
	virtual const wchar_t * GetName() override {
		return _name.c_str();
	}
	virtual PropertyType GetType() override {
		return PropertyInt;
	}

protected:
	int _value;
	std::wstring _name;
};

class CBoolProperty : public IProperty, public IBoolValue
{
public:
	CBoolProperty(const wchar_t * name, bool value = 0)
		: _name(name), _value(value) {}
	virtual bool GetValue() { return _value; }
	virtual void SetValue(bool value) { _value = value; }
	virtual const wchar_t * GetName() override {
		return _name.c_str();
	}
	virtual PropertyType GetType() override {
		return PropertyBool;
	}

protected:
	bool _value;
	std::wstring _name;
};

class CStringProperty : public IProperty, public IStringValue
{
public:
	CStringProperty(const wchar_t * name, const wchar_t * value = nullptr)
		: _name(name), _value(value) {}
	virtual const wchar_t * GetValue() { return _value.c_str(); }
	virtual void SetValue(const wchar_t * value) { _value = value; }
	virtual const wchar_t * GetName() override {
		return _name.c_str();
	}
	virtual PropertyType GetType() override {
		return PropertyString;
	}

protected:
	std::wstring _value;
	std::wstring _name;
};


// class CPropertyImp : public IProperty
// {
// public:
// 	CPropertyImp(const wchar_t * name, PropertyType type) : _name(name), _type(type) 
// 	{
// 		switch (type)
// 		{
// 		case PropertyBool:
// 			_value_interface = new CBoolValue;
// 			break;
// 		case PropertyInt:
// 			_value_interface = new CIntValue;
// 			break;
// 		case PropertyFloat:
// 			_value_interface = new CFloatValue;
// 			break;
// 		case PropertyString:
// 			_value_interface = new CStringValue;
// 			break;
// 		default:
// 			break;
// 		}
// 	}
// 
// 	~CPropertyImp()
// 	{
// 		// switch ... case ...
// 		delete _value_interface;
// 	}
// 
// 	virtual const wchar_t * GetName() override {
// 		return _name.c_str();
// 	}
// 
// 	virtual PropertyType GetType() override {
// 		return _type;
// 	}
// 
// 	virtual void * GetValueInterface() override {
// 		return _value_interface;
// 	}
// protected:
// 	std::wstring _name;
// 	PropertyType _type;
// 	void * _value_interface;
// };

class CPropertyEnumerator : public IPropertyEnumerator
{
public:
	bool AddProperty(IProperty * property) {
		if (property == nullptr)
			return false;

		if (_properties.find(property->GetName()) != _properties.end())
			return false;

		_properties.insert(std::make_pair(property->GetName(), property));
		return true;
	}

	virtual IProperty * GetFirst() override;
	virtual IProperty * GetNext() override;
	virtual IProperty * GetProperty(const wchar_t * name) override;
protected:
	std::map<std::wstring, IProperty*> _properties;
	std::map<std::wstring, IProperty*>::iterator _current;

};


struct PropertyException
{
	enum Type
	{
		PropertyNotFound,
		TypeNotMatch,
	};
	std::wstring property_name;
	Type type;
	PropertyException(const wchar_t * name, Type type_) : property_name(name), type(type_){}
};

class CProcessorImp :
	public IProcessor
{
public:
	CProcessorImp();
	virtual ~CProcessorImp();

	virtual IPortEnumerator * GetInputPortEnumerator() override;
	virtual IPortEnumerator * GetOutputPortEnumerator() override;

	virtual bool Init() override;
	virtual bool Input(const wchar_t * port, IData * data) override;
	virtual bool Link(const wchar_t * out_port, IProcessor* next_processor, wchar_t * in_port);

	bool AddInputPort(const wchar_t * name, unsigned int dimensions, DataType data_type);
	bool AddOutputPort(const wchar_t * name, unsigned int dimensions, DataType data_type);
	
	void Feed(const wchar_t * name, IData * data);

	bool AddProperty(const wchar_t * name, PropertyType type);

	void SetIntProperty(const wchar_t * name, int value);
	int GetIntProperty(const wchar_t * name);
	void SetFloatProperty(const wchar_t * name, double value);
	double GetFloatProperty(const wchar_t * name);
	void SetBoolProperty(const wchar_t * name, bool value);
	bool GetBoolProperty(const wchar_t * name);
	void SetStringProperty(const wchar_t * name, const wchar_t * value);
	const wchar_t * GetStringProperty(const wchar_t * name);

	virtual IPropertyEnumerator * GetProperties() override;

	CPortEnumerator _input_ports;
	CPortEnumerator _output_ports;
	CPropertyEnumerator _properties;

	std::multimap<std::wstring, Anchor> _links;
};

