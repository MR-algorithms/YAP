#pragma once
#include "..\Interface\Interface.h"

#include <map>
#include <string>
#include <memory>

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

class CDimensions
{
public:
	CDimensions(IDimensions * dimensions) : _dimensions(*dimensions) {
	}
	size_t TotalDataCount() {
		auto dimension_count = _dimensions.GetDimensionCount();
		size_t count = 1;
		for (unsigned int i = 0; i < dimension_count; ++i)
		{
			Dimension dimension;
			_dimensions.GetDimensionInfo(i, dimension.type, dimension.start_index, dimension.length);
			count *= dimension.length;
		}

		return count;
	}
protected:
	IDimensions& _dimensions;
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

	CPortEnumerator _input_ports;
	CPortEnumerator _output_ports;

	std::multimap<std::wstring, Anchor> _links;
};

