/*
    File:    collected_data_to_info.cpp
    Created: 08 January 2018 at 13:26 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include <cctype>
#include "../include/collected_data_to_info.h"
#include "../include/idx_to_string.h"
#include "../include/implement_automata.h"

using namespace info_for_constructing;

Regexps data_to_regexps(const Collected_data& d)
{
    Regexps result;
    result.numbers = d.numbers_regexp_;
    result.strings = d.strings_regexp_;
    result.idents  = d.idents_regexp_;
    return result;
}

Names data_to_names(const Collected_data&   d,
                    const Errors_and_tries& et)
{
    Names result;
    auto& idst                          = et.ids_trie;
    auto  indeces                       = d.indeces_;

    result.name_of_scaner_class         = idx_to_string(idst, indeces.scaner_name_idx);

    result.scaner_file_name_without_ext = result.name_of_scaner_class;
    for(char& c : scaner_file_name_without_ext){
        c = tolower(c);
    }

    result.codes_type_name              = idx_to_string(idst, indeces.codes_type_name_idx);
    result.ident_name                   =;
    result.token_fields                 =;
    result.class_members                =;
    result.header_additions             =;
    result.impl_additions               =;
    result.lexem_info_name              =;
// !        std::string name_of_scaner_class;
// !        std::string scaner_file_name_without_ext;
// !        std::string codes_type_name;
//         std::string ident_name;
//         std::string token_fields;
//         std::string class_members;
//         std::string header_additions;
//         std::string impl_additions;
//         std::string lexem_info_name;

    return result;
}

info_for_constructing::Info collected_data_to_info(const Collected_data&   d,
                                                   const Errors_and_tries& et)
{
    info_for_constructing::Info result;

    result.regexps = data_to_regexps(d);
    for(const size_t c : d.codes_){
        result.lexem_codes_names.push_back(idx_to_string(et.ids_trie,c));
    }

    result.names = data_to_names(d, et);

    implement_automata(result, d, et);

    return result;
}