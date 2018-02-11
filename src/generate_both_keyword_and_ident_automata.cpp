/*
    File:    generate_both_keyword_and_ident_automata.cpp
    Created: 30 January 2018 at 20:18 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include <string>
#include "../include/generate_both_keyword_and_ident_automata.h"
#include "../include/regexp_match.h"
#include "../include/regexp1_or_regexp2.h"
#include "../include/u32strings_to_commands.h"
#include "../include/automata_repres.h"
#include "../include/first_chars.h"
#include "../include/format.h"
#include "../include/add_category.h"

/*
 * If any keyword is an identifier, then
 *      1) we create a keywords automaton;
 *      2) we create an identifiers automaton;
 *      3) if the keywords automaton finds the character
 *         for which there are no transitions, then
 *          a) if the current character is admissible for an identifier automaton,
 *             then we go to the identifier automaton;
 *          b) otherwise, we returns a lexeme code from a transition table of
 *             the keywords automaton.
 *
 * If there are keywords that are not identifiers, then we glue the identifier
 * regexp and keywords in one regexp. Next, we create a minimal DFA by this glued
 * regexp, and generate an implementation of the automaton. If the automaton finished,
 * then we search the contents of the member buffer into a table of keywords, using
 * a binary search algorithm.
 */


using namespace std::string_literals;

static const std::string ident_aut_proc_ptr_fmt          = "&{0}::ident_proc"s;
static const std::string ident_aut_final_proc_proto      = "void ident_final_proc()"s;
static const std::string ident_aut_final_proc_ptr_fmt    = "&{0}::ident_final_proc"s;
static const std::string ident_aut_proc_proto             = "bool ident_proc()"s;
static const std::string idkeyw_begin_cat_name_by_default = "KEYWIDENT_BEGIN"s;
static const std::string ident_aut_name                   = "A_ident"s;
static const std::string unexpected_end                   =
    "At line %zu unexpectedly ended an identifier or a keyword.\n"s;
static const std::string ident_if_fmt = R"~(
    if(belongs({0}, char_categories)){{
        (loc->pcurrent_char)--;
        {1}
        automaton = A_ident;
        state     = -1;
        return t;
    }}
)~"s;

static std::string ident_automaton_impl_finals(const info_for_constructing::Info&  info)
{
    std::string result;
//     auto ident_postact = info.identifier_preactions;
//     result             = fmt::format(ident_aut_final_proc_fmt,
//                                      info.names.name_of_scaner_class,
//                                      ident_postact.empty() ? ident_postact :
//                                                              (ident_postact + '\n'));
    return result;
}

void generate_both_keyword_and_ident_automata(info_for_constructing::Info&     info,
                                              const Errors_and_tries&          et,
                                              const Trie_for_set_of_char32ptr& sets_from_automata,
                                              const std::shared_ptr<Scope>&    scope)
{
    auto& id_regexp                = info.regexps.idents;
    std::vector<std::u32string> keyword_strings;
    for(size_t kw_idx : info.kw_repres){
        auto keyword = et.strs_trie->get_string(kw_idx);
        keyword_strings.push_back(keyword);
    }

    bool  keywords_are_identifiers = regexp_match(id_regexp,
                                                  sets_from_automata,
                                                  keyword_strings.begin(),
                                                  keyword_strings.end());
    if(keywords_are_identifiers){
    }else{
        auto keywords_regexp = u32strings_to_commands(keyword_strings.begin(),
                                                      keyword_strings.end());
        auto summary         = regexp1_or_regexp2(id_regexp, keywords_regexp);
        info.regexps.idents  = summary;
        Str_data_for_automaton      f;
        auto begin_chars                  = first_chars(info.regexps.idents,
                                                        sets_from_automata);
        auto cat_res                      = add_category(info,
                                                        begin_chars.s,
                                                        idkeyw_begin_cat_name_by_default);
        std::string string_begin_cat_name = cat_res.second;
        auto ident_if                     = fmt::format(ident_if_fmt,
                                                        string_begin_cat_name,
                                                        info.identifier_preactions);

        Automaton_constructing_info result;
        info.ifs_of_start_procs.push_back(ident_if);

        f.automata_name                   = ident_aut_name;
        f.proc_name                       = "ident_proc"s;
        f.category_name_prefix            = "IDENTIFIER";
        f.diagnostic_msg                  = unexpected_end;
        f.final_states_set_name           = "final_states_for_idents";
//         f.final_actions                   = info.identifier_postactions + add_ident_to_table;
        result.name                       = ident_aut_name;
        result.proc_proto                 = ident_aut_proc_proto;
        result.proc_ptr                   = fmt::format(ident_aut_proc_ptr_fmt,
                                                        info.names.name_of_scaner_class);
        result.proc_impl                  = automata_repres(info,
                                                            f,
                                                            sets_from_automata,
                                                            et,
                                                            scope,
                                                            Regexp_kind::Ident);
        result.final_proc_proto = ident_aut_final_proc_proto;
        result.final_proc_ptr   = fmt::format(ident_aut_final_proc_ptr_fmt,
                                            info.names.name_of_scaner_class);
        result.final_proc_impl  = ident_automaton_impl_finals(info);
        info.automata_info.push_back(result);
    }
}

// #include "../include/generate_separate_identifier_automat.h"
//
// static const std::string add_ident_to_table              =
//     "\n        token.ids = ids_trie->insert(buffer);"s;
//
// static const std::string ident_aut_final_proc_fmt        =
//     R"~(void {0}::string_final_proc(){{
//     {1}token.ident_index = ids->insert(buffer);
// }})~"s;
//
// void generate_separate_identifier_automat(info_for_constructing::Info&     info,
//                                           const Errors_and_tries&          et,
//                                           const Trie_for_set_of_char32ptr& sets_from_automata,
//                                           const std::shared_ptr<Scope>&    scope)
// {
//
// }