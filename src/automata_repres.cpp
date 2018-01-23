/*
    File:    automata_repres.cpp
    Created: 04 February 2017 at 17:55 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include <string>
#include <list>
#include "../include/automata_repres.h"
#include "../include/groupped_dfa.h"
#include "../include/groupped_dfa_by_regexp.h"
#include "../include/format.h"
#include "../include/join.h"
#include "../include/get_act_repres.h"
#include "../include/add_category.h"
#include "../include/str_repres_for_set_of_size_t_const.h"

using namespace std::string_literals;

static constexpr size_t indent_width = 4;

static const std::string quadruple_indent = std::string(indent_width, ' ');

static const std::string newline_str = "\n"s;

static const std::string aut_proc_template_fmt =
    R"~(bool {0}::{1}(){{
    bool t             = true;
    bool there_is_jump = false;
    switch(state){{
        {{0}}
        default:
            ;
    }}
    if(!there_is_jump){{
        t = false;
        if(!is_elem(state, {2})){{
            printf("{3}", loc->current_line);
            en->increment_number_of_errors();
        }}
        {4}
    }}
    return t;
}})~"s;

static const std::string case_fmt =
R"~(        case {0}:
{1}
                break;)~"s;

static const std::string else_str = " else "s;

static const std::string all_chars_with_action_fmt =
R"~({0};
                state = {1};
                there_is_jump = true;)~"s;

static const std::string all_chars_without_action_fmt =
R"~(state = {0};
                there_is_jump = true;)~"s;

static const std::string set_of_cs_fmt =
R"~(if(belongs({0}, char_categories)){{
                    {1}
                    state         = {2};
                    there_is_jump = true;
                }})~"s;

static const std::string set_of_cs_fmt_without_action =
R"~(if(belongs({0}, char_categories)){{
                    state         = {2};
                    there_is_jump = true;
                }})~"s;

static const std::string set_of_cs_complement_fmt =
R"~(if(!belongs({0}, char_categories)){{
                    {1}
                    state         = {2};
                    there_is_jump = true;
                }})~"s;

static const std::string set_of_cs_complement_fmt_without_action =
R"~(if(!belongs({0}, char_categories)){{
                    state         = {2};
                    there_is_jump = true;
                }})~"s;


// struct Str_data_for_automaton {
//     std::string automata_name;
//     std::string proc_name;
//     std::string category_name_prefix;
//     std::string diagnostic_msg;
//     std::string final_actions;
//     std::string final_states_set_name;
// };



static G_DFA info_to_automaton(const info_for_constructing::Info& info,
                               const Trie_for_set_of_char32ptr&   sets,
                               const Regexp_kind                  kind)
{
    G_DFA gdfa;
    switch(kind){
        case Regexp_kind::Ident:
            grouped_DFA_by_regexp(gdfa, info.regexps.idents, sets);
            break;
        case Regexp_kind::Number:
            grouped_DFA_by_regexp(gdfa, info.regexps.numbers, sets);
            break;
        case Regexp_kind::String:
            grouped_DFA_by_regexp(gdfa, info.regexps.strings, sets);
            break;
        default:
            ;
    }
    return gdfa;
}

static std::string automata_repres_case_j(info_for_constructing::Info&  info,
                                          const Category&               cat,
                                          const DFA_state_with_action&  swa,
                                          const Str_data_for_automaton& f,
                                          const Errors_and_tries&       et,
                                          const std::shared_ptr<Scope>& scope,
                                          size_t& last_category_suffix)
{
    std::string result;
    auto        temp   = get_act_repres(et, scope, swa.action_idx);
    switch(cat.kind){
        case Category_kind::All_chars:
            if(!temp.empty()){
                result = fmt::format(all_chars_with_action_fmt,
                                     temp,
                                     std::to_string(swa.st));
            }else{
                result = fmt::format(all_chars_without_action_fmt,
                                     std::to_string(swa.st));
            }
            break;
        case Category_kind::Set_of_cs:
            {
                auto default_cat_name = f.category_name_prefix +
                                        std::to_string(last_category_suffix);
                auto result_cat       = add_category(info, cat.s, default_cat_name);
                if(result_cat.first){
                    // if there was not the category
                    last_category_suffix++;
                }
                if(!temp.empty()){
                    result = fmt::format(set_of_cs_fmt,
                                         result_cat.second,
                                         temp,
                                         std::to_string(swa.st));
                }else{
                    result = fmt::format(set_of_cs_fmt_without_action,
                                         result_cat.second,
                                         std::to_string(swa.st));
                }
            }
            break;
        case Category_kind::Set_of_cs_complement:
            {
                auto default_cat_name = f.category_name_prefix +
                                        std::to_string(last_category_suffix);
                auto result_cat       = add_category(info, cat.s, default_cat_name);
                if(result_cat.first){
                    // if there was not the category
                    last_category_suffix++;
                }
                if(!temp.empty()){
                    result = fmt::format(set_of_cs_complement_fmt,
                                         result_cat.second,
                                         temp,
                                         std::to_string(swa.st));
                }else{
                    result = fmt::format(set_of_cs_complement_fmt_without_action,
                                         result_cat.second,
                                         std::to_string(swa.st));
                }
            }
            break;
    }
    return result;
}

static std::string automata_repres_case(info_for_constructing::Info&    info,
                                        const   G_DFA_state_jumps&      m,
                                        const   Str_data_for_automaton& f,
                                        const   Errors_and_tries&       et,
                                        const   std::shared_ptr<Scope>& scope,
                                        size_t  counter,
                                        size_t& last_category_suffix)
{
    std::string result;
    if(m.empty()){
        return result;
    }

    std::vector<std::string> ifs;
    for(const auto& j : m){
        auto cat  = j.first;
        auto swa  = j.second;
        auto temp = automata_repres_case_j(info, cat,   swa,                 f,
                                           et,   scope, last_category_suffix);
        ifs.push_back(temp);
    }
    result = fmt::format(case_fmt,
                         std::to_string(counter),
                         quadruple_indent + join(ifs.begin(), ifs.end(), else_str));
    return result;
}

static std::string automata_repres_switch(info_for_constructing::Info&  info,
                                          const G_DFA&                  aut,
                                          const Str_data_for_automaton& f,
                                          const Errors_and_tries&       et,
                                          const std::shared_ptr<Scope>& scope)
{
    std::string            result;
    size_t                 counter              = 0;
    size_t                 last_category_suffix = 0;
    std::list<std::string> cases;
    for(const auto& m : aut.jumps){
        auto current_case = automata_repres_case(info,                m,     f,
                                                 et,                  scope, counter,
                                                 last_category_suffix);
        cases.push_back(current_case);
        counter++;
    }
    result = join(cases.begin(), cases.end(), newline_str);
    return result;
}

std::string automata_repres(info_for_constructing::Info&     info,
                            const Str_data_for_automaton&    f,
                            const Trie_for_set_of_char32ptr& sets,
                            const Errors_and_tries&          et,
                            const std::shared_ptr<Scope>&    scope,
                            const Regexp_kind                kind)
{
    std::string result;
    G_DFA       gdfa              = info_to_automaton(info, sets, kind);
    std::string proc_def_template = fmt::format(aut_proc_template_fmt,
                                                info.names.name_of_scaner_class,
                                                f.proc_name,
                                                f.final_states_set_name,
                                                f.diagnostic_msg,
                                                f.final_actions);
    std::string switch_for_proc   = automata_repres_switch(info, gdfa, f, et, scope);
    std::string proc_impl         = fmt::format(proc_def_template, switch_for_proc);
    std::string final_states_set  = str_repres_for_set_of_size_t_const(gdfa.final_states,
                                                                       f.final_states_set_name);
    result = final_states_set + "\n\n" + proc_impl;
    return result;
}

// #include "../include/add_category.h"
// #include "../include/get_act_repres.h"
// #include "../include/indent.h"
//
// static const std::string sp_else_sp = " else ";
//
// static std::string check_there_is_jump(const Str_data_for_automaton& f){
//     std::string result;
//     result = indent        + "if(!there_is_jump){\n" +
//              double_indent + "t = false;\n"          +
//              double_indent + "if(!is_elem(state, "   + f.final_states_set_name + ")){\n" +
//              triple_indent + "printf(\"" + f.diagnostic_msg + "\", loc->current_line);\n" +
//              triple_indent + "en->increment_number_of_errors();\n" + double_indent + "}\n";
//     auto temp = f.final_actions;
//     if(!temp.empty()){
//         result += double_indent + temp + "\n";
//     }
//     result += indent + "}\n";
//     return result;
// }
//
// static std::string qindent_string(const std::string& s){
//     std::string result;
//     if(!s.empty()){
//         result = quadruple_indent + s + "\n";
//     }
//     return result;
// }
// // // std::string automata_repres(Info_for_constructing& info, const G_DFA& aut,
// // //                             const Str_data_for_automaton& f)
// // // {
// // //     std::string result;
// // //
// // //     std::string proc_def = "bool " + info.name_of_scaner_class + "::" + f.proc_name + "{\n" +
// // //                            indent + "bool t             = true;\n" +
// // //                            indent + "bool there_is_jump = false;\n";
// // //     proc_def += automata_repres_switch(info, aut, f) + "\n";
// // //     proc_def += check_there_is_jump(f) + "\n";
// // //     proc_def += indent + "return t;\n}";
// // //     result = str_repres_for_set_of_size_t_const(aut.final_states, f.final_states_set_name) +
// // //              "\n\n" + proc_def;
// // //     return result;
// // // }