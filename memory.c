/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   https://github.com/kmorimatsu
*/

#include "./compiler.h"

/*
	HEAP_BEGIN
		Begin address of heap area

	HEAP_END
		End address of heap area

	TEMPVAR_NUMBER
		# of temporary blocks used for string handling etc

	ALLOC_BLOCK_NUM
		# of blocks that can be used for memory allocation.
		This # includes the ones for ALLOC_VAR_NUM, ALLOC_PCG_BLOCK etc, ALLOC_LNV_BLOCK,
		ALLOC_PERM_BLOCK.
		After ALLOC_VAR_NUM area, dedicated memory area and permanent area follows.

	ALLOC_TEMP_BLOCK
		Start # of temporary blocks.
		The blocks after this number are temporarily used.
		It will be released peridiodically.

	ALLOC_PERM_BLOCK
		Start # of permanent blocks.
		The blocks after this number are permanently stored.
		Therefore, it must be released when it's not used any more.

	int kmbasic_variables[ALLOC_BLOCK_NUM];
		Contain pointer to memory area.
	unsigned short kmbasic_var_size[ALLOC_BLOCK_NUM];
		Contain length of memory area assigned
*/

#define ALLOC_TEMP_BLOCK g_next_varnum
#define ALLOC_PERM_BLOCK (ALLOC_TEMP_BLOCK+TEMPVAR_NUMBER)
#define HEAP_BEGIN g_heap_begin
#define HEAP_END g_heap_end

static int* g_heap_begin;
static int* g_heap_end;

#define DELETE_LIST_SIZE 10
static int g_deleted_num;
static int g_deleted_pointer[DELETE_LIST_SIZE];
static unsigned short g_deleted_size[DELETE_LIST_SIZE];

void init_memory(void){
	int i;
	// Clear all variables
	for(i=0;i<ALLOC_BLOCK_NUM;i++) {
		kmbasic_variables[i]=0;
		kmbasic_var_size[i]=0;
	}
	// HEAP_BEGIN
	i=(int)(&object[1]);
	i&=0xfffffffc;
	g_heap_begin=(int*)i;
	// HEAP_END
	i=(int)(&g_objmax[0]);
	i&=0xfffffffc;
	g_heap_end=(int*)i;
	// Garbage collection in the very beginning
	g_garbage_collection=1;
	// Reset delete list
	g_deleted_num=0;
}

void* calloc_memory(int size, int var_num){
	int i;
	void* ret;
	// Allocate memory. ret always receives value
	ret=alloc_memory(size,var_num);
	// Fill zero in allocated memory
	for(i=0;i<size;i++){
		((int*)ret)[i]=0;
	}
	// return pointer to allocated memory
	return ret;
}

