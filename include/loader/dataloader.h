/* 
 * File:   dataloader.h
 * Author: ltsach
 * 
 * Implement
 * Author: Nguyen Thanh Phat (Zanis)
 * Student ID: 2312593
 */

#ifndef DATALOADER_H
#define DATALOADER_H
#include "loader/dataset.h"
#include "list/listheader.h"
#include "tensor/xtensor_lib.h"

using namespace std;

template<typename T>
xt::xarray<T> fixedShape(xt::xarray<T> origin)
{
    xt::svector<unsigned long> shape(origin.shape().begin(), origin.shape().end());
    shape.insert(shape.begin(), 1);
    xt::xarray<T> result = origin.reshape(shape);

    return result;
}  

template<typename DType, typename LType>
class DataLoader
{
public:
    class Iterator;  // Forward declaration
    class BWDIterator; // Forward declaration
private:
    Dataset<DType, LType>* ptr_dataset;
    int batch_size;
    bool shuffle;
    bool drop_last;
    int m_seed;

    DLinkedList<Batch<DType, LType>> batches;

public:
    // get amount of batches
    DLinkedList<Batch<DType, LType>>& getBatches()
    {
        return this->batches;
    }

    DataLoader(Dataset<DType, LType>* ptr_dataset,
            int batch_size,
            bool shuffle=true,
            bool drop_last=false,
            int seed = -1)
    {
        // Constructor
        this->batch_size = max(1, batch_size);
        this->shuffle = shuffle;
        this->drop_last = drop_last;
        this->ptr_dataset = ptr_dataset;
        this->m_seed = seed;

        int nsamples = ptr_dataset->len();

        int batch_amount = nsamples/this->batch_size;
        bool existed = (batch_amount!=0) ? true : false; // created for case nsamples less than batch size
        int modBatch = nsamples%this->batch_size;

        // batch_amount += (modBatch!=0 && !existed && !drop_last) ? 1 : 0;

        xt::xarray<DType> allData = ptr_dataset->getAllData();
        xt::xarray<LType> allLabel = ptr_dataset->getAllLabel();

        xt::xarray<unsigned long> indices = xt::arange<unsigned long>(0, nsamples, 1);

        /// SHUFFLE: shuffle dataset
        if (shuffle)
        {
            if (m_seed>=0) xt::random::seed(m_seed);
            xt::random::shuffle(indices);
        }

        /// BATCHES: divide dataset into batches
        /// DROPLAST: ignore last data samples 
        for (int i = 0; i<batch_amount; i++)
        {
            if (allLabel.shape()[0]!=allData.shape()[0] && allLabel.dimension()!=0) 
            {
                batches.add(Batch<DType, LType>());
                break;
            }

            int upper = 0;
            if (i== batch_amount-1)
            {
                upper = !drop_last ? nsamples : (i+1)*this->batch_size;
            }
            else if (i!=batch_amount-1)
            {
                upper = (i+1)*this->batch_size;
            }

            xt::xarray<DType> addData;
            xt::xarray<LType> addLabel; 
            // OLD WAY
                // = (allLabel.dimension()!=0) ?
                // xt::view(allLabel, xt::range(i*this->batch_size, upper)) : allLabel;

            for (unsigned long j = i*this->batch_size; j<upper; j++)
            {
                // Label
                DataLabel<DType, LType> item = ptr_dataset->getitem(indices(j));
                
                if (allLabel.dimension()!=0)
                {
                    xt::xarray<LType> label = fixedShape(item.getLabel());
                    addLabel = (j==i*this->batch_size) ? label : xt::concatenate(xt::xtuple(addLabel, label));
                }
                else {addLabel = allLabel;}
            
                xt::xarray<DType> data = fixedShape(item.getData());
                addData = (j==i*this->batch_size) ? data : xt::concatenate(xt::xtuple(addData, data));

                // cout << addData << "     " << addLabel << endl;
            }

            Batch<DType, LType> toCreate(addData, addLabel);
    
            batches.add(toCreate);
        } 

    }
    
    virtual ~DataLoader()
    {
    }
    
//////////////////////////////////////////////////////////////////////////
// The section for supportưing the iteration and for-each to DataLoader //
/// START: Section                                                      //
//////////////////////////////////////////////////////////////////////////

    // Forward Iterator
    static Iterator begin(DataLoader<DType, LType> loader)
    {
        return Iterator(&loader, true);
    }
    static Iterator end(DataLoader<DType, LType> loader)
    {
        return Iterator(&loader, false);
    }

    Iterator begin()
    {
        return Iterator(this, true);
    }
    Iterator end()
    {
        return Iterator(this, false);
    }

    // Backward Iterator
    BWDIterator bbegin()
    {
        return BWDIterator(this, true);
    }
    BWDIterator last()
    {
        return BWDIterator(this, true);
    }

    BWDIterator bend()
    {
        return BWDIterator(this, false);
    }
    BWDIterator beforeFirst()
    {
        return BWDIterator(this, false);
    }

public:
    /////////////////////////////////////////////////////////////////////
    class Iterator
    {
    private:
        int current;
        DataLoader<DType, LType> *loader;

    public:
        Iterator(DataLoader<DType, LType> *loader, bool begin = true)
        {
            if (begin)
            {
                this->current = 0;
            }
            else
            {
                this->current = loader->getBatches().size();
            }
            this->loader = loader;
        }

        Iterator &operator=(const Iterator &iterator)
        {
            this->current = iterator.current;
            return *this;
        }

        Batch<DType, LType> &operator*()
        {
            return loader->getBatches().get(this->current);
        }
        
        bool operator!=(const Iterator &iterator)
        {
            return this->current != iterator.current;
        }
        // Prefix ++ overload
        Iterator &operator++()
        {
            this->current++;
            return *this;
        }
        // Postfix ++ overload
        Iterator operator++(int)
        {
            Iterator iterator = *this;
            ++*this;
            return iterator;
        }
    };

    /////////////////////////////////////////////////////////////////////
    class BWDIterator
    {
    private:
        int current;
        DataLoader<DType, LType> *loader;

    public:
        BWDIterator(DataLoader<DType, LType> *loader, bool begin = true)
        {
            if (begin)
            {
                this->current = loader->getBatches().size();
            }
            else
            {
                this->current = 0;
            }
            this->loader = loader;
        }

        Iterator &operator=(const BWDIterator &iterator)
        {
            this->current = iterator.current;
            return *this;
        }

        Batch<DType, LType> &operator*()
        {
            return loader->getBatches().get(this->current);
        }
        
        bool operator!=(const Iterator &iterator)
        {
            return this->current != iterator.current;
        }
        // Prefix ++ overload
        Iterator &operator++()
        {
            this->current--;
            return *this;
        }
        // Postfix ++ overload
        Iterator operator++(int)
        {
            Iterator iterator = *this;
            ++*this;
            return iterator;
        }
    };
};
/////////////////////////////////////////////////////////////////////////
    
/////////////////////////////////////////////////////////////////////////
// The section for supporting the iteration and for-each to DataLoader //
/// END: Section                                                       //
/////////////////////////////////////////////////////////////////////////


#endif /* DATALOADER_H */

