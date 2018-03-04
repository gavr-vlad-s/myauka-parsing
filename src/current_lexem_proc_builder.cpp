/*
    File:    current_lexem_builder.cpp
    Created: 28 February 2018 at 09:36 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include <string>
#include "../include/current_lexem_proc_builder.h"
#include "../include/format.h"
#include "../include/print_char32.h"
#include "../include/list_to_columns.h"
#include "../include/attributed_char_trie.h"
#include "../include/char_conv.h"

using namespace std::string_literals;

static const std::string current_lexem_without_comments_proc_fmt =
R"~({0} {1}::current_lexem()
{{
    automaton = A_start; token.code = None;
    lexem_begin = loc->pcurrent_char;
    bool t = true;
    while((ch = *(loc->pcurrent_char)++)){{
        char_categories = get_categories_set(ch);
        t = (this->*procs[automaton])();
        if(!t){{
            /* We get here only if the lexeme has already been read. At the same time,
             * the current automaton reads the character immediately after the end of
             * the token read, based on this symbol, it is decided that the token has
             * been read and the transition to the next character has been made.
             * Therefore, in order to not miss the first character of the next lexeme,
             * we need to decrease the pcurrent_char pointer by one. */
            (loc->pcurrent_char)--;
            return token;
        }}
    }}
    /* Here we can be, only if we have already read all the processed text. In this
     * case, the pointer to the current symbol indicates a byte, which is immediately
     * after the zero character, which is a sign of the end of the text. To avoid
     * entering subsequent calls outside the text, we need to go back to the null
     * character. */
    (loc->pcurrent_char)--;
    /* Further, since we are here, the end of the current token (perhaps unexpected)
     * has not yet been processed. It is necessary to perform this processing, and,
     * probably, to display any diagnostics. */
    (this->*finals[automaton])();
    return token;
}}

)~"s;

static const std::string there_is_only_singlelined_fmt =
R"~(void {0}::omit_singlelined_comment()
{{
    while((ch = (loc->pcurrent_char)++)){{
        if('\n' == ch){{
            (loc->pcurrent_char)--;
            return;
        }}
    }}
    (loc->pcurrent_char)--;
    return;
}}

{1} {0}::current_lexem_()
{{
    automaton   = A_start;
    token.code  = {2}::None;
    lexem_begin = loc->pcurrent_char;
    bool t      = true;
    while((ch = *(loc->pcurrent_char)++)){{
        char_categories = get_categories_set(ch); //categories_table[ch];
        t = (this->*procs[automaton])();
        if(!t){{
            /* We get here only if the lexeme has already been read. At the same time,
             * the current automaton reads the character immediately after the end of
             * the token read, based on this symbol, it is decided that the token has
             * been read and the transition to the next character has been made.
             * Therefore, in order to not miss the first character of the next lexeme,
             * we need to decrease the pcurrent_char pointer by one. */
            (loc->pcurrent_char)--;
            return token;

        }}
    }}
    /* Here we can be, only if we have already read all the processed text. In this
     * case, the pointer to the current symbol indicates a byte, which is immediately
     * after the zero character, which is a sign of the end of the text. To avoid
     * entering subsequent calls outside the text, we need to go back to the null
     * character. */
    (loc->pcurrent_char)--;
    /* Further, since we are here, the end of the current token (perhaps unexpected)
     * has not yet been processed. It is necessary to perform this processing, and,
     * probably, to display any diagnostics. */
    (this->*finals[automaton])();
    return token;
}}

{1} {0}::current_lexem()
{{
    for( ; ;){{
        auto l = current_lexem_();
        if(l.code == {2}::SINGLE_LINED_COMMENT_MARK){{
            omit_singlelined_comment();
        }}else{{
            return token;
        }}
    }}
    return token;
}}

)~"s;

static const std::string only_singlelined_fmt =
R"~(    {0} current_lexem_();
    void omit_singlelined_comment();
)~"s;

static const std::string multilined_comment_not_nested_fmt =
R"~(struct Jump_for_multilined_end{{
    uint32_t next_state;
    char32_t jump_char;
}};

static const Jump_for_multilined_end multilined_jumps[] = {{
{0}
}};

