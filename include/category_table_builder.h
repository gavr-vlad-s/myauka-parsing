/*
     File:    category_table_builder.h
     Created: 13 March 2018 at 12:04 Moscow time
     Author:  Гаврилов Владимир Сергеевич
     E-mails: vladimir.s.gavrilov@gmail.com
              gavrilov.vladimir.s@mail.ru
              gavvs1977@yandex.ru
*/

#ifndef CATEGORY_TABLE_BUILDER_H
#define CATEGORY_TABLE_BUILDER_H
#include <string>
#include "../include/info_for_constructing.h"
class Category_table_builder{
public:
    Category_table_builder()                              = default;
    Category_table_builder(const Category_table_builder&) = default;
    Category_table_builder(const info_for_constructing::Info& info) : info_(info) {};

    ~Category_table_builder()                             = default;


    std::string build();
private:
    info_for_constructing::Info info_;
};
#endif