// HashTable.h, open addressing hash table with linear probing
// Maps string keys to User* values. Designed for a fixed, small set of
// users that are inserted once and never deleted.
#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <string>

namespace CSEN79
{
    class User;

    class HashTable
    {
    public:
        static const int SIZE = 32; // power of 2, well above expected 10 users

        HashTable()
        {
            for (int i = 0; i < SIZE; i++)
                occupied[i] = false;
        }

        // Hash a string key to a bucket index
        int hash(const std::string &key) const
        {
            int h = 0;
            for (char c : key)
                h = (h * 31 + c) % SIZE;
            return h;
        }

        // Insert a key-value pair. Assumes key is not already present.
        void insert(const std::string &key, User *value)
        {
            int i = hash(key);
            while (occupied[i])
                i = (i + 1) % SIZE;
            keys[i] = key;
            values[i] = value;
            occupied[i] = true;
        }

        // Find a value by key. Returns nullptr if not found.
        User *find(const std::string &key) const
        {
            int i = hash(key);
            while (occupied[i])
            {
                if (keys[i] == key)
                    return values[i];
                i = (i + 1) % SIZE;
            }
            return nullptr;
        }

        // Iterate over all stored values
        User *getValue(int i) const { return values[i]; }
        bool isOccupied(int i) const { return occupied[i]; }

    private:
        std::string keys[SIZE];
        User *values[SIZE];
        bool occupied[SIZE];
    };
}

#endif
