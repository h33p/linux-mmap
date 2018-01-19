#define STATIC_IMPLS
#include "dlopen_examples.h"

//To be moved to its own file
void load_mmap(const char* lib_name)
{
}

int main()
{
	int choice = -1;
	char* lib_name = NULL;

	printf("linux-mmap userspace\n\n[1] dlopen\n[2] dl_open_worker\n[3] Manual mapping\n[ ] Exit\n\nSelect an option: ");

	scanf("%d", &choice);

	if (choice < 1 || choice > 3)
		return 0;

	printf("Enter full library path: ");
	scanf("%ms", &lib_name);

	printf("Openning library '%s'\n", lib_name);

	if (choice == 1)
		load_dlopen(lib_name);
	else if (choice == 2)
		load_dl_open_worker(lib_name);
	else
		load_mmap(lib_name);

	return 0;
}
