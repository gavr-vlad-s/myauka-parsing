/*
    File:    generate_separate_keywords_automaton.cpp
    Created: 26 January 2018 at 10:18 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include <string>
#include "../include/generate_separate_keywords_automaton.h"
#include "../include/attributed_char_trie.h"
#include "../include/add_category.h"
#include "../include/format.h"
#include "../include/jump_table_string_repres.h"

using namespace std::string_literals;

static const std::string keyword_aut_name                  = "A_keyword"s;
static const std::string keyword_aut_proc_proto            = "bool keyword_proc()"s;
static const std::string keyword_aut_proc_ptr_fmt          = "&{0}::keyword_proc"s;
static const std::string keyword_aut_final_proc_proto      = "void keyword_final_proc()"s;
static const std::string keyword_aut_final_proc_ptr_fmt    = "&{0}::keyword_final_proc"s;

static const std::string keyword_begin_cat_name_by_default = "KEYWORD_BEGIN"s;
static const std::string keyword_if_fmt = R"~(
    if(belongs({0}, char_categories)){{
        (loc->pcurrent_char)--;
        automaton = A_keyword;
        state     = -1;
        return t;
    }}
)~"s;

static const std::string keyw_jump_table_name              = "keyword_jump_table"s;
static const std::string keyword_init_table_name           = "init_table_for_keywords"s;

static const std::string keyword_aut_proc_impl_fmt         =
    R"~(bool {0}::keyword_proc(){{
    bool t = false;
    if(-1 == state){{
        state = get_init_state(ch, init_table_for_delimiters,
                               sizeof(init_table_for_keywords)/sizeof(State_for_char));
        token.code = keyword_jump_table[state].code;
        t = true;
        return t;
    }}
    Elem elem  = keyword_jump_table[state];
    token.code = elem.code;
    int y = search_char(ch, elem.symbols);
    if(y != THERE_IS_NO_CHAR){{
        state = elem.first_state + y; t = true;
    }}
    {1}return t;
)~"s;

static const std::string postaction_fmt                    =
    R"~(    if(!t){{
        {0};
    }})~"s;

static const std::string keyword_aut_final_proc_fmt      =
    R"~(void {0}::keyword_final_proc(){{
    {1}token.code = keyword_jump_table[state].code;
}})~"s;

static std::string keyword_automaton_impl(info_for_constructing::Info&  info,
                                          const Errors_and_tries&       et,
                                          const std::shared_ptr<Scope>& scope)
{
    std::string result;
    std::set<char32_t>          first_char_for_keywords; /* This set consists of
                                                            characters from which
                                                            the keywords can begin. */
    Attributed_char_trie        atrie;
    std::vector<std::u32string> keyword_strings;

    for(size_t kw_idx : info.kw_repres){
        auto keyword = et.strs_trie->get_string(kw_idx);
        keyword_strings.push_back(keyword);
        first_char_for_keywords.insert(keyword[0]);
    }
    size_t counter = 0;
    for(size_t kw_idx : info.kw_repres){
        Attributed_cstring atrib_cstr;
        atrib_cstr.str       = const_cast<char32_t*>(keyword_strings[counter].c_str());
        auto& scope_         = scope;
        atrib_cstr.attribute = (scope_->strsc[kw_idx]).code;
        atrie.insert(attributed_cstring2string(atrib_cstr, 1));
        counter++;
    }

    Jumps_and_inits jmps = atrie.jumps(); /* We built a sketch for
                                             the transition table. */
    /* Now we need to add the desired text to the implementation of the start automaton
     * and generate a function that handles the delimiters. */
    auto cat_res                         = add_category(info,
                                                        first_char_for_keywords,
                                                        keyword_begin_cat_name_by_default);
    std::string keyword_begin_cat_name = cat_res.second;
    auto keyword_if                      = fmt::format(keyword_if_fmt,
                                                       keyword_begin_cat_name);
    info.ifs_of_start_procs.push_back(keyword_if);
    auto keyword_postact                     = info.keywords_postaction;
    std::string postaction_code_in_proc  = keyword_postact.empty() ?
                                           std::string() :
                                           fmt::format(postaction_fmt, keyword_postact);

    auto jump_table_repres = jump_table_string_repres(info,
                                                      jmps,
                                                      keyw_jump_table_name,
                                                      keyword_init_table_name);
    result = jump_table_repres +
             fmt::format(keyword_aut_proc_impl_fmt,
                         info.names.name_of_scaner_class,
                         postaction_code_in_proc);

    return result;
}

std::string keyword_automaton_impl_finals(const info_for_constructing::Info&  info)
{
    std::string result;
    auto keyword_postact = info.keywords_postaction;
    result           = fmt::format(keyword_aut_final_proc_fmt,
                                   info.names.name_of_scaner_class,
                                   keyword_postact.empty() ? keyword_postact :
                                                         (keyword_postact + '\n'));
    return result;
}

void generate_separate_keywords_automaton(info_for_constructing::Info&  info,
                                          const Errors_and_tries&       et,
                                          const std::shared_ptr<Scope>& scope)
{
    Automaton_constructing_info result;
    result.name             = keyword_aut_name;
    result.proc_proto       = keyword_aut_proc_proto;
    result.proc_ptr         = fmt::format(keyword_aut_proc_ptr_fmt,
                                          info.names.name_of_scaner_class);
    result.proc_impl        = keyword_automaton_impl(info, et, scope);
    result.final_proc_proto = keyword_aut_final_proc_proto;
    result.final_proc_ptr   = fmt::format(keyword_aut_final_proc_ptr_fmt,
                                          info.names.name_of_scaner_class);
    result.final_proc_impl  = keyword_automaton_impl_finals(info);
    info.automata_info.push_back(result);
}