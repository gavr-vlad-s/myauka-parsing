/*
    File:    implement_none_automaton.cpp
    Created: 11 January 2018 at 17:02 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include <string>
#include "../include/implement_none_automaton.h"
#include "../include/format.h"

using namespace std::string_literals;

static const std::string start_aut_name                = "A_start"s;
static const std::string start_aut_proc_proto          = "bool start_proc()"s;
static const std::string start_aut_proc_ptr_fmt        = "&{0}::start_proc"s;
static const std::string start_aut_final_proc_proto    = "void none_final_proc()"s;
static const std::string start_aut_final_proc_ptr_fmt  = "&{0}::none_final_proc"s;
static const std::string start_aut_final_proc_impl_fmt =
    R"~(void {0}::none_final_proc(){{
    /* This subroutine will be called if, after reading the input text, it turned
     * out to be in the A_start automaton. Then we do not need to do anything. */
}})~"s;

static const std::string start_proc_newline_is_not_lexem_fmt =
    R"~(bool {0}::start_proc(){{
    bool t = true;
    state  = -1;
    /* For an automaton that processes a lexeme, the state with the number (-1) is
     * the state in which this automaton is initialized. */
    if(belongs(SPACES, char_categories)){{
        loc->current_line += U'\n' == ch;
        return t;
    }}
    lexem_begin_line = loc->current_line;
    {{0}}
    return t;
}})~"s;

static const std::string start_proc_newline_is_lexem_fmt =
    R"~(bool {0}::start_proc(){{
    bool t = true;
    state  = -1;
    /* For an automaton that processes a lexeme, the state with the number (-1) is
     * the state in which this automaton is initialized. */
    if(belongs(SPACES, char_categories)){{
        if(U'\n' == ch){{
            token.code = {1}::Newline;
            lexem_begin_line = loc->current_line;
            loc->current_line += U'\n' == ch;
        }}
        return t;
    }}
    lexem_begin_line = loc->current_line;
    {{0}}
    return t;
}})~"s;


Automaton_constructing_info
    implement_none_automaton(const info_for_constructing::Info& info)
{
    Automaton_constructing_info result;
    result.name             = start_aut_name;
    result.proc_proto       = start_aut_proc_proto;
    auto& scaner_name       = info.names.name_of_scaner_class;
    result.proc_ptr         = fmt::format(start_aut_proc_ptr_fmt, scaner_name);
    if(info.newline_is_lexem){
        result.proc_impl = fmt::format(start_proc_newline_is_lexem_fmt,
                                       scaner_name,
                                       info.names.codes_type_name);
    }else{
        result.proc_impl = fmt::format(start_proc_newline_is_not_lexem_fmt,
                                       scaner_name);
    }
    result.final_proc_proto = start_aut_final_proc_proto;
    result.final_proc_ptr   = fmt::format(start_aut_final_proc_ptr_fmt, scaner_name);
    result.final_proc_impl  = fmt::format(start_aut_final_proc_impl_fmt, scaner_name);
    return result;
}