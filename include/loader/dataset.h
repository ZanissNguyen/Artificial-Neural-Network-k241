/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.h to edit this template
 */

/* 
 * File:   dataset.h
 * Author: ltsach
 *
 * Created on September 2, 2024, 3:59 PM
 */

#ifndef DATASET_H
#define DATASET_H
#include "tensor/xtensor_lib.h"
#include <string.h>
// #include <windows.h>
// #include <fstream>

#include <xtensor/xarray.hpp>

using namespace std;

template<typename DType, typename LType>
class DataLabel{
private:
    xt::xarray<DType> data;
    xt::xarray<LType> label;

    template<typename DType1, typename LType1>
    friend ostream &operator<<(ostream &os, const DataLabel<DType1, LType1>& dl);
public:
    DataLabel(){
    }
    DataLabel(xt::xarray<DType> data):
    data(data){
    }
    DataLabel(xt::xarray<DType> data, xt::xarray<LType> label):
    data(data), label(label){
    }
    xt::xarray<DType> getData() const{ return data; }
    xt::xarray<LType> getLabel() const{ return label; }
};

template<typename DType, typename LType>
ostream &operator<<(ostream &os, const DataLabel<DType, LType>& dl)
{
    int width = 15;
    os << "[ DataLabel To String >> ";
    os << dl.data;
    if (dl.label.dimension()!=0 || (dl.label.dimension()==0 && dl.label.size()==1))
    {
        os << " | ";
        os << dl.label; 
    }
    os << "]";
    return os;
}

template<typename DType, typename LType>
class Batch{
private:
    xt::xarray<DType> data;
    xt::xarray<LType> label;

    template<typename DType1, typename LType1>
    friend ostream &operator<<(ostream &os, const Batch<DType1, LType1>& batch);
public:
    Batch(xt::xarray<DType> data,  xt::xarray<LType> label):
    data(data), label(label){
    }
    Batch() {}
    virtual ~Batch(){}
    xt::xarray<DType>& getData(){return data; }
    xt::xarray<LType>& getLabel(){return label; }

    static string toString(Batch<DType, LType> & batch)
    {
        stringstream os;

        int width = 15;
        os << "[ Batch To String ]" << endl;
        os << setw(width) << left << "Data";
        if (((int)batch.label.dimension()!=0 && batch.label.shape()==batch.data.shape()) 
            || (batch.label.dimension()==0 && batch.label.size()==1)) // TODO: There is something wrong here!
        {
            os << " | ";
            os << setw(width) << "Label"<< endl; 
        }
        else os << endl;
        cout << "---------------------------------" << endl;
        xt::svector<unsigned long> data_shape(batch.data.shape().begin(), batch.data.shape().end());
        int size = data_shape[0];
        for (int i = 0; i<size; i++)
        {
            os << setw(width) << left << xt::view(batch.data, i);
            if (((int)batch.label.dimension()!=0 && batch.label.shape()==batch.data.shape()) 
                || (batch.label.dimension()==0 && batch.label.size()==1))
            {
                os << " | ";
                os << setw(width) << xt::view(batch.label, i) << endl;
            }
            else os << endl;
        }
        return os.str();
    }

    static string toString(Batch<DType, LType> *& batch)
    {
        return toString(*batch);
    }

    bool isEqual(Batch<DType, LType> *batch)
    {
        return (*this)==(*batch);
    }

    bool operator==(Batch<DType, LType> rhs)
    {
        return (this->data == rhs.getData() && this->label == rhs.getLabel());
    }
};

template<typename DType, typename LType>
ostream &operator<<(ostream &os, const Batch<DType, LType>& batch)
{
    int width = 15;
    os << "[ Batch To String ]" << endl;
    os << setw(width) << left << "Data";
    if (((int)batch.label.dimension()!=0 && batch.label.shape()==batch.data.shape()) 
        || (batch.label.dimension()==0 && batch.label.size()==1))
    {
        os << " | ";
        os << setw(width) << "Label"<< endl; 
    }
    else os << endl;
    cout << "---------------------------------" << endl;
    xt::svector<unsigned long> data_shape(batch.data.shape().begin(), batch.data.shape().end());
    int size = data_shape[0];
    for (int i = 0; i<size; i++)
    {
        os << setw(width) << left << xt::view(batch.data, i);
        if (((int)batch.label.dimension()!=0 && batch.label.shape()==batch.data.shape()) 
            || (batch.label.dimension()==0 && batch.label.size()==1))
        {
            os << " | ";
            os << setw(width) << xt::view(batch.label, i) << endl;
        }
        else os << endl;
    }
    return os;
}

template<typename DType, typename LType>
class Dataset{
private:
public:
    Dataset(){};
    virtual ~Dataset(){};
    
    virtual int len()=0;
    virtual DataLabel<DType, LType> getitem(int index)=0;
    virtual xt::svector<unsigned long> get_data_shape()=0;
    virtual xt::svector<unsigned long> get_label_shape()=0;
    
    virtual xt::xarray<DType> getAllData() = 0;
    virtual xt::xarray<LType> getAllLabel() = 0;
};

