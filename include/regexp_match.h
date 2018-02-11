/*
    File:    regexp_match.h
    Created: 31 January 2018 at 13:26 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef REGEXP_MATCH_H
#define REGEXP_MATCH_H
#include "../include/command.h"
#include "../include/trie_for_set.h"
#include "../include/groupped_dfa.h"
#include "../include/groupped_dfa_by_regexp.h"
#include "../include/match.h"
template<class InputIt>
bool regexp_match(const Command_buffer&            buffer,
                  const Trie_for_set_of_char32ptr& t,
                  InputIt                          first,
                  InputIt                          last)
{
    G_DFA gdfa;
    grouped_DFA_by_regexp(gdfa, buffer, t);
    return match_all(gdfa, first, last);
}
#endif