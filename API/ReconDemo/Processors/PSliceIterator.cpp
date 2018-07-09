#include "PSliceIterator.h"

#include "Implement/LogUserImpl.h"
#include "Implement/DataObject.h"
#include "Client/DataHelper.h"

using namespace Yap;
using namespace std;

PSliceIterator::PSliceIterator() :
    ProcessorImpl(L"PSliceIterator")
{
   // LOG_TRACE(L"PSliceIterator constructor called.", L"PSliceIterator");

    AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeComplexFloat);
    AddOutput(L"Output", 2, DataTypeComplexFloat);
}

PSliceIterator::PSliceIterator(const PSliceIterator &rhs) :
    ProcessorImpl(rhs)
{
    LOG_TRACE(L"PSliceIterator constructor called.", L"NiuMri");
}

PSliceIterator::~PSliceIterator()
{
    //    LOG_TRACE(L"PSliceIterator destructor called.", L"NiuMri");
}

bool PSliceIterator::Input(const wchar_t *name, IData *data)
{
    if (data == nullptr && _wcsicmp(name, L"Input") != 0)
        return false;
    if (data->GetDataType() != DataTypeComplexFloat)
        return false;

    DataHelper helper(data);
    auto slice_size = helper.GetSliceCount();
    assert(slice_size == 3);

    unsigned int slice_block_size = helper.GetBlockSize(DimensionSlice);
    Dimension slice_dimension = helper.GetDimension(DimensionSlice);
    assert(slice_dimension.type == DimensionSlice);

    unsigned int start_index = slice_dimension.start_index;

    // 定位像第一幅图片

    Dimensions slice_data_dimensions1(data->GetDimensions());
    slice_data_dimensions1.SetDimension(DimensionSlice, 1, start_index);

    auto slice1 = CreateData<complex<float>>(data,
                                             Yap::GetDataArray<complex<float>>(data) + start_index * slice_block_size,
                                             slice_data_dimensions1,
                                             data);
    auto slice1_geometry = YapShared(new Localization());
    slice1_geometry->SetReadoutVector(0.0, 1.0, 0.0);
    slice1_geometry->SetPhaseEncodingVector(0.0, 0.0, 1.0);
    slice1_geometry->SetSliceVector(-1.0, 0.0, 0.0);
    slice1_geometry->SetRowVector(0.0, 1.0, 0.0);
    slice1_geometry->SetColumnVector(0.0, 0.0, 1.0);
    slice1_geometry->SetCenter(0.0, 0.0, 0.0);
    slice1->SetGeometry(slice1_geometry.get());
    Feed(L"Output", slice1.get());

    //定位像第二幅图片
    Dimensions slice_data_dimensions2(data->GetDimensions());
    slice_data_dimensions2.SetDimension(DimensionSlice, 1, start_index + 1);

    auto slice2 = CreateData<complex<float>>(data,
                                             Yap::GetDataArray<complex<float>>(data) + (start_index + 1) * slice_block_size,
                                             slice_data_dimensions2,
                                             data);
    auto slice2_geometry = YapShared(new Localization());
    slice2_geometry->SetReadoutVector(1.0, 0.0, 0.0);
    slice2_geometry->SetPhaseEncodingVector(0.0, -1.0, 0.0);
    slice2_geometry->SetSliceVector(0.0, 0.0, 1.0);
    slice2_geometry->SetRowVector(1.0, 0.0, 0.0);
    slice2_geometry->SetColumnVector(0.0, -1.0, 0.0);
    slice2_geometry->SetCenter(0.0, 0.0, 0.0);
    slice2->SetGeometry(slice2_geometry.get());
    Feed(L"Output", slice2.get());

    //定位像第三幅图片
    Dimensions slice_data_dimensions3(data->GetDimensions());
    slice_data_dimensions3.SetDimension(DimensionSlice, 1, start_index + 2);

    auto slice3 = CreateData<complex<float>>(data,
                                             Yap::GetDataArray<complex<float>>(data) + (start_index + 2) * slice_block_size,
                                             slice_data_dimensions3,
                                             data);
    auto slice3_geometry = YapShared(new Localization());
    slice3_geometry->SetReadoutVector(1.0, 0.0, 0.0);
    slice3_geometry->SetPhaseEncodingVector(0.0, 0.0, 1.0);
    slice3_geometry->SetSliceVector(0.0, 1.0, 0.0);
    slice3_geometry->SetRowVector(1.0, 0.0, 0.0);
    slice3_geometry->SetColumnVector(0.0, 0.0, 1.0);
    slice3_geometry->SetCenter(0.0, 0.0, 0.0);
    slice3->SetGeometry(slice3_geometry.get());
    Feed(L"Output", slice3.get());

    return true;
}
