/*
    File:    collected_data_to_info.h
    Created: 08 January 2018 at 13:20 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef COLLECTED_DATA_TO_INFO_H
#define COLLECTED_DATA_TO_INFO_H
#include <memory>
#include "../include/collected_data.h"
#include "../include/info_for_constructing.h"
#include "../include/errors_and_tries.h"
#include "../include/scope.h"
info_for_constructing::Info collected_data_to_info(const Collected_data&         d,
                                                   const Errors_and_tries&       et,
                                                   const std::shared_ptr<Scope>& scope);
#endif