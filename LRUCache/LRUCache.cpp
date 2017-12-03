// LRUCache.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <list>
#include <unordered_map>
#include <assert.h>

using namespace std;

class LRUCache
{
    struct Node
    {
        int key;
        int val;
        Node() :
            key(-1),
            val(-1)
        {}
        Node(int k, int v) :
            key(k),
            val(v)
        {}
    };

public:
    LRUCache(int capacity) :
        _capacity(capacity)
    {
        assert(capacity > 0);
    }

    // get(key) - Get the value(will always be positive) of the key if the key exists in the cache, otherwise return -1.
    // O(1) time complexity
    int get(int key)
    {
        auto lookupTableIter = _lookupTable.find(key);
        if (lookupTableIter == _lookupTable.end())
        {
            return -1;
        }

        assert(lookupTableIter->first == key);
        list<Node*>::iterator dataListIter = lookupTableIter->second;
        Node* mostRecentlyUsedNode = *dataListIter;
        int retVal = mostRecentlyUsedNode->val;

        //
        // move this Node to front of the list since it's now the most-recently used
        //
        // NOTE: for std::list::erase(): 
        //          Iterators, pointers and references referring to elements removed by the function are invalidated.
        //          All other iterators, pointers and references keep their validity.
        // So we don't need to update the other _dataList iterator entries from the _lookupTable
        //

        _dataList.erase(dataListIter);
        _dataList.push_front(mostRecentlyUsedNode);
        lookupTableIter->second = _dataList.begin();

        return retVal;
    }

    // put(key, value) - Set or insert the value if the key is not already present.When the cache reached its capacity, it should invalidate the least recently used item before inserting a new item.
    // O(1) time complexity
    void put(int key, int value)
    {
        bool keyFound = false;
        auto lookupTableIter = _lookupTable.find(key);
        if (lookupTableIter != _lookupTable.end())
        {
            keyFound = true;
        }

        if (keyFound == false)
        {
            if (_lookupTable.size() >= _capacity)
            {
                _invalidate();
            }
            Node* newNode = new Node(key, value);
            _dataList.push_front(newNode);
            auto dataListIter = _dataList.begin();
            _lookupTable.insert(pair<int, list<Node*>::iterator>(key, dataListIter));
        }
        else
        {
            // if the key is already found in the lookup table, update the value
            // but should we also mark it as most-recently used, i.e. move it to front of the _dataList?? Let's go with yes!
            auto dataListIter = lookupTableIter->second;
            Node* temp = *(dataListIter);
            delete temp;
            _dataList.erase(dataListIter);
            Node* updatedNode = new Node(key, value);
            _dataList.push_front(updatedNode);
            lookupTableIter->second = _dataList.begin();
        }
    }

private:
    int _capacity;
    list<Node*> _dataList;
    unordered_map<int, list<Node*>::iterator> _lookupTable;

    void _invalidate()
    {
        int lookupTableSizeBefore = _lookupTable.size();
        auto lookupTableIter = _lookupTable.begin();

        auto dataListLRUIter = _dataList.end();
        dataListLRUIter--;

        Node* nodeToDelete = *dataListLRUIter;
        int keyToDelete = nodeToDelete->key;
        delete nodeToDelete;
        _lookupTable.erase(keyToDelete);
        _dataList.erase(dataListLRUIter);

        assert(lookupTableSizeBefore == _lookupTable.size() + 1);
    }
};

int keyToValue(int key)
{
    const int c_valueConvertor = 100;
    return key * c_valueConvertor;
}

int main()
{
    const int c_cacheSize = 3;

    LRUCache lruc(c_cacheSize);

    int key = -1;
    int value = -1;
    int retVal = -1;

    key = 1;
    value = keyToValue(key);
    retVal = lruc.get(key);
    assert(retVal == -1);

    key = 1;
    value = keyToValue(key);
    lruc.put(key, value);
    retVal = lruc.get(-1);
    assert(retVal == -1);
    retVal = lruc.get(1);
    assert(retVal == keyToValue(1));
    retVal = lruc.get(2);
    assert(retVal == -1);
    retVal = lruc.get(3);
    assert(retVal == -1);
    retVal = lruc.get(4);
    assert(retVal == -1);

    key = 2;
    value = keyToValue(key);
    lruc.put(key, value);
    retVal = lruc.get(-1);
    assert(retVal == -1);
    retVal = lruc.get(1);
    assert(retVal == keyToValue(1));
    retVal = lruc.get(2);
    assert(retVal == keyToValue(2));
    retVal = lruc.get(3);
    assert(retVal == -1);
    retVal = lruc.get(4);
    assert(retVal == -1);

    key = 3;
    value = keyToValue(key);
    lruc.put(key, value);
    retVal = lruc.get(-1);
    assert(retVal == -1);
    retVal = lruc.get(1);
    assert(retVal == keyToValue(1));
    retVal = lruc.get(2);
    assert(retVal == keyToValue(2));
    retVal = lruc.get(3);
    assert(retVal == keyToValue(3));
    retVal = lruc.get(4);
    assert(retVal == -1);

    key = 4;
    value = keyToValue(key);
    lruc.put(key, value);
    retVal = lruc.get(-1);
    assert(retVal == -1);
    retVal = lruc.get(1);
    assert(retVal == -1);
    retVal = lruc.get(2);
    assert(retVal == keyToValue(2));
    retVal = lruc.get(3);
    assert(retVal == keyToValue(3));
    retVal = lruc.get(4);
    assert(retVal == keyToValue(4));

    return 0;
}

