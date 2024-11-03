#ifndef PT_HASHMAP_H
#define PT_HASHMAP_H

#include <iostream>
#include <cstdint>
#include <utility>
#include <functional>
#include "Deque.h"

#define DEFAULT_CAPACITY 1000

template <typename K, typename V>
class THashMap {
private:
    TDeque<std::pair<K, V>> *Buckets;
    uint32_t Size;
    uint32_t Capacity;

    uint32_t Hash(K Key) const
    {
        return std::hash<K>{}(Key) % Capacity;
    }

    void Rehash()
    {
        uint32_t OldCapacity = Capacity;
        Capacity *= 2;
        TDeque<std::pair<K, V>> *NewBuckets = new TDeque<std::pair<K, V>>[Capacity];

        for (uint32_t I = 0; I < OldCapacity; I++) {
            while (!Buckets[I].IsEmpty()) {
                auto Pair = Buckets[I].Front();
                Buckets[I].PopFront();
                uint32_t NewIndex = Hash(Pair.first);
                NewBuckets[NewIndex].PushBack(Pair);
            }
        }

        delete[] Buckets;
        Buckets = NewBuckets;
    }

public:
    THashMap() : Size(0), Capacity(DEFAULT_CAPACITY)
    {
        Buckets = new TDeque<std::pair<K, V>>[Capacity];
    }

    ~THashMap()
    {
        delete[] Buckets;
    }

    void Insert(K Key, V Value)
    {
        uint32_t Index = Hash(Key);
        TNode<std::pair<K, V>> *Current = Buckets[Index].GetHead();
        while (Current != nullptr) {
            if (Current->Value.first == Key) {
                Current->Value.second = Value;
                return;
            }
            Current = Current->Back;
        }
        Buckets[Index].PushBack(std::make_pair(Key, Value));
        Size++;

        if (Size > Capacity * 0.7) {
            Rehash();
        }
    }

    bool Remove(K Key)
    {
        uint32_t Index = Hash(Key);
        TNode<std::pair<K, V>> *Current = Buckets[Index].GetHead();
        while (Current != nullptr) {
            if (Current->Value.first == Key) {
                Buckets[Index].PopFront();
                Size--;
                return true;
            }
            Current = Current->Back;
        }
        return false;
    }

    bool Find(K Key, V *Value) const
    {
        uint32_t Index = Hash(Key);
        TNode<std::pair<K, V>> *Current = Buckets[Index].GetHead();
        while (Current != nullptr) {
            if (Current->Value.first == Key) {
                *Value = Current->Value.second;
                return true;
            }
            Current = Current->Back;
        }
        return false;
    }

    V operator[](K Key)
    {
        uint32_t Index = Hash(Key);
        TNode<std::pair<K, V>> *Current = Buckets[Index].GetHead();

        while (Current != nullptr) {
            if (Current->Value.first == Key) {
                return Current->Value.second;
            }
            Current = Current->Back;
        }

        Buckets[Index].PushBack(std::make_pair(Key, V{}));
        Size++;

        if (Size > Capacity * 0.7) {
            Rehash();
        }

        return Buckets[Index].GetTail()->Value.second;
    }

    uint32_t GetSize() const
    {
        return Size;
    }

    bool IsEmpty() const
    {
        return Size == 0;
    }
};

#endif
