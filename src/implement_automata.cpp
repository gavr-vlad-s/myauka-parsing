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
#include "../include/used_automaton.h"
#include "../include/belongs.h"
#include "../include/implement_delimiter_automaton.h"
#include "../include/scope.h"
#include "../include/implement_string_automaton.h"
#include "../include/implement_number_automaton.h"

using namespace info_for_constructing;
void implement_automata(info_for_constructing::Info&     info,
                        const Trie_for_set_of_char32ptr& sets_from_automata,
                        const Errors_and_tries&          et,
                        const std::shared_ptr<Scope>&    scope)
{
    info.automata_info.push_back(implement_none_automaton(info));
    info.automata_info.push_back(implement_unknown_automaton(info));
    if(belongs(Delimiter_aut, info.set_of_used_automata)){
        auto da          = implement_delimiter_automaton(info, et, scope);
        info.automata_info.push_back(da);
        info.needed_Elem = true;
    }
    if(belongs(String_aut, info.set_of_used_automata)){
        auto sa          = implement_string_automaton(info,               et,
                                                      sets_from_automata, scope);
        info.automata_info.push_back(sa);
    }
    if(belongs(Number_aut, info.set_of_used_automata)){
        auto na          = implement_number_automaton(info,               et,
                                                      sets_from_automata, scope);
        info.automata_info.push_back(na);
    }
}