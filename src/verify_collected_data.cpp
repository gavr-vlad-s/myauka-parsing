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

bool verify(Collected_data&                  data,
            Errors_and_tries&                et,
            const Trie_for_set_of_char32ptr& t)
{
    bool t = (et.ec -> get_number_of_errors()) == 0;
    if(!t){
        return t;
    }

    if(!data.aut_data_.set_of_used_automata){
        printf("None of the specified lexeme codes is used for any lexeme.\n");
        et.ec -> increment_number_of_errors();
        return false;
    }

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

    if(belongs(String_aut, data.aut_data_.set_of_used_automata)){
//         begin_chars[String_beg_char] = first_chars(strings_regexp, t);
    }

    return true;
}