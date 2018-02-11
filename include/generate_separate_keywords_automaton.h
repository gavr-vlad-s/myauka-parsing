/*
    File:    generate_separate_keywords_automaton.h
    Created: 26 January 2018 at 08:02 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef GENERATE_SEPARATE_KEYWORDS_AUTOMATON_H
#define GENERATE_SEPARATE_KEYWORDS_AUTOMATON_H
#include <memory>
#include "../include/info_for_constructing.h"
// #include "../include/automaton_constructing_info.h"
#include "../include/errors_and_tries.h"
#include "../include/scope.h"
void generate_separate_keywords_automaton(info_for_constructing::Info&  info,
                                          const Errors_and_tries&       et,
                                          const std::shared_ptr<Scope>& scope);
#endif