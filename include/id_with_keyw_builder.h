/*
    File:    id_with_keyw_builder.h
    Created: 14 February 2018 at 08:11 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef ID_WITH_KEYW_BUILDER_H
#define ID_WITH_KEYW_BUILDER_H

// #include <string>
// #include <vector>
#include <memory>
#include "../include/scope.h"
#include "../include/errors_and_tries.h"
#include "../include/info_for_constructing.h"
#include "../include/trie_for_set.h"

class Id_with_keyw_builder{
public:
    Id_with_keyw_builder();

    Id_with_keyw_builder(const Trie_for_set_of_char32ptr& sets,
                         const Errors_and_tries&          et,
                         const std::shared_ptr<Scope>&    scope);

    Id_with_keyw_builder(const Id_with_keyw_builder&) = default;
    ~Id_with_keyw_builder();                           // = default;

    void build(info_for_constructing::Info& info);
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;

    void all_keywords_are_idents_case(info_for_constructing::Info& info);
    void some_keywords_are_not_idents_case(info_for_constructing::Info& info);
    void all_keywords_are_not_idents_case(info_for_constructing::Info& info);
//     Jumps_and_inits keywords_jumps(const info_for_constructing::Info&  info);
};
#endif