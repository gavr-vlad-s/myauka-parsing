/*
    File:    verify_collected_data.cpp
    Created: 10 December 2017 at 10:29 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include <map>
#include <string>
#include "../include/verify_collected_data.h"
#include "../include/begin_chars_for_aut.h"
#include "../include/belongs.h"
#include "../include/used_automaton.h"
#include "../include/first_chars.h"

using Intersection_msg_index = std::pair<Begin_chars_for_aut, Begin_chars_for_aut>;

static const std::map<Intersection_msg_index, std::string> intersection_msgs = {
    {{Keyword_beg_char, Delimiter_beg_char},
     "Error: the set of characters from which the keywords begin, intersects with "
     "the set of characters from which the delimiters begin."},
    {{Keyword_beg_char, String_beg_char},
     "Error: the set of characters from which the keywords begin, intersects with "
     "the set of characters from which string literals begin."},
    {{Keyword_beg_char, Number_beg_char},
     "Error: the set of characters from which the keywords begin, intersects with "
     "the set of characters from which the numeric literals begin."},
    {{Keyword_beg_char, Comment_beg_char},
     "Error: the set of characters from which the keywords begin, intersects with "
     "the set of characters from which comments begin."},

    {{Id_beg_char, Delimiter_beg_char},
     "Error: the set of characters from which the identifiers begin, intersects with "
     "the set of characters from which the delimiters begin."},
    {{Id_beg_char, String_beg_char},
     "Error: the set of characters from which identifiers begin, intersects with the "
     "set of characters from which string literals begin."},
    {{Id_beg_char, Number_beg_char},
     "Error: the set of characters from which identifiers begin, intersects with the "
     "set of characters from which numeric literals begin."},
    {{Id_beg_char, Comment_beg_char},
     "Error: the set of characters from which the identifiers begin, intersects with "
     "the set of characters from which comments begin."},

    {{Delimiter_beg_char, String_beg_char},
     "Error: the set of characters from which delimiters begin, intersects with the "
     "set of characters from which string literals begin."},
    {{Delimiter_beg_char, Number_beg_char},
     "Error: the set of characters from which delimiters begin, intersects with the "
     "set of characters from which numeric literals begin."},

    {{String_beg_char, Number_beg_char},
     "Error: the set of characters from which string literals begin, intersects with "
     "the set of characters from which numeric literals begin."},
    {{String_beg_char, Comment_beg_char},
     "Error: the set of characters from which string literals begin, intersects with "
     "the set of characters from which comments begin."},

    {{Number_beg_char, Comment_beg_char},
     "Error: the set of characters from which numeric literals begin, intersects with "
     "the set of characters from which comments begin."}
};

void collect_keywords_first_chars(Collected_data& data, const Errors_and_tries& et)
{
    if(belongs(Keyword_aut, data.aut_data_.set_of_used_automata)){
        Category temp_cat;
        temp_cat.kind = Category_kind::Set_of_cs;
        Set_of_char s;
        for(size_t kw_idx : data.kw_repres_){
            auto kw_str = et.strs_trie->get_string(kw_idx);
            s.insert(kw_str[0]);
        }
        temp_cat.s = s;
        data.begin_chars[Keyword_beg_char] = temp_cat;
    }
}

void collect_delimiters_first_chars(Collected_data& data, const Errors_and_tries& et)
{
    if(belongs(Delimiter_aut, data.aut_data_.set_of_used_automata)){
        Category temp_cat;
        temp_cat.kind = Category_kind::Set_of_cs;
        Set_of_char s;
        for(size_t del_idx : data.del_repres_){
            auto del_str = et.strs_trie->get_string(del_idx);
            s.insert(del_str[0]);
        }
        temp_cat.s = s;
        data.begin_chars[Delimiter_beg_char] = temp_cat;
    }
}

void collect_strings_first_chars(Collected_data&                  data,
                                 const Errors_and_tries&          et,
                                 const Trie_for_set_of_char32ptr& t)
{
    if(belongs(String_aut, data.aut_data_.set_of_used_automata)){
        data.begin_chars[String_beg_char] = first_chars(data.strings_regexp_, t);
    }
}

void collect_numbers_first_chars(Collected_data&                  data,
                                 const Errors_and_tries&          et,
                                 const Trie_for_set_of_char32ptr& t)
{
    if(belongs(Number_aut, data.aut_data_.set_of_used_automata)){
        data.begin_chars[Number_beg_char] = first_chars(data.numbers_regexp_, t);
    }
}

bool collect_identifiers_first_chars(Collected_data&                  data,
                                     Errors_and_tries&                et,
                                     const Trie_for_set_of_char32ptr& t)
{
    if(belongs(Id_aut, data.aut_data_.set_of_used_automata)){
        if(data.idents_regexp_.empty()){
            puts("Error: the name of the lexeme 'identifier' is defined, but the "
                 "structure of the identifiers is not described.");
            et.ec -> increment_number_of_errors();
            return false;
        }
        data.begin_chars[Id_beg_char] = first_chars(data.idents_regexp_, t);
    }
    return true;
}

void collect_comments_first_chars(Collected_data&                  data,
                                 const Errors_and_tries&          et,
                                 const Trie_for_set_of_char32ptr& t)
{
    if(belongs(Comment_aut, data.aut_data_.set_of_used_automata)){
        Category c1, c2;
        if(data.indeces_.mark_of_single_lined){
            auto s = et.strs_trie->get_string(data.indeces_.mark_of_single_lined);
            c1.kind = Category_kind::Set_of_cs;
            c1.s    = Set_of_char({s[0]});
        }
        if(data.indeces_.mark_of_multilined_begin){
            auto s = et.strs_trie->get_string(data.indeces_.mark_of_multilined_begin);
            c2.kind = Category_kind::Set_of_cs;
            c2.s    = Set_of_char({s[0]});
        }
        data.begin_chars[Comment_beg_char] = c1 + c2;
    }
}

bool verify(Collected_data&                  data,
            Errors_and_tries&                et,
            const Trie_for_set_of_char32ptr& t)
{
    bool result = (et.ec -> get_number_of_errors()) == 0;
    if(!result){
        return result;
    }

    if(!data.aut_data_.set_of_used_automata){
        printf("None of the specified lexeme codes is used for any lexeme.\n");
        et.ec -> increment_number_of_errors();
        return false;
    }

    collect_keywords_first_chars(data, et);
    collect_delimiters_first_chars(data, et);
    collect_strings_first_chars(data, et, t);
    collect_numbers_first_chars(data, et, t);
    result = collect_identifiers_first_chars(data, et, t);
    if(!result){
        return result;
    }
    collect_comments_first_chars(data, et, t);

    for(const auto& z : intersection_msgs){
        auto  msg_index = z.first;
        auto& msg       = z.second;
        if(data.begin_chars[msg_index.first] * data.begin_chars[msg_index.second]){
            puts(msg.c_str());
            et.ec -> increment_number_of_errors();
            result = false;
        }
    }

    return result;
}