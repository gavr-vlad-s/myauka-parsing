/*
    File:    automaton_constructing_info.h
    Created: 09 January 2018 at 17:11 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef AUTOMATON_CONSTRUCTING_INFO_H
#define AUTOMATON_CONSTRUCTING_INFO_H
#include <string>
#include <vector>
struct Automaton_constructing_info{
    std::string name;
    std::string proc_proto;
    std::string proc_ptr;
    std::string proc_impl;
    std::string final_proc_proto;
    std::string final_proc_ptr;
    std::string final_proc_impl;
};

using Automata_info = std::vector<Automaton_constructing_info>;
#endif