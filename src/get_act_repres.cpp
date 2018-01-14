/*
    File:    get_act_repres.cpp
    Created: 04 February 2017 at 18:35 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/get_act_repres.h"
#include "../include/idx_to_string.h"

std::string get_act_repres(const Errors_and_tries&       et,
                           const std::shared_ptr<Scope>& scope,
                           size_t                        i)
{
    /* A non-empty string is returned only if there is an action with
     * such an index name. */
    std::string result;
    if(!i){
        return result;
    }
    auto isc = scope->idsc;
    auto iti = isc.find(i);
    if(iti != isc.end()){
        size_t act_str_idx = iti->second.act_string;
        result = idx_to_string(et.strs_trie, act_str_idx);
    }
    return result;
}