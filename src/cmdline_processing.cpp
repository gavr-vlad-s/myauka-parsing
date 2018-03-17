/*
    File:    cmdline_processing.cpp
    Created: 13 March 2018 at 18:26 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include <cstdio>
#include <map>
#include <string>
#include <memory>
#include "../include/cmdline_processing.h"
#include "../include/get_processed_text.h"
#include "../include/location.h"
#include "../include/main_parser.h"
#include "../include/move_by_ext_from_curr_dir.h"
#include "../include/testing_builder.h"

static const char* help_str =
    R"~(Usage: myauka [option] file
Options:
    --help    Display this text.
    --version Display version information.
    --test    Generate testing program.)~";

static const char* version_str =
    R"~(Myauka, lexical analyzer generator, v.2.0.0
Copyright (c) Gavrilov Vladimir Sergeevich 2016--2018
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.)~";

static void version(){
    puts(version_str);
}

static void help(){
    puts(help_str);
}

enum class Option_kind{
    Help, Version, Test
};

static const std::map<std::string, Option_kind> option = {
    {"--version", Option_kind::Version},
    {"--help",    Option_kind::Help},
    {"--test",    Option_kind::Test}
};

Myauka_exit_codes only_compile(const char* file_name)
{
    Myauka_exit_codes exit_code = Myauka_exit_codes::Success;
    std::u32string    text      = get_processed_text(file_name);

    if(!text.length()){
        return Myauka_exit_codes::File_processing_error;
    }

    char32_t* p                 = const_cast<char32_t*>(text.c_str());
    auto      loc               = std::make_shared<Location>(p);
    auto      mp                = std::make_shared<Main_parser>(loc);
    mp->compile();
    int       errors            = mp->get_number_of_errors();

    if(errors){
        return Myauka_exit_codes::There_are_syntax_errors;
    }

    bool t                      = move_by_ext_from_curr_dir("src", "cpp");
    if(!t){
        puts("Moving files *.cpp failed.");
        return Myauka_exit_codes::Failed_moving_cpps;
    }
    t                           = move_by_ext_from_curr_dir("include", "h");
    if(!t){
        puts("Moving files *.h failed.");
        return Myauka_exit_codes::Failed_moving_hs;
    }

    return exit_code;
}

static Myauka_exit_codes compile_and_create_test(const char* file_name)
{
    Myauka_exit_codes exit_code = Myauka_exit_codes::Success;

    std::u32string    text      = get_processed_text(file_name);

    if(!text.length()){
        return Myauka_exit_codes::File_processing_error;
    }

    char32_t* p                 = const_cast<char32_t*>(text.c_str());
    auto      loc               = std::make_shared<Location>(p);
    auto      mp                = std::make_shared<Main_parser>(loc);
    mp->compile();
    int       errors            = mp->get_number_of_errors();

    if(errors){
        return Myauka_exit_codes::There_are_syntax_errors;
    }

    auto testing_info           = mp->get_testing_info();

    Testing_builder testing_builder {testing_info};
    testing_builder.build();

    bool t                      = move_by_ext_from_curr_dir("src", "cpp");
    if(!t){
        puts("Moving files *.cpp failed.");
        return Myauka_exit_codes::Failed_moving_cpps;
    }
    t                           = move_by_ext_from_curr_dir("include", "h");
    if(!t){
        puts("Moving files *.h failed.");
        return Myauka_exit_codes::Failed_moving_hs;
    }

    return exit_code;
}

Myauka_exit_codes Cmdline_processing::run()
{
    Myauka_exit_codes exit_code = Myauka_exit_codes::Success;
    if(1 == argc_){
        puts("Arguments are not specified.");
        help();
        return Myauka_exit_codes::No_args;
    }

    auto it = option.find(argv_[1]);
    if(it != option.end()){
        switch(it->second){
            case Option_kind::Help:
                help();
                break;
            case Option_kind::Version:
                version();
                break;
            case Option_kind::Test:
                if(2 == argc_){
                    puts("A file with scanner description is not specified.");
                    return Myauka_exit_codes::Not_enough_args;
                }
                exit_code = compile_and_create_test(argv_[2]);
                break;
        }
    }else{
        exit_code = only_compile(argv_[1]);
    }

    return exit_code;
}