/*
    File:    info_for_generating_testing_program.h
    Created: 14 March 2018 at 09:28 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef INFO_FOR_GENERATING_TESTING_PROGRAM_H
#define INFO_FOR_GENERATING_TESTING_PROGRAM_H
#include <string>
#include <vector>
struct Info_for_generating_testing_program{
    std::string              lexem_info_name;
    std::string              codes_type_name;
    std::string              scaner_name;
    std::vector<std::string> lexem_names;
};
#endif