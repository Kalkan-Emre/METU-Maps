#include "HashTable.h"

#include <cassert>
#include <iostream>
#include <iomanip>

const int KeyedHashTable::PRIME_LIST[PRIME_TABLE_COUNT] =
{
     2,    3,   5,   7,  11,  13,  17,  19,
     23,  29,  31,  37,  41,  43,  47,  53,
     59,  61,  67,  71,  73,  79,  83,  89,
     97, 101, 103, 107, 109, 113, 127, 131,
    137, 139, 149, 151, 157, 163, 167, 173,
    179, 181, 191, 193, 197, 199, 211, 223,
    227, 229, 233, 239, 241, 251, 257, 263,
    269, 271, 277, 281, 283, 293, 307, 311,
    313, 317, 331, 337, 347, 349, 353, 359,
    367, 373, 379, 383, 389, 397, 401, 409,
    419, 421, 431, 433, 439, 443, 449, 457,
    461, 463, 467, 479, 487, 491, 499, 503,
    509, 521, 523, 541
};

int KeyedHashTable::Hash(const std::string& key) const
{
    // TODO
    int hash_value=0;
    int i=0;
    
    while(i<key.length())
    {
        hash_value+=(key[i]*PRIME_LIST[i]);
        i++;
    }
    hash_value = hash_value%tableSize;
    
    if(hash_value<0)
    {
        hash_value+=tableSize;
    }
    
    
    return hash_value;
}

void KeyedHashTable::ReHash()
{
    // TODO

        
        HashData* new_table = new HashData[FindNearestLargerPrime(tableSize*2)];
        HashData* tmp = table;
        table = new_table;
        int oldTableSize = tableSize;
        tableSize = FindNearestLargerPrime(tableSize*2);
        for(int i=0;i<oldTableSize;i++)
        {
            std::string key = tmp[i].key;
            if(key==""||key=="__DELETED__")
            {
                continue;
            }
            int probing_val;
            for(int j=0; j<FindNearestLargerPrime(tableSize*2);j++)
            {
                probing_val = Hash(key)+(j*j);
                probing_val%=FindNearestLargerPrime(tableSize*2);
                if(probing_val<0)
                {
                    probing_val+=FindNearestLargerPrime(tableSize*2);
                }

                if(table[probing_val].key=="")
                {
                    table[probing_val].key = key;
                    table[probing_val].intArray = table[i].intArray;
                    break;

                }
            }
        }
        delete [] tmp;
}

int KeyedHashTable::FindNearestLargerPrime(int requestedCapacity)
{
    // TODO
    for(int i=0;i<PRIME_TABLE_COUNT;i++)
    {
        if(PRIME_LIST[i]>requestedCapacity)
            return PRIME_LIST[i];
    }
}

KeyedHashTable::KeyedHashTable()
{
    // TODO
    table = new HashData[2];
    tableSize=2;
    occupiedElementCount =0;
}

KeyedHashTable::KeyedHashTable(int requestedCapacity)
{
    // TODO
    table = new HashData[FindNearestLargerPrime(requestedCapacity)];
    tableSize=FindNearestLargerPrime(requestedCapacity);
    occupiedElementCount =0;
}

KeyedHashTable::KeyedHashTable(const KeyedHashTable& other)
{
    // TODO
    table = NULL;
    *(this)=other;
}

KeyedHashTable& KeyedHashTable::operator=(const KeyedHashTable& other)
{
    // TODO
    tableSize=other.tableSize;
    occupiedElementCount = other.occupiedElementCount;
    HashData* tmp = table;
    delete [] tmp;
    table = new HashData[tableSize];
    for(int i=0;i<tableSize;i++)
    {
        table[i]=other.table[i];
    }
    
    
}

KeyedHashTable::~KeyedHashTable()
{
    // TODO
    delete [] table;
    tableSize=0;
    occupiedElementCount =0;
}

bool KeyedHashTable::Insert(const std::string& key,
                            const std::vector<int>& intArray)
{
    // TODO
    int probing_val = Hash(key);
    for(int i=0;i<tableSize;i++)
    {
        probing_val = Hash(key)+(i*i);
        if(table[probing_val].key==""||table[probing_val].key=="__DELETED__")
        {
            table[probing_val].key = key;
            table[probing_val].intArray = intArray;
            occupiedElementCount++;
            if(occupiedElementCount * EXPAND_THRESHOLD >= tableSize)
            {
                ReHash();
            }
            return true;
        }
        else if(table[probing_val].key==key)
        {
            return false;
        }
    }
    return false;
}

bool KeyedHashTable::Remove(const std::string& key)
{
    int probing_val = Hash(key);
    for(int i=0;i<tableSize;i++)
    {
        probing_val = Hash(key)+(i*i);
        if(table[probing_val].key=="")
        {
            return false;
        }
        if(table[probing_val].key==key)
        {
            table[probing_val].key = "__DELETED__";
            occupiedElementCount--;
            return true;
        }
    }
    return false;
}

void KeyedHashTable::ClearTable()
{
   // TODO
   for(int i=0;i<tableSize;i++)
   {
       if(table[i].key!="")
       {
           table[i].key="";
       }
   }
    occupiedElementCount=0;
}

bool KeyedHashTable::Find(std::vector<int>& valueOut,
                          const std::string& key) const
{
    // TODO
    int probing_val = Hash(key);
    for(int i=0;i<tableSize;i++)
    {
        probing_val = Hash(key)+(i*i);
        if(table[probing_val].key=="")
        {
            return false;
        }
        if(table[probing_val].key==key)
        {
            valueOut = table[probing_val].intArray;
            return true;
        }
    }
    return false;
}

void KeyedHashTable::Print() const
{
    // ============================= //
    // This function is implemented  //
    // Do not edit this function !   //
    // ============================= //
    std::cout << "HT:";
    if(occupiedElementCount == 0)
    {
        std::cout << " Empty";
    }
    std::cout << "\n";
    for(int i = 0; i < tableSize; i++)
    {
        if(table[i].key == "") continue;

        std::cout << "[" << std::setw(3) << i << "] ";
        std::cout << "[" << table[i].key << "] ";
        std::cout << "[";
        for(size_t j = 0; j < table[i].intArray.size(); j++)
        {
            std::cout << table[i].intArray[j];
            if((j + 1) != table[i].intArray.size())
                std::cout << ", ";
        }
        std::cout << "]\n";
    }
    std::cout.flush();
}