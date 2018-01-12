/*
    File:    implement_automata.cpp
    Created: 10 January 2018 at 16:51 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/info_for_constructing.h"
#include "../include/implement_automata.h"
#include "../include/implement_none_automaton.h"
#include "../include/implement_unknown_automaton.h"

using namespace info_for_constructing;
void implement_automata(info_for_constructing::Info& info,
                        const Collected_data&        d,
                        const Errors_and_tries&      et)
{
    info.automata_info.push_back(implement_none_automaton(info));
    info.automata_info.push_back(implement_unknown_automaton(info));
    info.automata_info.push_back(implement_delimiter_automaton(info));
}