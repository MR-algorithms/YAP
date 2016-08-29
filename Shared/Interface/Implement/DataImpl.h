#pragma once

#include "Interface\IData.h"
#include <complex>
#include <vector>
// #include "Localization.h"
#include "interface\IMemory.h"
#include <assert.h>

namespace Yap
{

	class CDimensionsImpl : 
		public IDimensions, 
		public IDynamicObject, 
		public IClonable
	{
	public:
		CDimensionsImpl();
		explicit CDimensionsImpl(unsigned int dimension_count);
		CDimensionsImpl(const CDimensionsImpl& source);
		explicit CDimensionsImpl(IDimensions * source);

		bool ModifyDimension(DimensionType type, unsigned int length, unsigned int start_index = 0);
		unsigned int GetLength(unsigned int dimension_index);

		virtual unsigned int GetDimensionCount();
		virtual bool GetDimensionInfo(unsigned int dimension_index, DimensionType & dimension_type, unsigned int& start_index, unsigned int& length);
		virtual bool SetDimensionInfo(unsigned int dimension_index, DimensionType dimension_type, 
			unsigned int start_index, unsigned int length);

		virtual IClonable * Clone() override;
		virtual void DeleteThis() override;
		CDimensionsImpl & operator() (DimensionType type, unsigned int index, unsigned int length);
	private:
		std::vector<Dimension> _dimension_info;
	};

	template<typename T>
	class CDataImp : 
		public IData, 
		public ISharedObject
	{
	public:
		CDataImp(T* data, const CDimensionsImpl& dimensions, ISharedObject * parent = nullptr, bool own_data = false) :
			_data(data), _own_memory(own_data), _use_count(0), 
			_parent(YapSharedObject(parent))
		{
			assert(data != nullptr);
			_dimensions = dimensions;
		}
		
		CDataImp(T* data, IDimensions * dimensions, ISharedObject * parent = nullptr, bool own_data = false) : 
			_data(data), _own_memory(own_data), _dimensions(dimensions), _use_count(0), 
			_parent(SmartPtr::Make(parent))
		{
			assert(data != nullptr);
		}

		explicit CDataImp(IDimensions * dimensions) : _own_memory(true), _dimensions(dimensions), _use_count(0)
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

		virtual void * GetData()
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
		~CDataImp()
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
		CDimensionsImpl _dimensions;
//		CLocalization _localization;

		unsigned int _use_count;
		bool _own_memory;
		SmartPtr<ISharedObject> _parent;
	};

	typedef CDataImp<double> CDoubleData;
	typedef CDataImp<float> CFloatData;
	typedef CDataImp<char> CCharData;
	typedef CDataImp<std::complex<double>> CComplexDoubleData;
	typedef CDataImp<std::complex<float>> CComplexFloatData;
};

