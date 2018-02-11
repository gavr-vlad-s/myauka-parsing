/*
    File:    u32string_to_commands.h
    Created: 01 February 2018 at 15:22 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
             vladimir.gavrilov@itmm.unn.ru
*/

#ifndef U32STRING_TO_COMMANDS_H
#define U32STRING_TO_COMMANDS_H
#include <string>
#include "../include/command.h"
Command_buffer u32string_to_commands(const std::u32string& str);
#endif