/*
    File:    myauka.cpp
    Created: 15 March 2018 at 09:26 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include <cstdlib>
#include <cstdio>
#include <string>
#include <memory>
#include <map>
#include "../include/cmdline_processing.h"
// #include "../include/location.h"
// #include "../include/main_parser.h"
// #include "../include/move_by_ext_from_curr_dir.h"
// #include "../include/get_processed_text.h"
//
// static const char* help_str =
//     R"~(Usage: myauka [option] file
// Options:
//     --help    Display this text.
//     --version Display version information.
//     --test    Generate testing program.)~";
//
// static const char* version_str =
//     R"~(Myauka, lexical analyzer generator, v.2.0.0
// (c) Gavrilov Vladimir Sergeevich 2016--2018)~";
//
// typedef void (*FuncForKey)();
//
// static void version(){
//     puts(version_str);
// }
//
// static void help(){
//     puts(help_str);
// }
//
// static const std::map<std::string, FuncForKey> option_func = {
//     {"--version", version}, {"--help", help}
// };

// #define DEBUG_MODE
int main(int argc, char* argv[]){
    Cmdline_processing application {argc, argv};
    auto exit_code = application.run();
    return static_cast<int>(exit_code);
//     if(1 == argc){
//         puts("Arguments are not specified.");
//         help();
//         return 0;
//     }
//
//     auto it = option_func.find(argv[1]);
//     if(it != option_func.end()){
//         (it->second)();
//         return 0;
//     }
//
//     std::u32string t = get_processed_text(argv[1]);
//     if(t.length()){
//         char32_t* p      = const_cast<char32_t*>(t.c_str());
// #ifdef DEBUG_MODE
//         printf("text pointer:        %p\n", p);
// #endif
//         auto      loc    = std::make_shared<Location>(p);
// #ifdef DEBUG_MODE
//         printf("location pointer:    %p\n", loc.get());
// #endif
//         auto      mp     = std::make_shared<Main_parser>(loc);
// #ifdef DEBUG_MODE
//         printf("main parser pointer: %p\n", mp.get());
// #endif
//         mp->compile();
//         int       errors = mp->get_number_of_errors();
//         if(!errors){
//             bool t = move_by_ext_from_curr_dir("src", "cpp");
//             if(!t){
//                 puts("Moving files *.cpp failed.");
//             }else{
//                  t = move_by_ext_from_curr_dir("include", "h");
//                  if(!t){
//                      puts("Moving files *.h failed.");
//                 }
//             }
//         }
//     }
//
//     return 0;
}