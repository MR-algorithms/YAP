#pragma once

#include "Interface\IData.h"
#include <complex>
#include <vector>
// #include "Localization.h"
#include "interface\IMemory.h"
#include "Interface/Implement/SharedObjectImpl.h"
#include <assert.h>

namespace Yap
{

	class DimensionsImpl : 
		public IDimensions, 
		public SharedObjectImpl, 
		public IClonable
	{
	public:
		DimensionsImpl();
		explicit DimensionsImpl(unsigned int dimension_count);
		DimensionsImpl(const DimensionsImpl& source);
		explicit DimensionsImpl(IDimensions * source);

		bool ModifyDimension(DimensionType type, unsigned int length, unsigned int start_index = 0);
		unsigned int GetLength(unsigned int dimension_index);

		virtual unsigned int GetDimensionCount() override;

		virtual bool GetDimensionInfo(unsigned int dimension_index,
			DimensionType & dimension_type, unsigned int& start_index, unsigned int& length) override;

		virtual bool SetDimensionInfo(unsigned int dimension_index, DimensionType dimension_type, 
			unsigned int start_index, unsigned int length);

		virtual IClonable * Clone() const override;
		DimensionsImpl & operator() (DimensionType type, unsigned int index, unsigned int length);
	private:
		std::vector<Dimension> _dimension_info;
	};

	template<typename T>
	class DataImpl : 
		public IData, 
		public IDataArray<T>,
		public ISharedObject
	{
	public:
		DataImpl(T* data, const DimensionsImpl& dimensions, ISharedObject * parent = nullptr, bool own_data = false) :
			_data(data), _own_memory(own_data), _use_count(0), 
			_parent(YapShared(parent))
		{
			assert(data != nullptr);
			_dimensions = dimensions;
		}
		
		DataImpl(T* data, IDimensions * dimensions, ISharedObject * parent = nullptr, bool own_data = false) : 
			_data(data), _own_memory(own_data), _dimensions(dimensions), _use_count(0), 
			_parent(SmartPtr::Make(parent))
		{
			assert(data != nullptr);
		}

		explicit DataImpl(IDimensions * dimensions) : _own_memory(true), _dimensions(dimensions), _use_count(0)
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
		~DataImpl()
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
		DimensionsImpl _dimensions;
//		CLocalization _localization;

		unsigned int _use_count;
		bool _own_memory;
		SmartPtr<ISharedObject> _parent;
	};

	typedef DataImpl<double> CDoubleData;
	typedef DataImpl<float> CFloatData;
	typedef DataImpl<char> CCharData;
	typedef DataImpl<std::complex<double>> CComplexDoubleData;
	typedef DataImpl<std::complex<float>> CComplexFloatData;

	typedef DataImpl<unsigned short> CUnsignedShortData;
};

