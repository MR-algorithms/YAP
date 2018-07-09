import numpy as np
from PIL import Image
from radiomics import featureextractor
import sys
import nipy
import SimpleITK as sitk
import xlwt



radiomics_parameter_path = r'C:\projects\pyradiomics\examples\exampleSettings\Params.yaml'
extractor = featureextractor.RadiomicsFeaturesExtractor(radiomics_parameter_path)
extractor.disableAllFeatures()
extractor.enableFeatureClassByName('glcm')
extractor.enableFeatureClassByName('glrlm')
extractor.enableFeatureClassByName('glszm')
extractor.enableFeatureClassByName('gldm')
extractor.enableFeatureClassByName('ngtdm')
extractor.enableFeatureClassByName('firstorder')
extractor.enableFeatureClassByName('shape')

workbook = xlwt.Workbook(encoding='utf-8')
booksheet = workbook.add_sheet('features', cell_overwrite_ok=True)
	
def extract_feature(image, roi, width, height, slice):
    print('Invoking Method: [test_radiomics].')
    image1 = sitk.GetImageFromArray(np.array(image))
    nproi = np.array(roi)
    numb = np.unique(nproi)
    nproi[nproi!=0] = 1
    # if numb.size==4:        # some data roi label-4 for tumour, no label-4, then label-2 for tumour
        # nproi[nproi==2] = 0
        # nproi[nproi==4] = 1
    # if numb.size==3:
        # nproi[nproi==1] = 0
        # nproi[nproi==2] = 1
    image2 = sitk.GetImageFromArray(nproi)
    features = extractor.execute(image1,image2)
    features.pop('general_info_BoundingBox');
    features.pop('general_info_EnabledImageTypes');
    features.pop('general_info_GeneralSettings');
    features.pop('general_info_ImageHash');
    features.pop('general_info_ImageSpacing');
    features.pop('general_info_MaskHash');
    features.pop('general_info_Version');
    f_value = list(features.values())
    f_name = list(features.keys())
    for i,j in enumerate(f_name):
        booksheet.write(0,i,j)
    workbook.save('test.xls')
    f_size = len(f_value)
    return [1, f_value, f_size]
    # pil_image = Image.fromarray(np.array(images))
    # pil_image.show()

