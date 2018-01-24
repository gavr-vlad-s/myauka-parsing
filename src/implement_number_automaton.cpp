/*
    File:    implement_number_automaton.cpp
    Created: 24 January 2018 at 09:17 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include <string>
#include "../include/implement_number_automaton.h"
#include "../include/format.h"
#include "../include/automata_repres.h"
#include "../include/add_category.h"
#include "../include/first_chars.h"

using namespace std::string_literals;

static const std::string number_aut_name                  = "A_number"s;
static const std::string number_aut_proc_proto            = "bool number_proc()"s;
static const std::string number_aut_proc_ptr_fmt          = "&{0}::number_proc"s;
static const std::string number_aut_final_proc_proto      = "void number_final_proc()"s;
static const std::string number_aut_final_proc_ptr_fmt    = "&{0}::number_final_proc"s;

static const std::string number_begin_cat_name_by_default = "NUMBER_BEGIN"s;

// static const std::string add_string_to_table              =
//     "\n        token.string_index = strs->insert(buffer);"s;

static const std::string number_if_fmt = R"~(
    if(belongs({0}, char_categories)){{
        (loc->pcurrent_char)--;
        {1}
        automaton = A_string;
        state     = -1;
        return t;
    }}
)~"s;

static const std::string number_aut_final_proc_fmt      =
    R"~(void {0}::string_final_proc(){{
    {1}
}})~"s;

// struct Str_data_for_automaton {
//     std::string automata_name;
//     std::string proc_name;
//     std::string category_name_prefix;
//     std::string diagnostic_msg;
//     std::string final_actions;
//     std::string final_states_set_name;
// };
//


// std::string automata_repres(info_for_constructing::Info&     info,
//                             const Str_data_for_automaton&    f,
//                             const Trie_for_set_of_char32ptr& sets,
//                             const Errors_and_tries&          et,
//                             const std::shared_ptr<Scope>&    scope,
//                             const Regexp_kind                kind);


static std::string number_automaton_impl_finals(const info_for_constructing::Info&  info)
{
    std::string result;
    auto num_postact = info.number_postactions;
    result           = fmt::format(number_aut_final_proc_fmt,
                                   info.names.name_of_scaner_class,
                                   num_postact.empty() ? num_postact :
                                                         (num_postact + '\n'));
    return result;
}

Automaton_constructing_info
    implement_number_automaton(info_for_constructing::Info&     info,
                               const Errors_and_tries&          et,
                               const Trie_for_set_of_char32ptr& sets_from_automata,
                               const std::shared_ptr<Scope>&    scope)
{
    Automaton_constructing_info result;
    Str_data_for_automaton      f;
    auto begin_chars                  = first_chars(info.regexps.numbers,
                                                    sets_from_automata);
    auto cat_res                      = add_category(info,
                                                     begin_chars.s,
                                                     number_begin_cat_name_by_default);
    std::string number_begin_cat_name = cat_res.second;
    auto string_if                    = fmt::format(number_if_fmt,
                                                    number_begin_cat_name,
                                                    info.number_preactions);
    info.ifs_of_start_procs.push_back(string_if);

    f.automata_name                   = "A_number"s;
    f.proc_name                       = "number_proc";
    f.category_name_prefix            = "NUMBER";
    f.diagnostic_msg                  = "At line %zu unexpectedly ended a number literal.\n";
    f.final_states_set_name           = "final_states_for_numbers";
    f.final_actions                   = info.number_postactions;
    result.name                       = number_aut_name;
    result.proc_proto                 = number_aut_proc_proto;
    result.proc_ptr                   = fmt::format(number_aut_proc_ptr_fmt,
                                                    info.names.name_of_scaner_class);
    result.proc_impl                  = automata_repres(info,
                                                        f,
                                                        sets_from_automata,
                                                        et,
                                                        scope,
                                                        Regexp_kind::Number);
    result.final_proc_proto = number_aut_final_proc_proto;
    result.final_proc_ptr   = fmt::format(number_aut_final_proc_ptr_fmt,
                                          info.names.name_of_scaner_class);
    result.final_proc_impl  = number_automaton_impl_finals(info);
    return result;
}