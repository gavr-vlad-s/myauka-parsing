/*
    File:    implement_delimiter_automaton.cpp
    Created: 12 January 2018 at 14:57 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include <string>
#include "../include/implement_delimiter_automaton.h"
#include "../include/format.h"
#include "../include/used_automaton.h"
#include "../include/belongs.h"
#include "../include/attributed_char_trie.h"
#include "../include/add_category.h"
#include "../include/jump_table_string_repres.h"

using namespace std::string_literals;

static const std::string delimiter_aut_name                = "A_delimiter"s;
static const std::string delimiter_aut_proc_proto          = "bool delimiter_proc()"s;
static const std::string delimiter_aut_proc_ptr_fmt        = "&{0}::delimiter_proc"s;
static const std::string delimiter_aut_final_proc_proto    = "void delimiter_final_proc()"s;
static const std::string delimiter_aut_final_proc_ptr_fmt  = "&{0}::delimiter_final_proc"s;
static const std::string delimiter_aut_proc_impl_fmt       =
    R"~(bool {0}::delimiter_proc(){{
    bool t = false;
    if(-1 == state){{
        state = get_init_state(ch, init_table_for_delimiters,
                               sizeof(init_table_for_delimiters)/sizeof(State_for_char));
        token.code = delim_jump_table[state].code;
        t = true;
        return t;
    }}
    Elem elem  = delim_jump_table[state];
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

static const std::string del_begin_cat_name_by_default     = "DELIMITER_BEGIN"s;
static const std::string delim_if_fmt = R"~(
    if(belongs({0}, char_categories)){{
        (loc->pcurrent_char)--;
        automaton = A_delimiter;
        state = -1;
        return t;
    }}
)~"s;

static const std::string del_jump_table_name           = "delim_jump_table"s;
static const std::string del_init_table_name           = "init_table_for_delimiters"s;

static const std::string delim_aut_final_proc_fmt      =
    R"~(void {0}::delimiter_final_proc(){{
    {1}token.code = delim_jump_table[state].code;
}})~"s;

static std::vector<std::u32string>
    delims_str_repres(const info_for_constructing::Info&  info,
                      const Errors_and_tries&             et)
{
    std::vector<std::u32string> result;
    for(size_t del_idx : info.del_repres){
        auto delimiter = et.strs_trie->get_string(del_idx);
        result.push_back(delimiter);
    }
    return result;
}

template<class InputIterator>
std::set<char32_t> first_elems(InputIterator first, InputIterator last)
{
    std::set<char32_t> result;
    for(auto it = first; it != last; ++it){
        char32_t elem = (*it)[0];
        result.insert(elem);
    }
    return result;
}

static std::string delimiter_automaton_impl(info_for_constructing::Info&  info,
                                            const Errors_and_tries&       et,
                                            const std::shared_ptr<Scope>& scope)
{
    std::string result;
    std::set<char32_t>          first_chars_for_delims; /* This set consists of
                                                           characters from which
                                                           the delimiters can begin. */
    Attributed_char_trie        atrie;
    auto                        delimiter_strings = delims_str_repres(info, et);
    first_chars_for_delims = first_elems(delimiter_strings.begin(),
                                         delimiter_strings.end());

    size_t counter = 0;
    for(size_t del_idx : info.del_repres){
        Attributed_cstring atrib_cstr;
        atrib_cstr.str       = const_cast<char32_t*>(delimiter_strings[counter].c_str());
        auto& scope_         = scope;
        atrib_cstr.attribute = (scope_->strsc[del_idx]).code;
        atrie.insert(attributed_cstring2string(atrib_cstr, 1));
        counter++;
    }


    Jumps_and_inits jmps = atrie.jumps(); /* We built a sketch for
                                             the transition table. */
    /* Now we need to add the desired text to the implementation of the start automaton
     * and generate a function that handles the delimiters. */
    auto cat_res                         = add_category(info,
                                                        first_chars_for_delims,
                                                        del_begin_cat_name_by_default);
    std::string delimiter_begin_cat_name = cat_res.second;
    auto delim_if                        = fmt::format(delim_if_fmt,
                                                       delimiter_begin_cat_name);
    info.ifs_of_start_procs.push_back(delim_if);
    auto del_postact                     = info.delimiters_postaction;
    std::string postaction_code_in_proc  = del_postact.empty() ?
                                           std::string() :
                                           fmt::format(postaction_fmt, del_postact);

    auto jump_table_repres = jump_table_string_repres(info,
                                                      jmps,
                                                      del_jump_table_name,
                                                      del_init_table_name);
    result = jump_table_repres +
             fmt::format(delimiter_aut_proc_impl_fmt,
                         info.names.name_of_scaner_class,
                         postaction_code_in_proc);

    return result;
}

std::string delimiter_automaton_impl_finals(const info_for_constructing::Info&  info)
{
    std::string result;
    auto del_postact = info.delimiters_postaction;
    result           = fmt::format(delim_aut_final_proc_fmt,
                                   info.names.name_of_scaner_class,
                                   del_postact.empty() ? del_postact :
                                                         (del_postact + '\n'));
    return result;
}

Automaton_constructing_info
    implement_delimiter_automaton(info_for_constructing::Info&  info,
                                  const Errors_and_tries&       et,
                                  const std::shared_ptr<Scope>& scope)
{
    Automaton_constructing_info result;
    result.name             = delimiter_aut_name;
    result.proc_proto       = delimiter_aut_proc_proto;
    result.proc_ptr         = fmt::format(delimiter_aut_proc_ptr_fmt,
                                          info.names.name_of_scaner_class);
    result.proc_impl        = delimiter_automaton_impl(info, et, scope);
    result.final_proc_proto = delimiter_aut_final_proc_proto;
    result.final_proc_ptr   = fmt::format(delimiter_aut_final_proc_ptr_fmt,
                                          info.names.name_of_scaner_class);
    result.final_proc_impl  = delimiter_automaton_impl_finals(info);
    return result;
}