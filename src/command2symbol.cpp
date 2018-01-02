/*
    File:    command2symbol.cpp
    Created: 10 December 2017 at 14:24 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/command2symbol.h"

Symbol command2symbol(const Command com)
{
    Symbol result;
    switch(com.name){
        case Command_name::Char:
            result.kind       = Symbol_kind::Char;
            result.c          = com.c;
            break;
        case Command_name::Char_class:
            result.kind       = Symbol_kind::Char_class;
            result.idx_of_set = com.idx_of_set;
            break;
        case Command_name::Char_class_complement:
            result.kind       = Symbol_kind::Class_complement;
            result.idx_of_set = com.idx_of_set;
            break;
        default:
            ;
    }
    return result;
}
