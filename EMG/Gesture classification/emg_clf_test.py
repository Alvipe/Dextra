# -*- coding: utf-8 -*-
"""
Created on Tue Nov 10 09:03:53 2015

@author: Alvaro
"""

from emgesture import fextraction as fex
from emgesture import classifier as clf
import scipy.io as sio
import numpy as np

emg_data = sio.loadmat('emg_data_5_class_2ch')
#emg_data = sio.loadmat('emg_25-11-15')

nMotions = 5
nChannels = 2
nIterations = 4
emg = []
emg_seg = []

for m in range(1,nMotions+1):
    for i in range(nIterations):
        for c in range(1,nChannels+1):
            emg.append(emg_data['motion'+str(m)+'_ch'+str(c)][:,i]) #motion1_ch1_i1, motion1_ch2_i1, motion1_ch1_i2, motion1_ch2_i2

for n in range(len(emg)):
    emg_seg.append(fex.segmentation(emg[n]))

feature_list = [fex.mav, fex.rms, fex.var, fex.ssi, fex.zc, fex.wl, fex.ssc, fex.wamp]

nSegments = len(emg_seg[0][0])
nFeatures = len(feature_list)

feature_matrix = np.zeros((nSegments*nIterations*nMotions,nFeatures*nChannels))
n = 0

for i in range(0,len(emg_seg),nChannels):
    for j in range(nSegments):
        feature_matrix[n] = fex.features((emg_seg[i][:,j],emg_seg[i+1][:,j]),feature_list)
        n += 1



from sklearn.svm import SVC
import timeit

target = fex.gestures(nIterations*nSegments,nMotions)

#target = np.concatenate((0*np.ones((128,1)),1*np.ones((128,1)),2*np.ones((128,1)),3*np.ones((128,1)),4*np.ones((128,1))),axis=0).ravel()

tic = timeit.default_timer()

[feat_scaled,reductor,scaler] = fex.feature_scaling(feature_matrix, target)

#svm = SVC(kernel='rbf',C=100,gamma=1) #C=1e5,gamma=1e-23
#svm.fit(feat_scaled, target)

classifier = clf.train(feat_scaled,target)

toc = timeit.default_timer()

print("Feature transformation + training time = %0.5f s." %(toc - tic))

emg_val = []
emg_seg_val = []

for m in range(1,nMotions+1):
    for c in range(1,nChannels+1):
        emg_val.append(emg_data['motion'+str(m)+'_ch'+str(c)][:,4]) #motion1_ch1_i1, motion1_ch2_i1, motion1_ch1_i2, motion1_ch2_i2

for n in range(len(emg_val)):
    emg_seg_val.append(fex.segmentation(emg_val[n]))

feature_matrix_val = np.zeros((nMotions*1*len(emg_seg_val[0][0]),len(feature_list)*nChannels))
n = 0

for i in range(0,len(emg_seg_val),nChannels):
    for j in range(len(emg_seg_val[0][0])):
        feature_matrix_val[n] = fex.features((emg_seg_val[i][:,j],emg_seg_val[i+1][:,j]),feature_list)
        n = n + 1

nSegments = len(emg_seg_val[0][0])

target_val = fex.gestures(nSegments,nMotions)

#target_val = np.concatenate((0*np.ones((32,1)),1*np.ones((32,1)),2*np.ones((32,1)),3*np.ones((32,1)),4*np.ones((32,1))),axis=0).ravel()

tic = timeit.default_timer()

[feat_val_scaled,reductor,scaler] = fex.feature_scaling(feature_matrix_val,target_val,reductor,scaler)

#predict = svm.predict(feat_val_scaled)

predict = clf.classify(feat_val_scaled,classifier)

toc = timeit.default_timer()

print("Feature transformation + classification time = %0.5f s." %(toc - tic))

print("Classification accuracy = %0.5f." %(classifier.score(feat_val_scaled,target_val)))
