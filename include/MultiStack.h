#pragma once
#include"Stack.h"
template <class T>
class TMultiStack
{
private:
    T** data;           
    TStack<T>* stacks;  
    int len;            
    int StackCount;     
    int* Starts;
    void Repack(int index);
public:
   
    TMultiStack();
    TMultiStack(int totalLen, int stackCount);
    TMultiStack(const TMultiStack& obj);
    TMultiStack(TMultiStack&& obj);
    ~TMultiStack();

    
    int GetLen() const;
    int GetStackCount() const;
    

    
    void Push(int stackIndex, T value);
    T Pop(int stackIndex);
    bool IsEmpty(int stackIndex) const;
    bool IsFull(int stackIndex) const;

    
    

    
    TMultiStack& operator=(const TMultiStack& obj);
    TMultiStack& operator=(TMultiStack&& obj);
    bool operator==(const TMultiStack& obj) const;
    bool operator!=(const TMultiStack& obj) const;

    
    template <class O>
    friend std::ostream& operator<<(std::ostream& o, const TMultiStack<O>& v);
    template <class I>
    friend std::istream& operator>>(std::istream& i, TMultiStack<I>& v);

    class Iterator
    {
    private:
        TMultiStack<T>* multiStack;
        int stackIndex;
        int elementIndex;

    public:
        Iterator(TMultiStack<T>* ms, int stackIdx, int elemIdx)
            : multiStack(ms), stackIndex(stackIdx), elementIndex(elemIdx) 
        {
        }

        Iterator& operator++()
        {
            if (stackIndex >= multiStack->StackCount)
                return *this;

            elementIndex++;

            while (stackIndex < multiStack->StackCount &&
                elementIndex >= multiStack->stacks[stackIndex].GetLen())
            {
                stackIndex++;
                elementIndex = 0;
            }

            return *this;
        }

        Iterator operator++(int)
        {
            Iterator temp = *this;
            ++(*this);
            return temp;
        }

        T& operator*()
        {
            if (stackIndex >= multiStack->StackCount)
                throw std::out_of_range("Iterator out of range");

            T** stackData = multiStack->stacks[stackIndex].data;
            if (elementIndex >= multiStack->stacks[stackIndex].GetLen() ||
                stackData[elementIndex] == nullptr)
            {
                throw std::out_of_range("Invalid element access");
            }

            return *(stackData[elementIndex]);
        }

        T* operator->()
        {
            return &(**this);
        }

        bool operator==(const Iterator& other) const
        {
            return multiStack == other.multiStack &&
                stackIndex == other.stackIndex &&
                elementIndex == other.elementIndex;
        }

        bool operator!=(const Iterator& other) const
        {
            return !(*this == other);
        }
    };
};




template <class T>
TMultiStack<T>::TMultiStack() : data(nullptr), stacks(nullptr), len(0), StackCount(0), Starts(nullptr)
{}

template<class T>
inline TMultiStack<T>::TMultiStack(int size_, int stackCount_) 
{
    if (size_ < 0 || stackCount_ < 0)
        throw std::invalid_argument("bad size");
    data = nullptr;
    stacks = nullptr;
    len = 0;
    StackCount = 0;
    Starts = nullptr;
    if (size_ == 0 || stackCount_ == 0)
    {
        return;
    }
    else
    {
        len = size_;
        StackCount = stackCount_;
        data = new T * [len];
        for (int i = 0; i < len; i++)
            data[i] = nullptr;
        Starts = new int[StackCount];
        Starts[0] = 0;
        int stackSize = int(len / StackCount);
        for (int i = 1; i < StackCount; i++)
            Starts[i] = Starts[i - 1] + stackSize;
        stacks = new TStack<T>[StackCount];
        for (int i = 0; i < StackCount - 1; i++)
            stacks[i].SetData(data + Starts[i], stackSize);
        stacks[StackCount - 1].SetData(data + Starts[StackCount - 1], stackSize + (len - StackCount * stackSize));
    }
}

