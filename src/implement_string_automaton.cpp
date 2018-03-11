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
// #include "../include/automata_repres.h"
#include "../include/add_category.h"
#include "../include/first_chars.h"
#include "../include/automata_repres_builder.h"

using namespace std::string_literals;

static const std::string string_aut_name                  = "A_string"s;
static const std::string string_aut_proc_proto            = "bool string_proc()"s;
static const std::string string_aut_proc_ptr_fmt          = "&{0}::string_proc"s;
static const std::string string_aut_final_proc_proto      = "void string_final_proc()"s;
static const std::string string_aut_final_proc_ptr_fmt    = "&{0}::string_final_proc"s;
static const std::string string_diagnostic_msg            =
    "At line %zu unexpectedly ended a string literal.\\n"s;

static const std::string string_begin_cat_name_by_default = "STRING_BEGIN"s;

static const std::string add_string_to_table              =
    "\n        token.string_index = strs->insert(buffer);"s;

static const std::string string_if_fmt = R"~(
    if(belongs({0}, char_categories)){{
        (loc->pcurrent_char)--;
        {1}
        automaton = A_string;
        state     = -1;
        return t;
    }}
)~"s;

static const std::string string_aut_final_proc_fmt      =
    R"~(void {0}::string_final_proc(){{
    {1}token.string_index = strs->insert(buffer);
}})~"s;

static std::string string_automaton_impl_finals(const info_for_constructing::Info&  info)
{
    std::string result;
    auto str_postact = info.string_postactions;
    result           = fmt::format(string_aut_final_proc_fmt,
                                   info.names.name_of_scaner_class,
                                   str_postact.empty() ? str_postact :
                                                         (str_postact + "\n    "));
    return result;
}

Automaton_constructing_info
    implement_string_automaton(info_for_constructing::Info&     info,
                               const Errors_and_tries&          et,
                               const Trie_for_set_of_char32ptr& sets_from_automata,
                               const std::shared_ptr<Scope>&    scope)
{
    Automaton_constructing_info result;
    Str_data_for_automaton      f;
    auto begin_chars                  = first_chars(info.regexps.strings,
                                                    sets_from_automata);
    auto cat_res                      = add_category(info,
                                                     begin_chars.s,
                                                     string_begin_cat_name_by_default);
    std::string string_begin_cat_name = cat_res.second;
    auto string_if                    = fmt::format(string_if_fmt,
                                                    string_begin_cat_name,
                                                    info.string_preactions);
    info.ifs_of_start_procs.push_back(string_if);

    f.automata_name                   = string_aut_name;
    f.proc_name                       = "string_proc";
    f.category_name_prefix            = "STRING";
    f.diagnostic_msg                  = string_diagnostic_msg;
    f.final_states_set_name           = "final_states_for_strings";
    f.final_actions                   = info.string_postactions + add_string_to_table;
    result.name                       = string_aut_name;
    result.proc_proto                 = string_aut_proc_proto;
    result.proc_ptr                   = fmt::format(string_aut_proc_ptr_fmt,
                                                    info.names.name_of_scaner_class);
    Automata_repres_builder repres_builder {f, sets_from_automata, et, scope};
    result.proc_impl                  = repres_builder.build_repres(info,
                                                                    info.regexps.strings);
    result.final_proc_proto = string_aut_final_proc_proto;
    result.final_proc_ptr   = fmt::format(string_aut_final_proc_ptr_fmt,
                                          info.names.name_of_scaner_class);
    result.final_proc_impl  = string_automaton_impl_finals(info);
    return result;
}