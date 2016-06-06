#include "stdafx.h"
#include "DummyProcessor.h"

using namespace std;

CDummyProcessor::CDummyProcessor()
{
}


CDummyProcessor::~CDummyProcessor()
{
}

wchar_t * CDummyProcessor::GetId()
{
	return L"Dummy";
}

IPortEnumerator * CDummyProcessor::GetInputPortEnumerator()
{
	return nullptr;
}

IPortEnumerator * CDummyProcessor::GetOutputPortEnumerator()
{
	throw std::logic_error("The method or operation is not implemented.");
}

bool CDummyProcessor::Init()
{
	AddInputPort(L"Input", 2, DataTypeFloat);
	AddOutputPort(L"Output", 2, DataTypeFloat);

	return true;
}

bool CDummyProcessor::Input(const wchar_t * port, IData * data)
{
	if (wstring(port) != L"Input")
		return false;
	
	CDimensions dimensions(data->GetDimension());
	size_t count = dimensions.TotalDataCount();

	Feed(L"Output", data);

	return true;
}

bool CDummyProcessor::AddInputPort(const wchar_t * name, unsigned int dimensions, DataType data_type)
{
	try
	{
		return _input_ports.AddPort(new CPort(name, dimensions, data_type));
	}
	catch (std::bad_alloc&)
	{
		return false;
	}
}

bool CDummyProcessor::AddOutputPort(const wchar_t * name, unsigned int dimensions, DataType data_type)
{
	try
	{
		return _output_ports.AddPort(new CPort(name, dimensions, data_type));
	}
	catch (std::bad_alloc&)
	{
		return false;
	}
}

void CDummyProcessor::Feed(const wchar_t * out_port, IData * data)
{
	auto range = _links.equal_range(out_port);
	for (auto iter = range.first; iter != range.second; ++iter)
	{
		iter->second.processor->Input(iter->second.in_port.c_str(), data);
	}
}

bool CPortEnumerator::AddPort(IPort* port)
{
	if (port == nullptr)
		return false;
	_ports.insert(std::make_pair(port->GetName(), port));

	return true;
}

IPort * CPortEnumerator::GetFirstPort()
{
	_current_port = _ports.begin();
	if (_current_port != _ports.end())
	{
		return _current_port->second.get();
	}

	return nullptr;
}

IPort * CPortEnumerator::GetNextPort()
{
	return (_current_port == _ports.end() || ++_current_port == _ports.end()) ? 
		nullptr : _current_port->second.get();
}

CPort::CPort(const wchar_t * name, unsigned int dimensions, DataType data_type) : _name(name), _dimensions(dimensions), _data_type(data_type)
{
}

const wchar_t * CPort::GetName()
{
	return _name.c_str();
}

unsigned int CPort::GetDimensions()
{
	return _dimensions;
}

DataType CPort::GetDataType()
{
	return _data_type;
}
