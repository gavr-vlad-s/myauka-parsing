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

Automaton_constructing_info implement_none_automaton(const info_for_constructing::Info& info)
{
    Automaton_constructing_info result;
    result.name             = start_aut_name;
    result.proc_proto       = start_aut_proc_proto;
    result.proc_ptr         = fmt::format(start_aut_proc_ptr_fmt,
                                          info.names.name_of_scaner_class);
    result.proc_impl        = std::string();
    result.final_proc_proto = start_aut_final_proc_proto;
    result.final_proc_ptr   = fmt::format(start_aut_final_proc_ptr_fmt,
                                          info.names.name_of_scaner_class);
    result.final_proc_impl  = fmt::format(start_aut_final_proc_impl_fmt,
                                          info.names.name_of_scaner_class);
    return result;
}