template <class T>
TMultiStack<T>::TMultiStack(const TMultiStack& obj)
{
    data = nullptr;
    stacks = nullptr;
    len = 0;
    StackCount = 0;
    Starts = nullptr;
    
    if (obj.len == 0 || obj.StackCount == 0)
    {
        return;  
    }
    else
    {
        len = obj.len;
        StackCount = obj.StackCount;
        data = new T * [len];
        for (int i = 0; i < len; i++)
        {
            if (obj.data[i] != nullptr)  
            {
                data[i] = new T(*(obj.data[i]));  
            }
            else
            {
                data[i] = nullptr;  
            }
        }
        Starts = new int[StackCount];
        for (int i = 0; i < StackCount; i++)
            Starts[i] = obj.Starts[i];
        stacks = new TStack<T>[StackCount];
        for (int i = 0; i < StackCount - 1; i++)
            stacks[i].SetData(data + Starts[i], Starts[i + 1] - Starts[i]);
        stacks[StackCount - 1].SetData(data + Starts[StackCount - 1], len - Starts[StackCount - 1]);
    }
}

template <class T>
TMultiStack<T>::TMultiStack(TMultiStack&& obj)
{
    data = obj.data;
    stacks = obj.stacks;
    len = obj.len;
    StackCount = obj.StackCount;
    Starts = obj.Starts;

	obj.Starts = nullptr;
    obj.data = nullptr;
    obj.stacks = nullptr;
    obj.len = 0;
    obj.StackCount = 0;
}

template<class T>
inline TMultiStack<T>::~TMultiStack()
{
    if (data != nullptr)
    {
        for (int i = 0; i < len; i++)
            delete data[i];

        delete[] data;
        delete[] stacks;
        delete[] Starts;

        data = nullptr;
        len = 0;
        stacks = nullptr;
        StackCount = 0;
        Starts = nullptr;
    }
}

template <class T>
int TMultiStack<T>::GetLen() const
{
    return len;
}

template <class T>
int TMultiStack<T>::GetStackCount() const
{
    return StackCount;
}

template <class T>
void TMultiStack<T>::Push(int stackIndex, T value)
{
    if (stackIndex < 0 || stackIndex >= StackCount)
        throw std::out_of_range("invalid stack index");

	if (stacks == nullptr)
		throw std::logic_error("MultiStack is not initialized");

    if (IsFull(stackIndex))
    {
        Repack(stackIndex);
    }

    stacks[stackIndex].Push(value);
}

template <class T>
T TMultiStack<T>::Pop(int stackIndex)
{
    if (stackIndex < 0 || stackIndex >= StackCount)
        throw std::out_of_range("invalid stack index");

	if (stacks == nullptr)
		throw std::logic_error("MultiStack is not initialized");

    if (IsEmpty(stackIndex))
        throw std::logic_error("stack is empty");

    return stacks[stackIndex].Pop();
}

template <class T>
bool TMultiStack<T>::IsEmpty(int stackIndex) const
{
    if (stackIndex < 0 || stackIndex >= StackCount)
        throw std::out_of_range("invalid stack index");
    if (stacks == nullptr)
        throw std::logic_error("MultiStack is not initialized");
    return stacks[stackIndex].IsEmpty();
}

template <class T>
bool TMultiStack<T>::IsFull(int stackIndex) const
{
    if (stackIndex < 0 || stackIndex >= StackCount)
        throw std::out_of_range("invalid stack index");
    if (stacks == nullptr)
        throw std::logic_error("MultiStack is not initialized");
    return stacks[stackIndex].isFull();
}

