#include "hash.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

int HT_INITIAL_BASE_SIZE = 47;

bool is_prime(const int x) {
	if (x < 2) { return -1; }
	if (x < 4) { return 1; }
	if ((x % 2) == 0) { return 0; }
	for (int i = 3; i <= floor(sqrt((double)x)); i += 2) {
		if ((x % i) == 0) {
			return false;
		}
	}
	return true;
}

int next_prime(int x) {
	while (is_prime(x) != true) {
		x++;
	}
	return x;
}

//Function to allocate a chunk of memory the size of a hash table item
static ht_item* ht_new_item(const char* k, const char* v) {
	ht_item* i; 
	i = (ht_item*)malloc(sizeof(ht_item));
	i->key = strdup(k); // Allocates a copy of char* on the heap. Creates a mutable copy of the string
	i->value = strdup(v);
	return i;
}


ht_hash_table* ht_new()
{
	return ht_new_sized(HT_INITIAL_BASE_SIZE);
}

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

//Hash Function APIs
static ht_item HT_DELETED_ITEM = { NULL, NULL };
void hash_insert(ht_hash_table* ht, const char* key, const char* value) 
{
	const int load = ht->count * 100 / ht->size;
	if (load > 70) {
		ht_resize_up(ht);
	}
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
		}
		index = ht_get_hash(item->key, ht->size, i);
		cur_item = ht->items[index];
		
	}
	ht->items[index] = item;
	ht->count++;
}

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



static ht_hash_table* ht_new_sized(const int base_size) {
	ht_hash_table* ht = (ht_hash_table*)malloc(sizeof(ht_hash_table));
	ht->base_size = base_size;

	ht->size = next_prime(ht->base_size);

	ht->count = 0;
	ht->items = (ht_item**)calloc((size_t)ht->size, sizeof(ht_item*));
	return ht;
}



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