import numpy as np
import xlrd
from sklearn import svm
import matplotlib.pyplot as plt
from sklearn.decomposition import PCA
from sklearn.decomposition import TruncatedSVD
from sklearn.datasets import make_friedman1
from sklearn.feature_selection import RFE,RFECV
from sklearn.svm import SVR, SVC
from sklearn.model_selection import StratifiedKFold
import time

# def isNum(value):
#     try:
#         value +1
#     except TypeError:
#         return False
#     else:
#         return True

def ExtractFeaturesByPath(file_path):
    bk_train = xlrd.open_workbook(file_path)
    try:
        sh_train = bk_train.sheet_by_name("Sheet")
    except:
        print("no sheet in %s named Sheet")
    number_features = sh_train.nrows - 1

    data = []
    # Extract features
    for i in range(number_features):
        feature_item = sh_train.row_values(i)
        feature_item.pop(0)
        # To do, check is there any non-number.
        if not all(isinstance(x, str) for x in feature_item):
            temp_feature = np.asarray(feature_item, dtype=np.float32)
            data.append(np.nan_to_num(temp_feature, 0))
    data = np.asarray(data, dtype=np.float32)
    data = data.T

    # Extract labels
    label_train_value = sh_train.row_values(number_features)
    label_train_value.pop(0)
    label = np.asarray(label_train_value, dtype=np.uint16)

    assert (label.shape[0] == data.shape[0])
    print('The number of features is {:}, the number of samples of the training data is {:}'.format(
        data.shape[1], data.shape[0]))
    return data, label

# Load data and features
training_data, training_label = ExtractFeaturesByPath(r'D:\RadiomicsProject\new new data train all features .xlsx')
validation_data, validation_label = ExtractFeaturesByPath(r'D:\RadiomicsProject\new new data val all features .xlsx')

def MaxMinNormalization(data):
    for i in range(data.shape[1]):
         for j in range(data.shape[0]):
             data[j][i] = float(1.0/(1+np.exp(-float(data[j][i]))))
    return data


def Z_normalization(data):
    new_data = np.zeros([data.shape[0], data.shape[1]])
    mu = np.average(data, axis=0)
    sigma = np.std(data, axis=0)
    for i in range(data.shape[1]):
        for j in range(data.shape[0]):
            new_data[j][i] = (data[j][i]-mu[i])/sigma[i]
    new_data = np.nan_to_num(new_data)
    return new_data

training_data = Z_normalization(training_data)
validation_data = Z_normalization(validation_data)

# Select features

# f_test = r'D:\RadiomicsProject\new data test all features .xlsx'
# bk_test = xlrd.open_workbook(f_test)
# shxrange_test = range(bk_test.nsheets)
# try:
#     sh_test = bk_test.sheet_by_name("Sheet")
# except:
#     print("no sheet in %s named Sheet")
# nrows_test = sh_test.nrows
# ncols_test = sh_test.ncols
# print("nrows %d, ncols %d"%(nrows_test, ncols_test))
# row_list_test = []
# row_number_test = 0
# for i in range(570):
#     row_data_test = sh_test.row_values(i)
#     if isNum(row_data_test[1]):
#         row_list_test.append(row_data_test)
#         row_number_test += 1
#     else:
#         continue
# label_test_value = []
# label_test = np.zeros(42)
# label_test_value = sh_test.row_values(570)
# row_test = len(row_list_test)
# column_test = len(row_list_test[1])
# for i in range(1, column_test):
#     label_test[i-1] = label_test_value[i]
#
# print('the label test shape:', label_test.shape)
# data_test = np.zeros([row_test, column_test-1])
#
# for i in range(row_test):
#     for j in range(1, column_test):
#         data_test[i, j - 1] = row_list_test[i][j]
# where_tesr_are_nan = np.isnan(data_test)
# data_test[where_tesr_are_nan] = 0
# data_test = np.unique(data_test, axis = 0)
# data_test = data_test.T
# print('data test shape:', data_test.shape)


##SVD
# svd = TruncatedSVD(2)
# new_data_train = svd.fit_transform(data_train)
# print(svd.explained_variance_ratio_)




##PCA
# pca = PCA(n_components=10)
# new_data_train = pca.fit_transform(training_data)
# print(pca.explained_variance_ratio_)
# print(pca.explained_variance_)
# # new_data_test = pca.transform(data_test)
# print(new_data_train.shape)
# clf2 = svm.SVC(C=0.4)
# clf2.fit(new_data_train, label_train)
# print('train accuracy2: %f' % (np.mean(label_train == clf2.predict(new_data_train))))
# print('test accuracy2: %f' % (np.mean(label_test == clf2.predict(new_data_test))))

