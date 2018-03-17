/*
    File:    main_parser.h
    Created: 14 December 2015 at 15:25 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef MAIN_PARSER_H
#define MAIN_PARSER_H

#include <memory>
#include "../include/location.h"
#include "../include/info_for_generating_testing_program.h"

class Main_parser{
public:
    Main_parser()                        = default;
    Main_parser(const Location_ptr& loc);
    Main_parser(const Main_parser& orig) = default;
    ~Main_parser();

    void compile();

    int get_number_of_errors() const;
    Info_for_generating_testing_program get_testing_info() const;
private:
    struct Main_parser_data;
    std::unique_ptr<Main_parser_data> impl_;

    void add_new_lexem_code(size_t idx);

    void add_fictive_delimiter(const std::u32string& dcode,  size_t drepres_idx);
    void add_new_string(const size_t idx, const size_t code_);
    void add_fictive_delimiters();
    /* If there is a description of the comments, then the function adds a fictive
     * delimiters. Namely, if a single-line comment is described, a fictive delimiter
     * SINGLE_LINED_COMMENT_MARK is added, and if there is a description of a multi-line
     * comment, then the dummy delimiter MULTILINED_COMMENT_MARK is added. */
};
#endif