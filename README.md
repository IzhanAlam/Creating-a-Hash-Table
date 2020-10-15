# Creating-Hash-Table-In-C++
A guide on implementing a Hash Table using C++

# Usage


```cpp
# include <iostream>

#include "hash.h"

int main() 
{
	ht_hash_table* ht = ht_new();

	hash_insert(ht, "a", "password11");
	hash_insert(ht, "b", "password22");
	//hash_insert(ht, "c", "password33");


	char* val = hash_search(ht, "a");

	std::cout << val << std::endl;


	system("pause");
	return 0;
};
```



# Hash Tables

A hash table is a data structure which uses a fast implementation of the associative array API. An associative array is a collection of unordered key-value pairs, where duplicate keys are not permitted. The following API is used for an associative array:

>search(a,k): Return the value (v) associated with the key (k) from the associative array (a). If a key does not exists, then NULL is returned.

>insert(a, k, v): Stores the key-value pair in the associative array (a).

>delete(a, k): Delete the key-value pair associated with the key, or do nothing if the key does not exist.

Hash Tables consist of an array of 'buckets', where each bucket stores a key-value pair. To find the bucket where a key-value pair should be stored, the key is passed through a **hashing function**. The function returns the index which is used in the array of buckets. To retrieve a key-value pair, the key is given to the same hashing function to find the index, and the index is then used to find it in the array.

Array indexing has an algorithmic complexity of O(1) at average expectency. A non-performance associative array can be made by simply storing items into an array, and iterating through that array to search.

# Hash Table Structure

Key-value pairs will be stored in a structure as follows:

```cpp
//Key-value pairs/items will be stored in a structure.
typedef struct ht_item {
	char* key; // a list of characters: keys
	char* value; // a list of characters: value
};
```
The Hash Table will store an array of pointers to the items, as well as it's initial base size, it's size afterwards, and the number of items in the table.

```cpp
//Hash table stores an array of pointers to items, aswell as its size and the number of items
typedef struct ht_hash_table {
	int base_size;
	int size;
	int count;
	ht_item **items; //Double Pointer --> get the entire table
};
```
The first step is to initalize and create the hash table. To do so, we allocate a chunk of memory the size of the item. A copy of the strings (key, value), are stored in the the allocated chunk of memory. 


```cpp
static ht_item* ht_new_item(const char* k, const char* v) {
	ht_item* i; 
	i = (ht_item*)malloc(sizeof(ht_item));
	i->key = strdup(k); // Allocates a copy of char* on the heap. Creates a mutable copy of the string
	i->value = strdup(v);
	return i;
}
```

To create the hash table, we first need allocate memory to the hash table. We then set the paramters of the hash table. The intial base size is defined as '''int HT_INITIAL_BASE_SIZE = 47;''' which is just a prime number. The size of the hash table is then the next prime number.

The following is a simple implenetation on how to find the next prime number.
```cpp
bool is_prime(const int x) {
	if (x < 2) { return false; }
	if (x < 4) { return true; }
	if ((x % 2) == 0) { return false; }
	for (int i = 3; i <= floor(sqrt((double)x)); i += 2) {
		if ((x % i) == 0) {
			return false;
		}
	}
	return true;
}

int next_prime(int x) 
{
	while (is_prime(x) != true) 
	{
		x++;
	}
	return x;
}

```
The following shows how to set the initial hash table values. A NULL entry indicates that the bucket is empty.

```cpp
static ht_hash_table* ht_new_sized(const int base_size) {
	ht_hash_table* ht = (ht_hash_table*)malloc(sizeof(ht_hash_table));
	ht->base_size = base_size;

	ht->size = next_prime(ht->base_size);

	ht->count = 0;
	ht->items = (ht_item**)calloc((size_t)ht->size, sizeof(ht_item*));
	return ht;
}
```

Addionally, to avoid memory leaks, we need funcitons to delete the hash table, and the items from memory to free the memory which had been allocated to the hash table.

```cpp
static void ht_del_item(ht_item* i) 
{
	free(i->key);
	free(i->value);
	free(i);
}


void ht_del_hash_table(ht_hash_table* ht) 
{
	for (int i = 0; i < ht->size; i++) {
		ht_item* item = ht->items[i];
		if (item != NULL) {
			ht_del_item(item);
		}
	}
	free(ht->items);
	free(ht);
}
```

# Hash Function

The hash function is defined in the comments of the following code. 

```cpp
//A hash function should take a string as an input and return a number between 0 and the desired bucket array length
//An even distribution of bucket indexes for an average set of inputs. 
//If unevenly distributed it will put more items in some buckets than others leading to more collisions
//Collisions reduce efficiency of the hash table.
//Input: String to hash (s), a prime number larger than the ASCII limit of 128 (a), and the number of buckets (num_buckets)
static int hash_function(const char* str, const int& prime, const int& num_buckets)
{
	//Start hash at 0: (long is 64 bits vs int at 32 bits)
	long hash = 0;
	//Get length of the string
	const int str_len = strlen(str);
	//Hash function:
	//1. Convert the string to a large integer
	//2. Reduce the size of the integer to a fixed range by taking its remainder
	for (int i = 0; i < str_len; i++) 
	{
		//Hash the string
		hash += (long)pow(prime, str_len - (i + 1)) * str[i];
		//Get the remainder
		hash = hash % num_buckets;

	}

	return (int)hash;

}
```

# Collisions

Since a hash function maps infinite number of inputs to a finite number of outputs, different input keys will map to the same array index which cause bucket collisions. To solve this issue, double hashing is used which makes the use of two hash functions to calculate the index of an item after **i** collisions.


