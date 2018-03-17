/*
    File:    constructing_info_to_testing_info.cpp
    Created: 14 March 2018 at 09:53 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/constructing_info_to_testing_info.h"

// struct Info_for_generating_testing_program{
//     std::string              lexem_info_name;
//     std::string              codes_type_name;
//     std::string              scaner_name;
//     std::vector<std::string> lexem_names;
// };

Info_for_generating_testing_program
    constructing_info_to_testing_info(const info_for_constructing::Info& info)
{
    Info_for_generating_testing_program result;
    auto& names            = info.names;
    result.lexem_info_name = names.lexem_info_name;
    result.codes_type_name = names.codes_type_name;
    result.scaner_name     = names.name_of_scaner_class;
    result.lexem_names     = info.lexem_codes_names;
    return result;
}