void {1}::omit_multilined_comment()
{{
    size_t st = 0;
    while((ch = (loc->pcurrent_char)++)){{
        auto j  = multilined_jumps[st];
        auto jc = j.jump_char;
        if(!jc){{
            (loc->pcurrent_char)--;
            return;
        }}
        if(ch == jc){{
            st = j.next_state;
        }}else{{
            st = 0;
        }}
    }}
    printf("Unexpected end of a multi-line comment at line %zu.\n",
           lexem_begin_line_number());
    en->increment_number_of_errors();
    return;
}}

{2} {1}::current_lexem_()
{{
    automaton   = A_start;
    token.code  = {3}::None;
    lexem_begin = loc->pcurrent_char;
    bool t      = true;
    while((ch = *(loc->pcurrent_char)++)){{
        char_categories = get_categories_set(ch); //categories_table[ch];
        t = (this->*procs[automaton])();
        if(!t){{
            /* We get here only if the lexeme has already been read. At the same time,
             * the current automaton reads the character immediately after the end of
             * the token read, based on this symbol, it is decided that the token has
             * been read and the transition to the next character has been made.
             * Therefore, in order to not miss the first character of the next lexeme,
             * we need to decrease the pcurrent_char pointer by one. */
            (loc->pcurrent_char)--;
            return token;

        }}
    }}
    /* Here we can be, only if we have already read all the processed text. In this
     * case, the pointer to the current symbol indicates a byte, which is immediately
     * after the zero character, which is a sign of the end of the text. To avoid
     * entering subsequent calls outside the text, we need to go back to the null
     * character. */
    (loc->pcurrent_char)--;
    /* Further, since we are here, the end of the current token (perhaps unexpected)
     * has not yet been processed. It is necessary to perform this processing, and,
     * probably, to display any diagnostics. */
    (this->*finals[automaton])();
    return token;
}}

{2} {1}::current_lexem()
{{
    for( ; ; ){{
        auto l = current_lexem_();
        switch(l.code){{
            case {3}::MULTI_LINED_COMMENT_MARK:
                omit_multilined_comment();
                break;
            case {3}::MULTI_LINED_COMMENT_END:
                printf("Unexpected end of a multi-line comment at line %zu.\n",
                       lexem_begin_line_number());
                en->increment_number_of_errors();
                break;
            default:
                return token;
        }}
    }}
    return token;
}}

)~"s;

static const std::string multilined_comment_nested_fmt =
R"~(static const State_for_char nested_comments_init_table[] = {{
{0}
}};

static constexpr size_t NUM_OF_ELEMS_OF_COMM_INIT_TABLE =
    size(nested_comments_init_table);

static constexpr uint16_t BEGIN_MARKER = 1;
static constexpr uint16_t END_MARKER   = 2;

struct Comment_jump{{
    /** A pointer to a string of characters that can be crossed. */
    char32_t*       symbols;
    uint16_t        marker;
    /** If the current character matches symbols[0], then the transition to the state
     *  first_state;
     *  if the current character matches symbols[1], then the transition to the state
     *  first_state + 1;
     *  if the current character matches symbols[2], then the transition to the state
     *  first_state + 2, and so on. */
    uint16_t        first_state;
}};

static const Comment_jump comments_jump_table[] = {{
{1}
}};

void {2}::omit_multilined_comment()
{{
    ssize_t st     = -1;
    comment_level  = 1;
    uint16_t m;
    while((ch = (loc->pcurrent_char)++)){{
        if(-1 == st){{
            st = get_init_state(ch, nested_comments_init_table,
                                NUM_OF_ELEMS_OF_COMM_INIT_TABLE);
            continue;
        }}
        auto j   = comments_jump_table[st];
        int  idx = search_char(ch, j.symbols);
        m        = j.marker;
        if(idx != THERE_IS_NO_CHAR){{
            st = j.first_state + idx;
        }}else{{
            switch(m){{
                case BEGIN_MARKER:
                    comment_level++;
                    (loc->pcurrent_char)--;
                    st = -1;
                    break;
                case END_MARKER:
                    comment_level--;
                    (loc->pcurrent_char)--;
                    st = -1;
                    if(0 == comment_level){{
                        return;
                    }}
                    break;
                default:
                    (loc->pcurrent_char)--;
                    st = -1;
            }}
        }}
    }}

    if(-1 == st){{
        printf("Unexpected end of a multi-line comment at line %zu.\n",
               lexem_begin_line_number());
        en->increment_number_of_errors();
    }}else{{
        auto j   = comments_jump_table[st];
        m        = j.marker;
        switch(m){{
            case BEGIN_MARKER:
                comment_level++;
                (loc->pcurrent_char)--;
                break;
            case END_MARKER:
                comment_level--;
                (loc->pcurrent_char)--;
                break;
            default:
                (loc->pcurrent_char)--;
                printf("Unexpected end of a multi-line comment at line %zu.\n",
                       lexem_begin_line_number());
                en->increment_number_of_errors();
                return;
        }}
        if(comment_level != 0){{
            printf("Unexpected end of a multi-line comment at line %zu.\n",
                   lexem_begin_line_number());
            en->increment_number_of_errors();
        }}
    }}
}}

