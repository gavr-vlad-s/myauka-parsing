/*
    File:    implement_automata.h
    Created: 10 January 2018 at 15:24 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef IMPLEMENT_AUTOMATA_H
#define IMPLEMENT_AUTOMATA_H
#include "../include/collected_data.h"
#include "../include/info_for_constructing.h"
#include "../include/errors_and_tries.h"
void implement_automata(info_for_constructing::Info& info,
                        const Collected_data&        d,
                        const Errors_and_tries&      et);
#endif