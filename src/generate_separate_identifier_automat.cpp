/*
    File:    generate_separate_identifier_automat.cpp
    Created: 30 January 2018 at 17:36 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include <string>
#include "../include/generate_separate_identifier_automat.h"
#include "../include/format.h"
// #include "../include/automata_repres.h"
#include "../include/add_category.h"
#include "../include/first_chars.h"
#include "../include/automata_repres_builder.h"

using namespace std::string_literals;

static const std::string ident_begin_cat_name_by_default = "IDENT_BEGIN"s;
static const std::string ident_aut_name                  = "A_ident"s;
static const std::string ident_aut_proc_proto            = "bool ident_proc()"s;
static const std::string ident_aut_proc_ptr_fmt          = "&{0}::ident_proc"s;
static const std::string ident_aut_final_proc_proto      = "void ident_final_proc()"s;
static const std::string ident_aut_final_proc_ptr_fmt    = "&{0}::ident_final_proc"s;

static const std::string ident_if_fmt = R"~(
    if(belongs({0}, char_categories)){{
        (loc->pcurrent_char)--;
        {1}
        automaton = A_ident;
        state     = -1;
        return t;
    }}
)~"s;

static const std::string add_ident_to_table              =
    "\n        token.code = {0};"
    "\n        token.ids = ids_trie->insert(buffer);"s;

static const std::string ident_aut_final_proc_fmt        =
    R"~(void {0}::string_final_proc(){{
    {1}token.ident_index = ids->insert(buffer);
}})~"s;

static std::string ident_automaton_impl_finals(const info_for_constructing::Info&  info)
{
    std::string result;
    auto ident_postact = info.identifier_preactions;
    result             = fmt::format(ident_aut_final_proc_fmt,
                                     info.names.name_of_scaner_class,
                                     ident_postact.empty() ? ident_postact :
                                                             (ident_postact + '\n'));
    return result;
}

void generate_separate_identifier_automat(info_for_constructing::Info&     info,
                                          const Errors_and_tries&          et,
                                          const Trie_for_set_of_char32ptr& sets_from_automata,
                                          const std::shared_ptr<Scope>&    scope)
{
    Automaton_constructing_info result;
    Str_data_for_automaton      f;
    auto begin_chars                  = first_chars(info.regexps.idents,
                                                    sets_from_automata);
    auto cat_res                      = add_category(info,
                                                     begin_chars.s,
                                                     ident_begin_cat_name_by_default);
    std::string ident_begin_cat_name  = cat_res.second;
    auto ident_if                     = fmt::format(ident_if_fmt,
                                                    ident_begin_cat_name,
                                                    info.identifier_preactions);
    info.ifs_of_start_procs.push_back(ident_if);

    f.automata_name                   = ident_aut_name;
    f.proc_name                       = "ident_proc"s;
    f.category_name_prefix            = "IDENTIFIER";
    f.diagnostic_msg                  = "At line %zu unexpectedly ended an identifier.\\n";
    f.final_states_set_name           = "final_states_for_idents";
    f.final_actions                   = info.identifier_postactions +
                                        fmt::format(add_ident_to_table, info.names.ident_name);
    result.name                       = ident_aut_name;
    result.proc_proto                 = ident_aut_proc_proto;
    result.proc_ptr                   = fmt::format(ident_aut_proc_ptr_fmt,
                                                    info.names.name_of_scaner_class);
    Automata_repres_builder repres_builder {f, sets_from_automata, et, scope};
    result.proc_impl                  = repres_builder.build_repres(info,
                                                                    info.regexps.idents);
    result.final_proc_proto = ident_aut_final_proc_proto;
    result.final_proc_ptr   = fmt::format(ident_aut_final_proc_ptr_fmt,
                                          info.names.name_of_scaner_class);
    result.final_proc_impl  = ident_automaton_impl_finals(info);
    info.automata_info.push_back(result);
}