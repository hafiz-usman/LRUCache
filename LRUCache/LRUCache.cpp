// LRUCache.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <list>
#include <unordered_map>
#include <memory>
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
        auto iterLookupTable = _lookupTable.find(key);
        if (iterLookupTable == _lookupTable.end())
        {
            return -1;
        }

        assert(iterLookupTable->first == key);
        list<unique_ptr<Node>>::iterator iterDataList = iterLookupTable->second;
        int retVal = iterDataList->get()->val;
        //
        // move this Node to front of the list since it's now the most-recently used
        //
        // NOTE: for std::list::erase(): 
        //          Iterators, pointers and references referring to elements removed by the function are invalidated.
        //          All other iterators, pointers and references keep their validity.
        // So we don't need to update the other _dataList iterator entries from the _lookupTable
        //

        _dataList.erase(iterDataList);
        _dataList.push_front(make_unique<Node>(key, retVal));
        iterLookupTable->second = _dataList.begin();

        return retVal;
    }

    // put(key, value) - Set or insert the value if the key is not already present.When the cache reached its capacity, it should invalidate the least recently used item before inserting a new item.
    // O(1) time complexity
    void put(int key, int value)
    {
        bool keyFound = false;
        auto iterLookupTable = _lookupTable.find(key);
        if (iterLookupTable != _lookupTable.end())
        {
            keyFound = true;
        }

        if (keyFound == false)
        {
            if (_lookupTable.size() >= _capacity)
            {
                _invalidate();
            }
            _dataList.push_front(make_unique<Node>(key, value));
            auto dataListIter = _dataList.begin();
            _lookupTable.insert(pair<int, list<unique_ptr<Node>>::iterator>(key, dataListIter));
        }
        else
        {
            // if the key is already found in the lookup table, update the value
            // but should we also mark it as most-recently used, i.e. move it to front of the _dataList?? Let's go with yes!
            auto iterDataList = iterLookupTable->second;
            _dataList.erase(iterDataList);
            _dataList.push_front(make_unique<Node>(key, value));
            iterLookupTable->second = _dataList.begin();
        }
    }

private:
    int _capacity;
    list<unique_ptr<Node>> _dataList;
    unordered_map<int, list<unique_ptr<Node>>::iterator> _lookupTable;

    void _invalidate()
    {
        int lookupTableSizeBefore = _lookupTable.size();

        // we can't use rbegin() because later on we have to call erase on _dataList. erase() only takes forward iterators!
        //auto iterDataList = _dataList.rbegin();
        
        auto iterDataList = _dataList.end();
        --iterDataList;

        int keyToDelete = iterDataList->get()->key;
        _lookupTable.erase(keyToDelete);
        _dataList.erase(iterDataList);

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

