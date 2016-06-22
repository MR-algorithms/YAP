#pragma once

#include "..\Interface\Interface.h"
#include <complex>
#include <vector>
// #include "Localization.h"
#include "..\interface\SmartPtr.h"
#include <assert.h>

namespace Yap
{
	struct Dimension
	{
		DimensionType type;
		unsigned int start_index;
		unsigned int length;
		Dimension();
		Dimension(DimensionType type, unsigned start_index, unsigned int length);
	};

	class CDimensions : public IDimensions
	{
	public:
		CDimensions();
		CDimensions(unsigned int dimension_count);
		CDimensions(const CDimensions& source);
		CDimensions(IDimensions * source);

		bool ModifyDimension(DimensionType type, unsigned int length, unsigned int start_index = 0);
		unsigned int GetLength(unsigned int dimension_index);

		virtual unsigned int GetDimensionCount();
		virtual bool GetDimensionInfo(unsigned int dimension_index, DimensionType & dimension_type, unsigned int& start_index, unsigned int& length);
		virtual bool SetDimensionInfo(unsigned int dimension_index, DimensionType dimension_type, 
			unsigned int start_index, unsigned int length);

		virtual IDimensions * Clone();
		virtual void Release();
		CDimensions & operator() (DimensionType type, unsigned int index, unsigned int length);
	private:
		std::vector<Dimension> _dimension_info;
	};

	template<typename T, DataType DATA_TYPE>
	class CData : public IData, public IMemory
	{
	public:
		CData(T* data, const CDimensions& dimensions, IMemory * parent = nullptr, bool own_data = false) :
			_data(data), _own_memory(own_data), _use_count(0), _parent(parent)
		{
			assert(data != nullptr);
			_dimensions = dimensions;
		}
		
		CData(T* data, IDimensions * dimensions, IMemory * parent = nullptr, bool own_data = false) : 
			_data(data), _own_memory(own_data), _dimensions(dimensions), _use_count(0), _parent(parent)
		{
			assert(data != nullptr);
		}

		CData(IDimensions * dimensions) : _own_memory(true), _dimensions(dimensions), _use_count(0)
		{
			unsigned int total_size = 1;

			for (unsigned int i = 0; i < _dimensions.GetDimensionCount(); ++i)
			{
				total_size *= _dimensions.GetLength(i);
			}

			_data = new T[total_size];
		}


// 		void SetLocalization(CLocalization& localization)
// 		{
// 			_localization = localization;
// 		}
// 
// 		void SetSliceLocalization(IParams* param, unsigned int image_index) // index为image_index, 初步设计
// 		{
// 			_localization = C2dLocalization(param, image_index);
// 		}

// 		void SetSlabLocalization(IParams* param, unsigned int slab_index)
// 		{
// 			_localization = C3dLocalization(param, slab_index);
// 		}
// 
// 		virtual ILocalization * GetLocalization()
// 		{
// 			return &_localization;
// 		}

		virtual IDimensions * GetDimensions()
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
			assert(_use_count > 0);
			--_use_count;

			if (_parent)
			{
				_parent->Release();
			}

			if (_use_count == 0)
			{
				delete this;
			}
		}

	private:
		~CData()
		{
			if (_own_memory)
			{
				delete []_data;
			}
		}

		virtual DataType GetDataType() override
		{
			return DATA_TYPE;
		}

		virtual IDimensions * GetDimension() override
		{
			return &_dimensions;
		}


		T * _data;
		CDimensions _dimensions;
//		CLocalization _localization;

		unsigned int _use_count;
		bool _own_memory;
		CSmartPtr<IMemory> _parent;
	};

	typedef CData<double, DataTypeDouble> CDoubleData;
<<<<<<< HEAD
	typedef CData<std::complex<double>, DataTypeComplexDouble> CComplexDouble;
=======
	typedef CData<std::complex<double>, DataTypeComplexDouble> CComplexDoubleData;
>>>>>>> refs/remotes/MR-algorithms/master
};

