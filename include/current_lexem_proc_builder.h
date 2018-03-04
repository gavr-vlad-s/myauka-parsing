/*
    File:    current_lexem_proc_builder.h
    Created: 28 February 2018 at 07:11 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef CURRENT_LEXEM_PROC_BUILDER_H
#define CURRENT_LEXEM_PROC_BUILDER_H

#include <string>
#include "../include/info_for_constructing.h"

struct Current_lexem_proc_info{
    std::string implementation;
    std::string private_members;
};

class Current_lexem_proc_builder{
public:
    Current_lexem_proc_builder()                                  = default;
    Current_lexem_proc_builder(const Current_lexem_proc_builder&) = default;
    Current_lexem_proc_builder(const info_for_constructing::Info& info) : info_(info) {};
    ~Current_lexem_proc_builder()                                 = default;

    Current_lexem_proc_info build();
private:
    info_for_constructing::Info info_;

    Current_lexem_proc_info no_comments();
    Current_lexem_proc_info there_is_only_singlelined();
    Current_lexem_proc_info there_is_only_multilined();
};
#endif