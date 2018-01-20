from PIL import Image
import numpy as np
import matplotlib

"""
This is Module Py2C for c++
"""

def ShowImage(image, width, height):
    print('Invoking Method: [ShowImage]')
    if type(image[0]) is type(1+2j):
         img = image.real
         img_img = image.imag
    else:
        img = image
    if (len(img_img[0]) != 0):
        npimg = np.array(img)
        npimg = npimg / npimg.max() *255
        pil_image = Image.fromarray(npimg)
        pil_image.show()
    npimg = np.array(img)
    npimg = npimg / npimg.max() *255
    pil_image = Image.fromarray(npimg)
    pil_image.show()
    width = len(img)
    height = len(img[0])
    return [image,width,height]


## image is 2d list
def ShowImage2d(image, width, height):
        print('Invoking Method: [ShowImage2d].')
        print(len(image))
        print(len(image[0]))
        pil_image = Image.fromarray(np.array(image))
        pil_image2 = Image.fromarray(np.array(image)*2)
        pil_image.show()
        pil_image2.show()
        # image = image.tolist()
        return list([image, width, height])

## image is 3d list
def ShowImage3d(image,width,height,slice):
        print('Invoking Method: [ShowImage3d].')
        print(len(image))
        print(len(image[0]))
        print(len(image[0][0]))
        npimage = np.array(image)
        print(np.size(npimage,0))
        print(np.size(npimage,1))
        print(np.size(npimage,2))
        print(npimage[width-1][height-1][slice-1])
        return [image, width, height, slice]

## image is 4d list
def ShowImage4d(image,width,height,slice,time):
        print('Invoking Method: [ShowImage4d].')
        npdata = np.array(image)
        print(np.size(npdata,0))
        print(np.size(npdata,1))
        print(np.size(npdata,2))
        print(np.size(npdata,3))
        print(len(image))
        print(len(image[0]) )
        print(len(image[0][0]) )
        print(len(image[0][0][0]) )
        for x in [0,1]:
            pil_image = Image.fromarray(np.array(image[0][x]))
            pil_image.show()
        return [image, width, height, slice, time]

## test for 1d
def test1d(image, width):
        # print('Invoking Method: [test2d].')
        # print('\tpython rang: ',image[0][0],'~', image[height-1][width-1],'\t\t\t\t',width,height)
        return [1, image, width]

## test for 2d
def test2d(image, width, height):
        print('Invoking Method: [test2d].')
        # print('\tpython rang: ',image[0][0],'~', image[height-1][width-1],'\t\t\t\t',width,height)
        pil_image = Image.fromarray(np.array(image))
        pil_image.show()
        return [2, image, width, height]

## test for 3d
def test3d(image, width, height, slice):
        print('Invoking Method: [test3d].')
        # print('\tpython rang: ',image[0][0][0],'~', image[slice-1][height-1][width-1],'\t\t\t\t',width,height,slice)
        return [3, image, width, height, slice]

## test for 4d
def test4d(image,width,height, slice, time):
        # print('Invoking Method: [test4d].')
        # print('\tpython rang: ',image[0][0][0][0],'~', image[time-1][slice-1][height-1][width-1],'\t\t\t\t',width,height,slice,time)
        return [4, image, width, height, slice, time]

## test for input 3d, output 2d
def test3d2d(image, width, height, slice):
        image = np.array(image)[0].tolist()
		pil_image = Image.fromarray(np.array(image))
        pil_image.show()
        return [2, image, width, height]

def test3d2image(image1,image2, width, height, slice):
        return [3, image1, width, height, slice]

'''
# if __name__=='__main__':
#         print('############################################################')
#         #li2d = [[i for j in range(height)] for i in range(width)] # *width+j)*255/(width*height)
#         #image2d = ShowImage2D(li2d,width, height)
#         print('############################################################')
'''
