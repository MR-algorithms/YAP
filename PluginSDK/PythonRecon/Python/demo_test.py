import numpy as np
from PIL import Image
from radiomics import featureextractor
import sys
import nipy
import SimpleITK as sitk

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
    
def test_radiomics(image, roi, width, height, slice):
    print('Invoking Method: [test_radiomics].')
    image1 = sitk.GetImageFromArray(np.array(image))
    image2 = sitk.GetImageFromArray(np.array(roi))
    features = extractor.execute(image1,image2)
    features.pop('general_info_BoundingBox');
    features.pop('general_info_EnabledImageTypes');
    features.pop('general_info_GeneralSettings');
    features.pop('general_info_ImageHash');
    features.pop('general_info_ImageSpacing');
    features.pop('general_info_MaskHash');
    features.pop('general_info_Version');
    # for fn in features.keys():
    #    print("Compute %s : %s" %(fn,features[fn]))
    f_value = list(features.values())
    f_size = len(f_value)
    return [1, f_value, f_size]
    # print('Image size: ',width, height, slice)
    # print('Image rank: ',np.min(np.array(image)),np.max(np.array(image)))
    # images = np.array(image)[40].tolist()
    # pil_image = Image.fromarray(np.array(images))
    # pil_image.show()
    # print('Resize Image: ',len(images),len(images[0]))
    # return [ 2, images, width, height ]

