/*
 * File:   DLinkedList.h
 */

#ifndef DLINKEDLIST_H
#define DLINKEDLIST_H

#include "list/IList.h"

#include <sstream>
#include <iostream>
#include <type_traits>
using namespace std;

template <class T>
class DLinkedList : public IList<T>
{
public:
    class Node;        // Forward declaration
    class Iterator;    // Forward declaration
    class BWDIterator; // Forward declaration

protected:
    Node *head; // this node does not contain user's data
    Node *tail; // this node does not contain user's data
    int count;
    bool (*itemEqual)(T &lhs, T &rhs);        // function pointer: test if two items (type: T&) are equal or not
    void (*deleteUserData)(DLinkedList<T> *); // function pointer: be called to remove items (if they are pointer type)

public:
    DLinkedList(
        void (*deleteUserData)(DLinkedList<T> *) = 0,
        bool (*itemEqual)(T &, T &) = 0);
    DLinkedList(const DLinkedList<T> &list);
    DLinkedList<T> &operator=(const DLinkedList<T> &list);
    ~DLinkedList();

    // Inherit from IList: BEGIN
    void add(T e);
    void add(int index, T e);
    T removeAt(int index);
    bool removeItem(T item, void (*removeItemData)(T) = 0);
    bool empty();
    int size();
    void clear();
    T &get(int index);
    int indexOf(T item);
    bool contains(T item);
    string toString(string (*item2str)(T &) = 0);
    // Inherit from IList: END

    void println(string (*item2str)(T &) = 0)
    {
        cout << toString(item2str) << endl;
    }
    void setDeleteUserDataPtr(void (*deleteUserData)(DLinkedList<T> *) = 0)
    {
        this->deleteUserData = deleteUserData;
    }

    bool contains(T array[], int size)
    {
        int idx = 0;
        for (DLinkedList<T>::Iterator it = begin(); it != end(); it++)
        {
            if (!equals(*it, array[idx++], this->itemEqual))
                return false;
        }
        return true;
    }

    /*
     * free(DLinkedList<T> *list):
     *  + to remove user's data (type T, must be a pointer type, e.g.: int*, Point*)
     *  + if users want a DLinkedList removing their data,
     *      he/she must pass "free" to constructor of DLinkedList
     *      Example:
     *      DLinkedList<T> list(&DLinkedList<T>::free);
     */
    static void free(DLinkedList<T> *list)
    {
        typename DLinkedList<T>::Iterator it = list->begin();
        while (it != list->end())
        {
            delete *it;
            it++;
        }
    }

    /* begin, end and Iterator helps user to traverse a list forwardly
     * Example: assume "list" is object of DLinkedList

    DLinkedList<char>::Iterator it;
    for(it = list.begin(); it != list.end(); it++){
            char item = *it;
            std::cout << item; //print the item
    }
     */
    Iterator begin()
    {
        return Iterator(this, true);
    }
    Iterator end() 
    {
        return Iterator(this, false);
    }

    /* last, beforeFirst and BWDIterator helps user to traverse a list backwardly
     * Example: assume "list" is object of DLinkedList

    DLinkedList<char>::BWDIterator it;
    for(it = list.last(); it != list.beforeFirst(); it--){
            char item = *it;
            std::cout << item; //print the item
    }
     */
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

protected:
    void checkIndex(int index);     // check validity of index for accessing
    void copyFunc(const DLinkedList<T> &list); // copy function
    void checkAddIndex(int index);
    bool canAdd();
    // void insert(int index, T e);

    static bool equals(T &lhs, T &rhs, bool (*itemEqual)(T &, T &))
    {
        if (itemEqual == 0)
            return lhs == rhs;
        else
            return itemEqual(lhs, rhs);
    }
    void copyFrom(const DLinkedList<T> &list);
    void removeInternalData();
    Node *getPreviousNodeOf(int index);

    //////////////////////////////////////////////////////////////////////
    ////////////////////////  INNER CLASSES DEFNITION ////////////////////
    //////////////////////////////////////////////////////////////////////
public:
    class Node
    {
    public:
        T data;
        Node *next;
        Node *prev;
        friend class DLinkedList<T>;

