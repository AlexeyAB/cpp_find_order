#include <stdio.h>      /* printf, scanf, NULL */
#include <stdlib.h>     /* calloc, exit, free */
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */

const size_t c_array_size = 10000000;

/* Fields */
enum T_field_enum { amount_of_money_e, gender_e, age_e, code_e, height_e,   /*<<<<<- add fields here */	last_e };

/* Row */
struct T_cash_account_row {
    unsigned code:20;			// 0 - 1000000
	unsigned gender:1;			// 0 - 1
	unsigned age:7;			    // 0 - 100	
	unsigned amount_of_money:20;// 0 - 1000000
	unsigned height:9;			// 0 � 300
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
/* ----------------------------------------------------------------------- */

/* T_func is a pointer to one predicate function */
typedef unsigned char (*T_func_void)();
typedef unsigned char (*T_func)(struct T_cash_account_row const*const __restrict, struct 
	T_range_filters const*const __restrict, T_func_void, size_t *const __restrict index);

/* Compare row with filter amount_of_money */
static inline unsigned char test_predicate_amount_of_money(struct T_cash_account_row const*const __restrict row, 
    struct T_range_filters const*const __restrict range_filters, T_func func[], size_t *const __restrict index) 
{
    return (row->amount_of_money >= range_filters->begin.amount_of_money &&
            row->amount_of_money <= range_filters->end.amount_of_money) && 
			func[++(*index)](row, range_filters, (T_func_void)func, index);
}
/* ----------------------------------------------------------------------- */

/* Compare row with filter gender */
static inline unsigned char test_predicate_gender(struct T_cash_account_row const*const __restrict row, 
    struct T_range_filters const*const __restrict range_filters, T_func func[], size_t *const __restrict index) 
{
    return (row->gender >= range_filters->begin.gender && row->gender <= range_filters->end.gender) && 
		func[++(*index)](row, range_filters, (T_func_void)func, index);
}
/* ----------------------------------------------------------------------- */

/* Compare row with filter age */
static inline unsigned char test_predicate_age(struct T_cash_account_row const*const __restrict row, 
    struct T_range_filters const*const __restrict range_filters, T_func func[], size_t *const __restrict index) 
{
    return (row->age >= range_filters->begin.age && row->age <= range_filters->end.age) && 
		func[++(*index)](row, range_filters, (T_func_void)func, index);
}
/* ----------------------------------------------------------------------- */

/* Compare row with filter code */
static inline unsigned char test_predicate_code(struct T_cash_account_row const*const __restrict row, 
    struct T_range_filters const*const __restrict range_filters, T_func func[], size_t *const __restrict index)  
{
    return (row->code >= range_filters->begin.code && row->code <= range_filters->end.code) && 
		func[++(*index)](row, range_filters, (T_func_void)func, index);
}
/* ----------------------------------------------------------------------- */

/* Compare row with filter height */
static inline unsigned char test_predicate_height(struct T_cash_account_row const*const __restrict row, 
    struct T_range_filters const*const __restrict range_filters, T_func func[], size_t *const __restrict index) 
{
    return (row->height >= range_filters->begin.height && row->height <= range_filters->end.height) && 
		func[++(*index)](row, range_filters, (T_func_void)func, index);
}
/* ----------------------------------------------------------------------- */

/* Always false */
static inline unsigned char test_predicate_true(struct T_cash_account_row const*const __restrict row, 
    struct T_range_filters const*const __restrict range_filters, T_func func[], size_t *const __restrict index) 
{
    return 1;
}
/* ----------------------------------------------------------------------- */

/* C optimized search */
static inline size_t search_optimized(struct T_cash_account_row const*const __restrict array_ptr, const size_t c_array_size,
	struct T_cash_account_row *const __restrict result_ptr, struct T_range_filters const*const __restrict range_filters) 
{
	static T_func func_ptr[last_e + 1];
	size_t count_of_func = 0;
	if(range_filters->use_filter[amount_of_money_e]) func_ptr[count_of_func++] = (T_func)test_predicate_amount_of_money;
	if(range_filters->use_filter[gender_e]) func_ptr[count_of_func++] = (T_func)test_predicate_gender;
	if(range_filters->use_filter[code_e]) func_ptr[count_of_func++] = (T_func)test_predicate_code;
	if(range_filters->use_filter[age_e]) func_ptr[count_of_func++] = (T_func)test_predicate_age;
	if(range_filters->use_filter[height_e]) func_ptr[count_of_func++] = (T_func)test_predicate_height;
	func_ptr[count_of_func] = (T_func)test_predicate_true;

	size_t result_size = 0;
	size_t i; /* loop index */
	for(i = 0; i < c_array_size; ++i) {
		size_t k = 0;
		if(func_ptr[0](array_ptr + i, range_filters, (T_func_void)&func_ptr, &k))
			result_ptr[result_size] = array_ptr[i], ++result_size;
	}
	return result_size;
}
/* ----------------------------------------------------------------------- */

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

    struct T_range_filters range_filters = {};

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

	printf ("C-optimized-Searching...\n");

	start = clock();
	result_size = search_optimized(array_ptr, c_array_size, result_ptr, &range_filters);
	end = clock();
	float co_took_time = (float)(end - start);
	printf ("C-optimized-search took %f seconds.\n", co_took_time/CLOCKS_PER_SEC);

	printf ("Found rows: %d \n", result_size);

	printf ("C-Searching...\n");

	start = clock();
	result_size = search(array_ptr, c_array_size, result_ptr, &range_filters);
	end = clock();
	float c_took_time = (float)(end - start);
	printf ("C-search took %f seconds.\n", c_took_time/CLOCKS_PER_SEC);

	printf ("The C++ faster than C: %f times \n", c_took_time/co_took_time);

	printf ("Found rows: %d \n", result_size);
    
	/*for(i = 0; i < result_size; ++i) {
		printf ("%d, %d, %d, %d, %d \n", 
			result_ptr[i].age, result_ptr[i].amount_of_money, result_ptr[i].code, result_ptr[i].gender, result_ptr[i].height);
	}*/

    int qwerty;
    scanf ("%d",&qwerty);
    return 0;
}