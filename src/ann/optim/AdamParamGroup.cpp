/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   AdamParamGroup.cpp
 * Author: ltsach
 * 
 * Created on October 8, 2024, 1:43 PM
 */

#include "optim/AdamParamGroup.h"

AdamParamGroup::AdamParamGroup(double beta1, double beta2):
    m_beta1(beta1), m_beta2(beta2){
    //Create some maps:
    m_pParams = new xmap<string, xt::xarray<double>*>(&stringHash);
    m_pGrads = new xmap<string, xt::xarray<double>*>(&stringHash);
    m_pFirstMomment = new xmap<string, xt::xarray<double>*>(
            &stringHash,
            0.75,
            0,
            xmap<string, xt::xarray<double>*>::freeValue);
    m_pSecondMomment = new xmap<string, xt::xarray<double>*>(
            &stringHash,
            0.75,
            0,
            xmap<string, xt::xarray<double>*>::freeValue);
    //
    m_step_idx = 1;
    m_beta1_t = m_beta1;
    m_beta2_t = m_beta2;
}

AdamParamGroup::AdamParamGroup(const AdamParamGroup& orig):
    m_beta1(orig.m_beta1), m_beta2(orig.m_beta2){
    m_pParams = new xmap<string, xt::xarray<double>*>(&stringHash);
    m_pGrads = new xmap<string, xt::xarray<double>*>(&stringHash);
    m_pFirstMomment = new xmap<string, xt::xarray<double>*>(
            &stringHash,
            0.75,
            0,
            xmap<string, xt::xarray<double>*>::freeValue);
    m_pSecondMomment = new xmap<string, xt::xarray<double>*>(
            &stringHash,
            0.75,
            0,
            xmap<string, xt::xarray<double>*>::freeValue);
    //copy:
    *m_pParams = *orig.m_pParams;
    *m_pGrads = *orig.m_pGrads;
    *m_pFirstMomment = *orig.m_pFirstMomment;
    *m_pSecondMomment = *orig.m_pSecondMomment;
    //
    m_step_idx = 1;
    m_beta1_t = m_beta1;
    m_beta2_t = m_beta2;
}

AdamParamGroup::~AdamParamGroup() 
{
    if (m_pParams!=nullptr) delete m_pParams;
    if (m_pGrads!=nullptr) delete m_pGrads;
    if(m_pFirstMomment != nullptr) delete m_pFirstMomment;
    if(m_pSecondMomment != nullptr) delete m_pSecondMomment;
}

void AdamParamGroup::register_param(string param_name, 
        xt::xarray<double>* ptr_param,
        xt::xarray<double>* ptr_grad)
        {
    m_pParams->put(param_name, ptr_param);
    m_pGrads->put(param_name, ptr_grad); 
}
void AdamParamGroup::register_sample_count(unsigned long long* pCounter){
    m_pCounter = pCounter;
}

void AdamParamGroup::zero_grad()
{
    DLinkedList<string> keys = m_pGrads->keys();
    for(auto key: keys){
        xt::xarray<double>* pGrad = m_pGrads->get(key);
        xt::xarray<double>* pParam = m_pParams->get(key);
        xt::xarray<double>* pFirstMoment = m_pFirstMomment->get(key);
        xt::xarray<double>* pSecondMoment = m_pSecondMomment->get(key);
        *pGrad = xt::zeros<double>(pParam->shape());
        *pFirstMoment = xt::zeros<double>(pParam->shape());
        *pSecondMoment = xt::zeros<double>(pParam->shape());
    }

    m_beta1_t = m_beta1;
    m_beta2_t = m_beta2;
    m_step_idx = 1;
    *m_pCounter = 0;
}

void AdamParamGroup::step(double lr)
{
    //YOUR CODE IS HERE
    DLinkedList<string> keys = m_pGrads->keys();
    for(auto key: keys)
    {
        xt::xarray<double>& P = *m_pParams->get(key);
        xt::xarray<double>& grad_P = *m_pGrads->get(key);
        xt::xarray<double>& firstMoment = *m_pFirstMomment->get(key);
        xt::xarray<double>& secondMoment = *m_pSecondMomment->get(key);
        
        double epsilon = 1e-7;
        firstMoment = m_beta1_t * firstMoment + (1-m_beta1_t) * grad_P;
        secondMoment = m_beta2_t * secondMoment + (1-m_beta2_t) * xt::square(grad_P);

        xt::xarray<double> correct_firstMoment = firstMoment / (1 - m_beta1_t);
        xt::xarray<double> correct_secondMoment = secondMoment / (1 - m_beta2_t);

        lr = lr * sqrt(1 - m_beta2_t) / (1- m_beta1_t);

        P = P - lr * (correct_firstMoment / (xt::sqrt(correct_secondMoment) + epsilon));
    }
    
    //UPDATE step_idx:
    m_step_idx += 1;
    m_beta1_t *= m_beta1;
    m_beta2_t *= m_beta2;
}
