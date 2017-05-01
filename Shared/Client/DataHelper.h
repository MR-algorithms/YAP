#ifndef DATAHELPER_H_20160813
#define DATAHELPER_H_20160813

#include "Interface/Interfaces.h"

namespace Yap
{
	class DataHelper
	{
	public:
		DataHelper(IData * data) : _data_object(*data) {}

		int GetDataType();
		unsigned int GetDimensionCount();

                /// Return the actual number of dimensions of the data.
		/**
                \remarks
                For example, if dimension information is:
                    \li Readout 0， 255；
                    \li phaseencoding 0， 255；
                    \li slice 0, 1；
                    \li channel 0，1;

                then the actual number of dimensions is 2。If we change the slice to
                0, 2 while keeping all other dimensions unchanged, then the actual
                number of dimensions is 3.
		*/
		unsigned int GetActualDimensionCount() const;

		Dimension GetDimension(DimensionType dimension_type);
		unsigned int GetWidth();
		unsigned int GetHeight();

		size_t GetDataSize() const;
		unsigned int GetBlockSize(DimensionType type) const;

		unsigned int GetSliceCount();
		unsigned int GetCoilCount();
		unsigned int GetDim4();
		IData * GetData();

	protected:
		IData & _data_object;
	};

	template <typename T>
	T * GetDataArray(IData * data)
	{
		auto * data_array = dynamic_cast<IDataArray<T> *>(data);
		return (data_array != nullptr) ? data_array->GetData() : nullptr;
	}

} // namespace Yap.


#endif
