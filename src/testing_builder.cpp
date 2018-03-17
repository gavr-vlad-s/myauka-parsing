/*
    File:    testing_builder.cpp
    Created: 14 March 2018 at 11:28 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include <string>
#include <fstream>
#include "../include/testing_builder.h"
#include "../include/conv_case.h"
#include "../include/format.h"
#include "../include/list_to_columns.h"

using namespace std::string_literals;

struct Testing_builder::Impl{
public:
    Impl()            = default;
    Impl(const Impl&) = default;
    Impl(const Info_for_generating_testing_program& info) : info_(info) {};
    ~Impl()           = default;

    void build();
private:
    Info_for_generating_testing_program info_;
    std::string                         main_prog_name_;
    std::string                         lexem_file_name_without_ext_;
    std::string                         main_prog_name_without_suffix_;

    void prepare();
    void generate_print_lexem_header();
    void generate_print_lexem_impl();
    void generate_main();

    std::string names_as_string_literals();
};

Testing_builder::Testing_builder() : impl_(std::make_unique<Impl>()) {}

Testing_builder::Testing_builder(const Info_for_generating_testing_program& info) :
    impl_(std::make_unique<Impl>(info)) {}

Testing_builder::~Testing_builder() = default;

void Testing_builder::build()
{
    impl_->build();
}

void Testing_builder::Impl::build()
{
    prepare();
    generate_print_lexem_header();
    generate_print_lexem_impl();
    generate_main();
}

static const std::string print_header           = "print_lexem.h"s;
static const std::string print_impl             = "print_lexem.cpp"s;
static const std::string prog_name_fmt          = "{0}-test.cpp"s;

static const std::string print_lexem_header_fmt =
R"~(#ifndef PRINT_LEXEM_H
#define PRINT_LEXEM_H
#include <memory>
#include "../include/{0}.h"
#include "../include/{1}.h"
void print_lexem(const {2}& li);
void test_scaner(std::shared_ptr<{3}>& sc);
#endif)~"s;

static const std::string print_lexem_impl_fmt =
R"~(#include <cstdio>
#include <cstddef>
#include "../include/print_lexem.h"

static const char* lexem_names[] = {{
{0}
}};

void print_lexem(const {1}& li)
{{
    printf("%s\n", lexem_names[static_cast<size_t>(li.code)]);
}}

void test_scaner(std::shared_ptr<{2}>& sc)
{{
    {1} li;
    do{{
        li = sc->current_lexem();
        print_lexem(li);
    }}while(li.code != {3}::None);
}}
)~"s;

static const std::string quoted_fmt    = R"x("{0}")x";

static const std::string main_prog_fmt =
R"x(#include <cstdlib>
#include <cstdio>
#include <memory>
#include "../include/fsize.h"
#include "../include/error_count.h"
#include "../include/location.h"
#include "../include/trie.h"
#include "../include/{0}.h"
#include "../include/char_conv.h"
#include "../include/print_lexem.h"
#include "../include/errors_and_tries.h"
#include "../include/file_contents.h"
#include "../include/char_trie.h"

/* Function that opens a file with text. Returns a string with text if the file was
 * opened and the file size is not zero, and an empty string otherwise. */
static std::u32string get_processed_text(const char* name)
{{
    auto contents = get_contents(name);
    auto str      = contents.second;
    switch(contents.first){{
        case Get_contents_return_code::Normal:
            if(!str.length()){{
                puts("File length is equal to zero.");
                return std::u32string();
            }}else{{
                return utf8_to_u32string(str.c_str());
            }}
            break;

        case Get_contents_return_code::Impossible_open:
            puts("Unable to open file.");
            return std::u32string();

        case Get_contents_return_code::Read_error:
            puts("Error reading file.");
            return std::u32string();
    }}
    return std::u32string();
}}

enum class Myauka_exit_codes{{
    Success, No_args
}};

int main(int argc, char** argv)
{{
    if(1 == argc){{
        puts("Arguments are not specified.");
        return static_cast<int>(Myauka_exit_codes::No_args);
    }}

    std::u32string t = get_processed_text(argv[1]);
    if(t.length()){{
        char32_t* p   = const_cast<char32_t*>(t.c_str());
        auto      loc = std::make_shared<Location>(p);

        Errors_and_tries etr;
        etr.ec        = std::make_shared<Error_count>();
        etr.ids_trie  = std::make_shared<Char_trie>();
        etr.strs_trie = std::make_shared<Char_trie>();
        auto sc       = std::make_shared<{1}>(loc, etr);

        test_scaner(sc);
    }}

    return static_cast<int>(Myauka_exit_codes::Success);
}}
)x";
void Testing_builder::Impl::prepare()
{
    lexem_file_name_without_ext_   = tolower_case(info_.lexem_info_name);
    main_prog_name_without_suffix_ = tolower_case(info_.scaner_name);
    main_prog_name_                = fmt::format(prog_name_fmt,
                                                 main_prog_name_without_suffix_);
}

void Testing_builder::Impl::generate_print_lexem_header()
{
    auto contents = fmt::format(print_lexem_header_fmt,
                                lexem_file_name_without_ext_,
                                main_prog_name_without_suffix_,
                                info_.lexem_info_name,
                                info_.scaner_name);
    std::ofstream fout {print_header};
    fout << contents;
    fout.close();
}

static constexpr size_t INDENT_WIDTH = 4;

std::string Testing_builder::Impl::names_as_string_literals()
{
    std::string              result;
    std::vector<std::string> names;
    for(const auto& s : info_.lexem_names){
        auto temp = fmt::format(quoted_fmt, s);
        names.push_back(temp);
    }

    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 3;
    f.spaces_between_columns = 1;

    result                   = string_list_to_columns(names, f);
    return result;
}

void Testing_builder::Impl::generate_print_lexem_impl()
{
    auto table_body = names_as_string_literals();
    auto contents   = fmt::format(print_lexem_impl_fmt,
                                  table_body,
                                  info_.lexem_info_name,
                                  info_.scaner_name,
                                  info_.codes_type_name);
    std::ofstream fout {print_impl};
    fout << contents;
    fout.close();
}

void Testing_builder::Impl::generate_main()
{
    auto contents = fmt::format(main_prog_fmt,
                                main_prog_name_without_suffix_,
                                info_.scaner_name);
    std::ofstream fout {main_prog_name_};
    fout << contents;
    fout.close();
}