{3} {2}::current_lexem_()
{{
    automaton   = A_start; token.code = None;
    lexem_begin = loc->pcurrent_char;
    bool t      = true;
    while((ch = *(loc->pcurrent_char)++)){{
        char_categories = get_categories_set(ch);
        t = (this->*procs[automaton])();
        if(!t){{
            /* We get here only if the lexeme has already been read. At the same time,
             * the current automaton reads the character immediately after the end of
             * the token read, based on this symbol, it is decided that the token has
             * been read and the transition to the next character has been made.
             * Therefore, in order to not miss the first character of the next lexeme,
             * we need to decrease the pcurrent_char pointer by one. */
            (loc->pcurrent_char)--;
            return token;
        }}
    }}
    /* Here we can be, only if we have already read all the processed text. In this
     * case, the pointer to the current symbol indicates a byte, which is immediately
     * after the zero character, which is a sign of the end of the text. To avoid
     * entering subsequent calls outside the text, we need to go back to the null
     * character. */
    (loc->pcurrent_char)--;
    /* Further, since we are here, the end of the current token (perhaps unexpected)
     * has not yet been processed. It is necessary to perform this processing, and,
     * probably, to display any diagnostics. */
    (this->*finals[automaton])();
    return token;
}}

{3} {2}::current_lexem()
{{
    for( ; ; ){{
        auto l = current_lexem_();
        switch(l.code){{
            case {4}::MULTI_LINED_COMMENT_MARK:
                omit_multilined_comment();
                break;
            case {4}::MULTI_LINED_COMMENT_END:
                printf("Unexpected end of a multi-line comment at line %zu.\n",
                       lexem_begin_line_number());
                en->increment_number_of_errors();
                break;
            default:
                return token;
        }}
    }}
    return token;
}}

)~"s;

static const std::string multilined_private_fmt =
R"~(    {0} current_lexem_();
    void omit_multilined_comment();
)~"s;

static constexpr size_t BEGIN_MARKER = 1;
static constexpr size_t END_MARKER   = 2;

Current_lexem_proc_info Current_lexem_proc_builder::no_comments()
{
    Current_lexem_proc_info result;
    result.implementation  = fmt::format(current_lexem_without_comments_proc_fmt,
                                         info_.names.lexem_info_name,
                                         info_.names.name_of_scaner_class);
    result.private_members = std::string();
    return result;
}

Current_lexem_proc_info Current_lexem_proc_builder::there_is_only_singlelined()
{
    Current_lexem_proc_info result;
    result.implementation  = fmt::format(there_is_only_singlelined_fmt,
                                         info_.names.name_of_scaner_class,
                                         info_.names.lexem_info_name,
                                         info_.names.codes_type_name);
    result.private_members = fmt::format(only_singlelined_fmt,
                                         info_.names.lexem_info_name);
    return result;
}

static constexpr size_t INDENT_WIDTH = 4;

static std::string table_for_not_nested_multilined(const std::u32string& end_of_comment)
{
    std::string result;

    std::vector<std::string> table_body;
    uint32_t st = 0;
    for(const auto c : end_of_comment){
        st++;
        auto temp = "{" + std::to_string(st) + ", " + show_char32(c) + "}";
        table_body.push_back(temp);
    }
    auto last_elem = "{" + std::to_string(0) + ", " + show_char32(0) + "}";

    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 4;
    f.spaces_between_columns = 1;

    result = string_list_to_columns(table_body, f);
    return result;
}

static std::string generate_init_table_body_for_nested_comments(const Jumps_and_inits& ji)
{
    std::string result;
    std::vector<std::string> table_elems;
    auto& inits = ji.init_table;
    for(const auto& i : inits){
        std::string elem = "{" + std::to_string(i.first) + ", " +
                           char32_to_utf8(i.second) + "}";
        table_elems.push_back(elem);
    }

    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 3;
    f.spaces_between_columns = 1;

    result                   = string_list_to_columns(table_elems, f);
    return result;
}

