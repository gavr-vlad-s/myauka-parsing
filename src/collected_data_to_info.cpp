/*
    File:    collected_data_to_info.cpp
    Created: 08 January 2018 at 13:26 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/collected_data_to_info.h"

using namespace info_for_constructing;

Regexps data_to_regexps(const Collected_data& d)
{
    Regexps result;
    result.numbers = d.numbers_regexp_;
    result.strings = d.strings_regexp_;
    result.idents  = d.idents_regexp_;
    return result;
}

info_for_constructing::Info collected_data_to_info(const Collected_data& d)
{
    info_for_constructing::Info result;

    result.regexps = data_to_regexps(d);

    return result;
}