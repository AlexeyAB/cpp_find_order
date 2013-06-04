#include <stdio.h>      /* printf, scanf, NULL */
#include <stdlib.h>     /* calloc, exit, free */
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */

#include <memory>		/* std::unique_ptr<> */
#include <array>		/* std::array<> */

const size_t c_array_size = 10000000;

/* Fields */
enum T_field_enum { amount_of_money_e, gender_e, age_e, code_e, height_e,   /*<<<<<- add fields here */	last_e };

/* Row */
struct T_cash_account_row {
    unsigned code:20;			// 0 - 1000000
	unsigned gender:1;			// 0 - 1
	unsigned age:7;			    // 0 - 100	
	unsigned amount_of_money:20;// 0 - 1000000
	unsigned height:9;			// 0 – 300
};
/* ----------------------------------------------------------------------- */

/* Generate random data for the one row */
static inline struct T_cash_account_row generate_row() {
	struct T_cash_account_row cash_account_row;
	cash_account_row.age = rand() % 100;
	cash_account_row.amount_of_money = (rand() % 1000)*(rand() % 1000);
	cash_account_row.code = (rand() % 1000)*(rand() % 1000);
	cash_account_row.gender = rand() % 2;
	cash_account_row.height = rand() % 300;
	return cash_account_row;
}
/* ----------------------------------------------------------------------- */

/* Filters */
struct T_range_filters {
    struct T_cash_account_row begin, end;
    /* bytes array or bitset from https://gist.github.com/jmbr/667605 */
	unsigned char use_filter[last_e]; 
};
// -------------------------------------------------------------------------

// The templated constructor of unrolling of the class (no return, mandatory call to the constructor, called once)
template<unsigned unroll_count, template<unsigned> class T>
struct T_unroll_constructor {
	T_unroll_constructor<unroll_count-1, T> next_unroll;
	T<unroll_count-1> functor;
	template<typename T1> inline T_unroll_constructor(T1 & val1) : next_unroll(val1), functor(val1) {}
};
// End of unroll
template<template<unsigned> class T>
struct T_unroll_constructor<0, T> { 
	template<typename T1> inline T_unroll_constructor(T1 &) {} 
};
// -------------------------------------------------------------------------

// Abstract base class of filters for each search variant (range_filters)
struct T_filter {
	// search
	virtual size_t search(T_cash_account_row const*const __restrict array_ptr, const size_t c_array_size,
		T_cash_account_row *const __restrict result_ptr, T_range_filters const*const __restrict range_filters) = 0;
};
// -------------------------------------------------------------------------

// The filters for each search variant (range_filters)
template<unsigned index_pred>
struct T_custom_filter : T_filter {

	inline unsigned char test_predicate(T_cash_account_row const*const __restrict row, 
		T_range_filters const*const __restrict range_filters) 
	{    
		return 
			(!(index_pred & 1<<amount_of_money_e) || 
				(row->amount_of_money >= range_filters->begin.amount_of_money &&
				row->amount_of_money <= range_filters->end.amount_of_money)) &&
			(!(index_pred & 1<<gender_e) || 
				(row->gender >= range_filters->begin.gender && row->gender <= range_filters->end.gender)) &&
			(!(index_pred & 1<<age_e) || 
				(row->age >= range_filters->begin.age && row->age <= range_filters->end.age)) &&
			(!(index_pred & 1<<code_e) || 
				(row->code >= range_filters->begin.code && row->code <= range_filters->end.code)) &&
			(!(index_pred & 1<<height_e) || 
				(row->height >= range_filters->begin.height && row->height <= range_filters->end.height));
	}
	// -------------------------------------------------------------------------

	// search
	virtual size_t search(T_cash_account_row const*const __restrict array_ptr, const size_t c_array_size,
		T_cash_account_row *const __restrict result_ptr, T_range_filters const*const __restrict range_filters) final
	{
		size_t result_size = 0;
		size_t i; // loop index
		for(i = 0; i < c_array_size; ++i) {
			if(test_predicate(array_ptr + i, range_filters)) 
				result_ptr[result_size] = array_ptr[i], ++result_size;
		}
		return result_size;
	}
};
// -------------------------------------------------------------------------

class T_optimized_search {
	// unroll tamplates
	template<unsigned index_pred>
	struct T_unroll_find {
		template<typename T> T_unroll_find(T &filters) { 
			filters[index_pred].reset( new T_custom_filter<index_pred>() ); 
		}
	};
	// -------------------------------------------------------------------------

	// Get index of T_test_pred version for current search range
	inline unsigned get_index_pred(T_range_filters const*const __restrict range_filters) {
		unsigned result = 0;
		for(size_t i = 0; i < last_e; ++i) 
			result |= range_filters->use_filter[i]?(1<<i):0;
		return result;
	}

