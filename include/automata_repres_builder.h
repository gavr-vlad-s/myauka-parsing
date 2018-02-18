/*
    File:    automata_repres_builder.h
    Created: 08 February 2017 at 14:09 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef AUTOMATA_REPRES_BUILDER_H
#define AUTOMATA_REPRES_BUILDER_H
#include <string>
#include <memory>
#include "../include/scope.h"
#include "../include/errors_and_tries.h"
#include "../include/info_for_constructing.h"
#include "../include/trie_for_set.h"
#include "../include/command.h"
#include "../include/groupped_dfa.h"

struct Str_data_for_automaton {
    std::string automata_name;
    std::string proc_name;
    std::string category_name_prefix;
    std::string diagnostic_msg;
    std::string final_actions;
    std::string final_states_set_name;
};

class Automata_repres_builder{
public:
    Automata_repres_builder()                               = default;
    Automata_repres_builder(const Automata_repres_builder&) = default;

    Automata_repres_builder(const Str_data_for_automaton&    f,
                            const Trie_for_set_of_char32ptr& sets,
                            const Errors_and_tries&          et,
                            const std::shared_ptr<Scope>&    scope) :
                        f_(f), sets_(sets), et_(et), scope_(scope) {};

    ~Automata_repres_builder()                              = default;

    std::string build_repres(info_for_constructing::Info& info,
                             const Command_buffer&        regexp);

private:
    size_t                    last_category_suffix_;
    Str_data_for_automaton    f_;
    Trie_for_set_of_char32ptr sets_;
    Errors_and_tries          et_;
    std::shared_ptr<Scope>    scope_;

    std::string automata_repres_switch(info_for_constructing::Info& info,
                                       const G_DFA&                 aut);

    std::string automata_repres_case(info_for_constructing::Info& info,
                                     const   G_DFA_state_jumps&   m,
                                     size_t                       counter);

    std::string automata_repres_case_j(info_for_constructing::Info&  info,
                                       const Category&               cat,
                                       const DFA_state_with_action&  swa);
};
#endif