import numpy as np
from PIL import Image
from radiomics import featureextractor
import sys
import nipy
import SimpleITK as sitk


def test3d_2input(image1, image2, width,height,slice):
    print('Invoking Method: [test3d_2input].')
    for x in [int(slice/2),int(slice/2)+1]:
            pil_image = Image.fromarray(np.array(image1[x]))
            pil_image.show()
            pil_image2 = Image.fromarray(np.array(image2[x]))
            pil_image2.show()
    return [3, image1,width,height,slice]


def test_radiomics(image, roi, width, height, slice):
    print('Invoking Method: [test_radiomics].')
    image1 = sitk.GetImageFromArray(np.array(image))
    image2 = sitk.GetImageFromArray(np.array(roi))
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
    features = extractor.execute(image1,image2)
    for fn in features.keys():
        print("Compute %s : %s" %(fn,features[fn]))
    print('Image size: [ %d, %d, %d]',width, height, slice)
    images = np.array(image)[0].tolist()
    pil_image = Image.fromarray(np.array(images))
    pil_image.show()
    print('Resize Image: [ %d, %d]',len(images),len(images[0]))
    return [ 2, image, width, height ]
