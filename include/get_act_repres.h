/*
    File:    get_act_repres.h
    Created: 04 February 2017 at 18:29 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef GET_ACT_REPRES_H
#define GET_ACT_REPRES_H
#include <cstddef>
#include <cstdint>
#include <string>
#include <memory>
#include "../include/scope.h"
#include "../include/errors_and_tries.h"

/**
 *  \param [in] et    prefix trees of identifiers and strings
 *  \param [in] scope table of identifiers
 *  \param [in] i     index of action name in the prefix tree of identifiers
 *
 *  \return          string representation of action
 */
std::string get_act_repres(const Errors_and_tries& et,
                           const std::shared_ptr<Scope>& scope,
                           size_t                  i);
#endif