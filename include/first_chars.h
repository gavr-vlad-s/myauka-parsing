/*
    File:    first_chars.h
    Created: 13 January 2016 at 15:00 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef FIRST_CHARS_H
#define FIRST_CHARS_H

#include "../include/command.h"
#include "../include/categories.h"
#include "../include/trie_for_set.h"

Category first_chars(const Command_buffer& buf, const Trie_for_set_of_char32ptr& t);
#endif