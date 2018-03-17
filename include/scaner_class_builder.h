/*
    File:    scaner_class_builder.h
    Created: 12 March 2018 at 16:19 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef SCANER_CLASS_BUILDER_H
#define SCANER_CLASS_BUILDER_H
#include <memory>
#include "../include/info_for_constructing.h"
class Scaner_class_builder{
public:
    Scaner_class_builder();
    Scaner_class_builder(const Scaner_class_builder&) = default;
    ~Scaner_class_builder();
    Scaner_class_builder(const info_for_constructing::Info& info);

    void build();
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
#endif