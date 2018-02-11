/*
    File:    u32string_to_commands.cpp
    Created: 05 February 2017 at 17:56 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
             vladimir.gavrilov@itmm.unn.ru
*/

#include "../include/u32string_to_commands.h"

Command_buffer u32string_to_commands(const std::u32string& str){
    Command_buffer result;

    if(str.empty()){
        return result;
    }

    for(char32_t ch : str){
        Command command;
        command.action_name        = 0;
        command.name               = Command_name::Char;
        command.c                  = ch;
        result.push_back(command);
    }

    if(str.length() != 1){
        Command command;
        command.name               = Command_name::Multiconcat;
        command.args.first         = 0;
        command.args.second        = result.size() - 1;
        result.push_back(command);
    }

    return result;
}