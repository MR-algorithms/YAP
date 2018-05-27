import xlwt
from sklearn.cross_validation import KFold, cross_val_score
from sklearn import svm
import numpy as np
from math import floor


workbook = xlwt.Workbook(encoding='utf-8')
booksheet = workbook.add_sheet('features', cell_overwrite_ok=True)


def classified(data, width, height):
    print('Invoking Method: [classified].')
    save_feature(data)
    label = [x[-1] for x in data]
    feature = [x[0:-1] for x in data]
	feature = z_normalization(np.array(feature, np.float))
    #separate train and validation data.
    kf = StratifiedKFold(n_splits=10, shuffle=True, random_state=False)
	feature_name = np.load('features.npy')
	rand_count = 50
	for ti in range(1, rand_count):
		test_acc, train_acc, ranks, lab_tr, lab_te = [], [], [], [], []
		for tr_x, te_x in kf.split(features, label):
			X_train, X_test, y_train, y_test = features[tr_x], features[te_x], label[tr_x], label[te_x]
			# iterate over classifiers
			sel = RFE(SVC(kernel="linear", C=0.25, probability=True), n_features_to_select=ti, step=10, verbose=1)
			sel.fit(X_train, y_train)
			te_pred = sel.predict_proba(X_test)
			tr_pred = sel.predict_proba(X_train)
			test_acc.append(np.average((y_test == (te_pred[:, 1] >= 0.5).astype(np.int8)).astype(np.int8)))
			train_acc.append(np.average((y_train == (tr_pred[:, 1] >= 0.5).astype(np.int8)).astype(np.int8)))
			lab_te.append((y_test, te_pred[:, 1]))
			lab_tr.append((y_train, tr_pred[:, 1]))
			ranks.append(sorted(zip(map(lambda x: round(x, 4), sel.ranking_), feature_name)))
		features_rank.append(ranks)
		test_acc_score.append(test_acc)
		train_acc_score.append(train_acc)
		labels_test.append(lab_te)
		labels_train.append(lab_tr)
	averag_te_acc = np.mean(test_acc_score, axis=1)
	averag_tr_acc = np.mean(train_acc_score, axis=1)

	# plot accuracy
	plt.figure()
	X = np.array([x for x in range(1, rand_count)])
	plt.plot(X, averag_te_acc, label='test acc')
	plt.plot(X, averag_tr_acc, label='train acc')
	plt.xlabel('selected_features_numbers')
	plt.ylabel('accuracy')
	plt.title('features selected- accuracy')
	plt.legend(loc="lower right")
	plt.show()
	loc = averag_te_acc.tolist().index(averag_te_acc.max())
	for i in range(10):
    y_te, y_pr = labels_test[loc][i]
    auc_te, fpr, tpr = draw_roc_curve(y_te, y_pr, title='CV: %d Test ROC' % i)
    test_auc_range.append(auc_te)
    y_tr, y_pr = labels_train[loc][i]
    auc_tr, fpr, tpr = draw_roc_curve(y_tr, y_pr, title='CV: %d Train ROC' % i)
    train_auc_range.append(auc_tr)
	np.save('test_acc_score.npy', test_acc_score)
	np.save('train_acc_score.npy', train_acc_score)
	np.save('test_auc_range.npy', test_auc_range)
	np.save('features_rank', features_rank)
	np.save('labels_test', labels_test)
	np.save('labels_train', labels_train)
	
    return [1, test_auc_range.tolist(), test_auc_range.shape(0)]


def save_feature(feature):
    feature =[[row[i] for row in feature] for i in range(len(feature[0]))]
    for i, row in enumerate(feature):
        for j,col in enumerate(row):
            booksheet.write(i+1, j+1, col)
    workbook.save('test.xls')


# column normalization
def Z_normalization(data):
    new_data = np.zeros([data.shape[0], data.shape[1]])
    mu = np.average(data, axis=0)
    sigma = np.std(data, axis=0)
    for i in range(data.shape[1]):
        for j in range(data.shape[0]):
            new_data[j][i] = (data[j][i]-mu[i])/sigma[i]
    new_data = np.nan_to_num(new_data)
    return new_data

   
