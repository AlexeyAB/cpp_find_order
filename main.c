#include <stdio.h>      /* printf, scanf, NULL */
#include <stdlib.h>     /* calloc, exit, free */
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */
#include <assert.h>
#include <string.h>

#if defined _MSC_VER && _MSC_VER < 1600
#define __restrict
#endif 

#ifdef _MSC_VER
#define __Inline __inline
#define __Noinline __declspec(noinline)
#else
#define __Inline static inline
#define __Noinline __attribute__((noinline))
#endif

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
__Inline struct T_cash_account_row generate_row() {
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
/* ----------------------------------------------------------------------- */

/* T_func is a pointer to one predicate function */
typedef unsigned char (*T_func_void)();
typedef unsigned char (*T_func)(struct T_cash_account_row const*const __restrict, struct 
	T_range_filters const*const __restrict, T_func_void, size_t *const __restrict index);

/* Compare row with filter amount_of_money */
__Inline unsigned char test_predicate_amount_of_money(struct T_cash_account_row const*const __restrict row, 
    struct T_range_filters const*const __restrict range_filters, T_func func[], size_t *const __restrict index) 
{
    return (row->amount_of_money >= range_filters->begin.amount_of_money &&
            row->amount_of_money <= range_filters->end.amount_of_money) && 
			func[++(*index)](row, range_filters, (T_func_void)func, index);
}
/* ----------------------------------------------------------------------- */

/* Compare row with filter gender */
__Inline unsigned char test_predicate_gender(struct T_cash_account_row const*const __restrict row, 
    struct T_range_filters const*const __restrict range_filters, T_func func[], size_t *const __restrict index) 
{
    return (row->gender >= range_filters->begin.gender && row->gender <= range_filters->end.gender) && 
		func[++(*index)](row, range_filters, (T_func_void)func, index);
}
/* ----------------------------------------------------------------------- */

/* Compare row with filter age */
__Inline unsigned char test_predicate_age(struct T_cash_account_row const*const __restrict row, 
    struct T_range_filters const*const __restrict range_filters, T_func func[], size_t *const __restrict index) 
{
    return (row->age >= range_filters->begin.age && row->age <= range_filters->end.age) && 
		func[++(*index)](row, range_filters, (T_func_void)func, index);
}
/* ----------------------------------------------------------------------- */

/* Compare row with filter code */
__Inline unsigned char test_predicate_code(struct T_cash_account_row const*const __restrict row, 
    struct T_range_filters const*const __restrict range_filters, T_func func[], size_t *const __restrict index)  
{
    return (row->code >= range_filters->begin.code && row->code <= range_filters->end.code) && 
		func[++(*index)](row, range_filters, (T_func_void)func, index);
}
/* ----------------------------------------------------------------------- */

/* Compare row with filter height */
__Inline unsigned char test_predicate_height(struct T_cash_account_row const*const __restrict row, 
    struct T_range_filters const*const __restrict range_filters, T_func func[], size_t *const __restrict index) 
{
    return (row->height >= range_filters->begin.height && row->height <= range_filters->end.height) && 
		func[++(*index)](row, range_filters, (T_func_void)func, index);
}
/* ----------------------------------------------------------------------- */

/* Always false */
__Inline unsigned char test_predicate_true(struct T_cash_account_row const*const __restrict row, 
    struct T_range_filters const*const __restrict range_filters, T_func func[], size_t *const __restrict index) 
{
    return 1;
}
/* ----------------------------------------------------------------------- */

/* C optimized search */
__Inline size_t search_optimized(struct T_cash_account_row const*const __restrict array_ptr, const size_t c_array_size,
	struct T_cash_account_row *const __restrict result_ptr, struct T_range_filters const*const __restrict range_filters) 
{
	static T_func func_ptr[last_e + 1];
	size_t result_size = 0;
	size_t i; /* loop index */
	size_t count_of_func = 0;
	if(range_filters->use_filter[amount_of_money_e]) func_ptr[count_of_func++] = (T_func)test_predicate_amount_of_money;
	if(range_filters->use_filter[gender_e]) func_ptr[count_of_func++] = (T_func)test_predicate_gender;
	if(range_filters->use_filter[code_e]) func_ptr[count_of_func++] = (T_func)test_predicate_code;
	if(range_filters->use_filter[age_e]) func_ptr[count_of_func++] = (T_func)test_predicate_age;
	if(range_filters->use_filter[height_e]) func_ptr[count_of_func++] = (T_func)test_predicate_height;
	func_ptr[count_of_func] = (T_func)test_predicate_true;

	for(i = 0; i < c_array_size; ++i) {
		size_t k = 0;
		if(func_ptr[0](array_ptr + i, range_filters, (T_func_void)&func_ptr, &k))
			result_ptr[result_size] = array_ptr[i], ++result_size;
	}
	
	return result_size;
}
/* ----------------------------------------------------------------------- */

#include "permut.h"

#define filt_amount_of_money(k, row, filt) \
  ( !((k)&(1<<amount_of_money_e)) || \
    (row)->amount_of_money >= (filt)->begin.amount_of_money && \
    (row)->amount_of_money <= (filt)->end.amount_of_money )

#define filt_gender(k, row, filt) \
  ( !((k)&(1<<gender_e)) || \
    (row)->gender >= (filt)->begin.gender && \
    (row)->gender <= (filt)->end.gender )

#define filt_age(k, row, filt) \
  ( !((k)&(1<<age_e)) || \
    (row)->age >= (filt)->begin.age && \
    (row)->age <= (filt)->end.age )

#define filt_code(k, row, filt) \
  ( !((k)&(1<<code_e)) || \
    (row)->code >= (filt)->begin.code && \
    (row)->code <= (filt)->end.code )

#define filt_height(k, row, filt) \
  ( !((k)&(1<<height_e)) || \
    (row)->height >= (filt)->begin.height && \
    (row)->height <= (filt)->end.height )

double get_selectivity(int useit, unsigned begin, unsigned end, unsigned maxval)
{
    if ( !useit ) return (unsigned)-2;
    if ( begin > end ) return 0;
    return (double)(end-begin+1)/maxval;
}

#ifdef _FORCEINLINE
__forceinline
#else
__Inline 
#endif

size_t search_1(struct T_cash_account_row const *array_ptr, const size_t c_array_size,
	struct T_cash_account_row *result_ptr, struct T_range_filters const*range_filters) 
{
    #define get_selectivity(n) get_selectivity(range_filters->use_filter[n##_e],range_filters->begin.n,range_filters->end.n,maxvals.n); 
	size_t result_size = 0;
	size_t i; /* loop index */
    size_t filt_mask = 0;
    int prior = 0;
    struct T_cash_account_row maxvals;
    double selectivity[last_e];
    
    memset(&maxvals,0xff,sizeof(maxvals));
    selectivity[code_e]   = get_selectivity(code); 
    selectivity[age_e]    = get_selectivity(age); 
    selectivity[height_e] = get_selectivity(height); 
    selectivity[gender_e] = get_selectivity(gender); 
    selectivity[amount_of_money_e] = get_selectivity(amount_of_money); 
    
    for(i = 0; i < last_e; ++i )
    {
        int j, n = 0;
        for ( j = 1; j < last_e; ++j )
            if ( selectivity[n] > selectivity[j] )
                n = j;
        prior = (prior << 4) | (n+1);
        selectivity[n] = (unsigned)-1;
    }       
    
    for(i = 0; i < last_e; ++i )
        if( range_filters->use_filter[i] ) 
            filt_mask |= 1<<i;
        
    #define filt_field(n,filter) \
      case n: \
          for(i = 0; i < c_array_size; ++i) if ( filter ) \
              result_ptr[result_size] = array_ptr[i], ++result_size; \
          break

    switch ( prior ) { $$permut_5(filt_field,0x12345,\
                                filt_amount_of_money(filt_mask,array_ptr+i,range_filters),\
                                filt_gender(filt_mask,array_ptr+i,range_filters),\
                                filt_age(filt_mask,array_ptr+i,range_filters),\
                                filt_code(filt_mask,array_ptr+i,range_filters),\
                                filt_height(filt_mask,array_ptr+i,range_filters),\
                                1); }
                                
	return result_size; 
}

/* Compare row with filters */
__Inline unsigned char test_predicate(struct T_cash_account_row const*const __restrict row, 
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
size_t search(struct T_cash_account_row const*const __restrict array_ptr, const size_t c_array_size,
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

    int qwerty;
	size_t i; /* loop index */
    struct T_range_filters range_filters = {0,};
    struct T_cash_account_row *const array_ptr = ( struct T_cash_account_row *)calloc(c_array_size, sizeof(struct T_cash_account_row));
    struct T_cash_account_row *const result_ptr = calloc(c_array_size, sizeof(struct T_cash_account_row));
	size_t result_size_1, result_size_2;
	clock_t end, start;
    float co_took_time, c_took_time, cm_took_time;
    
    //srand(3);
    
    if (array_ptr == NULL) {
		printf ("calloc error\n");
		exit(1);
	}

	/* initialize random seed: */
	/* srand (time(NULL)); */

	/* Fill table random data */
	for(i = 0; i < c_array_size; ++i) 
		array_ptr[i] = generate_row();

	range_filters.use_filter[amount_of_money_e] = rand()%1 + 0;
	range_filters.use_filter[gender_e] = rand()%1 + 0;
	range_filters.use_filter[height_e] = rand()%1 + 0;

    range_filters.begin.age = rand() % 100;
    range_filters.end.age = range_filters.begin.age + 5;
    range_filters.use_filter[age_e] = rand()%1 + 1;

    range_filters.begin.code = rand() % 30000;
    range_filters.end.code = range_filters.begin.code + 5;
    range_filters.use_filter[code_e] = rand()%1 + 1;
    
	start = clock();
	result_size_1 = search_1(array_ptr, c_array_size, result_ptr, &range_filters);
	end = clock();
	cm_took_time = (float)(end - start);

	start = clock();
	result_size_2 = search(array_ptr, c_array_size, result_ptr, &range_filters);
	end = clock();
	c_took_time = (float)(end - start);

    assert(result_size_1 == result_size_2);
    printf("%d %d %f %f %f\n",c_array_size,result_size_1,cm_took_time/CLOCKS_PER_SEC,c_took_time/CLOCKS_PER_SEC,c_took_time/cm_took_time);
    return 0;
}