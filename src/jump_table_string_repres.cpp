/*
    File:    jump_table_string_repres.cpp
    Created: 04 February 2017 at 11:43 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include <string>
#include <vector>
#include "../include/jump_table_string_repres.h"
#include "../include/list_to_columns.h"
#include "../include/idx_to_string.h"
#include "../include/char_conv.h"
// #include "../include/indent.h"
#include "../include/format.h"
#include "../include/print_char32.h"
#include "../include/join.h"

using namespace std::string_literals;

static const std::string init_table_template_fmt =
    R"~(static const State_for_char {0}[] = {{
    {1}
}};

)~"s;

static const std::string table_template_fmt      =
    R"~(static const Elem {0}[] = {{
    {1}
}};)~"s;

static const std::string table_entry_fmt         =
    R"~({{const_cast<char32_t*>(U"{0}"), {3}::{1}, {2}}})~"s;

//    "static const Elem " + table_name + "[] = {\n";

static std::string table_gen(const info_for_constructing::Info& info,
                             const Jumps_and_inits&             ji,
                             const std::string&                 table_name)
{
    std::string result;
    std::vector<std::string> del_jumps;
    for(const auto& j : ji.jumps){
        std::string entry = fmt::format(table_entry_fmt,
                                        u32string_to_utf8(j.jump_chars),
                                        info.lexem_codes_names[j.code],
                                        std::to_string(j.first_state),
                                        info.names.codes_type_name);
        del_jumps.push_back(entry);
    }
    result = fmt::format(table_template_fmt,
                         table_name,
                         join(del_jumps.begin(), del_jumps.end(), ",\n"s ));
    return result;
}

static std::string init_table(const Jumps_and_inits& ji,
                              const std::string&     init_table_name)
{
    std::string result;
    std::vector<std::string> init_table_elems;
    for(const auto e : ji.init_table){
        std::string temp = '{'   + std::to_string(e.first) +
                           ", U" + show_char32(e.second)   + '}';
        init_table_elems.push_back(temp);
    }
    result = fmt::format(init_table_template_fmt,
                         init_table_name,
                         join(init_table_elems.begin(), init_table_elems.end(), ", "s));
    return result;
}

std::string jump_table_string_repres(const info_for_constructing::Info& info,
                                     const Jumps_and_inits&             ji,
                                     const std::string&                 table_name,
                                     const std::string&                 init_table_name)
{
    auto result =  init_table(ji, init_table_name);
    result      += table_gen(info, ji, table_name);
    return result;
}