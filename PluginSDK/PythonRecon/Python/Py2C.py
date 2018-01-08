from PIL import Image
import numpy as np


import matplotlib

"""
This is Module Py2C for c++
"""

class A: pass

class Py4C:
        def __init__(self):
                self.name = '[Class Py4C]'
                print('Call Class:',self.name)
                
        def ShowImage(self, image, width, height):
                print('Call Function: [ShowImage]')
                return image
        
        def ShowImage3D(self, image, width, height, slice):
                print('Call Function: [ShowImage3D]')
                return image


def ShowImage(image, width, height):
        img = [[]] * width
        if type(image[0]) is type(1+2j):
                for x in range(width):
                        for y in range(height):
                                img[x] = img[x] + [ image[x * width + y].real ]
        else:
                for x in range(width):
                        for y in range(height):
                                img[x] = img[x] + [image[x*width + y]]
        npimg = np.array(img)
        npimg = npimg / npimg.max() *255
        pil_image = Image.fromarray(npimg)
        pil_image.show()
        width = len(img)
        height = len(img[0])
        print('Invoking Method: [ShowImage]')
        return [image,width,height]

def ShowComplexImage(image, width, height):
        img = [[]] * width
        if type(image[0]) is type(1+2j):
                for x in range(width):
                        for y in range(height):
                                img[x] = img[x] + [ image[x * width + y].real ]
        print(img)
        width = len(img)
        height = len(img[0])
        return [image, width, height]

def ShowCharImage(image, width, height):
        print(image[0],image[width*height-1])
        return [image, width, height]

def doNothing():
        print('Invoking Method: [doNothing]. And return hello world!')
        return 'hello world!'
		
## image is 2d list
def ShowImage2D(image, width, height):
        pil_image = Image.fromarray(np.array(image))
        pil_image2 = Image.fromarray(np.array(image)*2)
        pil_image.show()
        pil_image2.show()
        return np.array(image)

def ShowImage3d(image,width,height,slice):
        print('Invoking Method: [ShowImage3d].')
        return [image, width, height, slice]


if __name__=='__main__':
        width = 256
        height = 256
        li = [i for i in range(width*height)]
        image = ShowImage(li, width, height)
        #         print('############################################################')
        #         doNothing()
        #         width = 256
        #         height = 256
        #         #li2d = [[i for j in range(height)] for i in range(width)] # *width+j)*255/(width*height)
        #         #image2d = ShowImage2D(li2d,width, height)
        #         p = Py4C()
        #         p.ShowImage3D(image, width, height, 1)
        #         print('############################################################')
