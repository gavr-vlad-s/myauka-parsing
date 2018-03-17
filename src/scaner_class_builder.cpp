/*
    File:    scaner_class_builder.cpp
    Created: 12 March 2018 at 16:31 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include <string>
#include <fstream>
#include <vector>
#include "../include/scaner_class_builder.h"
#include "../include/current_lexem_proc_builder.h"
#include "../include/conv_case.h"
#include "../include/format.h"
#include "../include/list_to_columns.h"
#include "../include/used_automaton.h"
#include "../include/belongs.h"
#include "../include/category_table_builder.h"
#include "../include/automata_impl_collector.h"

using namespace std::string_literals;

struct Scaner_class_builder::Impl{
public:
    Impl()            = default;
    ~Impl()           = default;
    Impl(const Impl&) = default;
    Impl(const info_for_constructing::Info& info) : info_(info) {};

    void build();
private:
    info_for_constructing::Info info_;
    Current_lexem_proc_info     lexeme_proc_info_;
    std::string                 lexeme_info_file_name_;
    std::string                 header_name_;
    std::string                 impl_name_;
    bool                        needed_Elem_;

    void prepare();
    void generate_elem();
    void generate_lexeme_info_file();
    void generate_scaner_header();
    void generate_scaner_impl();

    std::string enum_body();
    std::string union_body();

    std::string generate_automata_enum();
    std::string generate_automata_proc_protos();
    std::string generate_automata_final_procs_protos();

    std::string procs();
    std::string final_procs();
};

Scaner_class_builder::~Scaner_class_builder() = default;

Scaner_class_builder::Scaner_class_builder() : impl_(std::make_unique<Impl>()) {}

Scaner_class_builder::Scaner_class_builder(const info_for_constructing::Info& info) :
    impl_(std::make_unique<Impl>(info)) {}

void Scaner_class_builder::build()
{
    impl_->build();
}

static const std::string elem_file_name      = "elem.h"s;
static const std::string h_fmt               = "{0}.h"s;
static const std::string cpp_fmt             = "{0}.cpp"s;
static constexpr size_t  INDENT_WIDTH        = 4;
static constexpr size_t  DOUBLE_INDENT_WIDTH = INDENT_WIDTH * 2;

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

static const std::string scaner_header_fmt = R"~(#ifndef {0}_H
#define {0}_H
#include <string>
#include "../include/abstract_scaner.h"
#include "../include/error_count.h"
#include "../include/location.h"
#include "../include/{1}"
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

static const std::string impl_fmt_without_elems =
R"~(#include <set>
#include <string>
#include <vector>
#include "../include/{0}"
#include "../include/get_init_state.h"
#include "../include/search_char.h"
#include "../include/belongs.h"
#include "../include/operations_with_sets.h"
#include "../include/mysize.h"
{1}

{2}::::Automaton_proc {2}::procs[]  = {{
{3}
}};

{2}::Final_proc       {2}::finals[] = {{
{4}
}};

{5}

{6}

{7}
)~"s;

static const std::string impl_fmt_with_elems =
R"~(#include <set>
#include <string>
#include <vector>
#include "../include/{0}"
#include "../include/get_init_state.h"
#include "../include/search_char.h"
#include "../include/belongs.h"
#include "../include/operations_with_sets.h"
#include "../include/mysize.h"
#include "../include/elem.h"
{1}

{2}::::Automaton_proc {2}::procs[]  = {{
{3}
}};

{2}::Final_proc       {2}::finals[] = {{
{4}
}};

{5}

{6}

{7}
)~"s;

void Scaner_class_builder::Impl::build()
{
    prepare();
    generate_elem();
    generate_lexeme_info_file();
    generate_scaner_header();
    generate_scaner_impl();
}

void Scaner_class_builder::Impl::prepare()
{
    auto& names            = info_.names;
    lexeme_info_file_name_ = fmt::format(h_fmt, tolower_case(names.lexem_info_name));
    header_name_           = fmt::format(h_fmt, names.scaner_file_name_without_ext);
    impl_name_             = fmt::format(cpp_fmt, names.scaner_file_name_without_ext);
    needed_Elem_           = info_.needed_Elem;

    Current_lexem_proc_builder curr_lexem_builder {info_};
    lexeme_proc_info_      = curr_lexem_builder.build();
}

void Scaner_class_builder::Impl::generate_elem()
{
    if(needed_Elem_){
        auto contents = fmt::format(elem_str_fmt, info_.names.codes_type_name);
        std::ofstream fout {elem_file_name};
        fout << contents;
        fout.close();
    }
}

void Scaner_class_builder::Impl::generate_lexeme_info_file()
{
    auto& names            = info_.names;
    auto& lexeme_info_name = names.lexem_info_name;
    auto  sentinel         = toupper_case(lexeme_info_name);
    auto  codes_body       = enum_body();
    auto  fields           = union_body();
    auto  contents         = fmt::format(lexeme_info_file_fmt,
                                         sentinel,
                                         names.codes_type_name,
                                         codes_body,
                                         lexeme_info_name,
                                         fields);
    std::ofstream fout {lexeme_info_file_name_};
    fout << contents;
    fout.close();
}

std::string Scaner_class_builder::Impl::enum_body()
{
    std::string result;
    auto& lexem_codes_names  = info_.lexem_codes_names;
    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 3;
    f.spaces_between_columns = 0;
    result                   = string_list_to_columns(lexem_codes_names, f);
    return result;
}

std::string Scaner_class_builder::Impl::union_body()
{
    std::string result;
    bool t1 = belongs(Id_aut, info_.set_of_used_automata)     != 0;
    bool t2 = belongs(String_aut, info_.set_of_used_automata) != 0;

    enum Id_Str{
        There_is_no_id_and_there_is_no_str, There_is_no_id_and_there_is_str,
        There_is_id_and_there_is_no_str,    There_is_id_and_there_is_str
    };

    Id_Str e = static_cast<Id_Str>(t1 * 2 + t2);
    result   = fmt::format(id_str_fmt[e], info_.names.token_fields);
    return result;
}

std::string Scaner_class_builder::Impl::generate_automata_enum()
{
    std::string result;
    std::vector<std::string> automata_names;
    for(const auto& ai : info_.automata_info){
        automata_names.push_back(ai.name);
    }

    Format f;
    f.indent                 = DOUBLE_INDENT_WIDTH;
    f.number_of_columns      = 3;
    f.spaces_between_columns = 1;

    result                   = string_list_to_columns(automata_names, f);
    return result;
}

std::string Scaner_class_builder::Impl::generate_automata_proc_protos()
{
    std::string result;
    std::vector<std::string> automata_protos;
    for(const auto& ai : info_.automata_info){
        automata_protos.push_back(ai.proc_proto);
    }

    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 2;
    f.spaces_between_columns = 1;

    result = string_list_to_columns(automata_protos, f, ';');
    return result;
}

std::string Scaner_class_builder::Impl::generate_automata_final_procs_protos()
{
    std::string result;
    std::vector<std::string> final_procs_protos;
    for(const auto& ai : info_.automata_info){
        final_procs_protos.push_back(ai.final_proc_proto);
    }

    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 2;
    f.spaces_between_columns = 1;

    result                   = string_list_to_columns(final_procs_protos, f, ';');
    return result;
}

void Scaner_class_builder::Impl::generate_scaner_header()
{
    auto& names              = info_.names;
    auto  privates           = lexeme_proc_info_.private_members +
                               names.class_members               + '\n';
    auto header_sentinel     = toupper_case(names.scaner_file_name_without_ext);
    auto automaton_enum_body = generate_automata_enum();
    auto automata_procs      = generate_automata_proc_protos();
    auto final_procs         = generate_automata_final_procs_protos();
    auto header_contents     = fmt::format(scaner_header_fmt,
                                           header_sentinel,
                                           lexeme_info_file_name_,
                                           names.header_additions,
                                           names.name_of_scaner_class,
                                           names.lexem_info_name,
                                           privates,
                                           automaton_enum_body,
                                           automata_procs,
                                           final_procs);
    std::ofstream fout {header_name_};
    fout << header_contents;
    fout.close();
}

std::string Scaner_class_builder::Impl::procs()
{
    std::string result;
    std::vector<std::string> procs_list;
    for(const auto& ai : info_.automata_info){
        procs_list.push_back(ai.proc_ptr);
    }

    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 2;
    f.spaces_between_columns = 1;

    result                   = string_list_to_columns(procs_list, f);
    return result;
}

std::string Scaner_class_builder::Impl::final_procs()
{
    std::string result;
    std::vector<std::string> procs_list;
    for(const auto& ai : info_.automata_info){
        procs_list.push_back(ai.final_proc_ptr);
    }

    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 2;
    f.spaces_between_columns = 1;

    result                   = string_list_to_columns(procs_list, f);
    return result;
}

void Scaner_class_builder::Impl::generate_scaner_impl()
{
    auto  impl_format = needed_Elem_ ? impl_fmt_with_elems : impl_fmt_without_elems;
    auto& names       = info_.names;
    auto  procs_table = procs();
    auto  finals      = final_procs();

    Category_table_builder cat_table_builder {info_};
    auto  cat_table   = cat_table_builder.build();

    Automata_impl_collector impl_collector {info_};
    auto  impls       = impl_collector.collect();

    auto  contents    = fmt::format(impl_format,
                                    header_name_,
                                    names.impl_additions,
                                    names.name_of_scaner_class,
                                    procs_table,
                                    finals,
                                    cat_table,
                                    impls,
                                    lexeme_proc_info_.implementation);
    std::ofstream fout {impl_name_};
    fout << contents;
    fout.close();
}