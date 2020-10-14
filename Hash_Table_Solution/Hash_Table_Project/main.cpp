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