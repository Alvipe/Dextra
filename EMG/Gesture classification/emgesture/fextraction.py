# -*- coding: utf-8 -*-
"""
Created on Mon Sep 28 10:33:36 2015

@author: Alvaro
"""

import numpy as np
from sklearn.discriminant_analysis import LinearDiscriminantAnalysis as LDA
from sklearn.preprocessing import MinMaxScaler
from sklearn.pipeline import make_pipeline

def segmentation(emg, samples = 150):
    N = samples # number of samples per segment
    S = int(np.floor(emg.shape[0]/N)) # number of segments
    length = 0
    emg_seg = np.zeros((N,S))
    for s in range(S):
        for n in range(length,N+length):
            emg_seg[n-length,s] = emg[n] # 2D matrix with a EMG signal divided in s segments, each one with n samples
        length = length + N
    length = 0
    return emg_seg

def mav(emg_seg):
    mav = np.mean(np.abs(emg_seg))
    return mav
    
def rms(emg_seg):
    rms = np.sqrt(np.mean(np.power(emg_seg,2)))
    return rms
    
def var(emg_seg):
    N = len(emg_seg)
    var = 1.0/(N-1)*np.sum(np.power(emg_seg,2))
#   var[s] = np.var(emg_seg[:,s])
    return var
    
def ssi(emg_seg):
    ssi = np.sum(np.abs(np.power(emg_seg,2)))
    return ssi
    
def zc(emg_seg):
    zc = np.sum(np.diff(np.sign(emg_seg))!=0)
    return zc
    
def wl(emg_seg):
    wl = np.sum(np.abs(np.diff(emg_seg)))
    return wl
    
def ssc(emg_seg):
    N = len(emg_seg)
    ssc = 0
    for n in range(N-1):
        if n>0 and (emg_seg[n]-emg_seg[n-1])*(emg_seg[n]-emg_seg[n+1])>=0.001:
            ssc += 1
    return ssc
    
def wamp(emg_seg):
    N = len(emg_seg)
    wamp = 0
    for n in range(N-1):
        if np.abs(emg_seg[n]-emg_seg[n+1])>50:
            wamp += 1
    return wamp

def features(segment,feature_list):
    features = np.zeros((1,len(segment)*len(feature_list)))
    i = 0
    for feature in feature_list:
        features[0,i] = feature(segment[0])
        features[0,i+1] = feature(segment[1])
        i +=  len(segment)
    return features

def feature_scaling(feature_matrix,target,reductor=None,scaler=None):
    lda = LDA(n_components=2)    
    minmax = MinMaxScaler(feature_range=(-1,1))
    if not reductor:
        reductor = lda.fit(feature_matrix,target)
    feat_lda = reductor.transform(feature_matrix)
    if not scaler:
        scaler = minmax.fit(feat_lda)
    feat_lda_scaled = scaler.transform(feat_lda)
    
    return feat_lda_scaled,reductor,scaler

def gestures(nSamples,nGestures):
    gestures = []
    for m in range(nGestures):
        gestures.append((m*np.ones((nSamples))))
    gestures = np.array(gestures).ravel()
    return gestures

#def feature_scaling(feature_matrix,target,scaler=None):
#    lda = LDA(n_components=2)    
#    minmax = MinMaxScaler(feature_range=(-1,1))
#    if not scaler:
#        scaler = make_pipeline(lda,minmax)
#        scaler.fit(feature_matrix,target)
#    feat_lda_scaled = scaler.transform(feature_matrix)
#    
#    return feat_lda_scaled,scaler