/*
     File:    category_table_builder.cpp
     Created: 13 March 2018 at 12:15 Moscow time
     Author:  Гаврилов Владимир Сергеевич
     E-mails: vladimir.s.gavrilov@gmail.com
              gavrilov.vladimir.s@mail.ru
              gavvs1977@yandex.ru
*/

#include <vector>
#include <set>
#include <map>
#include <utility>
#include "../include/category_table_builder.h"
#include "../include/operations_with_sets.h"
#include "../include/list_to_columns.h"
#include "../include/map_as_vector.h"
#include "../include/segment.h"
#include "../include/group_pairs.h"
#include "../include/create_permutation_tree.h"
#include "../include/permutation_tree_to_permutation.h"
#include "../include/permutation.h"
#include "../include/create_permutation.h"
#include "../include/myconcepts.h"
#include "../include/list_to_columns.h"
#include "../include/print_char32.h"
#include "../include/format.h"

using namespace std::string_literals;

using Set_of_char           = std::set<char32_t>;
using Category_name_and_set = std::pair<std::string, Set_of_char>;
using Categories_info       = std::vector<Category_name_and_set>;

static constexpr size_t INDENT_WIDTH = 4;

static const std::string category_table_fmt =
R"~(enum Category{{
{0}
}};

/*
 * It happens that in std::map<K,V> the key type is integer, and a lot of keys with
 * the same corresponding values. If such a map must be a generated constant, then
 * this map can be optimized. Namely, iterating through a map using range-based for,
 * we will build a std::vector<std::pair<K, V>>. Then we group pairs std::pair<K, V>
 * in pairs in the form (segment, a value of type V), where 'segment' is a struct
 * consisting of lower bound and upper bound. Next, we permute the grouped pair in
 * the such way that in order to search for in the array of the resulting values we
 * can use the algorithm from the answer to exercise 6.2.24 of the book
 *     Knuth D.E. The art of computer programming. Volume 3. Sorting and search. ---
 *     2nd ed. --- Addison-Wesley, 1998.
 */

#define RandomAccessIterator typename
#define Callable             typename
#define Integral             typename
template<typename T>
struct Segment{{
    T lower_bound;
    T upper_bound;

    Segment()               = default;
    Segment(const Segment&) = default;
    ~Segment()              = default;
}};

template<typename T, typename V>
struct Segment_with_value{{
    Segment<T> bounds;
    V          value;

    Segment_with_value()                          = default;
    Segment_with_value(const Segment_with_value&) = default;
    ~Segment_with_value()                         = default;
}};

/* This function uses algorithm from the answer to the exercise 6.2.24 of the monography
 *  Knuth D.E. The art of computer programming. Volume 3. Sorting and search. --- 2nd ed.
 *  --- Addison-Wesley, 1998.
*/
template<RandomAccessIterator I, typename K>
std::pair<bool, size_t> knuth_find(I it_begin, I it_end, K key)
{{
    std::pair<bool, size_t> result = {{false, 0}};
    size_t                  i      = 1;
    size_t                  n      = it_end - it_begin;
    while (i <= n) {{
        const auto& curr        = it_begin[i - 1];
        const auto& curr_bounds = curr.bounds;
        if(key < curr_bounds.lower_bound){{
            i = 2 * i;
        }}else if(key > curr_bounds.upper_bound){{
            i = 2 * i + 1;
        }}else{{
            result.first = true; result.second = i - 1;
            break;
        }}
    }}
    return result;
}}

static const Segment_with_value<char32_t, uint64_t> categories_table[] = {{
{1}
}};

static constexpr size_t num_of_elems_in_categories_table = size(categories_table);

uint64_t get_categories_set(char32_t c)
{{
    auto t = knuth_find(categories_table,
                        categories_table + num_of_elems_in_categories_table,
                        c);
    return t.first ? categories_table[t.second].value : (1ULL << Other)
}}
)~"s;

static const std::string table_elem_fmt = R"~({{{{{0}, {1}}}, {2}}})~"s;

