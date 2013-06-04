#include <stdio.h>      /* printf, scanf, NULL */
#include <stdlib.h>     /* calloc, exit, free */
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */

#include <memory>    	// std::unique_ptr<>
#include <array>		// std::array<>
#include <type_traits>	// std::enable_if<>


const size_t c_array_size = 10000000;

/* Row */
struct T_cash_account_row {

	// Fields:
	enum T_field_enum { amount_of_money_e, gender_e, age_e, code_e, height_e,   /*<<<<<- add fields here (with index) */
		last_e 
			/*<<<<<- add included fields here (without index) */
	};
	static_assert(last_e > 0, "Number of indexed fields in enum of T_user_row must be greater than 0!");

    unsigned code:20;			// 0 - 1000000
	unsigned gender:1;			// 0 - 1
	unsigned age:7;			    // 0 - 100	
	unsigned amount_of_money:20;// 0 - 1000000
	unsigned height:9;			// 0 – 300


	// Get field value
	template<int field_enum> inline typename std::enable_if<field_enum == code_e, decltype(code)>::type		get_field_value() const { return code; }
	template<int field_enum> inline typename std::enable_if<field_enum == gender_e, decltype(gender)>::type get_field_value() const { return gender; }
	template<int field_enum> inline typename std::enable_if<field_enum == age_e, decltype(age)>::type		get_field_value() const { return age; }
	template<int field_enum> inline typename std::enable_if<field_enum == amount_of_money_e, decltype(amount_of_money)>::type get_field_value() const { return amount_of_money; }
	template<int field_enum> inline typename std::enable_if<field_enum == height_e, decltype(height)>::type get_field_value() const { return height; }

	template<int field_enum> inline typename std::enable_if<field_enum == last_e, bool>::type get_field_value() const { return true; }

	static_assert(5 == last_e, "Add/delete new field-case and correct this assert!");
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

/* C-style filters */
struct T_range_filters {
    struct T_cash_account_row begin, end;
    /* bytes array or bitset from https://gist.github.com/jmbr/667605 */
    unsigned char use_filter[T_cash_account_row::last_e]; 
};
/* ----------------------------------------------------------------------- */


// The templated constructor of unrolling of the class (no return, mandatory call to the constructor, called once)
template<unsigned unroll_count, template<unsigned> class T>
struct T_unroll_constructor {
	T_unroll_constructor<unroll_count-1, T> next_unroll;
	T<unroll_count-1> functor;
	template<typename T1> inline T_unroll_constructor(T1 &val1) : next_unroll(val1), functor(val1) {}
};
// End of unroll
template<template<unsigned> class T>
struct T_unroll_constructor<0, T> { 
	template<typename T1> inline T_unroll_constructor(T1 &) {} 
};
// -------------------------------------------------------------------------

// The templated functor of unrolling of the input templated functor (reusable)
template<unsigned unroll_count, template<unsigned> class T>
struct T_unroll_functor {
	T_unroll_functor<unroll_count-1, T> next_unroll;
	T<unroll_count-1> functor;
	template<typename T1, typename T2> inline bool operator()(T1 const& val1, T2 const& val2) {
		return next_unroll(val1, val2) && functor(val1, val2);
	}
};
// End of unroll
template<template<unsigned> class T>
struct T_unroll_functor<0, T> { 
	template<typename T1, typename T2> inline bool operator()(T1 const&, T2 const&) { return true; } 
};
// -------------------------------------------------------------------------

// Abstract base class of filters for each search variant (range_filters)
template<typename T_row>
struct T_filter {
	// whether to use the filter for a given predicate? (strictly at compile time)
	template <unsigned index_pred, unsigned number_filter> 
	struct T_get_use_filter : std::integral_constant<bool, !(index_pred & 1<<number_filter)> {};

	// Get index of T_test_pred version for current search range 
	static inline const unsigned get_index_pred(T_range_filters const*const __restrict range_filters) {
		unsigned result = 0;
		for(size_t i = 0; i <  T_row::last_e; ++i) result |= range_filters->use_filter[i]?(1<<i):0;
		return result;
	}
	
	// search
	virtual size_t search(T_row const*const __restrict array_ptr, const size_t c_array_size,
		T_row *const __restrict result_ptr, T_range_filters const*const __restrict filters) = 0;
};
// -------------------------------------------------------------------------

// The filters for each search variant (range_filters)
template<typename T_row, unsigned index_pred>
struct T_custom_filter : T_filter<T_row> {

