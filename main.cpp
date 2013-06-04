#include <stdio.h>      /* printf, scanf, NULL */
#include <stdlib.h>     /* calloc, exit, free */
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */

#include <memory>		// std::unique_ptr<>
#include <array>		// std::array<>
#include <type_traits>	// std::enable_if<>

#include <iostream>		// std::cout
#include <map>			// std::map<>

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

	// Get cardinality
	template<int field_enum>
	// constexpr // not supported in the MSVS2012(MVCC11)
	static inline const unsigned int get_bitf_size() { 
		return (field_enum == gender_e)?1:
			(field_enum == age_e)?		100:
			(field_enum == height_e)?	300:
			(field_enum == code_e)?		1000000:
			(field_enum == amount_of_money_e)?1000000:
			0;
		static_assert(5 == last_e, "Add/delete new field-case and correct this assert!");
	}

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

// Templated constructor of class unrolling (no return, mandatory call to the constructor, called once)
template<unsigned unroll_count, template<unsigned> class T>
struct T_unroll_constructor {
	T_unroll_constructor<unroll_count-1, T> next_unroll;
	T<unroll_count-1> functor;
	template<typename T1> inline T_unroll_constructor(T1 &val1) : next_unroll(val1), functor(val1) {}
	template<typename T1, typename T2> inline T_unroll_constructor(T1 *const __restrict val1, T2 *const __restrict val2) : next_unroll(val1, val2), functor(val1, val2) {}
};
// End of unroll
template<template<unsigned> class T>
struct T_unroll_constructor<0, T> { 
	template<typename T1> inline T_unroll_constructor(T1 &) {} 
	template<typename T1, typename T2> inline T_unroll_constructor(T1 *const __restrict, T2 *const __restrict) {}
};
// -------------------------------------------------------------------------

// Templated functor unrolling (reusable)
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
		for(size_t i = 0; i < T_row::last_e; ++i) result |= range_filters->use_filter[i]?(1<<i):0;
		return result;
	}
	// -------------------------------------------------------------------------

	// The unrolling filling of selectivity in a compile-time
	template<unsigned field_num>
	struct T_selectivity_fill {
		T_selectivity_fill(std::map<unsigned, unsigned> *const __restrict ordered_filters, T_range_filters const*const __restrict range_filters) { 
			const auto begin = range_filters->begin.template get_field_value<field_num>(), end = range_filters->end.template get_field_value<field_num>();
			const auto delta = (begin > end)?0:(1 + end - begin);
			const unsigned selectivity = range_filters->use_filter[field_num]?
				(delta*c_array_size / 
				T_row::template get_bitf_size<field_num>() // cardinality
				)
				:c_array_size;
			ordered_filters->insert(std::make_pair(field_num, selectivity));		// selectivity for each field-filter
		}
	};
		
	// Get index of T_test_pred version for current search range 
	static inline const unsigned get_index_order(T_range_filters const*const __restrict range_filters) {
		unsigned result = 0;

		std::map<unsigned, unsigned> ordered_filters;	
		T_unroll_constructor<T_row::last_e, T_selectivity_fill> selectivity_fill(&ordered_filters, range_filters);

		unsigned multiplexor = 1;
		for(size_t i = 0; i < T_row::last_e; ++i) {
			unsigned cur_index = 0, min_index = 0;
			unsigned field_num = ordered_filters.cbegin()->first, min = c_array_size;
			for(auto const& it : ordered_filters) {
				if (it.second < min) min = it.second, field_num = it.first, min_index = cur_index;
				++cur_index;
			}
			ordered_filters.erase(field_num);
			result += min_index*multiplexor;
			multiplexor *= (T_row::last_e - i);
		}
		return result;
	}
	// -------------------------------------------------------------------------
	// Brute force permutations fields at compile-time

	// Get the sequence number the remaining field for the number_filter, after removal of all previous 
	template <unsigned index_order, unsigned number_filter, unsigned index = T_row::last_e> 
	struct T_number_remaining_field : std::integral_constant<unsigned, T_number_remaining_field<index_order / index, number_filter - 1, index - 1>::value> {};

	// End of unroll
	template <unsigned index_order, unsigned index> 
	struct T_number_remaining_field<index_order, 0, index> : std::integral_constant<unsigned, index_order % index> {};
	// -------------------------------------------------------------------------

	// Get 1 or 0 offset if current_field (for number_filter) affect to number_field
	template <unsigned index_order, unsigned number_filter, unsigned number_field>
	struct T_offset {
		enum { current_field = T_number_remaining_field<index_order, number_filter>::value,
			value = (current_field <= number_field)?1:0 };
	};
	
	// Get offset of number_field (enum in row-type) for number_filter
	template <unsigned index_order, unsigned number_filter, unsigned number_field>
	struct T_offset_number_field {
		enum { offset = T_offset<index_order, number_filter-1, number_field>::value,
			value = T_offset_number_field<index_order, number_filter-1, number_field + offset>::value + offset };
	};

	// End of unroll
	template <unsigned index_order, unsigned number_field>
	struct T_offset_number_field<index_order, 0, number_field> : std::integral_constant<unsigned, 0> {};

	// (Main) Get number of field (enum in row-type) for number_filter
	template <unsigned index_order, unsigned number_filter>
	struct T_number_field {
		enum { remaining_field = T_number_remaining_field<index_order, number_filter>::value,
			value = remaining_field + T_offset_number_field<index_order, number_filter, remaining_field>::value };
	};
	// -------------------------------------------------------------------------

	// search
	virtual size_t search(T_row const*const __restrict array_ptr, const size_t c_array_size,
		T_row *const __restrict result_ptr, T_range_filters const*const __restrict filters) = 0;
};
// -------------------------------------------------------------------------

