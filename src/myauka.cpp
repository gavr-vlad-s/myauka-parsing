#include <cstdlib>
#include <cstdio>
#include <string>
#include <memory>
#include <map>
#include "../include/location.h"
#include "../include/main_parser.h"
#include "../include/char_conv.h"
#include "../include/file_contents.h"
#include "../include/move_by_ext_from_curr_dir.h"

/* Function that opens a file with text. Returns a string with text if the file was
 * opened and the file size is not zero, and an empty string otherwise. */
std::u32string get_processed_text(const char* name){
    auto contents = get_contents(name);
    auto str      = contents.second;
    switch(contents.first){
        case Get_contents_return_code::Normal:
            if(!str.length()){
                puts("File length is equal to zero.");
                return U"";
            }else{
                return utf8_to_u32string(str.c_str());
            }
            break;

        case Get_contents_return_code::Impossible_open:
            puts("Unable to open file.");
            return U"";

        case Get_contents_return_code::Read_error:
            puts("Error reading file.");
            return U"";
    }
    return U"";
}

static const char* help_str =
    R"~(Usage: myauka [option] file
Options:
    --help    Display this text.
    --version Display version information)~";

static const char* version_str =
    R"~(Myauka, lexical analyzer generator, v.2.0.0
(c) Gavrilov Vladimir Sergeevich 2016--2018)~";

typedef void (*FuncForKey)();

void version(){
    puts(version_str);
}

void help(){
    puts(help_str);
}

std::map<std::string, FuncForKey> option_func = {
    {"--version", version}, {"--help", help}
};

// #define DEBUG_MODE
int main(int argc, char* argv[]){
    if(1 == argc){
        puts("Arguments are not specified.");
        help();
        return 0;
    }

    auto it = option_func.find(argv[1]);
    if(it != option_func.end()){
        (it->second)();
        return 0;
    }

    std::u32string t = get_processed_text(argv[1]);
    if(t.length()){
        char32_t* p      = const_cast<char32_t*>(t.c_str());
#ifdef DEBUG_MODE
        printf("text pointer:        %p\n", p);
#endif
        auto      loc    = std::make_shared<Location>(p);
#ifdef DEBUG_MODE
        printf("location pointer:    %p\n", loc.get());
#endif
        auto      mp     = std::make_shared<Main_parser>(loc);
#ifdef DEBUG_MODE
        printf("main parser pointer: %p\n", mp.get());
#endif
        mp->compile();
        int       errors = mp->get_number_of_errors();
        if(!errors){
            bool t = move_by_ext_from_curr_dir("src", "cpp");
            if(!t){
                puts("Moving files *.cpp failed.");
            }else{
                 t = move_by_ext_from_curr_dir("include", "h");
                 if(!t){
                     puts("Moving files *.h failed.");
                }
            }
        }
    }

    return 0;
}