#ifndef _HASHTABLE_
#define _HASHTABLE_

#include "HashUtils.h"

// Do not modify the public interface of this class.
// Otherwise, your code will note compile!
template <class T>
class HashTable {
    struct Entry {
        std::string Key;             // the key of the entry
        T Value;   // the value of the entry
        bool Deleted;        // flag indicating whether this entry is deleted
        bool Active;         // flag indicating whether this item is currently used

        Entry() : Key(), Value(), Deleted(false), Active(false) {}
    };

    struct Bucket {
        Entry entries[3];
    };

    int _capacity; // INDICATES THE SIZE OF THE TABLE
    int _size; // INDICATES THE NUMBER OF ITEMS IN THE TABLE

    Bucket* _table; // HASH TABLE
    
    // You can define private methods and variables
    int total_probes;

public:
    // TODO: IMPLEMENT THESE FUNCTIONS.
    // CONSTRUCTORS, ASSIGNMENT OPERATOR, AND THE DESTRUCTOR
    HashTable();
    HashTable(const HashTable<T>& rhs);
    HashTable<T>& operator=(const HashTable<T>& rhs);
    ~HashTable();

    // TODO: IMPLEMENT THIS FUNCTION.
    // INSERT THE ENTRY IN THE HASH TABLE WITH THE GIVEN KEY & VALUE
    // IF THE GIVEN KEY ALREADY EXISTS, THE NEW VALUE OVERWRITES
    // THE ALREADY EXISTING ONE. 
    // IF LOAD FACTOR OF THE TABLE IS BIGGER THAN 0.5,
    // RESIZE THE TABLE WITH THE NEXT PRIME NUMBER.
    void Insert(std::string key, const T& value);

    // TODO: IMPLEMENT THIS FUNCTION.
    // DELETE THE ENTRY WITH THE GIVEN KEY FROM THE TABLE
    // IF THE GIVEN KEY DOES NOT EXIST IN THE TABLE, JUST RETURN FROM THE FUNCTION
    // HINT: YOU SHOULD UPDATE ACTIVE & DELETED FIELDS OF THE DELETED ENTRY.
    void Delete(std::string key);

    // TODO: IMPLEMENT THIS FUNCTION.
    // IT SHOULD RETURN THE VALUE THAT CORRESPONDS TO THE GIVEN KEY.
    // IF THE KEY DOES NOT EXIST, THIS FUNCTION MUST RETURN T()
    T Get(std::string key) const;

    // TODO: IMPLEMENT THIS FUNCTION.
    // AFTER THIS FUNCTION IS EXECUTED THE TABLE CAPACITY MUST BE
    // EQUAL TO newCapacity AND ALL THE EXISTING ITEMS MUST BE REHASHED
    // ACCORDING TO THIS NEW CAPACITY.
    // WHEN CHANGING THE SIZE, YOU MUST REHASH ALL OF THE ENTRIES FROM 0TH ENTRY TO LAST ENTRY
    void Resize(int newCapacity);
    
    // TODO: IMPLEMENT THIS FUNCTION.
    // RETURNS THE AVERAGE NUMBER OF PROBES FOR SUCCESSFUL SEARCH
    double getAvgSuccessfulProbe();
    
    // TODO: IMPLEMENT THIS FUNCTION.
    // RETURNS THE AVERAGE NUMBER OF PROBES FOR UNSUCCESSFUL SEARCH
    double getAvgUnsuccessfulProbe();

    // THE IMPLEMENTATION OF THESE FUNCTIONS ARE GIVEN TO YOU
    // DO NOT MODIFY!
    int Capacity() const;
    int Size() const;
};



template <class T>
HashTable<T>::HashTable() {
// TODO: CONSTRUCTOR
    _table = NULL;
    _capacity = 0;
    _size = 0;
    total_probes = 0;

}

template <class T>
HashTable<T>::HashTable(const HashTable<T>& rhs) {
// TODO: COPY CONSTRUCTOR
    this->_capacity = rhs._capacity;
    this->_size = rhs._size;
    _table = new Bucket[_capacity];
    
    for(int i = 0; i < rhs._capacity; i++) {
        for(int j = 0; j < 3; j++)
            _table[i].entries[j] = rhs._table[i].entries[j];
    }
}

template <class T>
HashTable<T>& HashTable<T>::operator=(const HashTable<T>& rhs) {
// TODO: OPERATOR=
    if(this != &rhs) {
        delete[] _table;

        _capacity = rhs._capacity;
        _size = rhs._size;
        _table = new Bucket[_capacity];

        for(int i = 0; i < rhs._capacity; i++) {
            for(int j = 0; j < 3; j++)
                _table[i].entries[j] = rhs._table[i].entries[j];
        }
    }

    return *this;
}


template <class T>
HashTable<T>::~HashTable() {
// TODO: DESTRUCTOR
    delete[] _table;
    _size = 0;
    total_probes = 0;
    _capacity = 0;
}

