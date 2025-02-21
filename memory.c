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

	PERMVAR_NUMBER
		# of permanent blocks used for string handling etc

	ALLOC_BLOCK_NUM
		# of blocks that is used for memory allocation. Now, it is 256.

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
#define PERMVAR_NUMBER (ALLOC_BLOCK_NUM-ALLOC_PERM_BLOCK)
#define HEAP_BEGIN g_heap_begin
#define HEAP_END g_heap_end

static int* g_heap_begin;
static int* g_heap_end;

#define DELETE_LIST_SIZE 10
static int g_deleted_num;
static int* g_deleted_pointer[DELETE_LIST_SIZE];
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

void reset_memory(void){
	g_heap_begin=g_heap_end;
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
	// TODO: confirm the logick here
	while(1){
		// Try the block previously deleted
		// This is for fast allocation of memory for class object
		candidate=0;
		while(g_deleted_num){
			// Check if the last deleted block fits
			// If not, these cannot be used anymore
			g_deleted_num--;
			if (size<=g_deleted_size[g_deleted_num]) {
				candidate=g_deleted_pointer[g_deleted_num];
				break;
			}
		}
		if (candidate) {
			// Candidate found
			break;
		} else {
			// Candidate not found in previously deleted blocks
			// Lets forget the deleted block list as the area assgined below may invade the deleted blocks area
			g_deleted_num=0;
		}
		// Try block after the last block
		candidate=HEAP_BEGIN;
		for(i=0;i<ALLOC_BLOCK_NUM;i++){
			if (0==kmbasic_var_size[i]) continue; // Not using heap
			var=(int*)kmbasic_variables[i];
			if (var<HEAP_BEGIN || HEAP_END <=var) continue; // Invalid
			if (&var[kmbasic_var_size[i]]<candidate) continue; // Not last one
			candidate=&var[kmbasic_var_size[i]];
		}
		if (&candidate[size]<=HEAP_END) {
			// Found an available block
			break;
		}
		// Check between blocks
		for(i=-1;i<ALLOC_BLOCK_NUM;i++){
			if (i<0) {
				// First candidate is beginning address
				candidate=HEAP_BEGIN;
			} else {
				if (!kmbasic_var_size[i]) {
					candidate=HEAP_BEGIN-1;
					continue;
				}
				// Candidate is after this block.
				var=(int*)kmbasic_variables[i];
				// Check if valid area
				if (var<HEAP_BEGIN || HEAP_END<var) {
					candidate=HEAP_BEGIN-1;
					continue;
				}
				candidate=var+kmbasic_var_size[i];
			}
			// Check if there is an overlap.
			for(j=0;j<ALLOC_BLOCK_NUM;j++){
				if (!kmbasic_var_size[j]) continue;
				// Check this block for the overlap
				var=(int*)kmbasic_variables[j];
				if (candidate+size<=var) continue;
				if (var+kmbasic_var_size[j]<=candidate) continue;
				// This block overlaps with the candidate
				candidate=HEAP_BEGIN-1;
				break;
			}
			if (HEAP_BEGIN<=candidate && candidate+size<=HEAP_END) {
				// Available block found
				break;
			}
		}
		if (HEAP_BEGIN<=candidate && candidate+size<=HEAP_END) {
			// Available block found
			break;
		}
		// New memory block cannot be allocated
		stop_with_error(ERROR_OUT_OF_MEMORY);
	}
	// A free area found
	kmbasic_var_size[var_num]=size;
	kmbasic_variables[var_num]=(int)candidate;
	g_last_var_num=var_num;
	return candidate;
}

void delete_memory(void* data){
	int i,j,min;
	int size;
	int* var;
	if (!data) return;
	// Delete the corresponding area (multiple variables may exist)
	size=0;
	for(i=0;i<ALLOC_BLOCK_NUM;i++){
		if (!kmbasic_var_size[i]) continue;
		var=(int*)kmbasic_variables[i];
		if (var!=data) continue;
		// Remember the size
		if (size<kmbasic_var_size[i]) size=kmbasic_var_size[i];
		// Delete the var area
		kmbasic_variables[i]=0;
		kmbasic_var_size[i]=0;
	}
	// Update the list of deleted area
	if (DELETE_LIST_SIZE<=g_deleted_num) {
		// List is full
		// Let's check the last data
		g_deleted_num=DELETE_LIST_SIZE;
		if (g_deleted_size[DELETE_LIST_SIZE-1]<size) {
			// Size is bigger than the previous one
			// Let's replace it
			g_deleted_pointer[DELETE_LIST_SIZE-1]=data;
			g_deleted_size[DELETE_LIST_SIZE-1]=size;
		}
	} else {
		// Add a record at the end
		g_deleted_pointer[g_deleted_num]=data;
		g_deleted_size[g_deleted_num]=size;
		g_deleted_num++;
	}
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

void garbage_collection(void* data){
	int i;
	for(i=0;i<TEMPVAR_NUMBER;i++) {
		if (0==kmbasic_var_size[ALLOC_TEMP_BLOCK+i]) continue;
		if ((int)data!=kmbasic_variables[ALLOC_TEMP_BLOCK+i]) continue;
		// Found it.
		kmbasic_var_size[ALLOC_TEMP_BLOCK+i]=0;
	}
}

int get_permanent_block_number(void){
	int i;
	for(i=0;i<PERMVAR_NUMBER;i++){
		if (0==kmbasic_var_size[ALLOC_PERM_BLOCK+i]) return ALLOC_PERM_BLOCK+i;
	}
	stop_with_error(ERROR_OBJ_TOO_MANY);
	return ERROR_UNKNOWN;
}

int get_number_of_remaining_blocks(void){
	int i;
	int j=0;
	for(i=0;i<PERMVAR_NUMBER;i++){
		if (0==kmbasic_var_size[ALLOC_PERM_BLOCK+i]) j++;
	}
	return j;
}

void var2permanent(int var_num){
	int i;
	int val=kmbasic_variables[var_num];
	if (kmbasic_var_size[var_num] && g_heap_begin<=(int*)val && (int*)val<g_heap_end) {
		for(i=0;i<PERMVAR_NUMBER;i++){
			if (kmbasic_var_size[var_num]!=kmbasic_var_size[ALLOC_PERM_BLOCK+i]) continue;
			if (kmbasic_variables[ALLOC_PERM_BLOCK+i]!=val) continue;
			// Permanent block already exists. Do not copy, but delete the source.
			// This happens at and after 2nd time of calling this function with the same var.
			// Note that moving of array from object field to var doesn't happen
			// and that only moving the pointer happens.
			// This mechanism is for avoinding accidental overlap of integer value (but not pointer) 
			// over a permanent block area. 
			kmbasic_var_size[var_num]=0;
			return;
		}
		// Permanent block not found. Create a new one.
		i=get_permanent_block_number();
		kmbasic_variables[i]=kmbasic_variables[var_num];
		kmbasic_var_size[i]=kmbasic_var_size[var_num];
		kmbasic_var_size[var_num]=0;
	}
}

/*
	Wrappers for malloc/calloc/free
*/

void* machikania_malloc(int size){
	if (g_heap_begin<g_heap_end) return alloc_memory((3+size)/4,get_permanent_block_number());
	else return 0;
}
void machikania_free(void *ptr){
	if (g_heap_begin<g_heap_end) delete_memory(ptr);
}
void* machikania_calloc(int nmemb, int size){
	if (g_heap_begin<g_heap_end) return calloc_memory((3+nmemb*size)/4,get_permanent_block_number());
	else return 0;
}