	// Test predicates functor for unrolling
	template<unsigned field_num>
	struct T_test_pred {
		bool inline operator()(T_row const*const __restrict row, T_range_filters const*const __restrict range_filters) const 
		{
			typedef typename T_row::T_field_enum T_field_enum;
			// Without fields where use_filter==0
			return ( T_filter<T_row>::template T_get_use_filter<index_pred, field_num>::value ||
				(row->template get_field_value<static_cast<T_field_enum>(field_num)>() >= 
					range_filters->begin.template get_field_value<static_cast<T_field_enum>(field_num)>()	&& 
				row->template get_field_value<static_cast<T_field_enum>(field_num)>() <= 
					range_filters->end.template get_field_value<static_cast<T_field_enum>(field_num)>()) );
		}			
	};
	// -----------------------------------------------------------------------
	
	// search
	virtual size_t search(T_row const*const __restrict array_ptr, const size_t c_array_size,
		T_row *const __restrict result_ptr, T_range_filters const*const __restrict range_filters) final
	{
		size_t result_size = 0;
		size_t i; // loop index
		T_unroll_functor<T_row::last_e, T_test_pred> test_predicate;
		for(i = 0; i < c_array_size; ++i) {
			if(test_predicate(array_ptr + i, range_filters)) 
				result_ptr[result_size] = array_ptr[i], ++result_size;
		}
		return result_size;
	}
};
// -------------------------------------------------------------------------

template<typename T_row>
class T_optimized_search {
	// unroll tamplates
	template<unsigned index_pred>
	struct T_unroll_find {
		template<typename T> T_unroll_find(T &filters) { 
			filters[index_pred].reset( new T_custom_filter<T_row, index_pred>() ); 
		}
	};
	// -------------------------------------------------------------------------

	std::array<std::unique_ptr<T_filter<T_row>>, 1<<T_row::last_e> filters;
	T_unroll_constructor< 1<<T_row::last_e, T_unroll_find> fill_filter;
public:
	T_optimized_search() : fill_filter(filters) {}

	// C++ optimized search
	inline size_t search(T_row const*const __restrict array_ptr, const size_t c_array_size,
			T_row *const __restrict result_ptr, T_range_filters const*const __restrict range_filters) 
	{
		auto const& filter = filters[T_filter<T_row>::get_index_pred(range_filters)];
		return filter->search(array_ptr, c_array_size, result_ptr, range_filters);
	}
};
// -------------------------------------------------------------------------

/* Compare row with filters */
static inline unsigned char test_predicate(struct T_cash_account_row const*const __restrict row, 
    struct T_range_filters const*const __restrict range_filters) 
{    
    return 
        (!range_filters->use_filter[T_cash_account_row::amount_of_money_e] || 
            (row->amount_of_money >= range_filters->begin.amount_of_money &&
            row->amount_of_money <= range_filters->end.amount_of_money)) &&
        (!range_filters->use_filter[T_cash_account_row::gender_e] || 
            (row->gender >= range_filters->begin.gender && row->gender <= range_filters->end.gender)) &&
        (!range_filters->use_filter[T_cash_account_row::age_e] || 
            (row->age >= range_filters->begin.age && row->age <= range_filters->end.age)) &&
        (!range_filters->use_filter[T_cash_account_row::code_e] || 
            (row->code >= range_filters->begin.code && row->code <= range_filters->end.code)) &&
        (!range_filters->use_filter[T_cash_account_row::height_e] || 
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

	/* C-style range filters */
    struct T_range_filters range_filters = {};  

	range_filters.use_filter[T_cash_account_row::amount_of_money_e] = rand()%1 + 0;
	range_filters.use_filter[T_cash_account_row::gender_e] = rand()%1 + 0;
	range_filters.use_filter[T_cash_account_row::height_e] = rand()%1 + 0;

    range_filters.begin.age = rand() % 100;
    range_filters.end.age = range_filters.begin.age + 5;
    range_filters.use_filter[T_cash_account_row::age_e] = rand()%1 + 1;

    range_filters.begin.code = rand() % 30000;
    range_filters.end.code = range_filters.begin.code + 5;
    range_filters.use_filter[T_cash_account_row::code_e] = rand()%1 + 1;

    struct T_cash_account_row *const result_ptr = ( struct T_cash_account_row *)calloc(c_array_size, sizeof(struct T_cash_account_row));

	size_t result_size;
	clock_t end, start;
	T_optimized_search<T_cash_account_row> optimized_search;	// C++ optimized search

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