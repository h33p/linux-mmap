#ifndef LMMAP_UTILS
#define LMMAP_UTILS
#include <dlfcn.h>
#include <sys/types.h>
#include <link.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL 1
#endif

#define DEBMSG(level, ...) if (level >= DEBUG_LEVEL) printf(__VA_ARGS__)
#define MAX_LIBRARIES 128

struct dlinfo_t
{
	const char* library;
    void* address;
    unsigned long size;
};

struct linked_dlinfo
{
	struct dlinfo_t info;
	struct linked_dlinfo *prev, *next;
};

#ifdef STATIC_IMPLS
struct linked_dlinfo* libraries = NULL;

struct linked_dlinfo lib_buffer[MAX_LIBRARIES];
int lib_index = 0;

int iterate_phdr(struct dl_phdr_info *info, size_t s, void* data)
{
	if (lib_index >= MAX_LIBRARIES - 1) {
		DEBMSG(1, "Out of memory for libraries!");
		return 0;
	}

	struct dlinfo_t library_info;

	library_info.library = info->dlpi_name;
	library_info.address = (void*)(info->dlpi_addr + info->dlpi_phdr[0].p_vaddr);
	library_info.size = info->dlpi_phdr[0].p_memsz;

	struct linked_dlinfo* new_library = lib_buffer + (lib_index++);

	new_library->info = library_info;
	new_library->prev = NULL;
	new_library->next = libraries;

	if (libraries)
		libraries->prev = new_library;

	libraries = new_library;

	return 0;
}

void pattern_to_bytes(const char* pattern, short** bytes, unsigned long* size)
{
	int len = strlen(pattern);
    *bytes = (short*)malloc(sizeof(short) * (len / 3 + 1));
	unsigned long idx = 0;

	char* current = (char*)pattern;

	while(*current != '\0') {
		if (*current == '?') {
			current++;
			if (*current == '?')
				current++;
			(*bytes)[idx++] = -1;
		} else
			(*bytes)[idx++] = strtoul(current, &current, 16);
	}

	*size = idx;
}

int get_library_info(const char* library, void** address, unsigned long* size)
{
	if (libraries == NULL)
		dl_iterate_phdr(iterate_phdr, NULL);

	struct linked_dlinfo* lib = libraries;

	while (lib) {
		if (strcasestr(lib->info.library, library)) {
			if (address)
				*address = lib->info.address;
			if (size)
				*size = lib->info.size;
			return 1;
		}

		lib = lib->next;
	}

	return 0;
}

void* pattern_scan(const char* lib_name, const char* signature)
{
	void* address;
	unsigned long size;

	if (!get_library_info(lib_name, &address, &size))
		return NULL;

	short* pattern_bytes = NULL;
	unsigned long s = 0;
	char* scan_bytes = (char*)address;
	pattern_to_bytes(signature, &pattern_bytes, &s);

	for (unsigned long i = 0; i < size - s; i++) {
		int found = 1;

		for (unsigned long j = 0; j < s && found; j++)
			if (scan_bytes[i + j] != (char)pattern_bytes[j] && pattern_bytes[j] != -1)
				found = 0;

		if (found)
			return &scan_bytes[i];
	}

	return NULL;
}

#else
extern struct linked_dlinfo* libraries;
int iterate_phdr(struct dl_phdr_info *info, size_t s, void* data);
void pattern_to_bytes(const char* pattern, short** bytes, unsigned long* size);
int get_library_info(const char* library, void** address, unsigned long* size);
void* pattern_scan(const char* lib_name, const char* signature);
#endif


#endif