/* The next function for a given character of type char32_t builds the set of categories
 * to which it belongs. Information about categories is transmitted in a vector whose
 * elements are of type Category_name_and_set. The set of categories is represented as a
 * number of the type uint64_t: the equality to zero of some category means that the
 * category with the corresponding number does not belong to this set, and the equality
 * to one that belongs to. */
static uint64_t construct_categories_set_for_char(char32_t               c,
                                                  const Categories_info& categories_info)
{
    using operations_with_sets::is_elem;
    uint64_t result  = 0;
    uint64_t counter = 0;
    for(const auto& z : categories_info){
        if(is_elem(c, z.second)){
            result |= 1ULL << counter;
        }
        counter++;
    }
    return result;
}

std::string construct_category_enum(const Categories_info& categories_info)
{
    std::string              result;
    std::vector<std::string> category_names;
    for(const auto& z : categories_info){
        category_names.push_back(z.first);
    }
    category_names.push_back("Other");
    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 2;
    f.spaces_between_columns = 1;
    auto body                = string_list_to_columns(category_names, f);
    return result;
}

template<RandomAccessIterator DestIt, RandomAccessIterator SrcIt, Callable F>
void permutate(DestIt dest_begin, SrcIt src_begin, SrcIt src_end, F f){
    size_t num_of_elems = src_end - src_begin;
    for(size_t i = 0; i < num_of_elems; ++i){
        dest_begin[f(i)] = src_begin[i];
    }
}

template<Integral K, typename V>
SegmentsV<K, V> create_classification_table(const std::map<K, V>& m)
{
   SegmentsV<K,V> grouped_pairs = group_pairs(map_as_vector(m));
   size_t         n             = grouped_pairs.size();
   auto           result        = SegmentsV<K, V>(n);
   auto           perm          = create_permutation(grouped_pairs.size());
   auto           f             = [&perm](size_t i) -> size_t{return perm[i];};
   permutate(result.begin(), grouped_pairs.begin(), grouped_pairs.end(), f);
   return result;
}

std::string show_table_elem(const Segment_with_value<char32_t, uint64_t> e)
{
    std::string result;
    auto     bounds = e.bounds;
    uint64_t val    = e.value;
    result          = fmt::format(table_elem_fmt,
                                  show_char32(bounds.lower_bound),
                                  show_char32(bounds.upper_bound),
                                  val);
    return result;
}

std::string create_table_body(const std::map<char32_t, uint64_t>& m)
{
    std::string result;
    auto        t = create_classification_table(m);

    Format      f;
    f.indent                 = 4;
    f.number_of_columns      = 3;
    f.spaces_between_columns = 2;

    std::vector<std::string> elems;
    for(const auto& e : t){
        elems.push_back(show_table_elem(e));
    }

    result = string_list_to_columns(elems, f);
    return result;
}

std::string Category_table_builder::build()
{
    std::string     result;
    Categories_info categories_info;
    Set_of_char     categorized_chars;
    for(const auto& c : info_.category_name){
        /* Look through the names of all categories and write down information
         * about the categories of characters in the categories_info vector. */
        auto cat_idx         = c.first;
        auto set_for_cat_idx = info_.char_cat.get_set(cat_idx);
        auto x               = std::make_pair(c.second, set_for_cat_idx);
        categories_info.push_back(x);
        /* In addition, we collect in one set all the symbols belonging to a category. */
        categorized_chars.insert(set_for_cat_idx.begin(), set_for_cat_idx.end());
    }

    /* Now for each character from the set categorized_chars we build a set of
     * categories to which it belongs */
    std::map<char32_t, uint64_t> splitting_characters_by_category;
    for(char32_t c : categorized_chars){
        splitting_characters_by_category[c] =
            construct_categories_set_for_char(c, categories_info);
    }

    /* Then create an enumeration of the names of the character categories. */
    auto enum_body  = construct_category_enum(categories_info);
    auto table_body = create_table_body(splitting_characters_by_category);
    result          = fmt::format(category_table_fmt, enum_body, table_body);
    return result;
}