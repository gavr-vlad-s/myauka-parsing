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
    };

    struct Comment_info{
        std::u32string mark_of_single_lined;
        std::u32string mark_of_multilined_begin;
        std::u32string mark_of_multilined_end;
        bool           multilined_is_nested     = false;
    };

    struct Info{
        size_t                        set_of_used_automata    = 0;
        size_t                        write_action_name_idx;
        Regexps                       regexps;
        Names                         names;
        std::vector<std::string>      lexem_codes_names;
        Automata_info                 automata_info;
        std::vector<std::string>      ifs_of_start_procs;
        std::vector<size_t>           del_repres;
        std::vector<size_t>           kw_repres;
        Trie_for_set_of_char32        char_cat;
        std::map<size_t, std::string> category_name;
        bool                          newline_is_lexem        = false;
        bool                          needed_Elem             = false;
        std::string                   keywords_postaction;
        std::string                   delimiters_postaction;
        std::string                   string_preactions;
        std::string                   string_postactions;
        std::string                   number_preactions;
        std::string                   number_postactions;
        std::string                   identifier_preactions;
        std::string                   identifier_postactions;
        Comment_info                  about_comments;
//         std::vector<Delimiter_with_code> delims;
//         Codes_info  codes_info;
    };
};
#endif