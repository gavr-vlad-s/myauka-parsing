/*
    File:    regexp1_or_regexp2.h
    Created: 05 February 2017 at 17:37 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
             vladimir.gavrilov@itmm.unn.ru
*/
#ifndef REGEXP1_OR_REGEXP2_H
#define REGEXP1_OR_REGEXP2_H
#include "../include/command.h"
/**
 *  \brief Returns commands corresponding to regexp a | b.
 *  \param [in] a The commands that define the regular expression a.
 *  \param [in] b The commands that define the regular expression b.
 *  \return       The commands that define the regular expression a | b.
 */
Command_buffer regexp1_or_regexp2(const Command_buffer& a, const Command_buffer& b);
#endif