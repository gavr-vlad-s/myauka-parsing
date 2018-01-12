/*
    File:    join.h
    Created: 12 January 2018 at 09:43 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef JOIN_H
#define JOIN_H
#include <string>
template<typename BidirectionalIt,
         typename CharT,
         typename Traits    = std::char_traits<CharT>,
         typename Allocator = std::allocator<CharT>>
std::basic_string<CharT, Traits, Allocator> join(BidirectionalIt first,
                                                 BidirectionalIt last,
                                                 const std::basic_string<CharT, Traits, Allocator>& separator)
{
    using str_type = std::basic_string<CharT, Traits, Allocator>;
    str_type result;
    if(first == last){
        return result;
    }
    auto pre_last = --last;
    if(first == pre_last){
        return *first;
    }
    for(auto it = first; it != pre_last; ++it){
        result += *it + separator;
    }
    result += *pre_last;
    return result;
}
#endif