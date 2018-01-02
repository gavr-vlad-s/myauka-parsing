/*
    File:    collected_data.h
    Created: 10 December 2017 at 09:50 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef COLLECTED_DATA_H
#define COLLECTED_DATA_H
#include <cstdint>
#include <vector>
#include "../include/command.h"
#include "../include/init_and_final_acts.h"
#include "../include/categories.h"

struct Postactions{
    Postactions()                   = default;
    Postactions(const Postactions&) = default;
    ~Postactions()                  = default;

    size_t keywords_postaction      = 0;
    size_t delimiters_postaction    = 0;
};

struct Indeces_from_tries{
    Indeces_from_tries()                          = default;
    Indeces_from_tries(const Indeces_from_tries&) = default;
    ~Indeces_from_tries()                         = default;

    //! The index of the identifier, which is the name of the scanner, in the prefix
    //! tree of identifiers.
    size_t scaner_name_idx          = 0;

    //! The index of the identifier, which is the name of the type of lexem codes,
    //! in the prefix tree of identifiers.
    size_t codes_type_name_idx      = 0;

    //! The index of the identifier, which is the code of lexem 'identifier', in the
    //! prefix tree of identifiers.
    size_t ident_name_idx           = 0;

    //! Index of a string literal with fields added to a type that describes
    //! information about the lexeme.
    size_t token_fields_idx         = 0;

    //! Index of a string literal with fields added to the scanner class.
    size_t class_members_idx        = 0;

    //! Index of a string literal with additions to the header file of
    //! the generated scaner.
    size_t header_additions_idx     = 0;

    //! Index of a string literal with additions to the implementation file of
    //! the generated scaner.
    size_t impl_additions_idx       = 0;

    //! The index of the identifier, which is the name of the lexeme info type.
    size_t lexem_info_name_idx      = 0;

    size_t write_action_name_idx;

    size_t mark_of_single_lined     = 0;
    size_t mark_of_multilined_begin = 0;
    size_t mark_of_multilined_end   = 0;
    size_t multilined_is_nested     = 0;
};

struct Automata_data{
    uint64_t       set_of_used_automata;
};

struct Collected_data{
    Collected_data()                      = default;
    Collected_data(const Collected_data&) = default;
    ~Collected_data()                     = default;

    Indeces_from_tries  indeces_;
    Postactions         postactions_;
    Init_and_final_acts acts_for_numbers_;
    Init_and_final_acts acts_for_strings_;
    Init_and_final_acts acts_for_idents_;
    Automata_data       aut_data_;

    bool                newline_is_lexem_;


    std::vector<size_t> codes_;  /* A vector consisting of indexes of identifiers
                                  * from an enumeration defining lexem codes. */

    size_t              last_code_val_     = 1; /* The last used numerical value
                                                 * of the lexem code. */
    std::vector<size_t> del_repres_; /* A vector consisting of indices of string
                                      * literals representing delimiters. */

    std::vector<size_t> kw_repres_;  /* A vector consisting of indices of string
                                      * literals representing keywords. */
    Command_buffer      numbers_regexp_;
    Command_buffer      strings_regexp_;
    Command_buffer      idents_regexp_;
    Category            begin_chars[6];
};
#endif