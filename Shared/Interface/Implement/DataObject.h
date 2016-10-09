#pragma once

#include "Interface\IData.h"
#include <complex>
#include <vector>
#include "interface\IMemory.h"
#include "Interface/Implement/SharedObjectImpl.h"
#include <assert.h>

namespace Yap
{

	class Dimensions : 
		public IDimensions, 
		public SharedObjectImpl, 
		public IClonable
	{
	public:
		Dimensions();
		explicit Dimensions(unsigned int dimension_count);
		Dimensions(const Dimensions& source);
		explicit Dimensions(IDimensions * source);

		bool SetDimension(DimensionType type, unsigned int length, unsigned int start_index = 0);
		unsigned int GetLength(unsigned int dimension_index);

		virtual unsigned int GetDimensionCount() override;

		virtual bool GetDimensionInfo(unsigned int dimension_index,
			DimensionType & dimension_type, unsigned int& start_index, unsigned int& length) override;

		virtual bool SetDimensionInfo(unsigned int dimension_index, DimensionType dimension_type, 
			unsigned int start_index, unsigned int length);

		virtual IClonable * Clone() const override;
		Dimensions & operator() (DimensionType type, unsigned int index, unsigned int length);
	private:
		std::vector<Dimension> _dimension_info;
	};

	template<typename T>
	class DataObject : 
		public IData, 
		public IDataArray<T>,
		public ISharedObject
	{
	public:
		DataObject(T* data, const Dimensions& dimensions, ISharedObject * parent = nullptr, bool own_data = false) :
			_data(data), _own_memory(own_data), _use_count(0), 
			_parent(YapShared(parent))
		{
			assert(data != nullptr);
			_dimensions = dimensions;
		}
		
		DataObject(T* data, IDimensions * dimensions, ISharedObject * parent = nullptr, bool own_data = false) : 
			_data(data), _own_memory(own_data), _dimensions(dimensions), _use_count(0), 
			_parent(SmartPtr::Make(parent))
		{
			assert(data != nullptr);
		}

		explicit DataObject(IDimensions * dimensions) : _own_memory(true), _dimensions(dimensions), _use_count(0)
		{
			unsigned int total_size = 1;

			for (unsigned int i = 0; i < _dimensions.GetDimensionCount(); ++i)
			{
				total_size *= _dimensions.GetLength(i);
			}

			_data = new T[total_size];
		}

		virtual IDimensions * GetDimensions() override
		{
			return &_dimensions;
		}

		virtual T * GetData()
		{
			return _data;
		}

		virtual void Lock()
		{
			++_use_count;
			if (_parent)
			{
				_parent->Lock();
			}
		}

		virtual void Release()
		{
			if (_parent)
			{
				_parent->Release();
			}
			assert(_use_count > 0 && "Logic error. Forget to Lock()?");

			if (_use_count == 0 || --_use_count == 0)
			{

				if (_use_count == 0)
				{
					delete this;
				}
			}
		}

	private:
		~DataObject()
		{
			if (_own_memory)
			{
				delete []_data;
			}
		}

		virtual int GetDataType() override
		{
			return type_id<T>::type;
		}

		T * _data;
		Dimensions _dimensions;
//		CLocalization _localization;

		unsigned int _use_count;
		bool _own_memory;
		SmartPtr<ISharedObject> _parent;
	};

	typedef DataObject<int> IntData;
	typedef DataObject<double> DoubleData;
	typedef DataObject<float> FloatData;
	typedef DataObject<char> CharData;
	typedef DataObject<std::complex<double>> ComplexDoubleData;
	typedef DataObject<std::complex<float>> ComplexFloatData;
	typedef DataObject<unsigned short> UnsignedShortData;
};