#REF-Val
start = time.time()
svc = SVC()
rfecv = RFECV(estimator=svc, step=1, cv=StratifiedKFold(2), scoring="accuracy", verbose=1)
rfecv.fit(validation_data, validation_label)
end = time.time()
print(end-start)
print("Optical number of features:%d"% rfecv.n_features_)
plt.figure()
plt.xlabel("Number of features selected")
plt.ylabel("Cross validation score(nb of correct classifications)")
plt.plot(range(1, len(rfecv.grid_scores_) + 1), rfecv.grid_scores_)
plt.show()
#################################################best is 14#####################################

# start = time.time()
# estimator = SVR(kernel='linear')
# selector = RFE(estimator, n_features_to_select=220, step=10, verbose=1)
# selector = selector.fit(training_data, training_label)
# end = time.time()
# print(end - start)
# print(selector.support_)
# print(selector.ranking_.shape)
# selected_features = []
# for x in range(len(selector.support_)):
#     if selector.support_[x] == 1:
#         selected_features.append(x+1)
#     else:
#         continue
# print("the selected_featuresa are:")
# print(selected_features)
# mask = selector.support_.tolist()
# print(mask)

mask = [False, False, False, False, False, False, False, False, False, False, 
False, False, False, False, False, False, False, False, False, False, False, 
False, False, False, False, False, False, False, False, False, False, 
False, False, False, False, False, False, False, False, False, False, False, 
False, False, False, False, False, False, False, True, True, False, False, 
False, False, False, False, False, False, False, False, False, False, False, 
False, False, False, False, False, False, False, False, False, False, False, 
False, False, False, False, False, False, False, False, False, False, False, 
False, False, False, False, False, False, False, False, False, False, False, 
False, False, False, False, False, True, False, False, False, False, False, 
False, False, False, False, False, False, False, False, False, False, False, 
False, False, False, False, False, False, False, False, False, False, False, 
False, False, False, False, False, False, False, False, False, False, False, 
False, False, False, False, False, False, True, False, False, False, False, 
False, False, False, False, False, False, True, False, False, False, False, 
False, False, False, False, False, False, False, False, False, False, False, 
False, False, False, False, False, False, False, False, False, False, False, 
False, False, False, True, False, False, False, False, False, True, False, 
False, False, False, False, True, False, False, False, False, False, False, 
False, False, False, False, False, False, False, False, False, False, False, 
False, False, False, False, False, False, False, False, False, False, False, 
False, False, False, False, False, False, False, False, False, False, False, 
False, False, False, False, False, False, False, False, False, False, False, 
False, False, False, False, False, False, False, False, False, False, False, 
False, False, False, False, False, False, True, False, False, False, False, 
False, False, False, False, False, False, False, False, False, False, False, 
False, True, False, False, False, False, False, False, False, False, False, 
False, False, False, False, False, False, False, False, False, False, False, 
False, False, False, True, False, False, False, False, False, False, False, 
False, False, False, True, False, False, False, False, False, False, False, 
False, False, False, False, False, False, False, False, False, False, False, 
False, False, False, False, False, False, False, False, False, True, False, 
False, False, False, False, False, False, False, False, False, False, False, 
False, False, False, False, False, False, False, False, False, False, False, 
False, False, False, False, False, False, False, False, False, False, False, 
False, False, False, False, False, False, False, False, False, False, False, 
False, False, False, False, False, False, False, False, False, False, False, 
False, False, False, False, False, False, False, False, False, False, False, 
False, False, False, False, False, False, False, False, False, False, False, 
False, False, False, False, False, False, False, False, False, False, False, 
False, False, False, False, False, False, False, False, False, False, False, 
False, False, False, False, False, False, False, True, False, False, False, 
False, False, False, False, False, False, False, False, False, False, False, 
False, False, False, False, False, False, False, False, False, False, False, 
False, False, False, False, False, False, False, False, False, False, False, 
False, False, False, False, False, False, False, False, False, False, False, 
False, False, False, False, False, False, False, False, False, False, False, 
False, False, False, False, False, False, False, False, False, False, False, 
False, False, False, False, False, False, False, False, False]