//////////////////////////////////////////////////////////////////////
template<typename DType, typename LType>
class TensorDataset: public Dataset<DType, LType>
{
private:
    xt::xarray<DType> data;
    xt::xarray<LType> label;
    xt::svector<unsigned long> data_shape, label_shape;
    
public:
    /* TensorDataset: 
     * need to initialize:
     * 1. data, label;
     * 2. data_shape, label_shape
    */
    TensorDataset(xt::xarray<DType> data, xt::xarray<LType> label)
    {
        this->data = data;
        xt::svector<unsigned long> data_shape(data.shape().begin(), data.shape().end());
        this->data_shape = data_shape;
        
        if (label.dimension()==0 && label!=xt::xarray<LType>())
        {   
            LType init = label(0);
            xt::xarray<LType> toCreate(data.shape());
            label = xt::full_like(toCreate, init);
        }
        this->label = label;
        xt::svector<unsigned long> label_shape(label.shape().begin(), label.shape().end());
        this->label_shape = label_shape;
    }
    /* len():
     *  return the size of dimension 0
    */
    int len()
    {
        return this->data_shape[0]; 
        //remove it when complete
    }
    
    /* getitem:
     * return the data item (of type: DataLabel) that is specified by index
     */
    DataLabel<DType, LType> getitem(int index)
    {
        if (index<0 || index>=len()) throw std::out_of_range("Index is out of range!");

        DataLabel<DType, LType> toReturn;

        if ((int)this->label.dimension()==0) //Doesn't have label
        {
            toReturn = DataLabel<DType, LType>(xt::view(this->data, index));
            return toReturn;
        }
        else if (this->label_shape[0] == this->data_shape[0])
        {
            toReturn = DataLabel<DType, LType>(xt::view(this->data, index), xt::view(this->label, index));
            return toReturn;
        }
        else cout << "Data and label size are not valid, each data we must have one label." << endl;

        return toReturn;   
    }
    
    xt::svector<unsigned long> get_data_shape()
    {
        return this->data_shape;
    }
    xt::svector<unsigned long> get_label_shape()
    {
        return this->label_shape;
    }

    xt::xarray<DType> getAllData() override
    {
        return this->data;
    }

    xt::xarray<LType> getAllLabel() override
    {
        return this->label;
    }
};

template<typename DType, typename LType>
class ImageFolderDataset: public Dataset<DType, LType>{
private:
    xt::xarray<DType> data;
    xt::xarray<LType> label;
    xt::svector<unsigned long> data_shape, label_shape;
    string path;
    
public:
    /* TensorDataset: 
     * need to initialize:
     * 1. data, label;
     * 2. data_shape, label_shape
    */
    ImageFolderDataset(string path)
    {
        this->path = path;
    }

    ImageFolderDataset(xt::xarray<DType> data, xt::xarray<LType> label, string path)
    {
        this->data = data;
        xt::svector<unsigned long> data_shape(data.shape().begin(), data.shape().end());
        this->data_shape = data_shape;

        if (label.dimension()==0 && label!=xt::xarray<LType>())
        {   
            LType init = label(0);
            xt::xarray<LType> toCreate(data.shape());
            label = xt::full_like(toCreate, init);
        }
        this->label = label;
        xt::svector<unsigned long> label_shape(label.shape().begin(), label.shape().end());
        this->label_shape = label_shape;

        this->path = path;
    }
    /* len():
     *  return the size of dimension 0
    */
    int len()
    {
        return 0;
        // return countFilesInDirectory(this->path);
        //remove it when complete
    }
    
    /* getitem:
     * return the data item (of type: DataLabel) that is specified by index
     */
    DataLabel<DType, LType> getitem(int index)
    {
        if (index<0 || index>=len()) throw std::out_of_range("Index is out of range!");

        DataLabel<DType, LType> toReturn;

        if ((int)this->label.dimension()==0) //Doesn't have label
        {
            toReturn = DataLabel<DType, LType>(xt::view(this->data, index));
            return toReturn;
        }
        else if (this->label_shape[0] == this->data_shape[0])
        {
            toReturn = DataLabel<DType, LType>(xt::view(this->data, index), xt::view(this->label, index));
            return toReturn;
        }
        else cout << "Data and label size are not valid, each data we must have one label." << endl;

        return toReturn;   
    }
    
    xt::svector<unsigned long> get_data_shape()
    {
        return this->data_shape;
    }
    xt::svector<unsigned long> get_label_shape()
    {
        return this->label_shape;
    }

    xt::xarray<DType> getAllData() override
    {
        return this->data;
    }

    xt::xarray<LType> getAllLabel() override
    {
        return this->label;
    }

private:
    // i have no idea for this function. It provided by ChatGPT after i told this to show me how to count file in directory.
    // int countFilesInDirectory(const std::string& directoryPath) 
    // {
    //     WIN32_FIND_DATA findFileData;
    //     HANDLE hFind = FindFirstFile((directoryPath + "\\*").c_str(), &findFileData);
    //     if (hFind == INVALID_HANDLE_VALUE) 
    //     {
    //         std::cerr << "Error: Could not open directory.\n";
    //         return -1;
    //     }

    //     int fileCount = 0;
    //     do 
    //     {
    //         // Check if the entry is a file (not a directory)
    //         if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
    //         {
    //             ++fileCount;
    //         }
    //     } while (FindNextFile(hFind, &findFileData) != 0);

    //     FindClose(hFind);
    //     return fileCount;
    // }
};

#endif /* DATASET_H */

