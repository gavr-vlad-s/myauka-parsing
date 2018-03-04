/*
    File:    implement_ident_and_keywords_automata.cpp
    Created: 24 January 2018 at 17:07 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/implement_ident_and_keywords_automata.h"
#include "../include/belongs.h"
#include "../include/used_automaton.h"
#include "../include/generate_separate_keywords_automaton.h"
#include "../include/generate_separate_identifier_automat.h"
#include "../include/id_with_keyw_builder.h"

void implement_ident_and_keywords_automata(INFO&                   info,
                                           const Errors_and_tries& et,
                                           const Trie_for_chars&   sets_from_automata,
                                           const Ptr_to_scope&     scope)
{
    bool t1 = belongs(Id_aut, info.set_of_used_automata)      != 0;
    bool t2 = belongs(Keyword_aut, info.set_of_used_automata) != 0;

    enum Idkw {
        There_is_no_id_and_there_are_no_keywords,
        There_is_no_id_and_there_are_keywords,
        There_is_id_and_there_are_no_keywords,
        There_is_id_and_there_are_keywords
    };
    Idkw t = static_cast<Idkw>(t1 * 2 + t2);
    switch(t){
        case There_is_no_id_and_there_are_no_keywords:
            break;
        case There_is_no_id_and_there_are_keywords:
            /* Here keyword processing will be similar to the processing of delimiters. */
            generate_separate_keywords_automaton(info, et, scope);
            info.needed_Elem = true;
            break;
        case There_is_id_and_there_are_no_keywords:
            generate_separate_identifier_automat(info, et, sets_from_automata, scope);
            break;
        case There_is_id_and_there_are_keywords:
            {
                Id_with_keyw_builder idkeyw_builder {sets_from_automata, et, scope};
                idkeyw_builder.build(info);
            }
            break;
        default:
            ;
    }
    return;
}