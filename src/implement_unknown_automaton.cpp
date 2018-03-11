/*
    File:    implement_unknown_automaton.cpp
    Created: 11 January 2018 at 18:59 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include <string>
#include "../include/implement_unknown_automaton.h"
#include "../include/format.h"

using namespace std::string_literals;

static const std::string unknown_aut_name                = "A_unknown"s;
static const std::string unknown_aut_proc_proto          = "bool unknown_proc()"s;
static const std::string unknown_aut_proc_ptr_fmt        = "&{0}::unknown_proc"s;
static const std::string unknown_aut_final_proc_proto    = "void unknown_final_proc()"s;
static const std::string unknown_aut_final_proc_ptr_fmt  = "&{0}::unknown_final_proc"s;
static const std::string unknown_aut_final_proc_impl_fmt =
    R"~(void {0}::unknown_final_proc(){{
    /* This subroutine will be called if, after reading the input text, it turned
     * out to be in the automaton A_unknown. Then we do not need to do anything. */
}})~"s;
static const std::string unknown_aut_proc_impl_fmt       =
    R"~(bool {0}::unknown_proc(){{
    return belongs(Other, char_categories);
}})~"s;

Automaton_constructing_info
    implement_unknown_automaton(const info_for_constructing::Info& info)
{
    Automaton_constructing_info result;
    result.name             = unknown_aut_name;
    result.proc_proto       = unknown_aut_proc_proto;
    auto& scaner_name       = info.names.name_of_scaner_class;
    result.proc_ptr         = fmt::format(unknown_aut_proc_ptr_fmt, scaner_name);
    result.proc_impl        = fmt::format(unknown_aut_proc_impl_fmt, scaner_name);
    result.final_proc_proto = unknown_aut_final_proc_proto;
    result.final_proc_ptr   = fmt::format(unknown_aut_final_proc_ptr_fmt, scaner_name);
    result.final_proc_impl  = fmt::format(unknown_aut_proc_impl_fmt, scaner_name);
    return result;
}