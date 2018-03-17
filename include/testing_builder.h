/*
    File:    testing_builder.h
    Created: 14 March 2018 at 10:56 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef TESTING_BUILDER_H
#define TESTING_BUILDER_H
#include <memory>
#include "../include/info_for_generating_testing_program.h"
class Testing_builder{
public:
    Testing_builder();
    Testing_builder(const Testing_builder&) = default;
    Testing_builder(const Info_for_generating_testing_program& info);
    ~Testing_builder();

    void build();
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
//     Info_for_generating_testing_program info_;
//
//     std::string                         header_name_;
//     std::string                         impl_name_;
//
//     void
//     void generate_print_lexem_header();
//     void generate_print_lexem_impl();
//     void generate_main();
};
#endif