mask = [False, True, False, True, False, False, False, False, False, True, True, 
True, False, False, False, True, True, False, False, True, False, False, False, 
False, False, False, True, False, True, False, False, False, True, False, False, 
True, True, True, False, False, False, False, True, False, False, True, True, False, 
False, True, True, True, False, False, True, True, False, False, False, False, False, 
False, True, False, False, True, True, True, False, True, False, True, True, False, 
True, False, False, True, False, True, False, True, False, True, True, True, False, 
True, True, False, True, True, False, True, False, False, True, False, False, True, 
False, True, True, False, False, False, False, False, True, False, True, False, False, 
False, False, False, True, True, True, False, False, False, False, False, True, True, 
False, False, True, True, False, True, False, False, True, True, False, False, False, 
False, False, False, False, False, False, True, True, True, False, False, True, False, 
True, False, True, False, True, True, True, False, False, False, False, True, True, 
True, True, False, True, True, False, True, False, True, False, False, False, True, 
False, False, True, False, True, False, False, False, True, False, True, True, True, 
True, False, False, True, False, True, False, False, True, True, False, False, True,
 False, False, False, False, False, True, False, False, True, True, False, True, False, 
True, False, False, False, False, False, True, True, True, False, False, False, False, 
False, False, True, True, True, True, True, False, False, False, True, True, True, True, True, True, True, False, 
False, False, False, True, False, False, False, False, False, True, True, False, False, 
False, True, True, False, True, False, False, True, False, True, True, True, False, 
False, True, False, False, False, False, False, False, False, False, False, True, True, 
True, True, True, True, False, False, False, False, True, True, True, False, False, 
False, False, False, False, False, True, False, False, True, True, False, False, False, False, False, False, True, False, 
True, False, True, False, True, False, True, False, False, False, False, False, True, 
True, True, False, False, False, True, True, False, False, True, False, True, False,
 False, True, False, True, True, False, True, False, True, False, True, False, False, 
False, True, True, True, True, False, False, False, True, False, True, True, False, 
False, False, False, False, False, True, True, False, True, True, True, False, True, True, False, True, False, True, False, 
False, True, False, True, True, False, True, True, False, False, False, False, True, 
False, False, False, False, False, False, False, True, True, True, False, True, False, 
True, False, True, True, False, False, True, False, False, True, False, False, False,
False, True, False, True, False, False, False, False, False, True, True, True, False,
False, False, False, True, False, True, False, True, False, True, True, True, False, True, True, True, True, False, True, 
False, True, True, False, True, False, True, False, True, False, False, True, True, 
True, True, False, True, False, True, True, False, False, False, False, False, False, 
False, False, False, True, False, False, True, False, True, False, False, False, False, 
True, False, True, False, False, True, True, False, False, False, False, False, False, 
False, False, True, False, False, False, False, False, False, False, False, False, False, 
False, False, True, False, False, False, True, True, True, True]

# mask = np.array(mask)
# delete_column = []
# for x in range(len(mask)):
#     if mask[x]:
#         continue
#     else:
#         delete_column.append(x)
# print(delete_column)
# training_data = np.delete(training_data, delete_column, 1)
# validation_data = np.delete(validation_data, delete_column, 1)
# def Extracted_data(data):
#     data = data*(mask.T)
#     delete_column =[]
#     for x in range(data.shape[1]):
#         if all(data[:, x]):
#             continue
#         else:
#             delete_column.append(x)
#     data = np.delete(data, delete_column, 1)
#     return data

# training_data = Extracted_data(training_data)
# validation_data = Extracted_data(validation_data)

# C越大，相当于惩罚松弛变量，希望松弛变量接近0，即对误分类的惩罚增大，
# 趋向于对训练集全分对的情况，这样对训练集测试时准确率很高，但泛化能力弱。C值小，对误分类的惩罚减小，允许容错，将他们当成噪声点，泛化能力较强。
# clf = svm.SVC(C=)
# clf.fit(training_data, training_label)
# print(clf.fit(training_data, training_label))
# print('train accuracy: %f' % (np.mean(training_label == clf.predict(training_data))))
# print('test accuracy: %f' % (np.mean(testing_label == clf.predict(testing_data))))


# train_accuracy = []
# validation_accuracy = []
# for x in range(1,10):
#     x = x/10
#     clf = svm.SVC(C=x)
#     clf.fit(training_data, training_label)
#     train_accuracy.append(np.mean(training_label == clf.predict(training_data)))
#     validation_accuracy.append(np.mean(validation_label == clf.predict(validation_data)))
# print('Best C:', validation_accuracy.index(max(validation_accuracy)))
# print('Best training accuracy:', train_accuracy[validation_accuracy.index(max(validation_accuracy))])
# print('Best validation accuracy:', validation_accuracy[validation_accuracy.index(max(validation_accuracy))])
#
# plt.plot(train_accuracy, label='train')
# plt.plot(validation_accuracy, label='validation')
# plt.ylabel("Clasification accuracy")
# plt.title('Classification accuracy')
# plt.xlabel("C in 400")
# plt.show()


