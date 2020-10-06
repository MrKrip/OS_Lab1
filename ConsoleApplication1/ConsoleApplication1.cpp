#include <iostream>
#include "Windows.h"
#include "Allocator.h"
using namespace std;

int main() {
	auto allocator = new MyAllocator();
	setlocale(LC_ALL, "Russian");
	cout << "Приклад роботи mem_alloc (видiлення пам'ятi)" << endl;
	auto memory = allocator->mem_alloc(sizeof(long));
	auto memory2 = allocator->mem_alloc(sizeof(long));
	allocator->mem_dump();
	cout << endl << endl;
	cout << "Приклад роботи mem_realloc (перевизначення до 7 байт)" << endl;
	auto memory3 = allocator->mem_realloc(memory, 7);
	allocator->mem_dump();
	cout << endl << endl;
	cout << endl << endl;
	cout << "Приклад роботи mem_free" << endl;
	allocator->mem_free(memory2);
	allocator->mem_dump();

	system("pause");
}