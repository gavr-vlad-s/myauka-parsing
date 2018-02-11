/*
    File:    implement_ident_and_keywords_automata.h
    Created: 24 January 2018 at 16:34 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef IMPLEMENT_IDENT_AND_KEYWORDS_AUTOMATA_H
#define IMPLEMENT_IDENT_AND_KEYWORDS_AUTOMATA_H
#include <memory>
#include "../include/info_for_constructing.h"
#include "../include/automaton_constructing_info.h"
#include "../include/errors_and_tries.h"
#include "../include/scope.h"
#include "../include/trie_for_set.h"

using INFO           = info_for_constructing::Info;
using Trie_for_chars = Trie_for_set_of_char32ptr;
using Ptr_to_scope   = std::shared_ptr<Scope>;

void implement_ident_and_keywords_automata(INFO&                   info,
                                           const Errors_and_tries& et,
                                           const Trie_for_chars&   sets_from_automata,
                                           const Ptr_to_scope&     scope);
#endif