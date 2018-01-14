/*
    File:    info_for_constructing.h
    Created: 08 January 2018 at 11:18 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef INFO_FOR_CONSTRUCTING_H
#define INFO_FOR_CONSTRUCTING_H
#include <string>
#include <utility>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <map>
#include "../include/command.h"
#include "../include/automaton_constructing_info.h"
#include "../include/trie_for_set.h"

namespace info_for_constructing{
    struct Regexps{
        Command_buffer numbers;
        Command_buffer strings;
        Command_buffer idents;
    };

//     using Lexem_with_code = std::pair<std::string, size_t>;
//     using Lexems          = std::vector<Lexem_with_code>;
//
//     struct Codes_info{
//     };

    struct Names{
        std::string name_of_scaner_class;
        std::string scaner_file_name_without_ext;
        std::string codes_type_name;
        std::string ident_name;
        std::string token_fields;
        std::string class_members;
        std::string header_additions;
        std::string impl_additions;
        std::string lexem_info_name;
//         std::string fields_for_comments_handling;
    };

    using Delimiter_with_code = std::pair<std::u32string, size_t>;
    struct Info{
        size_t                           set_of_used_automata = 0;
        Regexps                          regexps;
        Names                            names;
        std::vector<std::string>         lexem_codes_names;
        Automata_info                    automata_info;
        std::vector<std::string>         ifs_of_start_procs;
        std::vector<size_t>              del_repres;
        Trie_for_set_of_char32           char_cat;
        std::map<size_t, std::string>    category_name;
        bool                             newline_is_lexem     = false;
        std::string                      keywords_postaction;
        std::string                      delimiters_postaction;
//         std::vector<Delimiter_with_code> delims;
//         Codes_info  codes_info;
    };
};


//     size_t scaner_name_idx          = 0;
//
//     //! The index of the identifier, which is the name of the type of lexem codes,
//     //! in the prefix tree of identifiers.
//     size_t codes_type_name_idx      = 0;
//
//     //! The index of the identifier, which is the code of lexem 'identifier', in the
//     //! prefix tree of identifiers.
//     size_t ident_name_idx           = 0;
//
//     //! Index of a string literal with fields added to a type that describes
//     //! information about the lexeme.
//     size_t token_fields_idx         = 0;
//
//     //! Index of a string literal with fields added to the scanner class.
//     size_t class_members_idx        = 0;
//
//     //! Index of a string literal with additions to the header file of
//     //! the generated scaner.
//     size_t header_additions_idx     = 0;
//
//     //! Index of a string literal with additions to the implementation file of
//     //! the generated scaner.
//     size_t impl_additions_idx       = 0;
//
//     //! The index of the identifier, which is the name of the lexeme info type.
//     size_t lexem_info_name_idx      = 0;
//
//     size_t write_action_name_idx;
//
//     size_t mark_of_single_lined     = 0;
//     size_t mark_of_multilined_begin = 0;
//     size_t mark_of_multilined_end   = 0;
//     size_t multilined_is_nested     = 0;

#endif