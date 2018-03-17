/*
     File:    automata_impl_collector.cpp
     Created: 13 March 2018 at 18:54 Moscow time
     Author:  Гаврилов Владимир Сергеевич
     E-mails: vladimir.s.gavrilov@gmail.com
              gavrilov.vladimir.s@mail.ru
              gavvs1977@yandex.ru
*/

#include <vector>
#include "../include/automata_impl_collector.h"
#include "../include/format.h"
#include "../include/join.h"

using namespace std::string_literals;

static const std::string impl_fmt =
R"x(
{0}

{1}

{2}
)x"s;

static const std::string newline = "\n"s;

std::string Automata_impl_collector::collect()
{
    std::string result;
    auto start_proc  = collect_start_proc();
    auto other_procs = collect_procs_without_start();
    auto finals      = collect_finals();
    result           = fmt::format(impl_fmt, start_proc, other_procs, finals);
    return result;
}

std::string Automata_impl_collector::collect_procs_without_start()
{
    std::string result;
    auto& about_automata = info_.automata_info;
    std::vector<std::string> procs;
    for(auto it = about_automata.begin() + 1; it != about_automata.end(); ++it){
        auto& ai = *it;
        procs.push_back(ai.proc_impl);
    }
    result = join(procs.begin(), procs.end(), newline);
    return result;
}

std::string Automata_impl_collector::collect_finals()
{
    std::string result;
    std::vector<std::string> finals;
    for(const auto& ai : info_.automata_info){
        finals.push_back(ai.final_proc_impl);
    }
    result = join(finals.begin(), finals.end(), newline);
    return result;
}

std::string Automata_impl_collector::collect_start_proc()
{
    std::string result;
    auto& ifs           = info_.ifs_of_start_procs;
    auto  collected_ifs = join(ifs.begin(), ifs.end(), newline);
    auto& start_fmt     = info_.automata_info[0].proc_impl;
    result              = fmt::format(start_fmt, collected_ifs);
    return result;
}