    public:
        Node(Node *next = 0, Node *prev = 0)
        {
            this->next = next;
            this->prev = prev;
        }
        Node(T data, Node *next = 0, Node *prev = 0)
        {
            this->data = data;
            this->next = next;
            this->prev = prev;
        }
    };

    //////////////////////////////////////////////////////////////////////
    class Iterator
    {
    private:
        DLinkedList<T> *pList;
        Node *pNode;

    public:
        Iterator(DLinkedList<T> *pList = 0, bool begin = true)
        {
            if (begin)
            {
                if (pList != 0)
                    this->pNode = pList->head->next;
                else
                    pNode = 0;
            }
            else
            {
                if (pList != 0)
                    this->pNode = pList->tail;
                else
                    pNode = 0;
            }
            this->pList = pList;
        }

        Iterator &operator=(const Iterator &iterator)
        {
            this->pNode = iterator.pNode;
            this->pList = iterator.pList;
            return *this;
        }
        void remove(void (*removeItemData)(T) = 0)
        {
            pNode->prev->next = pNode->next;
            pNode->next->prev = pNode->prev;
            Node *pNext = pNode->prev; 
            if (removeItemData != 0)
                removeItemData(pNode->data);
            delete pNode;
            pNode = pNext;
            pList->count -= 1;
        }

