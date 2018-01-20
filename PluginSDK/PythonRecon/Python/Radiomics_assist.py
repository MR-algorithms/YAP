import glob
import os


def ReplaceSlash(paths):
    for path_list_name in paths:
        index = 0
        if type(paths[path_list_name]) == list:
            for item in paths[path_list_name]:
                paths[path_list_name][index] = str(item.replace('/', '\\'))
                index += 1
    return paths


def ExtractFiles(folder_path, keys):
    paths = {}
    paths['t2_path'] = glob.glob(folder_path + keys['T2'])
    paths['adc_path'] = glob.glob(folder_path + keys['ADC'])
    dwi_path = glob.glob(folder_path + keys['DWI'])
    paths['dwi_path'] = [x for x in dwi_path if not (x in paths['adc_path'])]
    paths['ca_path'] = glob.glob(folder_path + keys['malignant'])
    non_ca_path = glob.glob(folder_path + keys['allROI'])
    paths['non_ca_path']= [x for x in non_ca_path if not (x in paths['ca_path'])]
    return ReplaceSlash(paths)


def ShowResults(result):
    for x in result.items():
        print(x)

def CatchFileName(file_path):
    files = file_path.split('\\')
    command = 'echo ' + files[-1] + '| clip'
    os.system(command)
    return files[-1]

