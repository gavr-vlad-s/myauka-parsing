/*
    File:    verify_collected_data.h
    Created: 10 December 2017 at 10:22 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef VERIFY_COLLECTED_DATA_H
#define VERIFY_COLLECTED_DATA_H
#include "../include/collected_data.h"
#include "../include/errors_and_tries.h"
#include "../include/trie_for_set.h"
bool verify(Collected_data&                  data,
            Errors_and_tries&                et,
            const Trie_for_set_of_char32ptr& t);
#endif