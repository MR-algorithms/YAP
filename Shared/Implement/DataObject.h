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
		Localization(IGeometry * source);

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
		DataObject(IData * reference, T* data, const Dimensions& dimensions,
			ISharedObject * parent = nullptr,
			ISharedObject * module = nullptr) :
			_data(data),
			_parent(YapShared(parent)),
			_module(YapShared(module)),
			_geometry{ YapShared(reference != nullptr ? new Localization(reference->GetGeometry()) : nullptr) },
			_variables{ YapShared(reference != nullptr ? reference->GetVariables() : nullptr) }
		{
			assert(data != nullptr);
			_dimensions = dimensions;
		}

		DataObject(IData * reference, T * data, IDimensions * dimensions,
			ISharedObject * parent = nullptr,
			ISharedObject * module = nullptr) :
			_data(data),
			_dimensions(dimensions),
			_parent(YapShared(parent)),
			_module(YapShared(module)),
			_geometry{ YapShared(reference != nullptr ? new Localization(reference->GetGeometry()) : nullptr) },
			_variables{ YapShared(reference != nullptr ? reference->GetVariables() : nullptr) }
		{
			assert(data != nullptr);
		}

		DataObject(IData * reference, IDimensions * dimensions, ISharedObject * module) :
			_dimensions(dimensions),
			_module(module),
			_geometry{ reference != nullptr ? new Localization(reference->GetGeometry()) : nullptr },
			_variables{ reference != nullptr ? reference->GetVariables() : nullptr }
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
			_dimensions{ rhs._dimensions },
			_module{ module },
			_geometry{ rhs._geometry },
			_variables{ rhs._variables }
		{
			unsigned int total_size = 1;

			for (unsigned int i = 0; i < _dimensions.GetDimensionCount(); ++i)
			{
				total_size *= _dimensions.GetLength(i);
			}

			_data = new T[total_size];
			memcpy(_data, rhs._data, total_size * sizeof(T));
		}

		DataObject(IVariableContainer * variables, ISharedObject * module) :
			_dimensions {},
			_variables{YapShared(variables)},
			_module{YapShared(module)}
		{
			assert(variables != nullptr);
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

		IGeometry * GetGeometry() override
		{
			return _geometry.get();
		}

		bool SetVariables(IVariableContainer * variables)
		{
			if (!variables)
				return false;

			_variables = YapShared(variables);
			return true;
		}

		IVariableContainer * GetVariables() override
		{
			return _variables.get();
		}

		static SmartPtr<DataObject<int>> CreateVariableObject(IVariableContainer * variable_space,
			ISharedObject * module = nullptr)
		{
			try 
			{
				return YapShared(new DataObject<int>(variable_space, module));
			}
			catch (std::bad_alloc&)
			{
				return YapShared<DataObject<T>>(nullptr);
			}
		}

		static SmartPtr<DataObject<T>> Create(IData * reference, T* data, const Dimensions& dimensions,
			ISharedObject * parent = nullptr, ISharedObject * module = nullptr)
		{
			try
			{
				return YapShared(new DataObject<T>(reference, data, dimensions, parent, module));
			}
			catch (std::bad_alloc&)
			{
				return YapShared<DataObject<T>>(nullptr);
			}
		}

		static SmartPtr<DataObject<T>> Create(IData * reference, T* data, IDimensions * dimensions,
			ISharedObject * parent = nullptr, ISharedObject * module = nullptr)
		{
			try
			{
				return YapShared(new DataObject<T>(reference, data, dimensions, parent, module));
			}
			catch (std::bad_alloc&)
			{
				return YapShared<DataObject<T>>(nullptr);
			}
		}

		static SmartPtr<DataObject<T>> Create(IData * reference, IDimensions * dimensions = nullptr,
			ISharedObject * module = nullptr)
		{
			try
			{
				return YapShared<DataObject<T>>(new DataObject<T>(reference,
					(dimensions != nullptr) ? dimensions : ((reference != nullptr) ? reference->GetDimensions() : nullptr),
					module));
			}
			catch (std::bad_alloc&)
			{
				return YapShared<DataObject<T>>(nullptr);
			}
		}

		/**
		\remarks  Deep copy a data object. The new object will own the copied data
		even if the rhs does not own its data.
		*/
		static SmartPtr<DataObject<T>> Create(const DataObject<T>& rhs, ISharedObject * module)
		{
			try
			{
				return YapShared(new DataObject<T>(rhs, module));
			}
			catch (std::bad_alloc&)
			{
				return YapShared<DataObject<T>>(nullptr);
			}
		}

	private:
		~DataObject()
		{
			if (!_parent)
			{
				delete[]_data;
				_data = nullptr;
			}
		}

		virtual int GetDataType() override
		{
			return data_type_id<T>::type;
		}

		T * _data = nullptr;
		Dimensions _dimensions;

		unsigned int _use_count = 0;

		SmartPtr<ISharedObject> _parent;	// default to null pointer
		SmartPtr<ISharedObject> _module;	// default to null pointer

		SmartPtr<Localization> _geometry;
		SmartPtr<IVariableContainer> _variables;

		friend class ProcessorImpl;
	};

	typedef DataObject<int> IntData;
	typedef DataObject<double> DoubleData;
	typedef DataObject<float> FloatData;
	typedef DataObject<char> CharData;
	typedef DataObject<unsigned char> UCharData;
	typedef DataObject<std::complex<double>> ComplexDoubleData;
	typedef DataObject<std::complex<float>> ComplexFloatData;
	typedef DataObject<unsigned short> UnsignedShortData;
};
