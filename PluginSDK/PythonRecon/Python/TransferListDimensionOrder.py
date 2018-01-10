'''
modify list order
from time -> slice -> height -> width
to width -> height -> slice -> time
'''
import numpy as np
import matplotlib as ptl
from PIL import Image
import SimpleITK as itk


def ReverseOrder2d(image,width,height):
        reverse_image = [];
        for x in range(width):
            reverse_image.append([])
            for y in range(height):
                reverse_image[x].append(image[y][x]) 
        return reverse_image


def ReverseOrder3d(image,width,height,slice):
        reverse_image = []
        for w in range(width):
                reverse_image.append([])
                for h in range(height):
                        reverse_image[w].append([])
                        for s in range(slice):
                                reverse_image[w][h].append(image[s][h][w])
        return reverse_image


def ReverseOrder4d(image, width, height, slice, time):
        reverse_image = []
        for w in range(width):
                reverse_image.append([])
                for h in range(height):
                        reverse_image[w].append([])
                        for s in range(slice):
                                reverse_image[w][h].append([])
                                for t in range(time):
                                        reverse_image[w][h][s].append(image[t][s][h][w])
        return reverse_image

