/*
    File:    implement_string_automaton.h
    Created: 16 January 2018 at 16:44 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef IMPLEMENT_STRING_AUTOMATON_H
#define IMPLEMENT_STRING_AUTOMATON_H
#include <memory>
#include "../include/info_for_constructing.h"
#include "../include/automaton_constructing_info.h"
#include "../include/errors_and_tries.h"
#include "../include/scope.h"
#include "../include/trie_for_set.h"

Automaton_constructing_info
    implement_string_automaton(info_for_constructing::Info&     info,
                               const Errors_and_tries&          et,
                               const Trie_for_set_of_char32ptr& sets_from_automata,
                               const std::shared_ptr<Scope>&    scope);
#endif