from radiomics import featureextractor
from excel_helper import Save_Column_Exel, Save_Column_Title, Save_Row_Exel
import numpy as np
## import h5py
import SimpleITK as sitk
import nibabel as nib


# f_train = h5py.File(r'Z:\Radiomics_ZhangJing\ProstateX\new_data\training_2D_101x101_box_center.h5')
# f_test = h5py.File(r'Z:\Radiomics_ZhangJing\ProstateX\new_data\testing_2D_101x101_box_center_no_label.h5')
# f_val = h5py.File(r'Z:\Radiomics_ZhangJing\ProstateX\new_data\validation_2D_101x101_box_center.h5')


# extract features from data

param_path = r'C:\projects\pyradiomics\examples\exampleSettings\Params.yaml';
extractor = featureextractor.RadiomicsFeaturesExtractor(param_path)
# extractor.disableAllFeatures()
# extractor.enableFeatureClassByName('glcm')
# extractor.enableFeatureClassByName('glrlm')
# extractor.enableFeatureClassByName('glszm')
# extractor.e# nableFeatureClassByName('gldm')
# extractor.enableFeatureClassByName('ngtdm')
# # extractor.enableFeatureClassByName('firstorder')
# extractor.enableFeatureClassByName('shape')

root = 'D:\\test_data\\MR0309928\\'
image_roi_name = [(r'3 T2_tra.nii', r'3T2_tra_seg.nii'),
                  (r'4 t1_fl2d_tra.nii', r'4t1_fl2d_tra_seg.nii'),
                  (r'5 T2_FLAIR_tra.nii', r'5T2_FLAIR_tra_seg.nii'),
                  (r'7 resolve_4scan_trace_tra_p2_192_ADC.nii', r'7resolve_4scan_trace_tra_p2_192_ADC_seg.nii'),
                  (r'12 ep2d_diff_mddw_20_p2_FA.nii', r'12ep2d_diff_mddw_20_p2_FA_seg.nii'),
                  (r'15 Mag_Images.nii', r'15Mag_Images_seg.nii'),
                  (r'16 Pha_Images.nii', r'16Pha_Images_seg.nii'),
                  (r'17 mIP_Images(SW).nii', r'17mIP_Images(SW)_seg.nii'),
                  (r'18 SWI_Images.nii', r'18SWI_Images_seg.nii'),
                  (r'23 T1_Images_B1corr.nii', r'23T1_Images_B1corr_seg.nii'),
                  (r'60 t1_fl2d_tra.nii', r'60t1_fl2d_tra_seg.nii')]

image_name = {x[1]: x[0] for x in image_roi_name}
set_title = True
column_start = 1
row_start = 2
for img_n, roi_n in image_roi_name:
    img = nib.load(root + img_n)
    img = sitk.GetImageFromArray(np.array(img.get_data(), np.float32))
    roi = nib.load(root + roi_n)
    roi = sitk.GetImageFromArray(np.array(roi.get_data(), np.uint16))
    feature = extractor.execute(img, roi)
    if set_title:
        if Save_Column_Title(r'D:\test_data\test_result.xlsx', feature, 2, 1) | Save_Row_Exel(r'D:\test_data\test_result.xlsx', image_name, 1, 2) is False:
            assert (0 & r'error')
        set_title = False
    column_start += 1
    if Save_Column_Exel(r'D:\test_data\test_result.xlsx', feature, row_start, column_start) is False:
        assert 0