```cpp
//Handling Collisions:
//Collisions will occur becuase there are an infinite numbers mapped to a finite number of outputs
//When the multiple keys get mapped to the same array index, a bucket collision occurs
//Thus there must be something to deal with collisions
//Method: Double Hashing ---> The index which should be used after i collisions
// i = 0 if no collisions occur which means it is just hash of a. If there is collision, the index
// is modified by the hash of b. If the hash of b is 0, then the hash table will keep inserting the item into the same bucket
//To resolve this, 1 is added to the hash of b to ensure the value is never 0.

static int ht_get_hash(const char* str, const int& num_buckets, const int& attempt_num)
{
	const unsigned int prime_a = 811;
	const unsigned int prime_b = 433;

	const int hash_a = hash_function(str, prime_a, num_buckets);
	const int hash_b = hash_function(str, prime_b, num_buckets);

	int val = (hash_a + (attempt_num * (hash_b + 1))) % num_buckets;
	return val;
}
```

# API

# Insert:

To insert a new key-value pair, the indexes are iterated until an empty bucket is found. Once found, the item is inserted into that bucket, and the hash-table's **count** is incremented. This indicates that a new item has been added. The code is as follows:

```cpp
static ht_item HT_DELETED_ITEM = { NULL, NULL };

void hash_insert(ht_hash_table* ht, const char* key, const char* value) 
/* For resizing
{
	const int load = ht->count * 100 / ht->size;
	if (load > 70) 
  {
		ht_resize_up(ht);
	}
 */
  //Implementation of insert
	ht_item* item = ht_new_item(key, value);
	int index = ht_get_hash(item->key, ht->size, 0);
	ht_item* cur_item = ht->items[index];
	int i = 1;
	while (cur_item != NULL) 
	{
		if (cur_item != &HT_DELETED_ITEM)
		{
			if (strcmp(cur_item->key, key) == 0)
			{
				ht_del_item(cur_item);
				ht->items[index] = item;
				return;
			}
    index = ht_get_hash(item->key, ht->size, i);
		cur_item = ht->items[index];
    i++;
		}
	}
	ht->items[index] = item;
	ht->count++;
}
```
Essentially, we create a new item using the **ht_new_item** function. We get the value of the hash for the key and value that were given using **ht_get_hash**. We then find the current item for that particlar hash index. In the case that it is not NULL (the index is already in use), we first check that the item is not deleted (i.e. that it is not {NULL, NULL}. In that case that it is not, we compare the key of the current item, to the given key. We delete the current item if they are equal, and update the hash table. In the case that the current item is not deleted, we can then get the hash and insert the current item into the hash table's index and iterate i for the case of collision. In the case that the while loop is NULL (no item in the bucket), we can just directly insert into the hash table and update the count.

# Search

```cpp
char* hash_search(ht_hash_table* ht, const char* key) 
{
	int index = ht_get_hash(key, ht->size, 0);
	ht_item* item = ht->items[index];
	int i = 1;
	while (item != NULL) 
	{
		if (item != &HT_DELETED_ITEM) 
		{
			if (strcmp(item->key, key) == 0) 
			{
				return item->value;
			}
		}
		if (strcmp(item->key, key) == 0) 
		{
			return item->value;
		}
		index = ht_get_hash(key, ht->size, i);
		item = ht->items[index];
		i++;
	}
	return NULL;
}
```

This is similar to the insert checks. The difference is that we do not need to create a new hash item. We simply get the index using the hash function, the key, the size, and the count. A new item is created, and the key and value from the index are retrieved. 

# Delete

```cpp
void hash_delete(ht_hash_table* ht, const char* key) 
{
	const int load = ht->count * 100 / ht->size;
	if (load < 10) {
		ht_resize_down(ht);
	}
	int index = ht_get_hash(key, ht->size, 0);
	ht_item* item = ht->items[index];
	int i = 1;
	while (item != NULL) 
	{
		if (item != &HT_DELETED_ITEM) 
		{
			if (strcmp(item->key, key) == 0) 
			{
				ht_del_item(item);
				ht->items[index] = &HT_DELETED_ITEM;
			}
		}
		index = ht_get_hash(key, ht->size, i);
		item = ht->items[index];
		i++;
	}
	ht->count--;
}
```

Same as before, but instead we set the items at the index given by the hash function to HT_DELETED_ITEM = {NULL, NULL}.

# Resizing

```cpp
static void ht_resize(ht_hash_table* ht, const int base_size) 
{
	if (base_size < HT_INITIAL_BASE_SIZE) 
	{
		return;
	}
	ht_hash_table* new_ht = ht_new_sized(base_size);
	for (int i = 0; i < ht->size; i++) 
	{
		ht_item* item = ht->items[i];
		if (item != NULL && item != &HT_DELETED_ITEM) 
		{
			hash_insert(new_ht, item->key, item->value);
		}
	}

	ht->base_size = new_ht->base_size;
	ht->count = new_ht->count;

	// To delete new_ht, we give it ht's size and items 
	const int tmp_size = ht->size;
	ht->size = new_ht->size;
	new_ht->size = tmp_size;

	ht_item** tmp_items = ht->items;
	ht->items = new_ht->items;
	new_ht->items = tmp_items;

	ht_del_hash_table(new_ht);
}

static void ht_resize_up(ht_hash_table* ht) 
{
	const int new_size = ht->base_size * 2;
	ht_resize(ht, new_size);
}


static void ht_resize_down(ht_hash_table* ht) 
{
	const int new_size = ht->base_size / 2;
	ht_resize(ht, new_size);
}
```
