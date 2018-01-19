#include "dlopen_examples.h"

void load_dlopen(const char* lib_name)
{
	void* handle = dlopen(lib_name, RTLD_LAZY);

	printf("dlopen method. Handle %p\n", handle);
}

typedef long int Lmid_t;
struct dl_open_args
{
	const char *file;
	int mode;
	/* This is the caller of the dlopen() function.  */
	const void *caller_dlopen;
	/* This is the caller of _dl_open().  */
	const void *caller_dl_open;
	void *map;
	/* Namespace ID.  */
	Lmid_t nsid;
	/* Original parameters to the program and the current environment.  */
	int argc;
	char **argv;
	char **env;
};

void load_dl_open_worker(const char* lib_name)
{
	printf("dl_open_worker method. Finding dl_open_worker inside ld-linux.so...\n");

	typedef void(*dl_open_worker_fn)(void*);
	//Let's find the dl_open_worker address, this signature is only for x86_64
	dl_open_worker_fn dl_open_worker = pattern_scan("ld-linux", "55 48 89 E5 41 57 41 56 41 55 41 54 53 48 83 EC 68");

	printf("Worker address: %p\n", dl_open_worker);

	if (dl_open_worker == NULL)
	{
		printf("Worker not found! Exiting.\n");
		return;
	}

	struct dl_open_args args;

	args.file = lib_name;
	args.mode = RTLD_LAZY;
	args.caller_dl_open = (void*)dl_open_worker;

	printf("Calling the worker...");

	dl_open_worker(&args);

	printf("Handle: %p\n", args.map);
}

