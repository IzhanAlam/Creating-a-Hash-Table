# Creating-Hash-Table-In-C++
A guide on implementing a Hash Table using C++


# Hash Tables

A hash table is a data structure which uses a fast implementation of the associative array API. An associative array is a collection of unordered key-value pairs, where duplicate keys are not permitted. The following API is used for an associative array:

> 'search(a,k)': Return the value (v) associated with the key (k) from the associative array (a). If a key does not exists, then NULL is returned.

>'insert(a, k, v)': Stores the key-value pair in the associative array (a).

>'delete(a, k)': Delete the key-value pair associated with the key, or do nothing if the key does not exist.

Hash Tables consist of an array of 'buckets', where each bucket stores a key-value pair. To find the bucket where a key-value pair should be stored, the key is passed through a **hashing function**. The function returns the index which is used in the array of buckets. To retrieve a key-value pair, the key is given to the same hashing function to find the index, and the index is then used to find it in the array.

Array indexing has an algorithmic complexity of O(1) at average expectency. A non-performance associative array can be made by simply storing items into an array, and iterating through that array to search.

# Hash Table Structure

Key-value pairs will be stored in a structure as follows:

'
//Key-value pairs/items will be stored in a structure.
typedef struct ht_item {
	char* key; // a list of characters: keys
	char* value; // a list of characters: value
};
'
