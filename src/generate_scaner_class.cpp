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
#include "../include/used_automaton.h"
#include "../include/belongs.h"
#include "../include/current_lexem_proc_builder.h"
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

static const std::string id_str_fmt[] = {
    "{0}"s,
    R"~(size_t    string_index;
        char32_t  c;
        {0})~"s,
    R"~(size_t    ident_index;
        {0})~"s,
    R"~(size_t    ident_index;
        size_t    string_index;
        char32_t  c;
        {0})~"s
};

static const std::string elem_str_fmt = R"~(#ifndef ELEM_H
#define ELEM_H
struct Elem{{
    /** A pointer to a string of characters that can be crossed. */
    char32_t*       symbols;
    /** A lexeme code. */
    {0} code;
    /** If the current character matches symbols[0], then the transition to the state
     *  first_state;
     *  if the current character matches symbols[1], then the transition to the state
     *  first_state + 1;
     *  if the current character matches symbols[2], then the transition to the state
     *  first_state + 2, and so on. */
    uint16_t        first_state;
}};
#endif)~"s;

static const std::string scaner_header_fmt = R"~(#ifndef {0}_H
#define {0}_H
#include <string>
#include "../include/abstract_scaner.h"
#include "../include/error_count.h"
#include "../include/location.h"
#include "../include/{1}.h"
{2}

class {3} : public Abstract_scaner<{4}>{{
public:
    {3}()           = default;
    {3}(const Location_ptr& location, const Errors_and_tries& et) :
        Abstract_scaner<{4}>(location, et) {{}}
    {3}(const {3}&) = default;
    virtual ~{3}()  = default;
    virtual {4} current_lexem();
private:
    {5}

    enum Automaton_name{{
{6}
    }};

    Automaton_name automaton; /* current automaton */

    typedef bool ({3}::*Automaton_proc)();
    /* This is the type of pointer to the member function that implements the
     * automaton that processes the lexeme. This function must return true if
     * the lexeme is not yet parsed, and false otherwise. */

    typedef void ({3}::*Final_proc)();
    /* And this is the type of the pointer to the member function that performs
     * the necessary actions in the event of an unexpected end of the lexeme. */

    static Automaton_proc procs[];
    static Final_proc     finals[];

    /* Lexeme processing functions: */
{7}

    /* Functions for performing actions in case of an
     * unexpected end of the token: */
{8}
}};
#endif)~"s;

static const std::string elem_file_name = "elem.h"s;

static const std::string h_extension = ".h"s;

static constexpr size_t INDENT_WIDTH = 4;

static constexpr size_t DOUBLE_INDENT_WIDTH = INDENT_WIDTH * 2;

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
    bool t1 = belongs(Id_aut, info.set_of_used_automata)     != 0;
    bool t2 = belongs(String_aut, info.set_of_used_automata) != 0;

    enum Id_Str{
        There_is_no_id_and_there_is_no_str, There_is_no_id_and_there_is_str,
        There_is_id_and_there_is_no_str,    There_is_id_and_there_is_str
    };

    Id_Str e = static_cast<Id_Str>(t1 * 2 + t2);
    result   = fmt::format(id_str_fmt[e], info.names.token_fields);
    return result;
}

static void write_file(const std::string& file_name, const std::string& contents)
{
    FILE* fptr = fopen(file_name.c_str(), "w");
    if(fptr){
        fputs(contents.c_str(), fptr);
        fclose(fptr);
    }else{
        printf("Could not create file %s.\n", file_name.c_str());
    }
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
    write_file(file_name, contents);
    return file_name;
}

static void generate_elem(const info_for_constructing::Info& info)
{
    std::string elem_contents = fmt::format(elem_str_fmt, info.names.codes_type_name);
    write_file(elem_file_name, elem_contents);
}

static std::string generate_automata_enum(const info_for_constructing::Info& info)
{
    std::string result;
    std::vector<std::string> automata_names;
    for(const auto& ai : info.automata_info){
        automata_names.push_back(ai.name);
    }

    Format f;
    f.indent                 = DOUBLE_INDENT_WIDTH;
    f.number_of_columns      = 3;
    f.spaces_between_columns = 1;

    result                   = string_list_to_columns(automata_names, f);
    return result;
}

static std::string generate_automata_proc_protos(const info_for_constructing::Info& info)
{
    std::string result;
    std::vector<std::string> automata_protos;
    for(const auto& ai : info.automata_info){
        automata_protos.push_back(ai.proc_proto);
    }

    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 2;
    f.spaces_between_columns = 1;

    result = string_list_to_columns(automata_protos, f, ';');
    return result;
}

static std::string
    generate_automata_final_procs_protos(const info_for_constructing::Info& info)
{
    std::string result;
    std::vector<std::string> final_procs_protos;
    for(const auto& ai : info.automata_info){
        final_procs_protos.push_back(ai.final_proc_proto);
    }

    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 2;
    f.spaces_between_columns = 1;

    result                   = string_list_to_columns(final_procs_protos, f, ';');
    return result;
}

static void generate_header(const info_for_constructing::Info& info)
{
    auto lexeme_info_file_name = generate_lexeme_info_file(info);

    Current_lexem_proc_builder curr_lexem_builder {info};
    auto curr_lexem_proc     = curr_lexem_builder.build();
    auto privates            = curr_lexem_proc.private_members +
                               info.names.class_members        + '\n';
    auto header_sentinel     = toupper_case(info.names.scaner_file_name_without_ext);
    auto automaton_enum_body = generate_automata_enum(info);
    auto automata_procs      = generate_automata_proc_protos(info);
    auto final_procs         = generate_automata_final_procs_protos(info);
    auto header_contents     = fmt::format(scaner_header_fmt,
                                           header_sentinel,
                                           lexeme_info_file_name,
                                           info.names.header_additions,
                                           info.names.name_of_scaner_class,
                                           info.names.lexem_info_name,
                                           privates,
                                           automaton_enum_body,
                                           automata_procs,
                                           final_procs);
    auto header_file_name    = info.names.scaner_file_name_without_ext + h_extension;
    write_file(header_file_name, header_contents);
}

void generate_scaner_class(const info_for_constructing::Info& info)
{
    generate_header(info);
    if(info.needed_Elem){
        generate_elem(info);
    }
}