// The filters for each search variant (range_filters)
template<typename T_row, unsigned index_pred, unsigned index_order = 0>
struct T_custom_filter : T_filter<T_row> {

	// Test predicates functor for unrolling
	template<unsigned field_num>
	struct T_test_pred {
		bool inline operator()(T_row const*const __restrict row, T_range_filters const*const __restrict range_filters) const 
		{
			typedef typename T_row::T_field_enum T_field_enum;
			// Without fields where use_filter==0
			enum { ordered_field_number = T_filter<T_row>::template T_number_field<index_order, field_num>::value };
			return ( T_filter<T_row>::template T_get_use_filter<index_pred, ordered_field_number>::value ||
				(row->template get_field_value<static_cast<T_field_enum>(ordered_field_number)>() >= 
					range_filters->begin.template get_field_value<static_cast<T_field_enum>(ordered_field_number)>()	&& 
				row->template get_field_value<static_cast<T_field_enum>(ordered_field_number)>() <= 
					range_filters->end.template get_field_value<static_cast<T_field_enum>(ordered_field_number)>()) );
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

template <unsigned N> struct factorial : std::integral_constant<unsigned, N * factorial<N-1>::value> {};
template <> struct factorial<0> : std::integral_constant<unsigned, 1> {};

template<typename T_row>
class T_optimized_search {
	// unroll tamplates
	template<unsigned index_pred>
	struct T_unroll_find {
		template<unsigned index_order>
		struct T_unroll_order {
			template<typename T> T_unroll_order(T &filters) { 
				filters[index_pred + index_order*(1<<T_row::last_e)].reset( new T_custom_filter<T_row, index_pred, index_order>() ); 
			}
		};
		T_unroll_constructor<factorial<T_row::last_e>::value, T_unroll_order> fill_ordered_filter;

		template<typename T> T_unroll_find(T &filters) : fill_ordered_filter(filters)
		{ 
		}
	};
	// -------------------------------------------------------------------------

	std::array<std::unique_ptr<T_filter<T_row>>, (1<<T_row::last_e)*factorial<T_row::last_e>::value> filters;
	T_unroll_constructor< 1<<T_row::last_e, T_unroll_find> fill_filter;
public:
	T_optimized_search() : fill_filter(filters) {}

	// C++ optimized search
	inline size_t search(T_row const*const __restrict array_ptr, const size_t c_array_size,
			T_row *const __restrict result_ptr, T_range_filters const*const __restrict range_filters) 
	{
		auto const& filter = filters[T_filter<T_row>::get_index_pred(range_filters) + T_filter<T_row>::get_index_order(range_filters)*(1<<T_row::last_e)];
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