std::string generate_jump_table_body_for_nested_comments(const Jumps_and_inits& ji){
    std::string result;
    auto& jmps = ji.jumps;
    std::vector<std::string> jmp_table_body;
    for(const auto& j : jmps){
        std::string jump = "{const_cast<char32_t*>(U\"" +
                           u32string_to_utf8(j.jump_chars) + "\"), " +
                           std::to_string(j.code) + ", " +
                           std::to_string(j.first_state) + "}";
        jmp_table_body.push_back(jump);
    }

    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 3;
    f.spaces_between_columns = 1;

    result                   = string_list_to_columns(jmp_table_body, f);
    return result;
}

Jumps_and_inits prepare_jumps(const info_for_constructing::Info& info)
{
    Jumps_and_inits result;
    Attributed_char_trie atrie;
    Attributed_cstring   atrib_cstr_b;
    Attributed_cstring   atrib_cstr_e;

    auto& comments_info    = info.about_comments;
    auto& marker_b         = comments_info.mark_of_multilined_begin;
    auto& marker_e         = comments_info.mark_of_multilined_end;

    atrib_cstr_b.str       = const_cast<char32_t*>(marker_b.c_str());
    atrib_cstr_e.str       = const_cast<char32_t*>(marker_e.c_str());
    atrib_cstr_b.attribute = BEGIN_MARKER;
    atrib_cstr_e.attribute = END_MARKER;
    atrie.insert(attributed_cstring2string(atrib_cstr_b, 0));
    atrie.insert(attributed_cstring2string(atrib_cstr_e, 0));

    result                 = atrie.jumps();
    return result;
}

Current_lexem_proc_info Current_lexem_proc_builder::there_is_only_multilined()
{
    Current_lexem_proc_info result;
    auto& comments_info = info_.about_comments;
    if(comments_info.multilined_is_nested){
        auto jumps             = prepare_jumps(info_);
        auto init_table_body   = generate_init_table_body_for_nested_comments(jumps);
        auto table_body        = generate_jump_table_body_for_nested_comments(jumps);
        result.implementation  = fmt::format(multilined_comment_nested_fmt,
                                             init_table_body,
                                             table_body,
                                             info_.names.name_of_scaner_class,
                                             info_.names.lexem_info_name,
                                             info_.names.codes_type_name);
    }else{
        const auto& table_body =
            table_for_not_nested_multilined(comments_info.mark_of_multilined_end);
        result.implementation  = fmt::format(multilined_comment_not_nested_fmt,
                                             table_body,
                                             info_.names.name_of_scaner_class,
                                             info_.names.lexem_info_name,
                                             info_.names.codes_type_name);
    }
    result.private_members = fmt::format(multilined_private_fmt,
                                         info_.names.lexem_info_name);
    return result;
}

Current_lexem_proc_info Current_lexem_proc_builder::there_are_all_comments()
{
    Current_lexem_proc_info result;
    return result;
}

Current_lexem_proc_info Current_lexem_proc_builder::build()
{
    Current_lexem_proc_info result;

    enum class Comment_kind {
        No_comments,              There_is_only_singlelined,
        There_is_only_multilined, There_are_all
    };

    auto& comments_info = info_.about_comments;
    bool  t1            = comments_info.mark_of_single_lined.empty();
    bool  t2            = comments_info.mark_of_multilined_begin.empty();

    Comment_kind k = static_cast<Comment_kind>(t2 * 2 + t1);

    switch(k){
        case Comment_kind::No_comments:
            result = no_comments();
            break;
        case Comment_kind::There_is_only_singlelined:
            result = there_is_only_singlelined();
            break;
        case Comment_kind::There_is_only_multilined:
            result = there_is_only_multilined();
            if(comments_info.multilined_is_nested){
                result.private_members += "\n    int comment_level;";
            }
            break;
        case Comment_kind::There_are_all:
            result                       = there_are_all_comments();
//             info.fields_for_comments_handling = "\n" + indent + info.lexem_info_name +
//                                          + " " + "current_lexem_();\n"
//                                          + indent + "void omit_singlelined_comment();\n"
//                                          + indent + "void omit_multilined_comment();";
//             if(info.multilined_is_nested){
//                 info.fields_for_comments_handling += "\n" + indent + "int comment_level;";
//             }
            break;
    }
    return result;
}

