/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   SGDParamGroup.cpp
 * Author: ltsach
 * 
 * Created on October 7, 2024, 9:45 PM
 */

#include "optim/SGDParamGroup.h"

SGDParamGroup::SGDParamGroup() {
    m_pParams = new xmap<string, xt::xarray<double>*>(&stringHash);
    m_pGrads = new xmap<string, xt::xarray<double>*>(&stringHash);
}

SGDParamGroup::SGDParamGroup(const SGDParamGroup& orig) 
{
    *this->m_pGrads = *orig.m_pGrads;
    *this->m_pParams = *orig.m_pParams;
    *this->m_pCounter = *orig.m_pCounter;
}

SGDParamGroup::~SGDParamGroup()
{
    if (m_pParams!=nullptr) delete m_pParams;
    if (m_pGrads!=nullptr) delete m_pGrads;
}

void SGDParamGroup::register_param(string param_name, xt::xarray<double>* ptr_param, xt::xarray<double>* ptr_grad)
{
    m_pParams->put(param_name, ptr_param);
    // cout << "Put " << param_name << " value, now size is" << m_pParams->size();
    m_pGrads->put(param_name, ptr_grad);
    // cout << "Put " << param_name << " gradient, now size is" << m_pGrads->size();
}
void SGDParamGroup::register_sample_count(unsigned long long* pCounter)
{
    m_pCounter = pCounter;
}
void SGDParamGroup::zero_grad()
{
    DLinkedList<string> keys = m_pGrads->keys();
    for(auto key: keys)
    {
        xt::xarray<double>* pGrad = m_pGrads->get(key);
        xt::xarray<double>* pParam = m_pParams->get(key);

        *pGrad = xt::zeros<double>(pParam->shape());
    }
    //reset sample_counter
    *m_pCounter = 0;
}

void SGDParamGroup::step(double lr)
{
    DLinkedList<string> keys = m_pGrads->keys();
    for(auto key: keys)
    {
        xt::xarray<double>& P = *m_pParams->get(key);
        xt::xarray<double>& grad_P = *m_pGrads->get(key);

        // xt::xarray<double> pre = P;
        // cout << "grad_" << key  <<endl<< grad_P << endl;
        P = P - lr*grad_P;
        // cout << P-pre << endl;
    }
}
