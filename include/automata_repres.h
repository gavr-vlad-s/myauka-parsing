/*
    File:    automata_repres.h
    Created: 04 February 2017 at 14:59 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/
#ifndef AUTOMATA_REPRES_H
#define AUTOMATA_REPRES_H
#include <string>
#include "../include/scope.h"
#include "../include/errors_and_tries.h"
#include "../include/info_for_constructing.h"
#include "../include/trie_for_set.h"

enum class Regexp_kind{
    Ident, Number, String
};

struct Str_data_for_automaton {
    std::string automata_name;
    std::string proc_name;
    std::string category_name_prefix;
    std::string diagnostic_msg;
    std::string final_actions;
    std::string final_states_set_name;
};

std::string automata_repres(info_for_constructing::Info&     info,
                            const Str_data_for_automaton&    f,
                            const Trie_for_set_of_char32ptr& sets,
                            const Errors_and_tries&          et,
                            const std::shared_ptr<Scope>&    scope,
                            const Regexp_kind                kind);
#endif