	std::array<std::unique_ptr<T_filter>, 1<<last_e> filters;
	T_unroll_constructor<1<<last_e, T_unroll_find> fill_filter;
public:
	T_optimized_search() : fill_filter(filters) {}

	// C++ optimized search
	inline size_t search(T_cash_account_row const*const __restrict array_ptr, const size_t c_array_size,
			T_cash_account_row *const __restrict result_ptr, T_range_filters const*const __restrict range_filters) 
	{
		auto const& filter = filters[get_index_pred(range_filters)];
		return filter->search(array_ptr, c_array_size, result_ptr, range_filters);
	}
};
// -------------------------------------------------------------------------

/* Compare row with filters */
static inline unsigned char test_predicate(struct T_cash_account_row const*const __restrict row, 
    struct T_range_filters const*const __restrict range_filters) 
{    
    return 
        (!range_filters->use_filter[amount_of_money_e] || 
            (row->amount_of_money >= range_filters->begin.amount_of_money &&
            row->amount_of_money <= range_filters->end.amount_of_money)) &&
        (!range_filters->use_filter[gender_e] || 
            (row->gender >= range_filters->begin.gender && row->gender <= range_filters->end.gender)) &&
        (!range_filters->use_filter[age_e] || 
            (row->age >= range_filters->begin.age && row->age <= range_filters->end.age)) &&
        (!range_filters->use_filter[code_e] || 
            (row->code >= range_filters->begin.code && row->code <= range_filters->end.code)) &&
        (!range_filters->use_filter[height_e] || 
            (row->height >= range_filters->begin.height && row->height <= range_filters->end.height));
}
/* ----------------------------------------------------------------------- */

/* search */
static inline size_t search(struct T_cash_account_row const*const __restrict array_ptr, const size_t c_array_size,
	struct T_cash_account_row *const __restrict result_ptr, struct T_range_filters const*const __restrict range_filters) 
{
	size_t result_size = 0;
	size_t i; /* loop index */
	for(i = 0; i < c_array_size; ++i) {
		if(test_predicate(array_ptr + i, range_filters)) 
			result_ptr[result_size] = array_ptr[i], ++result_size;
	}
	return result_size;
}
/* ----------------------------------------------------------------------- */

int main() {
    
	size_t i; /* loop index */
    struct T_cash_account_row *const array_ptr = ( struct T_cash_account_row *)calloc(c_array_size, sizeof(struct T_cash_account_row));
    if (array_ptr == NULL) {
		printf ("calloc error\n");
		exit(1);
	}

	/* initialize random seed: */
	/* srand (time(NULL)); */

	/* Fill table random data */
	for(i = 0; i < c_array_size; ++i) 
		array_ptr[i] = generate_row();
	printf ("Generated rows: %d \n", c_array_size);

    struct T_range_filters range_filters = {0};  

	range_filters.use_filter[amount_of_money_e] = rand()%1 + 0;
	range_filters.use_filter[gender_e] = rand()%1 + 0;
	range_filters.use_filter[height_e] = rand()%1 + 0;

    range_filters.begin.age = rand() % 100;
    range_filters.end.age = range_filters.begin.age + 5;
    range_filters.use_filter[age_e] = rand()%1 + 1;

    range_filters.begin.code = rand() % 30000;
    range_filters.end.code = range_filters.begin.code + 5;
    range_filters.use_filter[code_e] = rand()%1 + 1;
    
    struct T_cash_account_row *const result_ptr = ( struct T_cash_account_row *)calloc(c_array_size, sizeof(struct T_cash_account_row));

	size_t result_size;
	clock_t end, start;
	T_optimized_search optimized_search;	// C++ optimized search

	printf ("C++-Searching...\n");
	
	start = clock();
	result_size = optimized_search.search(array_ptr, c_array_size, result_ptr, &range_filters);
	end = clock();
	float cpp_took_time = static_cast<float>(end - start);
	printf ("C++-optimized search took %f seconds.\n", cpp_took_time/CLOCKS_PER_SEC);

	printf ("Found rows: %d \n", result_size);

	printf ("C-Searching...\n");

	start = clock();
	result_size = search(array_ptr, c_array_size, result_ptr, &range_filters);
	end = clock();
	float c_took_time = static_cast<float>(end - start);
	printf ("C-search took %f seconds.\n", c_took_time/CLOCKS_PER_SEC);

	printf ("The C++ faster than C: %f times \n", c_took_time/cpp_took_time);

	printf ("Found rows: %d \n", result_size);
   
	//for(i = 0; i < result_size; ++i) {
	//	printf ("%d, %d, %d, %d, %d \n", 
	//		result_ptr[i].age, result_ptr[i].amount_of_money, result_ptr[i].code, result_ptr[i].gender, result_ptr[i].height);
	//}

    int qwerty;
    scanf ("%d",&qwerty);
    return 0;
}