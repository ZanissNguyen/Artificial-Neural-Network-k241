/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   CrossEntropy.cpp
 * Author: ltsach
 * 
 * Created on August 25, 2024, 2:47 PM
 */

#include "loss/CrossEntropy.h"
#include "ann/functions.h"

CrossEntropy::CrossEntropy(LossReduction reduction): ILossLayer(reduction)
{
    
}

CrossEntropy::CrossEntropy(const CrossEntropy& orig):
ILossLayer(orig){
}

CrossEntropy::~CrossEntropy() {
}

double CrossEntropy::forward(xt::xarray<double> Y, xt::xarray<double> T)
{
    m_aCached_Ypred = Y;
    m_aYtarget = T;

    double loss = cross_entropy(Y, T, m_eReduction==2);

    return loss;

    // xt::xarray<unsigned long> y_true_check = xt::argmax(T, {1});
    // xt::xarray<double> y_pred_check = xt::zeros_like(y_true_check);
    // for (std::size_t i = 0; i<y_true_check.shape()[0]; i++)
    // {
    //     xt::view(y_pred_check, i) = Y(i, y_true_check(i));
    // }
    // xt::xarray<double> y_pred_clipped = xt::clip(y_pred_check, 1e-7, 1 - 1e-7);
    
    // int N_norm = (this->m_eReduction==2) ? nsamples : 1;
    // xt::xarray<double> log = -xt::log(y_pred_clipped);
    // double result = 1.0/N_norm * xt::sum(log)();
    // return result;
}

xt::xarray<double> CrossEntropy::backward() 
{
    int nsamples = m_aCached_Ypred.shape()[0];

    double N_norm = (this->m_eReduction==2) ? nsamples : 1;
    xt::xarray<double> DY = -1.0/N_norm * (m_aYtarget/(m_aCached_Ypred+1e-7)); 

    return DY;
}