template <class T>
inline void TMultiStack<T>::Repack(int curStack)
{
    int* newStart = new int[StackCount];
    int* newSizes = new int[StackCount];
    int busy = 0;

    for (int i = 0; i < StackCount; i++)
        busy += stacks[i].GetTop();

    int freeCells = len - busy;

    if (freeCells == 0) return;

    int ns = int(freeCells / StackCount);
    int tail = freeCells - ns * StackCount;

    newStart[0] = 0;
    newSizes[0] = stacks[0].GetTop() + ns;

    for (int i = 1; i < StackCount; i++)
    {
        newSizes[i] = stacks[i].GetTop() + ns;
        if (i == curStack)
            newSizes[i] += tail;
        newStart[i] = newStart[i - 1] + newSizes[i - 1];
    }

    for (int i = 0; i < StackCount; i++)
    {
        if (newStart[i] == Starts[i])
            continue;

        else if (newStart[i] < Starts[i])
        {
            int j = 0, k = 0, n = 0;
            for (j = Starts[i], k = newStart[i], n = 0; n < stacks[i].GetTop(); j++, k++, n++)
            {
                data[k] = data[j];
            }
            for (; k < Starts[i] + stacks[i].GetTop(); k++)
                data[k] = nullptr;
        }

        else if (newStart[i] > Starts[i])
        {
            int q = i;
            while (newStart[q] > Starts[q])
                q++;

            for (int w = q - 1; w >= i; w--)
            {
                int j = 0, k = 0, n = 0;
                for (j = Starts[w] + stacks[w].GetTop() - 1, k = newStart[w] + stacks[w].GetTop() - 1, n = stacks[w].GetTop() - 1; n >= 0; k--, j--, n--)
                {
                    data[k] = data[j];
                }
                for (; k >= Starts[w]; k--)
                    data[k] = nullptr;
            }
            i = q - 1;
        }
    }

    for (int i = 0; i < StackCount; i++)
    {
        stacks[i].SetData(&(data[newStart[i]]), newSizes[i]);
    }
}




template <class T>
TMultiStack<T>& TMultiStack<T>::operator=(const TMultiStack& obj)
{
    if (this == &obj)
        return *this;

    
    if (data != nullptr)
    {
        for (int i = 0; i < len; i++)
        {
            if (data[i] != nullptr)
            {
                delete data[i];
            }
        }
        delete[] data;
    }

    if (stacks != nullptr)
    {
        delete[] stacks;
    }

    
    len = obj.len;
    StackCount = obj.StackCount;

    if (obj.len > 0 && obj.StackCount > 0)
    {
        data = new T * [len];
        for (int i = 0; i < len; i++)
        {
            data[i] = nullptr;
        }

       
        for (int i = 0; i < len; i++)
        {
            if (obj.data[i] != nullptr)
            {
                data[i] = new T(*(obj.data[i]));
            }
        }

        stacks = new TStack<T>[StackCount];
        for (int i = 0; i < StackCount; i++)
        {
            stacks[i] = obj.stacks[i];
        }
    }
    else
    {
        data = nullptr;
        stacks = nullptr;
    }

    return *this;
}

template <class T>
TMultiStack<T>& TMultiStack<T>::operator=(TMultiStack&& obj)
{
    if (this == &obj)
        return *this;

    
    if (data != nullptr)
    {
        for (int i = 0; i < len; i++)
        {
            if (data[i] != nullptr)
            {
                delete data[i];
            }
        }
        delete[] data;
    }

    if (stacks != nullptr)
    {
        delete[] stacks;
    }

    
    data = obj.data;
    stacks = obj.stacks;
    len = obj.len;
    StackCount = obj.StackCount;

    obj.data = nullptr;
    obj.stacks = nullptr;
    obj.len = 0;
    obj.StackCount = 0;

    return *this;
}

template <class T>
bool TMultiStack<T>::operator==(const TMultiStack& obj) const
{
    if (len != obj.len || StackCount != obj.StackCount)
        return false;

    for (int i = 0; i < StackCount; i++)
    {
        if (stacks[i] != obj.stacks[i])
            return false;
    }

    return true;
}

template <class T>
bool TMultiStack<T>::operator!=(const TMultiStack& obj) const
{
    return !(*this == obj);
}

template <class O>
std::ostream& operator<<(std::ostream& o, const TMultiStack<O>& v)
{
    o << "TMultiStack[totalLen=" << v.len << ", StackCount=" << v.StackCount << "]\n";
    for (int i = 0; i < v.StackCount; i++)
    {
        o << "Stack " << i << ":\n";
        // Используем оператор вывода TStack
        o << v.stacks[i];
    }
    return o;
}

template <class I>
std::istream& operator>>(std::istream& i, TMultiStack<I>& v)
{
    int totalLen, stackCount;
    i >> totalLen >> stackCount;

    if (!i.good())
        return i;

    if (totalLen < 0 || stackCount < 0)
        throw std::invalid_argument("length or stack count cannot be negative");

    
    TMultiStack<I> temp(totalLen, stackCount);

    
    for (int stackIdx = 0; stackIdx < stackCount && i.good(); stackIdx++)
    {
        i >> temp.stacks[stackIdx];
    }

    v = std::move(temp);
    return i;
}
