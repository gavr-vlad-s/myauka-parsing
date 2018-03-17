/*
     File:    automata_impl_collector.h
     Created: 13 March 2018 at 18:49 Moscow time
     Author:  Гаврилов Владимир Сергеевич
     E-mails: vladimir.s.gavrilov@gmail.com
              gavrilov.vladimir.s@mail.ru
              gavvs1977@yandex.ru
*/

#ifndef AUTOMATA_IMPL_COLLECTOR_H
#define AUTOMATA_IMPL_COLLECTOR_H
#include <string>
#include "../include/info_for_constructing.h"
class Automata_impl_collector{
public:
    Automata_impl_collector()                               = default;
    Automata_impl_collector(const Automata_impl_collector&) = default;
    Automata_impl_collector(const info_for_constructing::Info& info) : info_(info) {};
    ~Automata_impl_collector()                              = default;

    std::string collect();
private:
    info_for_constructing::Info info_;

    std::string collect_start_proc();
    std::string collect_procs_without_start();
    std::string collect_finals();
};
#endif