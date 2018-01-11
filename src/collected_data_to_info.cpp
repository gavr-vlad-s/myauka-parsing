/*
    File:    collected_data_to_info.cpp
    Created: 08 January 2018 at 13:26 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include <cctype>
#include <string>
#include "../include/collected_data_to_info.h"
#include "../include/idx_to_string.h"
#include "../include/implement_automata.h"

using namespace info_for_constructing;

static Regexps data_to_regexps(const Collected_data& d)
{
    Regexps result;
    result.numbers = d.numbers_regexp_;
    result.strings = d.strings_regexp_;
    result.idents  = d.idents_regexp_;
    return result;
}

using namespace std::string_literals;
static const std::string default_codes_type_name = "Lexem_code"s;
static const std::string default_lexem_info_name = "Lexem_info"s;
static const std::string default_scaner_name     = "Scaner"s;

static Names data_to_names(const Collected_data&   d,
                           const Errors_and_tries& et)
{
    Names result;
    auto& idst                          = et.ids_trie;
    auto  indeces                       = d.indeces_;

    result.name_of_scaner_class         = idx_to_string(idst,
                                                        indeces.scaner_name_idx,
                                                        default_scaner_name);

    result.scaner_file_name_without_ext = result.name_of_scaner_class;
    for(char& c : result.scaner_file_name_without_ext){
        c = tolower(c);
    }

    result.codes_type_name              = idx_to_string(idst,
                                                        indeces.codes_type_name_idx,
                                                        default_codes_type_name);
    result.ident_name                   = idx_to_string(idst,
                                                        indeces.ident_name_idx);
    result.token_fields                 = idx_to_string(idst,
                                                        indeces.token_fields_idx);
    result.class_members                = idx_to_string(idst,
                                                        indeces.class_members_idx);
    result.header_additions             = idx_to_string(idst,
                                                        indeces.header_additions_idx);
    result.impl_additions               = idx_to_string(idst,
                                                        indeces.impl_additions_idx);
    result.lexem_info_name              = idx_to_string(idst,
                                                        indeces.lexem_info_name_idx,
                                                        default_lexem_info_name);
// !        std::string name_of_scaner_class;
// !        std::string scaner_file_name_without_ext;
// !        std::string codes_type_name;
// !        std::string ident_name;
// !        std::string token_fields;
// !        std::string class_members;
// !        std::string header_additions;
// !        std::string impl_additions;
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