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
#include <set>
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

static const std::set<char32_t> spaces = {
    0,  1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,
    17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32
};

static const std::string spaces_name             = "SPACES"s;

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

    return result;
}

//         std::string                      ;
//         std::string                      ;

static void collect_pre_and_postaction(info_for_constructing::Info& info,
                                       const Collected_data&        d,
                                       const Errors_and_tries&      et)
{
    auto& strst                 = et.strs_trie;
    info.keywords_postaction    = idx_to_string(strst,
                                                d.postactions_.keywords_postaction);
    info.delimiters_postaction  = idx_to_string(strst,
                                                d.postactions_.delimiters_postaction);
    info.string_preactions      = idx_to_string(strst,
                                                d.acts_for_strings_.init_acts);
    info.string_postactions     = idx_to_string(strst,
                                                d.acts_for_strings_.fin_acts);
    info.number_preactions      = idx_to_string(strst,
                                                d.acts_for_numbers_.init_acts);
    info.number_postactions     = idx_to_string(strst,
                                                d.acts_for_numbers_.fin_acts);
    info.identifier_preactions  =  idx_to_string(strst,
                                                 d.acts_for_idents_.init_acts);
    info.identifier_postactions =  idx_to_string(strst,
                                                 d.acts_for_idents_.fin_acts);
}

static Comment_info collect_comment_info(const Collected_data&   d,
                                         const Errors_and_tries& et)
{
    Comment_info result;
    result.mark_of_single_lined     =
        et.strs_trie->get_string(d.indeces_.mark_of_multilined_begin);
    result.mark_of_multilined_begin =
        et.strs_trie->get_string(d.indeces_.mark_of_multilined_begin);
    result.mark_of_multilined_end   =
        et.strs_trie->get_string(d.indeces_.mark_of_multilined_end);
    result.multilined_is_nested     = d.indeces_.multilined_is_nested;
    return result;
}

info_for_constructing::Info
    collected_data_to_info(const Collected_data&            d,
                           const Errors_and_tries&          et,
                           const Trie_for_set_of_char32ptr& sets_from_automata,
                           const std::shared_ptr<Scope>&    scope)
{
    info_for_constructing::Info result;

    result.regexps = data_to_regexps(d);
    for(const size_t c : d.codes_){
        result.lexem_codes_names.push_back(idx_to_string(et.ids_trie,c));
    }

    result.names = data_to_names(d, et);
    result.set_of_used_automata      = d.aut_data_.set_of_used_automata;
    result.newline_is_lexem          = d.newline_is_lexem_;

    size_t sp_indeces                = result.char_cat.insertSet(spaces);
    result.category_name[sp_indeces] = spaces_name;

    result.del_repres                = d.del_repres_;
    result.kw_repres                 = d.kw_repres_;

    collect_pre_and_postaction(result, d, et);

    result.write_action_name_idx     = d.indeces_.write_action_name_idx;

    result.about_comments            = collect_comment_info(d, et);
    implement_automata(result, sets_from_automata, et, scope);

    return result;
}