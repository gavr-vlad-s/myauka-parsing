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

using namespace std::string_literals;

static const std::string delimiter_aut_name                = "A_delimiter"s;
static const std::string delimiter_aut_proc_proto          = "bool delimiter_proc()"s;
static const std::string delimiter_aut_proc_ptr_fmt        = "&{0}::delimiter_proc"s;
static const std::string delimiter_aut_final_proc_proto    = "void delimiter_final_proc()"s;
static const std::string delimiter_aut_final_proc_ptr_fmt  = "&{0}::delimiter_final_proc"s;
// static const std::string unknown_aut_final_proc_impl_fmt =
//     R"~(void {0}::unknown_final_proc(){{
//     /* This subroutine will be called if, after reading the input text, it turned
//      * out to be in the automaton A_unknown. Then we do not need to do anything. */
// }})~"s;
// static const std::string unknown_aut_proc_impl_fmt       =
//     R"~(bool {0}::unknown_proc(){
//     return belongs(Other, char_categories);
// })~"s;

Automaton_constructing_info implement_unknown_automaton(info_for_constructing::Info& info)
{
    Automaton_constructing_info result;
    result.name             = delimiter_aut_name;
    result.proc_proto       = delimiter_aut_proc_proto;
    result.proc_ptr         = fmt::format(delimiter_aut_proc_ptr_fmt,
                                          info.names.name_of_scaner_class);
//     result.proc_impl        = fmt::format(unknown_aut_proc_impl_fmt,
//                                           info.names.name_of_scaner_class);
    result.final_proc_proto = delimiter_aut_final_proc_proto;
    result.final_proc_ptr   = fmt::format(delimiter_aut_final_proc_ptr_fmt,
                                          info.names.name_of_scaner_class);
//     result.final_proc_impl  = fmt::format(unknown_aut_proc_impl_fmt,
//                                           info.names.name_of_scaner_class);
    return result;
}

// // // #include "../include/generate_delim_automaton_impl.h"

// // // #include "../include/attributed_char_trie.h"
// // // #include "../include/errors_and_tries.h"
// // // #include "../include/idx_to_string.h"
// // // #include "../include/jump_table_string_repres.h"
// // // #include "../include/add_category.h"
// // // #include "../include/get_act_repres.h"
// // // #include "../include/indent.h"
// // // #include <string>
// // // #include <vector>
// // //
// // // static const std::string del_begin_cat_name_by_default = "DELIMITER_BEGIN";
// // // static const std::string del_jump_table_name           = "delim_jump_table";
// // // static const std::string del_init_table_name           = "init_table_for_delimiters";
// // // static const std::string delim_proc_body_              = R"~(::delimiter_proc(){
// // //     bool t = false;
// // //     if(-1 == state){
// // //         state = get_init_state(ch, init_table_for_delimiters,
// // //                                sizeof(init_table_for_delimiters)/sizeof(State_for_char));
// // //         token.code = delim_jump_table[state].code;
// // //         t = true;
// // //         return t;
// // //     }
// // //     Elem elem = delim_jump_table[state];
// // //     token.code = delim_jump_table[state].code;
// // //     int y = search_char(ch, elem.symbols);
// // //     if(y != THERE_IS_NO_CHAR){
// // //         state = elem.first_state + y; t = true;
// // //     })~";
// // //
// // // static std::string delim_proc_body(const std::string& s){
// // //     std::string result;
// // //     result = delim_proc_body_;
// // //     if(s.empty()){
// // //         result += "\n" + indent + "return t;\n}";
// // //     }else{
// // //         result += "\n" +
// // //                   indent + "if(!t){\n" +
// // //                   double_indent + s + "\n" +
// // //                   indent + "}\n" +
// // //                   indent + "return t;\n}";
// // //     }
// // //     return result;
// // // }
// // //
// // // void generate_delim_automaton_impl(Info_for_constructing& info){
// // //     /* This function builds an implementation of the automaton
// // //      * that processes delimiters. */
// // //     if(!belongs(Delimiter_aut, info.set_of_used_automata)){
// // //         return;
// // //     }
// // //
// // //     std::set<char32_t>          first_chars_for_delims; /* This set consists of
// // //                                                            characters from which
// // //                                                            the delimiters can begin. */
// // //     Attributed_char_trie        atrie;
// // //
// // //     std::vector<std::u32string> delimiter_strings;
// // //
// // //     for(size_t del_idx : info.del_repres){
// // //         auto delimiter = info.et.strs_trie->get_string(del_idx);
// // //         delimiter_strings.push_back(delimiter);
// // //         first_chars_for_delims.insert(delimiter[0]);
// // //     }
// // //     size_t counter = 0;
// // //     for(size_t del_idx : info.del_repres){
// // //         Attributed_cstring atrib_cstr;
// // //         atrib_cstr.str       = const_cast<char32_t*>(delimiter_strings[counter].c_str());
// // //         auto& scope_         = info.scope;
// // //         atrib_cstr.attribute = (scope_->strsc[del_idx]).code;
// // //         atrie.insert(attributed_cstring2string(atrib_cstr, 1));
// // //         counter++;
// // //     }
// // //
// // //     Jumps_and_inits jmps = atrie.jumps(); /* We built a workpiece for
// // //                                              the transition table. */
// // //     /* Now we need to add the desired text to the implementation of the start automaton
// // //      * and generate a function that handles the delimiters. */
// // //     auto cat_res = add_category(info, first_chars_for_delims, del_begin_cat_name_by_default);
// // //     std::string delimiter_begin_cat_name = cat_res.second;
// // //
// // //     info.aut_impl[Start_aut] += "\n    if(belongs(" + delimiter_begin_cat_name +
// // //         ", char_categories)){\n        (loc->pcurrent_char)--; " +
// // //         "automaton = A_delimiter;\n        state = -1;\n        return t;\n    }\n";
// // //
// // //     auto del_postact = get_act_repres(info, info.del_postaction);
// // //
// // //     info.aut_impl[Delimiter_aut] = jump_table_string_repres(info, jmps, del_jump_table_name,
// // //                                                             del_init_table_name) +
// // //                               "bool " + info.name_of_scaner_class + delim_proc_body(del_postact);
// // //
// // //     info.aut_impl_fin_proc[Delimiter_aut] = "void " + info.name_of_scaner_class +
// // //                                        R"~(::delimiter_final_proc(){
// // //     )~" + indent + del_postact +
// // //     R"~(
// // //     token.code = delim_jump_table[state].code;
// // //     )~" + "\n}";
// // // }