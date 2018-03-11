/*
    File:    double_chars.h
    Created: 11 March 2018 at 14:56 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef DOUBLE_THE_CHAR_H
#define DOUBLE_THE_CHAR_H

#include <string>
#include <algorithm>
#include <cstddef>
#include "../include/operations_with_sets.h"

template<typename CharT,
         typename Traits    = std::char_traits<CharT>,
         typename Allocator = std::allocator<CharT>>
std::basic_string<CharT, Traits, Allocator>
    double_chars(const std::basic_string<CharT, Traits, Allocator>& str,
                 const std::set<CharT>&                             doubled_chars)

{
    using str_type = std::basic_string<CharT, Traits, Allocator>;
    using operations_with_sets::is_elem;
    size_t n = 0;
    for(CharT c : str){
        if(is_elem(c, doubled_chars)){
            ++n;
        }
    }
    if(!n){
        return str;
    }
    str_type result;
    result.reserve(str.length() + n);
    for(CharT c : str){
        result += c;
        if(is_elem(c, doubled_chars)){
            result += c;
        }
    }
    return result;
}
#endif