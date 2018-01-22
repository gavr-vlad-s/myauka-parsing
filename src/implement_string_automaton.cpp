/*
    File:    implement_string_automaton.cpp
    Created: 16 January 2018 at 16:51 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include <string>
#include "../include/implement_string_automaton.h"
#include "../include/format.h"
#include "../include/automata_repres.h"

using namespace std::string_literals;

static const std::string string_aut_name                = "A_string"s;
static const std::string string_aut_proc_proto          = "bool string_proc()"s;
static const std::string string_aut_proc_ptr_fmt        = "&{0}::string_proc"s;
static const std::string string_aut_final_proc_proto    = "void string_final_proc()"s;
static const std::string string_aut_final_proc_ptr_fmt  = "&{0}::string_final_proc"s;

// struct Str_data_for_automaton {
//     std::string automata_name;
//     std::string proc_name;
//     std::string category_name_prefix;
//     std::string diagnostic_msg;
//     std::string final_actions;
//     std::string final_states_set_name;
// };
//

// static const std::string delimiter_aut_proc_impl_fmt       =
//     R"~(bool {0}::delimiter_proc(){{
//     bool t = false;
//     if(-1 == state){{
//         state = get_init_state(ch, init_table_for_delimiters,
//                                sizeof(init_table_for_delimiters)/sizeof(State_for_char));
//         token.code = delim_jump_table[state].code;
//         t = true;
//         return t;
//     }}
//     Elem elem  = delim_jump_table[state];
//     token.code = elem.code;
//     int y = search_char(ch, elem.symbols);
//     if(y != THERE_IS_NO_CHAR){{
//         state = elem.first_state + y; t = true;
//     }}
//     {1}return t;
// )~"s;
//
// static const std::string postaction_fmt                    =
//     R"~(    if(!t){{
//         {0};
//     }})~"s;
//
// static const std::string del_begin_cat_name_by_default     = "DELIMITER_BEGIN"s;
// static const std::string delim_if_fmt = R"~(
//     if(belongs({0}, char_categories)){{
//         (loc->pcurrent_char)--;
//         automaton = A_delimiter;
//         state = -1;
//         return t;
//     }}
// )~"s;
//
// static const std::string del_jump_table_name           = "delim_jump_table"s;
// static const std::string del_init_table_name           = "init_table_for_delimiters"s;
//
// static const std::string delim_aut_final_proc_fmt      =
//     R"~(void {0}::delimiter_final_proc(){{
//     {1}token.code = delim_jump_table[state].code;
// }})~"s;

// std::string automata_repres(info_for_constructing::Info&     info,
//                             const Str_data_for_automaton&    f,
//                             const Trie_for_set_of_char32ptr& sets,
//                             const Errors_and_tries&          et,
//                             const std::shared_ptr<Scope>&    scope,
//                             const Regexp_kind                kind);


Automaton_constructing_info
    implement_string_automaton(info_for_constructing::Info&     info,
                               const Errors_and_tries&          et,
                               const Trie_for_set_of_char32ptr& sets_from_automata,
                               const std::shared_ptr<Scope>&    scope)
{
    Automaton_constructing_info result;
    Str_data_for_automaton      f;
    f.automata_name         = "A_string"s;
    f.proc_name             = "string_proc";
    f.category_name_prefix  = "STRING";
    f.diagnostic_msg        = "At line %zu unexpectedly ended a string literal.\n";
    f.final_states_set_name = "final_states_for_strings";
    result.name             = string_aut_name;
    result.proc_proto       = string_aut_proc_proto;
    result.proc_ptr         = fmt::format(string_aut_proc_ptr_fmt,
                                          info.names.name_of_scaner_class);
    result.proc_impl        = automata_repres(info,
                                              f,
                                              sets_from_automata,
                                              et,
                                              scope,
                                              Regexp_kind::String);
//     result.proc_impl        = delimiter_automaton_impl(info, et, scope);
    result.final_proc_proto = string_aut_final_proc_proto;
    result.final_proc_ptr   = fmt::format(string_aut_final_proc_ptr_fmt,
                                          info.names.name_of_scaner_class);
//     result.final_proc_impl  = delimiter_automaton_impl_finals(info);
    return result;
}
