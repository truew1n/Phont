#ifndef PT_DEQUE_H
#define PT_DEQUE_H

#include <iostream>
#include <stdexcept>

template <typename T>
class TNode {
public:
    TNode *Back;
    TNode *Front;
    T Value;

    TNode(T Value) : Back(nullptr), Front(nullptr), Value(Value) {}
};

template <typename T>
class TDeque {
private:
    TNode<T> *Head;
    TNode<T> *Tail;
    uint32_t Size;

public:
    TDeque() : Head(nullptr), Tail(nullptr), Size(0) {}

    void PushFront(T Value)
    {
        TNode<T> *NewNode = new TNode<T>(Value);
        if (IsEmpty()) {
            Head = Tail = NewNode;
        }
        else {
            NewNode->Front = Head;
            Head->Back = NewNode;
            Head = NewNode;
        }
        Size++;
    }

    void PushBack(T Value)
    {
        TNode<T> *NewNode = new TNode<T>(Value);
        if (IsEmpty()) {
            Head = Tail = NewNode;
        }
        else {
            NewNode->Back = Tail;
            Tail->Front = NewNode;
            Tail = NewNode;
        }
        Size++;
    }

    void PopFront()
    {
        if (IsEmpty()) {
            std::cerr << "Deque is empty\n";
            return;
        }
        TNode<T> *Temp = Head;
        Head = Head->Front;
        if (Head) {
            Head->Back = nullptr;
        }
        else {
            Tail = nullptr;
        }
        delete Temp;
        Size--;
    }

    void PopBack()
    {
        if (IsEmpty()) {
            std::cerr << "Deque is empty\n";
            return;
        }
        TNode<T> *Temp = Tail;
        Tail = Tail->Back;
        if (Tail) {
            Tail->Front = nullptr;
        }
        else {
            Head = nullptr;
        }
        delete Temp;
        Size--;
    }

    TNode<T> *GetHead()
    {
        return Head;
    }

    TNode<T> *GetTail()
    {
        return Tail;
    }

    T Front() const
    {
        if (IsEmpty()) {
            throw std::runtime_error("Deque is empty");
        }
        return Head->Value;
    }

    T Back() const
    {
        if (IsEmpty()) {
            throw std::runtime_error("Deque is empty");
        }
        return Tail->Value;
    }

    bool IsEmpty() const
    {
        return Size == 0;
    }

    uint32_t GetSize() const
    {
        return Size;
    }

    ~TDeque()
    {
        while (!IsEmpty()) {
            PopFront();
        }
    }
};

#endif
