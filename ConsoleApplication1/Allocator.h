#pragma once

#include <iostream>
#include <list>
#include "Windows.h"

using namespace std;

class MyAllocator
{
	struct block
	{
		uint8_t* pointer; 
		size_t size; 
		block* next; 
		block* prev; 
	};

public:
	explicit MyAllocator();
	void* mem_alloc(size_t size);
	void* mem_realloc(void* addr, size_t size);
	void mem_free(void* addr);
	void mem_dump();

private:
	list<block*>* free_blocks;
	list<block*>* used_blocks;
	block* first_block;
	block* last_block;

	size_t correct_size(size_t size);
	block* block_search(size_t size);
	block* alloc_block(size_t size);
	void merge(block* block);
	block* merge_blocks(block* left, block* right);
	bool contains_block(const std::list<block*>* list, block* block);
};


MyAllocator::MyAllocator()
{
	used_blocks = new list<block*>(); 
	free_blocks = new list<block*>(); 
	first_block = NULL; 
	last_block = NULL; 
}


void* MyAllocator::mem_alloc(size_t size)
{

	size = correct_size(size);
	block* block = block_search(size);

	if (block == NULL)
	{
		block = alloc_block(size);

		if (block == NULL)
		{
			return NULL;
		}
	}
	used_blocks->push_back(block);
	return block->pointer;
}

size_t MyAllocator::correct_size(size_t size)
{
	return (size + sizeof(int) - 1) & ~(sizeof(int) - 1);
}

MyAllocator::block* MyAllocator::block_search(size_t size) {
	for (block* block : *free_blocks)
	{
		if (block->size < size)
			continue;
		{
			free_blocks->remove(block);
			return block;
		}

	}

	return NULL;
}


MyAllocator::block* MyAllocator::alloc_block(size_t size)
{
	const auto mem = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);

	if (mem == NULL)
	{
		return NULL;
	}

	const auto block_ = new block();
	block_->next = NULL;
	block_->prev = NULL;
	block_->pointer = static_cast<uint8_t*>(mem);
	block_->size = size;

	if ((free_blocks->size() == 0) && (used_blocks->size() == 0)) first_block = block_;

	if (last_block != NULL)
	{
		last_block->next = block_;
		block_->prev = last_block;
	}

	last_block = block_;

	return block_;
}


void* MyAllocator::mem_realloc(void* addr, size_t size)
{
	mem_free(addr);
	return mem_alloc(size);
}


void MyAllocator::mem_free(void* addr)
{
	const auto pointer = reinterpret_cast<uint8_t*>(addr);
	for (auto block : *used_blocks)
	{
		if (block->pointer != pointer)
			continue;
		used_blocks->remove(block);
		free_blocks->push_back(block);
		merge(block);
		return;
	}
}


void MyAllocator::merge(block* block)
{
	if (block->prev != NULL  && contains_block(free_blocks, block->prev))
	{
		block = merge_blocks(block->prev, block);
	}

	if (block->next != NULL && contains_block(free_blocks, block->next))
	{
		block = merge_blocks(block, block->next);
	}
}


MyAllocator::block* MyAllocator::merge_blocks(block* left, block* right)
{
	left->size += right->size;
	left->next = right->next;
	return left;
}


bool MyAllocator::contains_block(const std::list<block*>* list, block* block)
{

	for (auto list_block : *list)
	{
		if (list_block->pointer == block->pointer)
			return true;
	}
	return false;
}


void MyAllocator::mem_dump()
{
	setlocale(LC_ALL, "Russian");
	auto curr_block = first_block;
	while (true) {

		cout << "Видiлено пам'ятi: " << curr_block->size << endl;
		cout << "Стан блоку: ";
		if (contains_block(free_blocks, curr_block))
			cout << "вiльний" << endl;
		else if (contains_block(used_blocks, curr_block))
			cout << "використовується" << endl;

		if (curr_block->next == NULL)
			break;

		curr_block = curr_block->next;
	}
}
