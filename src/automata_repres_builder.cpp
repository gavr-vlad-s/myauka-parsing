/*
    File:    automata_repres_builder.cpp
    Created: 08 February 2017 at 14:35 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include <string>
#include <list>
#include "../include/automata_repres_builder.h"
#include "../include/groupped_dfa_by_regexp.h"
#include "../include/format.h"
#include "../include/join.h"
#include "../include/get_act_repres.h"
#include "../include/add_category.h"

// #define DEBUG_MODE_ON
#ifdef DEBUG_MODE_ON
#include <cstdio>
#endif

using namespace std::string_literals;

static constexpr size_t indent_width           = 4;

static const std::string newline_str           = "\n"s;
static const std::string quadruple_indent      = std::string(indent_width * 4, ' ');
static const std::string nq                    = ";\n" + quadruple_indent;
static const std::string else_str              = " else "s;

static const std::string all_chars_fmt =
R"~({0}state = {1};
                there_is_jump = true;)~"s;

static const std::string set_of_cs_fmt =
R"~(if(belongs({0}, char_categories)){{
                    {1}state      = {2};
                    there_is_jump = true;
                }})~"s;

static const std::string set_of_cs_complement_fmt =
R"~(if(!belongs({0}, char_categories)){{
                    {1}state      = {2};
                    there_is_jump = true;
                }})~"s;

static const std::string aut_proc_template_fmt =
    R"~(bool {0}::{1}(){{{{
    bool t             = true;
    bool there_is_jump = false;
    switch(state){{{{
        {{0}}
        default:
            ;
    }}}}
    if(!there_is_jump){{{{
        t = false;
        if(!is_elem(state, {2})){{{{
            printf("{3}", loc->current_line);
            en->increment_number_of_errors();
        }}}}
        {4}
    }}}}
    return t;
}}}})~"s;

static const std::string case_fmt =
R"~(        case {0}:
{1}
                break;)~"s;

static G_DFA info_to_automaton(const Command_buffer&            regexp,
                               const Trie_for_set_of_char32ptr& sets)
{
    G_DFA gdfa;
    grouped_DFA_by_regexp(gdfa, regexp, sets);
    return gdfa;
}

static std::string qstring(const std::string& s)
{
    return s.empty() ? s : (s + nq);
}

std::string
    Automata_repres_builder::automata_repres_case_j(info_for_constructing::Info&  info,
                                                    const Category&               cat,
                                                    const DFA_state_with_action&  swa)
{
    std::string result;
    auto        temp   = get_act_repres(et_, scope_, swa.action_idx);
    switch(cat.kind){
        case Category_kind::All_chars:
            result = fmt::format(all_chars_fmt, qstring(temp), std::to_string(swa.st));
            break;
        case Category_kind::Set_of_cs:
            {
                auto default_cat_name = f_.category_name_prefix +
                                        std::to_string(last_category_suffix_);
                auto result_cat       = add_category(info, cat.s, default_cat_name);
                if(result_cat.first){
                    // if there was not the category
                    last_category_suffix_++;
                }
                result = fmt::format(set_of_cs_fmt,   result_cat.second,
                                     qstring(temp),   std::to_string(swa.st));
            }
            break;
        case Category_kind::Set_of_cs_complement:
            {
                auto default_cat_name = f_.category_name_prefix +
                                        std::to_string(last_category_suffix_);
                auto result_cat       = add_category(info, cat.s, default_cat_name);
                if(result_cat.first){
                    // if there was not the category
                    last_category_suffix_++;
                }
                result = fmt::format(set_of_cs_complement_fmt, result_cat.second,
                                     qstring(temp),            std::to_string(swa.st));
            }
            break;
    }
    return result;
}

std::string
    Automata_repres_builder::automata_repres_case(info_for_constructing::Info& info,
                                                  const   G_DFA_state_jumps&   m,
                                                  size_t                       counter)
{
    std::string result;
    if(m.empty()){
        return result;
    }

    std::list<std::string> ifs;
    for(const auto& j : m){
        auto cat  = j.first;
        auto swa  = j.second;
        auto temp = automata_repres_case_j(info, cat,   swa);
        ifs.push_back(temp);
    }
    result = fmt::format(case_fmt,
                         std::to_string(counter),
                         quadruple_indent + join(ifs.begin(), ifs.end(), else_str));
    return result;
}

std::string
    Automata_repres_builder::automata_repres_switch(info_for_constructing::Info&  info,
                                                    const G_DFA&                  aut)
{
    std::string result;
    size_t      counter   = 0;
    last_category_suffix_ = 0;
    std::list<std::string> cases;
    for(const auto& m : aut.jumps){
        auto current_case = automata_repres_case(info, m, counter);
        cases.push_back(current_case);
        counter++;
    }
    result = join(cases.begin(), cases.end(), newline_str);
    return result;
}

std::string Automata_repres_builder::build_repres(info_for_constructing::Info& info,
                                                  const Command_buffer&        regexp)
{
    std::string result;
    G_DFA       gdfa              = info_to_automaton(regexp, sets_);
    std::string proc_def_template = fmt::format(aut_proc_template_fmt,
                                                info.names.name_of_scaner_class,
                                                f_.proc_name,
                                                f_.final_states_set_name,
                                                f_.diagnostic_msg,
                                                f_.final_actions);
#ifdef DEBUG_MODE_ON
    puts("proc_def_template:");
    puts(proc_def_template.c_str());
    puts("***********************************************************************");
#endif
    std::string switch_for_proc   = automata_repres_switch(info, gdfa);
    std::string proc_impl         = fmt::format(proc_def_template, switch_for_proc);
#ifdef DEBUG_MODE_ON
    puts("proc_impl:");
    puts(proc_impl.c_str());
    puts("***********************************************************************");
#endif
    return result;
}