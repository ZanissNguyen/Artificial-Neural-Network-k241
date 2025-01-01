/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   ReLU.cpp
 * Author: ltsach
 * 
 * Created on August 25, 2024, 2:44 PM
 */

#include "layer/ReLU.h"
#include "sformat/fmt_lib.h"
#include "ann/functions.h"

ReLU::ReLU(string name) {
    if(trim(name).size() != 0) m_sName = name;
    else m_sName = "ReLU_" + to_string(++m_unLayer_idx);
}

ReLU::ReLU(const ReLU& orig) {
    m_sName = "ReLU_" + to_string(++m_unLayer_idx);
}

ReLU::~ReLU() {
}

xt::xarray<double> ReLU::forward(xt::xarray<double> X) 
{
    xt::xarray<double> Y;
    // xt::xarray<double> temp = xt::zeros_like(X);
    m_aMask = xt::where(X >= 0, +1.0, 0.0);
    // cout << m_aMask;

    Y = m_aMask * X;
    double epsilon = 1e-10; // Small threshold
    Y = xt::where(xt::abs(Y) < epsilon, 0.0, Y);

    return Y;
}
xt::xarray<double> ReLU::backward(xt::xarray<double> DY) 
{
    xt::xarray<double> DX;

    DX = m_aMask * DY; 

    return DX;
}

string ReLU::get_desc(){
    string desc = fmt::format("{:<10s}, {:<15s}:",
                    "ReLU", this->getname());
    return desc;
}