/*
    File:    generate_scaner_class.cpp
    Created: 07 March 2018 at 10:14 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include <vector>
#include <string>
#include <cstdio>
#include "../include/generate_scaner_class.h"
#include "../include/conv_case.h"
#include "../include/format.h"
#include "../include/list_to_columns.h"

using namespace std::string_literals;

static const std::string lexeme_info_file_fmt =
R"~(#ifndef {0}_H
#define {0}_H
enum class {1} : unsigned short {{
{2}
}};

struct {3}{{
    {1} codes;
    union{{
        {4}
    }}
}};
#endif)~"s;

static const std::string h_extension = ".h"s;

static constexpr size_t INDENT_WIDTH = 4;

static std::string enum_body(const info_for_constructing::Info& info)
{
    std::string result;
    auto& lexem_codes_names  = info.lexem_codes_names;
    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 3;
    f.spaces_between_columns = 0;
    result                   = string_list_to_columns(lexem_codes_names, f);
    return result;
}

static std::string union_body(const info_for_constructing::Info& info)
{
    std::string result;
    return result;
}

static std::string generate_lexeme_info_file(const info_for_constructing::Info& info)
{
    std::string file_name;
    auto&       names            = info.names;
    auto&       lexeme_info_name = names.lexem_info_name;
    auto        sentinel         = toupper_case(lexeme_info_name);
    auto        contents         = fmt::format(lexeme_info_file_fmt,
                                               sentinel,
                                               names.codes_type_name,
                                               enum_body(info),
                                               lexeme_info_name,
                                               union_body(info));

    file_name                    = tolower_case(lexeme_info_name) + h_extension;
    return file_name;
}

void generate_scaner_class(const info_for_constructing::Info& info)
{
    auto lexeme_info_file_name = generate_lexeme_info_file(info);
}