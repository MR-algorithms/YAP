from PIL import Image
import numpy as np
import matplotlib


"""
This is Module Py2C for C++
"""

def ShowImage(image, width, height):
    img = [[]] * width
    if type(image[0]) is type(1+2j):
        for x in range(width):
            for y in range(height):
                img[x] = img[x] + [image[x * width + y].real]
    else:
        for x in range(width):
            for y in range(height):
                img[x] = img[x] + [image[x*width + y]]
    print('Invoking Method: [ShowImage]')
    print('Input image size: ',width,' * ',height)
    print('Input image begin & end: ',image[0],' ~ ',image[width*height-1])
    width = len(img)
    height = len(img[0])
    print('Output image size: ',width,' * ', height)
    return [image,width,height]


def ShowImageResize(image, width, height):
    img = [[]] * (width+1)
    if type(image[0]) is type(1+2j):
        for x in range(width):
            for y in range(height):
                img[x] = img[x] + [image[x * width + y].real]
            img[x].append(x)
    else:
        for x in range(width):
            for y in range(height):
                img[x] = img[x] + [image[x*width + y]]
            img[x].append(x)
    width = len(img)
    height = len(img[0])
    print('Invoking Method: [ShowImageResize]')
    return [image,width,height]

def ShowImage3D(image, width, height, slice):
    img = [ [ []*slice ] * height ] * width
    if type(image[0]) is type(1+2j):
        for x in range(width):
            for y in range(height):
                img[x] = img[x] + [image[x * width + y].real]
            img[x].append(x)
    else:
        for x in range(width):
            for y in range(height):
                img[x] = img[x] + [image[x*width + y]]
            img[x].append(x)
    width = len(img)
    height = len(img[0])
    print('Invoking Method: [ShowImageResize]')
    return [image,width,height]


    
# if __name__=='__main__':
#         print('############################################################')
#         doNothing()
#         width = 256
#         height = 256
#         li = [i for i in range(width*height)]
#         image = ShowImage(li, width, height)
#         #li2d = [[i for j in range(height)] for i in range(width)] # *width+j)*255/(width*height)
#         #image2d = ShowImage2D(li2d,width, height)
#         p = Py4C()
#         p.ShowImage3D(image, width, height, 1)
#         print('############################################################')
