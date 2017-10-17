from PIL import Image
import numpy as np


import matplotlib

"""
This is Module Py2C for c++
"""

class A: pass

class B: pass

def ShowImage(image, width, height):
        img = [[]] * width
        for x in range(width):
                for y in range(height):
                        img[x] = img[x] + [image[x*width + y]]
        npimg = np.array(img)
        npimg = npimg / npimg.max() *255
        pil_image = Image.fromarray(npimg)
        pil_image.show()
        return 'success!'

## image is 2d list
def ShowImage2D(image, width, height):
        pil_image = Image.fromarray(np.array(image))
        pil_image2 = Image.fromarray(np.array(image)*2)
        pil_image.show()
        pil_image2.show()
        return np.array(image)

if __name__=='__main__':
        width = 256
        height = 256
        li = [i for i in range(width*height)]
        image = ShowImage(li, width, height)
        li2d = [[i for j in range(height)] for i in range(width)] # *width+j)*255/(width*height)
        image2d = ShowImage2D(li2d,width, height)
