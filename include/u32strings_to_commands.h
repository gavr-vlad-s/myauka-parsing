/*
    File:    u32strings_to_commands.h
    Created: 05 February 2017 at 17:52 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
             vladimir.gavrilov@itmm.unn.ru
*/
#ifndef U32STRINGS_TO_COMMANDS_H
#define U32STRINGS_TO_COMMANDS_H
#include <vector>
#include <string>
#include "../include/command.h"
#include "../include/u32string_to_commands.h"
#include "../include/regexp1_or_regexp2.h"
template<class InputIt>
Command_buffer u32strings_to_commands(InputIt first, InputIt last)
{
    Command_buffer result;
    for(auto it = first; it != last; ++it){
        auto current_commands = u32string_to_commands(*it);
        result                = regexp1_or_regexp2(result, current_commands);
    }
    return result;
}
#endif