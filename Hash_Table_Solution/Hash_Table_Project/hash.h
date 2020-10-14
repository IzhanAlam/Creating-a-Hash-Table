#pragma once

#define _CRT_NONSTDC_NO_DEPRECATE

//Key-value pairs/items will be stored in a structure.
typedef struct ht_item {
	char* key; // a list of characters: keys
	char* value; // a list of characters: value
};

//Hash table stores an array of pointers to items, aswell as its size and the number of items
typedef struct ht_hash_table {
	int base_size;
	int size;
	int count;
	ht_item **items; //Double Pointer --> get the entire table
};

bool is_prime(const int x);
int next_prime(int x);


static ht_item* ht_new_item(const char* k, const char* v);
ht_hash_table* ht_new();
static void ht_del_item(ht_item* i);
void ht_del_hash_table(ht_hash_table* ht);

static int hash_function(const char* str, const int& prime, const int& num_buckets);
static int ht_get_hash(const char* str, const int& num_buckets, const int& attempt_num);


//Hash Function API's
//Insert
void hash_insert(ht_hash_table* ht, const char* key, const char* value);
//Search
char* hash_search(ht_hash_table* ht, const char* key);
//Delete
void hash_delete(ht_hash_table* ht, const char* key);

//
static ht_hash_table* ht_new_sized(const int base_size);
//ht_hash_table* ht_new();
static void ht_resize_up(ht_hash_table* ht);
static void ht_resize_down(ht_hash_table* ht);