// #include "../include/implement_scaner.h"
// #include "../include/indent.h"
// #include "../include/generate_category_table.h"
// #include "../include/add_newline_if_non_empty.h"
// #include <cstdio>
// #include <string>
//
// static std::string impl_includes(const std::string& impl_hn){
//     std::string result;
//     result = "#include \"../include/" + impl_hn + "\"\n" +
//              R"~(#include "../include/get_init_state.h"
// #include "../include/search_char.h"
// #include "../include/belongs.h"
// #include <set>
// #include <string>
// #include <vector>
// #include "../include/operations_with_sets.h"
// )~";
//     return result;
// }
//
// static const std::string start_proc_end =
//     R"~(
//     automaton = A_unknown;
//     return t;
// })~";
//
// static const Comment_jump comments_jump_table[] = {\n)~";
//
// static const std::string current_lexem_with_omitting_all =
//     R"~(::current_lexem(){
//     for( ; ; ){
//         auto l = current_lexem_();
//         switch(l.code){
//             case SINGLE_LINED_COMMENT_MARK:
//                 omit_singlelined_comment();
//                 break;
//             case MULTI_LINED_COMMENT_MARK:
//                 omit_multilined_comment();
//                 break;
//             case MULTI_LINED_COMMENT_END:
//                 printf("Unexpected end of a multi-line comment in line %zu.\n",
//                        lexem_begin_line_number());
//                 en->increment_number_of_errors();
//                 break;
//             default:
//                 return token;
//         }
//     }
//     return token;
// }
//
// )~";
//
// static std::string there_are_all_comments(Info_for_constructing& info){
//     std::string result;
//     result = "void " + info.name_of_scaner_class + omit_multilined_comment_proc(info) +
//              "void " + info.name_of_scaner_class + omit_singlelined_comment_proc_str +
//              info.lexem_info_name + info.name_of_scaner_class + current_lexem__without_comments_proc +
//              info.lexem_info_name + info.name_of_scaner_class + current_lexem_with_omitting_all;
//     return result;
// }
// static std::string automata_table(const Info_for_constructing& info){
//     std::string result = info.name_of_scaner_class + "::Automaton_proc " +
//                          info.name_of_scaner_class + "::procs[] = {\n";
//
//     std::vector<std::string> procs_list;
//     for(const auto ap : info.automaton_info){
//         procs_list.push_back(ap.proc_ptr);
//     }
//
//     Format f;
//     f.indent                 = INDENT_WIDTH;
//     f.number_of_columns      = 2;
//     f.spaces_between_columns = 1;
//
//     result += string_list_to_columns(procs_list, f) + "\n};";
//     return result;
// }
//
// static std::string final_table(const Info_for_constructing& info){
//     std::string result = info.name_of_scaner_class + "::Final_proc " +
//                          info.name_of_scaner_class + "::finals[] = {\n";
//
//     std::vector<std::string> fprocs_list;
//     for(const auto ap : info.automaton_info){
//         fprocs_list.push_back(ap.fin_proc_ptr);
//     }
//
//     Format f;
//     f.indent                 = INDENT_WIDTH;
//     f.number_of_columns      = 2;
//     f.spaces_between_columns = 1;
//
//     result += string_list_to_columns(fprocs_list, f) + "\n};";
//     return result;
// }
//
// static std::string procs_tables(const Info_for_constructing& info){
//     std::string result = automata_table(info) + "\n\n" + final_table(info) + "\n\n";
//     return result;
// }
//
// static std::string collect_automata_impls(Info_for_constructing& info){
//     std::string result;
//     info.aut_impl[Start_aut] += start_proc_end;
//     for(const auto s : info.aut_impl){
//         result += s.second + "\n\n";
//     }
//     for(const auto s : info.aut_impl_fin_proc){
//         result += s.second + "\n\n";
//     }
//     return result;
// }
//
// void implement_scaner(Info_for_constructing& info){
//     std::string impl_text      = impl_includes(info.header_name)                   +
//                                  add_newline_if_non_empty(info.impl_additions_str) +
//                                  procs_tables(info);
//
//     impl_text += generate_category_table(info) + collect_automata_impls(info) +
//                  generate_current_lexem_proc(info);
//
//     FILE* fptr = fopen(info.impl_file_name.c_str(), "w");
//     if(fptr){
//         fputs(impl_text.c_str(), fptr);
//         fputs("\n",fptr);
//         fclose(fptr);
//     }else{
//         printf("Could not create the scanner implementation file.\n");
//         info.et.ec -> increment_number_of_errors();
//     }
// }