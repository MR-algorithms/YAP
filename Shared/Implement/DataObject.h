#pragma once

#include <complex>
#include <vector>
#include "Interface/Interfaces.h"
#include <assert.h>

namespace Yap
{

	class Dimensions : 
		public IDimensions
	{
		IMPLEMENT_SHARED(Dimensions)
	public:
		Dimensions();
		explicit Dimensions(unsigned int dimension_count);
		Dimensions(const Dimensions& source);
		explicit Dimensions(IDimensions * source);

		~Dimensions();

	public:

		bool SetDimension(DimensionType type, unsigned int length, unsigned int start_index = 0);
		unsigned int GetLength(unsigned int dimension_index);

		virtual unsigned int GetDimensionCount() override;

		virtual bool GetDimensionInfo(unsigned int dimension_index,
			DimensionType & dimension_type, unsigned int& start_index, unsigned int& length) override;

		virtual bool SetDimensionInfo(unsigned int dimension_index, DimensionType dimension_type, 
			unsigned int start_index, unsigned int length);

		Dimensions & operator() (DimensionType type, unsigned int index, unsigned int length);
	private:
		std::vector<Dimension> _dimension_info;
	};

	struct Vector
	{
		double x;
		double y;
		double z;
		Vector() : x{ 0 }, y{ 0 }, z{ 0 } {}
		Vector(double x_, double y_, double z_) : x{ x_ }, y{ y_ }, z{ z_ } {}
		bool operator == (const Vector& rhs) const {
			return (x == rhs.x && y == rhs.y && z == rhs.z);
		}
	};

	struct Point
	{
		double x;
		double y;
		double z;
		Point() : x{ 0 }, y{ 0 }, z{ 0 } {};
		Point(double x_, double y_, double z_) : x{ x_ }, y{ y_ }, z{ z_ } {};
		bool operator == (const Point& rhs) const {
			return (x == rhs.x && y == rhs.y && z == rhs.z);
		};
	};

	class Localization :
		public IGeometry
	{
		IMPLEMENT_SHARED(Localization)
	public:
		Localization();
		Localization(const Localization& source);
		explicit Localization(IGeometry * source);

		~Localization();

		virtual void GetSpacing(double& x, double& y, double& z) override;
		virtual void GetRowVector(double& x, double& y, double& z) override;
		virtual void GetColumnVector(double& x, double& y, double& z) override;
		virtual void GetSliceVector(double& x, double& y, double& z) override;
		virtual void GetReadoutVector(double& x, double& y, double& z) override;
		virtual void GetPhaseEncodingVector(double& x, double& y, double& z) override;
		virtual void GetCenter(double& x, double& y, double& z) override;

		virtual void SetSpacing(double x, double y, double z) override;
		virtual void SetRowVector(double x, double y, double z) override;
		virtual void SetColumnVector(double x, double y, double z) override;
		virtual void SetSliceVector(double x, double y, double z) override;
		virtual void SetReadoutVector(double x, double y, double z) override;
		virtual void SetPhaseEncodingVector(double x, double y, double z) override;
		virtual void SetCenter(double x, double y, double z) override;

		virtual bool IsValid() override;

	private:
		double _spacing_x;
		double _spacing_y;
		double _spacing_z;
		Vector _row;
		Vector _column;
		Vector _slice;
		Vector _readout;
		Vector _phase_encoding;
		Point _center;
	};

	template<typename T>
	class DataObject : 
		public IDataArray<T>
	{
		IMPLEMENT_CLONE(DataObject<T>)
        public:
		DataObject(T* data, const Dimensions& dimensions, 
			ISharedObject * parent = nullptr, bool own_data = false, ISharedObject * module = nullptr) :
			_data(data), _own_memory(own_data), _use_count(0), 
			_parent(YapShared(parent)),
			_module(YapShared(module)),
			_geometry(YapShared<Localization>(nullptr))
		{
			assert(data != nullptr);
			_dimensions = dimensions;
		}
		
		DataObject(T* data, IDimensions * dimensions, ISharedObject * parent = nullptr, 
			bool own_data = false, ISharedObject * module = nullptr) : 
			_data(data), _own_memory(own_data), _dimensions(dimensions), _use_count(0), 
			_parent(YapShared(parent)),
			_module(YapShared(module)),
			_geometry(YapShared<Localization>(nullptr))
		{
			assert(data != nullptr);
		}

		DataObject(IDimensions * dimensions, ISharedObject * module) :
			_own_memory(true), 
			_dimensions(dimensions), 
			_use_count(0),
			_module(YapShared(module)),
			_geometry(YapShared<Localization>(nullptr))
		{
			unsigned int total_size = 1;

			for (unsigned int i = 0; i < _dimensions.GetDimensionCount(); ++i)
			{
				total_size *= _dimensions.GetLength(i);
			}

			_data = new T[total_size];
		}

		/// Copy constructor
		/**
			\remarks  Deep copy a data object. The new object will own the copied data
			even if the rhs does not own its data.
		*/
		DataObject(const DataObject& rhs, ISharedObject * module = nullptr) : 
			_own_memory{true}, _dimensions{rhs._dimensions}, _use_count(0),
			_module(YapShared(module)), _geometry(rhs._geometry)
		{
			unsigned int total_size = 1;

			for (unsigned int i = 0; i < _dimensions.GetDimensionCount(); ++i)
			{
				total_size *= _dimensions.GetLength(i);
			}

			_data = new T[total_size];
			memcpy(_data, rhs._data, total_size * sizeof(T));
		}

	public:
		virtual IDimensions * GetDimensions() override
		{
			return &_dimensions;
		}

		virtual T * GetData() override
		{
			return _data;
		}

		virtual void Lock() override
		{
			++_use_count;
		}

		virtual void Release() override
		{
			assert(_use_count > 0 && "Logic error. Forget to Lock()?");

			if (_use_count == 0 || --_use_count == 0)
			{
				delete this;
			}
		}

		bool SetGeometry(IGeometry * geometry)
		{
			if (!geometry)
				return false;

			_geometry = YapShared(geometry);
			return true;
		}

		IGeometry * GetGeometry()
		{
			return _geometry.get();
		}

	private:
		~DataObject()
		{
			if (_own_memory)
			{
				delete []_data;
				_data = nullptr;
			}
		}

		virtual int GetDataType() override
		{
			return data_type_id<T>::type;
		}

		T * _data;
		Dimensions _dimensions;
//		CLocalization _localization;

		unsigned int _use_count;
		bool _own_memory;
		SmartPtr<ISharedObject> _parent;
		SmartPtr<ISharedObject> _module;

		SmartPtr<Localization> _geometry;

		friend class ProcessorImpl;
	};

	typedef DataObject<int> IntData;
	typedef DataObject<double> DoubleData;
	typedef DataObject<float> FloatData;
	typedef DataObject<char> CharData;
	typedef DataObject<std::complex<double>> ComplexDoubleData;
	typedef DataObject<std::complex<float>> ComplexFloatData;
	typedef DataObject<unsigned short> UnsignedShortData;
};