void* alloc_memory(int size, int var_num){
	int* candidate;
	int* var;
	int i,j,min;
	if (g_garbage_collection && var_num<0) {
		// Garbage collection
		g_garbage_collection=0;
		for(i=0;i<TEMPVAR_NUMBER;i++) kmbasic_var_size[ALLOC_TEMP_BLOCK+i]=0;
	}
	if (var_num<0) {
		// Assign temp var number
		for(i=0;i<TEMPVAR_NUMBER;i++) {
			if (kmbasic_var_size[ALLOC_TEMP_BLOCK+i]) continue;
			var_num=ALLOC_TEMP_BLOCK+i;
			break;
		}
		if (var_num<0) stop_with_error(ERROR_NO_TEMP_VAR);
	} else if (var_num<ALLOC_BLOCK_NUM) {
		// Realocate the block if fits
		if (size<=kmbasic_var_size[var_num]) {
			var=(int*)kmbasic_variables[var_num];
			if (HEAP_BEGIN<=var && var<HEAP_END) {
				// This valid area can be used
				kmbasic_var_size[var_num]=size;
				return &var[0];
			}
		}
		// Discard previous block, first
		kmbasic_var_size[var_num]=0;
	} else {
		stop_with_error(ERROR_UNKNOWN);
	}
	// Try to find a block
	while(1){
		// Try the block previously deleted
		// This is for fast allocation of memory for class object
		min=0; // minimum size
		for(i=j=0;i<g_deleted_num;i++){
			if (0<min && min<=g_deleted_size[i]) continue;
			if (g_deleted_size[i]<size) continue;
			min=g_deleted_size[i];
			j=i;
			if (min==size) break;
		}
		if (min) {
			// Found one
			candidate=(int*)g_deleted_pointer[j];
			// Discard it from the deleted list
			for(i=j;i<g_deleted_num-1;i++){
				g_deleted_pointer[i]=g_deleted_pointer[i+1];
				g_deleted_size[i]=g_deleted_size[i+1];
			}
			g_deleted_num--;
			break;
		} else {
			// Not found
			// Invalidate deleted list
			g_deleted_num=0;
		}
		// Try block after the last block
		candidate=HEAP_BEGIN;
		for(i=0;i<ALLOC_BLOCK_NUM;i++){
			if (0==kmbasic_var_size[i]) continue; // Not using heap
			var=(int*)kmbasic_variables[i];
			if (var<HEAP_BEGIN || HEAP_END <=var) continue; // Invalid
			candidate=&var[kmbasic_var_size[i]];
		}
		if (&candidate[size]<=HEAP_END) break; // Found an available block
		// Check between blocks
		// TODO: confirm this logic
		candidate=HEAP_BEGIN;
		for(i=0;i<=ALLOC_BLOCK_NUM;i++){
			// Check the overlap
			for(j=0;j<ALLOC_BLOCK_NUM;j++){
				if (0==kmbasic_var_size[j]) continue; // Not using heap
				var=(int*)kmbasic_variables[j];
				if (var<HEAP_BEGIN || HEAP_END <=var) continue; // Invalid
				if (&candidate[size]<=&var[0] || &var[kmbasic_var_size[j]] <=&candidate[0]) continue; // No overlap
				candidate=0;
				break;
			}
			if (candidate) break;
			if (ALLOC_BLOCK_NUM<=i) break;
			// Check after a block
			for(i=i;i<ALLOC_BLOCK_NUM;i++){
				if (0==kmbasic_var_size[i]) continue; // Not using heap
				var=(int*)kmbasic_variables[i];
				if (var<HEAP_BEGIN || HEAP_END <=var) continue; // Invalid
				var=(int*)kmbasic_variables[i];
				candidate=&var[kmbasic_var_size[i]];
				break;
			}
		}
		if (candidate) break;
		// No free area found
		stop_with_error(ERROR_OUT_OF_MEMORY);
	}
	// A free area found
	kmbasic_var_size[var_num]=size;
	kmbasic_variables[var_num]=(int)candidate;
	return candidate;
}

void delete_memory(void* data){
	int i,j,min;
	// Check if number of data reaches maximum
	if (DELETE_LIST_SIZE<=g_deleted_num) {
		// Delete one of data
		// Determine minimum one
		min=65535;
		for(i=j=0;i<DELETE_LIST_SIZE;i++){
			if (g_deleted_size[i]<min) j=i;
		}
		// g_deleted_size[j] is the minimum
		for(i=j;i<DELETE_LIST_SIZE-1;i++){
			g_deleted_pointer[i]=g_deleted_pointer[i+1];
			g_deleted_size[i]=g_deleted_size[i+1];
		}
		g_deleted_num=DELETE_LIST_SIZE-1;
	}
	// Delete all data to fit
	// It must be only one, but delete all the data to fit anyway
	g_deleted_size[g_deleted_num]=0;
	for(i=0;i<TEMPVAR_NUMBER;i++) {
		if (data!=(void*)kmbasic_variables[i]) continue;
		if (0==kmbasic_var_size[i]) continue;
		// Found it
		// Register to the delete list
		g_deleted_pointer[g_deleted_num]=kmbasic_variables[i];
		g_deleted_size[g_deleted_num]=kmbasic_var_size[i];
		// Delete the record
		kmbasic_var_size[i]=0;
	}
	g_deleted_num++;
}

int move_from_temp(int vn, int pdata){
	int i,var_num;
	for(i=0;i<TEMPVAR_NUMBER;i++) {
		if (0==kmbasic_var_size[ALLOC_TEMP_BLOCK+i]) continue;
		if (pdata!=kmbasic_variables[ALLOC_TEMP_BLOCK+i]) continue;
		// Found it.
		kmbasic_variables[vn]=kmbasic_variables[ALLOC_TEMP_BLOCK+i];
		kmbasic_var_size[vn]=kmbasic_var_size[ALLOC_TEMP_BLOCK+i];
		kmbasic_var_size[ALLOC_TEMP_BLOCK+i]=0;
		return 1;
	}
	// Not found
	return 0;
}