template <class T>
void HashTable<T>::Insert(std::string key, const T& value) {
// TODO: IMPLEMENT THIS FUNCTION.
// INSERT THE ENTRY IN THE HASH TABLE WITH THE GIVEN KEY & VALUE
// IF THE GIVEN KEY ALREADY EXISTS, THE NEW VALUE OVERWRITES
// THE ALREADY EXISTING ONE. IF LOAD FACTOR OF THE TABLE IS BIGGER THAN 0.5,
// RESIZE THE TABLE WITH THE NEXT PRIME NUMBER.
    if(_capacity == 0) {
        _capacity = NextCapacity(0);
        _table = new Bucket[NextCapacity(0)];
    }

    if((double) _size / (_capacity * 3.0) > 0.5) {
        int nextCapacity = NextCapacity(_capacity);
        Resize(nextCapacity);
        total_probes = 0;
    }
    
    int hashKey = Hash(key)%_capacity;
    int probe = 1;
    for (int i = 0; i < 3; i++) {
        if (!_table[hashKey].entries[i].Active) {
            _table[hashKey].entries[i].Key = key;
            _table[hashKey].entries[i].Value = value;
            _table[hashKey].entries[i].Active = true;
            _table[hashKey].entries[i].Deleted = false;
            _size++;
            total_probes++;
            break;
        }
        else if (_table[hashKey].entries[i].Key == key) {
            _table[hashKey].entries[i].Value = value;
            total_probes++;
            break;
        }
        probe++;
        total_probes++;
        i = 0;
        hashKey = (Hash(key)%_capacity + probe * probe) %_capacity;
    }
}

template <class T>
void HashTable<T>::Delete(std::string key) {
// TODO: IMPLEMENT THIS FUNCTION.
// DELETE THE ENTRY WITH THE GIVEN KEY FROM THE TABLE
// IF THE GIVEN KEY DOES NOT EXIST IN THE TABLE, JUST RETURN FROM THE FUNCTION
// HINT: YOU SHOULD UPDATE ACTIVE & DELETED FIELDS OF THE DELETED ENTRY.
    if(_capacity == 0) {
        return;
    }


    int hashKey = Hash(key) % _capacity;
    int probe = 1;
    
    for (int i = 0; i < 3; ++i) {
        if (_table[hashKey].entries[i].Active == true) {
            if(_table[hashKey].entries[i].Key == key) {
                _table[hashKey].entries[i].Active = false;
                _table[hashKey].entries[i].Deleted = true;
                _size--;
                break;
            }

            probe++;
            hashKey = (hashKey + probe * probe) %_capacity;
            
        }
        if (_table[hashKey].entries[i].Active == false) {
            if(_table[hashKey].entries[i].Deleted == false) {
                break;
            }
            probe++;
            hashKey = (hashKey + probe * probe) %_capacity;
        }
        
    }
    
    
}

template <class T>
T HashTable<T>::Get(std::string key) const {
// TODO: IMPLEMENT THIS FUNCTION. IT SHOULD RETURN THE VALUE THAT
// IT SHOULD RETURN THE VALUE THAT CORRESPONDS TO THE GIVEN KEY.
// IF THE KEY DOES NOT EXIST, THIS FUNCTION MUST RETURN T()
    if(_capacity == 0) {
        return T();
    }

    int hashKey = Hash(key) % _capacity;
    int probe = 1;
    
    for (int i = 0; i < 3; ++i) {
        if (_table[hashKey].entries[i].Active == true) {
            if(_table[hashKey].entries[i].Key == key) {
                return _table[hashKey].entries[i].Value;
            }

            probe++;
            hashKey = (hashKey + probe * probe) %_capacity;
            
        }
        else if (_table[hashKey].entries[i].Active == false) {
            if(_table[hashKey].entries[i].Deleted != true) {
                return T();
            }
            probe++;
            hashKey = (hashKey + probe * probe) %_capacity;
        }
        
    }
}

template <class T>
void HashTable<T>::Resize(int newCapacity) {
// TODO: IMPLEMENT THIS FUNCTION. AFTER THIS FUNCTION IS EXECUTED
// THE TABLE CAPACITY MUST BE EQUAL TO newCapacity AND ALL THE
// EXISTING ITEMS MUST BE REHASHED ACCORDING TO THIS NEW CAPACITY.
// WHEN CHANGING THE SIZE, YOU MUST REHASH ALL OF THE ENTRIES FROM 0TH ENTRY TO LAST ENTRY
    HashTable* newTable = new HashTable();
    newTable->_capacity = newCapacity;
    newTable->_table = new Bucket[newCapacity];
    
    _size = 0;
    for(int i = 0; i < _capacity; i++) {
        for(int j = 0; j < 3; j++) {
            if(_table[i].entries[j].Active == true)
                newTable->Insert(_table[i].entries[j].Key, _table[i].entries[j].Value);
        }
    }
    *this = *newTable;
    delete newTable;

}


template <class T>
double HashTable<T>::getAvgSuccessfulProbe() {
// TODO: IMPLEMENT THIS FUNCTION.
// RETURNS THE AVERAGE NUMBER OF PROBES FOR SUCCESSFUL SEARCH
    return (_size == 0)?1:(double)total_probes/(double)_size;
}

template <class T>
double HashTable<T>::getAvgUnsuccessfulProbe() {
// TODO: IMPLEMENT THIS FUNCTION.
// RETURNS THE AVERAGE NUMBER OF PROBES FOR UNSUCCESSFUL SEARCH

}

template <class T>
int HashTable<T>::Capacity() const {
    return _capacity;
}

template <class T>
int HashTable<T>::Size() const {
    return _size;
}

#endif