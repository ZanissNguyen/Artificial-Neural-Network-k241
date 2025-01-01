/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   Softmax.cpp
 * Author: ltsach
 * 
 * Created on August 25, 2024, 2:46 PM
 */

#include "layer/Softmax.h"
#include "ann/functions.h"
#include "sformat/fmt_lib.h"
#include <filesystem> //require C++17
namespace fs = std::filesystem;

Softmax::Softmax(int axis, string name): m_nAxis(axis) {
    if(trim(name).size() != 0) m_sName = name;
    else m_sName = "Softmax_" + to_string(++m_unLayer_idx);
}

Softmax::Softmax(const Softmax& orig) {
}

Softmax::~Softmax() {
}

xt::xarray<double> Softmax::forward(xt::xarray<double> X) 
{
//     if (X.dimension()==1) 
//     {
        // m_nAxis = X.shape()[1];
        xt::xarray<double> Y = softmax(X, this->m_nAxis);
        m_aCached_Y = Y;
        return Y;
    // }

    // xt::xarray<double> exp_X = xt::exp(X); 
    // xt::xarray<double> sum_exp = xt::sum(exp_X, {1}, xt::keep_dims);
    
    // xt::xarray<double> Y = exp_X/sum_exp;

    // m_nAxis = X.shape()[1];
    // m_aCached_Y = Y;

    // return Y;
}
xt::xarray<double> Softmax::backward(xt::xarray<double> DY) 
{
    if (DY.dimension()==1)
    {
        xt::xarray<double> diag = xt::diag(m_aCached_Y);
        return xt::linalg::dot((diag - xt::linalg::outer(m_aCached_Y, xt::transpose(m_aCached_Y))), DY);
    }
    xt::xarray<double> DX = xt::zeros_like(m_aCached_Y);

    for (std::size_t i = 0; i< m_aCached_Y.shape()[0]; i++)
    {
        xt::xarray<double> Yi = xt::view(m_aCached_Y, i);
        xt::xarray<double> DYi = xt::view(DY, i);

        xt::xarray<double> diag = xt::diag(Yi);
        xt::view(DX, i) = xt::linalg::dot((diag - xt::linalg::outer(Yi, xt::transpose(Yi))), DYi);
    }

    return DX;
}

string Softmax::get_desc(){
    string desc = fmt::format("{:<10s}, {:<15s}: {:4d}",
                    "Softmax", this->getname(), m_nAxis);
    return desc;
}
