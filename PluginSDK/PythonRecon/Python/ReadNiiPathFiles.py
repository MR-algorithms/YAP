"""
return the names of path included files and the
included-dir files' name.

"""

import os


def walk_dir(paths,topdown=True):
    files_name = []
    for root, dirs, files in os.walk(paths, topdown):
        for name in files:
            files_name.append(os.path.join(root,name))
    return files_name


if __name__ == "__main__":
    dir = input('please input the path:')
    dir  = r'D:\数据\MICCAI_BraTS17_Data_Training_for_TGuo_2017modified\Brats17TrainingData\HGG'
    files = walk_dir(dir)
