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
#include "../include/generate_both_keyword_and_ident_automata.h"

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
            generate_both_keyword_and_ident_automata(info, et, sets_from_automata, scope);
//             if(!(info.begin_chars[Keyword_beg_char] * info.begin_chars[Id_beg_char])){
//                 /* If the set of first characters of keywords does not intersect with
//                  * the set of first characters of identifier, then create a separate
//                  * identifier handling automaton and a separate keyword processing
//                  * automaton, without the possibility of switching between them. In
//                  * this case, the processing of keywords will be performed similarly
//                  * to the processing of delimiters. */
//                 generate_separate_keywords_automat(info);
//                 generate_separate_identifier_automat(info);
//             }else{
//                 /* If the specified sets intersect, then we need to glue the regular
//                  * expression for the identifiers with regular expression for the
//                  * keywords, create a minimized deterministic finite automaton with the
//                  * grouped transitions from this regular expression, and from the resulting
//                  * automaton we need to build its implementation. */
//                 info.set_of_used_automata &= ~((1ULL << Id_aut) | (1ULL << Keyword_aut));
//                 info.set_of_used_automata |= 1ULL << IdKeyword_aut;
//                 generate_idkeyword_automat(info);
//             }
            break;
        default:
            ;
    }
    return;
}
