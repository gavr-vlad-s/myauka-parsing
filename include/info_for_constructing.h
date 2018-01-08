/*
    File:    info_for_constructing.h
    Created: 08 January 2018 at 11:18 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef INFO_FOR_CONSTRUCTING_H
#define INFO_FOR_CONSTRUCTING_H
#include <string>
#include <utility>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "../include/command.h"

namespace info_for_constructing{
    struct Regexps{
        Command_buffer numbers;
        Command_buffer strings;
        Command_buffer idents;
    };

//     using Lexem_with_code = std::pair<std::string, size_t>;
//     using Lexems          = std::vector<Lexem_with_code>;
//
//     struct Codes_info{
//     };

    struct Names{
        std::string name_of_scaner_class;
        std::string scaner_file_name_without_ext;
    };

    struct Info{
        Regexps     regexps;
        Names       names;
//         Codes_info  codes_info;
    };
};
#endif