        T &operator*()
        {
            return pNode->data;
        }
        bool operator!=(const Iterator &iterator)
        {
            return pNode != iterator.pNode;
        }
        // Prefix ++ overload
        Iterator &operator++()
        {
            pNode = pNode->next;
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

    //////////////////////////////////////////////////////////////////////
    class BWDIterator
    {
    private:
        DLinkedList<T> *pList;
        Node *pNode;

    public:
        BWDIterator(DLinkedList<T> *pList = 0, bool begin = true)
        {
            if (begin)
            {
                if (pList != 0)
                    this->pNode = pList->tail->prev;
                else
                    pNode = 0;
            }
            else
            {
                if (pList != 0)
                    this->pNode = pList->head;
                else
                    pNode = 0;
            }
            this->pList = pList;
        }

        BWDIterator &operator=(const Iterator &iterator)
        {
            this->pNode = iterator.pNode;
            this->pList = iterator.pList;
            return *this;
        }
        void remove(void (*removeItemData)(T) = 0)
        {
            pNode->prev->next = pNode->next;
            pNode->next->prev = pNode->prev;
            Node *pNext = pNode->next; // MUST prev, so iterator++ will go to end
            if (removeItemData != 0)
                removeItemData(pNode->data);
            delete pNode;
            pNode = pNext;
            pList->count -= 1;
        }

        T &operator*()
        {
            return pNode->data;
        }
        bool operator!=(const BWDIterator &iterator)
        {
            return pNode != iterator.pNode;
        }
        // Prefix ++ overload
        BWDIterator &operator--()
        {
            pNode = pNode->prev;
            return *this;
        }
        // Postfix ++ overload
        BWDIterator operator--(int)
        {
            BWDIterator biterator = *this;
            --*this;
            return biterator;
        }
    };
};
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
////////////////////////     METHOD DEFNITION      ///////////////////
//////////////////////////////////////////////////////////////////////

template <class T>
DLinkedList<T>::DLinkedList(
    void (*deleteUserData)(DLinkedList<T> *),
    bool (*itemEqual)(T &, T &))
{
    this->deleteUserData = deleteUserData;
    this->itemEqual = itemEqual;
    this->head = new Node();
    this->tail = new Node();
    this->head->next = tail;
    this->tail->prev = head;

    this->count = 0;
}

template <class T>
DLinkedList<T>::DLinkedList(const DLinkedList<T> &list)
{
    copyFunc(list);
}

template <class T>
DLinkedList<T> &DLinkedList<T>::operator=(const DLinkedList<T> &list)
{
    copyFunc(list);
    return *this;
}

template <class T>
DLinkedList<T>::~DLinkedList()
{
    removeInternalData();
}

template <class T>
void DLinkedList<T>::add(T e)
{
    if (!canAdd()) return;

    Node * newNode = new Node(e);

    if (empty())
    {
        head->next = newNode;
        tail->prev = newNode;
        newNode->next = tail;
        newNode->prev = head;
    }
    else
    {
        Node * last = tail->prev;
        last->next = newNode;
        tail->prev = newNode;
        newNode->next = tail;
        newNode->prev = last;
    }

    count++;
}
template <class T>
void DLinkedList<T>::add(int index, T e)
{
    try 
    {
        checkAddIndex(index);
        if (!canAdd()) return;

        if (index == count) 
        {
            add(e);
        }
        else
        {
            Node * previousNode = getPreviousNodeOf(index);
            Node * newNode = new Node(e);

            if (previousNode == this->head) // index = 0
            {
                newNode->prev = head;
                (head->next)->prev = newNode;
                newNode->next = head->next;
                head->next = newNode;
            }
            else
            {
                newNode->next = previousNode->next;
                (previousNode->next)->prev = newNode;
                previousNode->next= newNode;
                newNode->prev = previousNode;
            }

            count++;
        }
    }
    catch (const std::out_of_range& e)
    {
        throw e;
    }
}

template <class T>
typename DLinkedList<T>::Node *DLinkedList<T>::getPreviousNodeOf(int index)
{
    /**
     * Returns the node preceding the specified index in the doubly linked list.
     * If the index is in the first half of the list, it traverses from the head; otherwise, it traverses from the tail.
     * Efficiently navigates to the node by choosing the shorter path based on the index's position.
     */
    int half = size()/2;

    if (index==0) return this->head;
    if (index==size()-1) return this->tail->prev->prev;

    Node * current = nullptr;
    if (index<=half)
    {
        current = head;
        for (int i = 0; i<index; i++)
        {
            current = current->next;
        }
        return current;
    }
    else
    {
        current = this->tail;
        for (int i = size()-1; i>index-1; i--)
        {
            current = current->prev;
        }
        return current->prev;
    }

    return current; 
}

template <class T>
T DLinkedList<T>::removeAt(int index)
{
    try 
    {
        checkIndex(index);
        T toReturn;

        Node * removeNode = getPreviousNodeOf(index+1);

        if (index == 0)
        {
            this->head->next = removeNode->next;
            (removeNode->next)->prev = this->head;
        }
        else if (index == this->count-1)
        {
            this->tail->prev = removeNode->prev;
            removeNode->prev->next = this->tail;
        }
        else
        {
            Node * previousRemove = getPreviousNodeOf(index);

            previousRemove->next = removeNode->next;
            removeNode->next->prev = previousRemove;
            removeNode->next = nullptr;
            removeNode->prev = nullptr;
        }

        toReturn = removeNode->data;
        delete removeNode;
        count--;

        if (count == 0)
        {
            tail->prev = head;
            head->next = tail;
        }
        return toReturn;
    }
    catch (const std::out_of_range& e)
    {
        throw e;
    }
}

template <class T>
bool DLinkedList<T>::empty()
{
    return (this->count==0);
}

template <class T>
int DLinkedList<T>::size()
{
    return this->count;
}

template <class T>
void DLinkedList<T>::clear()
{
    if (this->deleteUserData!=nullptr)
    {
        (*this->deleteUserData)(this);
    }
    else
    {
        int size = this->count;
        for (int i = 0; i<size; i++)
        {
            removeAt(0);
        }
    }
    this->count = 0;
    head->next = tail;
    tail->prev = head;
}

template <class T>
T &DLinkedList<T>::get(int index)
{
    try
    {
        checkIndex(index);

        Node * current = this->head;
        for (int i = 0; i<=index; i++)
        {
            current = current->next;
        }
        return current->data; 
    }
    catch (const std::out_of_range& e)
    {
        throw e;
    }
}

template <class T>
int DLinkedList<T>::indexOf(T item)
{
    int index = 0;
    for (Iterator it = begin(); it!=end(); it++)
    {
        if (itemEqual!=nullptr)
        {
            if ((*itemEqual)(item, *it)) return index;
        }
        else
        {
            if (item == *it) return index;
        }
        index++;
    }

    return -1;
}

template <class T>
bool DLinkedList<T>::removeItem(T item, void (*removeItemData)(T))
{
    int index = indexOf(item);
    if (index==-1) return false;

    Node * removeNode = getPreviousNodeOf(index+1);

    if (index == 0)
    {
        head->next = removeNode->next;
        (removeNode->next)->prev = head;
    }
    else if (index == this->count-1)
    {
        tail->prev = removeNode->prev;
        removeNode->prev->next = tail;
    }
    else
    {
        Node * previousRemove = getPreviousNodeOf(index);

        previousRemove->next = removeNode->next;
        removeNode->next->prev = previousRemove;
        removeNode->next = nullptr;
        removeNode->prev = nullptr;
    }

    if (removeItemData!=nullptr) (*removeItemData)(removeNode->data);
    delete removeNode;
    count--;

    if (count == 0)
    {
        tail->prev = head;
        head->next = tail;
    }

    return true;
}

template <class T>
bool DLinkedList<T>::contains(T item)
{
    return (indexOf(item)!=-1);
}

template <class T>
string DLinkedList<T>::toString(string (*item2str)(T &))
{
    /**
     * Converts the list into a string representation, where each element is formatted using a user-provided function.
     * If no custom function is provided, it directly uses the element's default string representation.
     * Example: If the list contains {1, 2, 3} and the provided function formats integers, calling toString would return "[1, 2, 3]".
     *
     * @param item2str A function that converts an item of type T to a string. If null, default to string conversion of T.
     * @return A string representation of the list with elements separated by commas and enclosed in square brackets.
     */
    // TODO
    std::stringstream ss;
    ss << "[";

    for (int i = 0; i<count; i++)
    {
        if (item2str!=nullptr)
        {
            ss << item2str(get(i));
        }
        else
        {
            ss << get(i);
        }

        if (i < count-1) ss << ", ";
    }

    ss << "]";
    return ss.str();
}

template <class T>
void DLinkedList<T>::copyFrom(const DLinkedList<T> &list) 
{
    /**
     * Copies the contents of another doubly linked list into this list.
     * Initializes the current list to an empty state and then duplicates all data and pointers from the source list.
     * Iterates through the source list and adds each element, preserving the order of the nodes.
     */
    clear();
    this->deleteUserData = list->deleteUserData;
    this->itemEqual = list->itemEqual;
    this->count = list->count;
    this->head = list->head;
    this->tail = list->tail;
}

template <class T>
void DLinkedList<T>::removeInternalData()
{
    /**
     * Clears the internal data of the list by deleting all nodes and user-defined data.
     * If a custom deletion function is provided, it is used to free the user's data stored in the nodes.
     * Traverses and deletes each node between the head and tail to release memory.
     */
    clear();
    delete this->head;
    delete this->tail;
}

// (private method definition)
template <class T>
void DLinkedList<T>::checkIndex(int index)
{
    /**
     * Validates whether the given index is within the valid range of the list.
     * Throws an std::out_of_range exception if the index is negative or exceeds the number of elements.
     * Ensures safe access to the list's elements by preventing invalid index operations.
     */
    /*
    * in other function (except add) index must be [0; count-1]
    */

    if (index>=0 && index<=this->count-1) return;
    throw std::out_of_range("Index is out of range!");
}

template <class T>
void DLinkedList<T>::checkAddIndex(int index)
{
    /**
     * in add function index must be [0; count]
     */
    if (index>=0 && index<=this->count) return;
    throw std::out_of_range("Index is out of range!");
}

template <class T>
void DLinkedList<T>::copyFunc(const DLinkedList<T> &list) 
{
    this->deleteUserData = list.deleteUserData;
    this->itemEqual = list.itemEqual;
    this->head = nullptr;
    this->tail = nullptr;
    this->count = list.count;

    DLinkedList<T>::Node * temp = list.head;
    while (temp!=list.tail)
    {
        temp = temp->next;
        add(temp->data);
    }
}

template <class T>
bool DLinkedList<T>::canAdd()
{
    try {
        Node * newNode = new Node();
        bool toReturn = (newNode!=nullptr);
        delete newNode;
        return toReturn;
    }
    catch (const std::bad_alloc& e)
    {
        return false;
    }
}

#endif /* DLINKEDLIST_H */
