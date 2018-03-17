/*
    File:    cmdline_processing.h
    Created: 13 March 2018 at 17:20 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef CMDLINE_PROCESSING_H
#define CMDLINE_PROCESSING_H
enum class Myauka_exit_codes{
    Success,                 No_args,               Not_enough_args,
    There_are_syntax_errors, File_processing_error, Failed_moving_cpps,
    Failed_moving_hs
};

class Cmdline_processing{
public:
    Cmdline_processing()                          = default;
    Cmdline_processing(const Cmdline_processing&) = default;
    Cmdline_processing(int argc, char* argv[]) : argc_(argc), argv_(argv) {};
    ~Cmdline_processing()                         = default;

    Myauka_exit_codes run();
private:
    int    argc_;
    char** argv_;
};
#endif