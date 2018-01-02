/*
    File:    num_str_id_parser.cpp
    Created: 02 December 2017 at 16:08 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/num_str_id_parser.h"
#include "../include/belongs.h"
#include <tuple>

NSI_settings String_parser::settings(){
    return Kw_strings;
}

NSI_settings Number_parser::settings(){
    return Kw_numbers;
}

NSI_settings Idents_parser::settings(){
    return Kw_idents;
}

void NSI_parser::add_action_definition(size_t action_id, size_t action_def_idx)
{
    auto& isc   = scope_->idsc;
    auto& ssc   = scope_->strsc;
    auto  id_it = isc.find(action_id);
    auto  s_it  = ssc.find(action_def_idx);

    if( (id_it != isc.end()) && ((id_it->second).kind != Action_name) ){
        printf("Error in line %zu: the identifier ", msc->lexem_begin_line_number());
        et_.ids_trie->print(action_id);
        printf(" is already defined, but not as an action name.\n");
        et_.ec->increment_number_of_errors();
        return;
    }

    if( (s_it != ssc.end()) && ((s_it->second).kind != Action_definition) ){
        printf("Error in line %zu: the string literal ", msc->lexem_begin_line_number());
        et_.strs_trie->print(action_def_idx);
        printf(" is already defined, but not as an action definition.\n");
        et_.ec->increment_number_of_errors();
        return;
    }

    Id_attributes a;
    a.kind                        = Action_name;
    a.act_string                  = action_def_idx;
    scope_->idsc[action_id]       = a;

    Str_attributes sa;
    sa.kind                       = Action_definition;
    sa.code                       = 0;
    scope_->strsc[action_def_idx] = sa;
}

void NSI_parser::compile(Command_buffer& buf, Init_and_final_acts& acts)
{
    buf_.clear();
    sec_begin = settings();
    bool t    = begin_of_num_or_str_sec();
    if(!t){
        return;
    }
    a_parser_->compile(buf_);
    buf  = buf_;
    acts = acts_;
}

bool NSI_parser::begin_of_num_or_str_sec(){
    lc = (li = msc->current_lexem()).code;
    bool t = false;
    if(!lc){
        /* If the text with the description of the scanner has ended,
         * then the section will not be processed further. */
        msc->back();
        return t;
    }

    msc->back();

    acts_.init_acts = acts_.fin_acts = 0;
    state = Num_str_kw;
    t     = true;
    while((lc = (li = msc->current_lexem()).code)){
        t = (this->*procs[state])();
        if(!t){
            return t;
        }
        if(Act_expr_beg == state){
            return t;
        }
    }

    if(state != Act_expr_beg){
        printf("Unexpected end of section header in line %zu.\n",
               msc->lexem_begin_line_number());
        et_.ec -> increment_number_of_errors();
    }
    msc->back();
    return t;
}

NSI_parser::State_proc NSI_parser::procs[] = {
    &NSI_parser::num_str_kw_proc,   &NSI_parser::maybe_init_acts_proc,
    &NSI_parser::init_acts_proc,    &NSI_parser::maybe_final_acts_proc,
    &NSI_parser::final_acts_proc,   &NSI_parser::action_sec_proc,
    &NSI_parser::act_expr_beg_proc, &NSI_parser::act_def_proc
};

bool NSI_parser::num_str_kw_proc()
{
    bool t = true;
    if(sec_begin == lc){
        state = Maybe_init_acts;
        return t;
    }
    t = false;
    if(belongs(lc, (1ULL << Kw_numbers) | (1ULL << Kw_strings))){
        msc->back();
        return t;
    }
    msc->back();
    printf("Line %zu omits the keyword %s.\n", msc->lexem_begin_line_number(),
           (Kw_strings == sec_begin) ? "%strings" : "%numbers");
    et_.ec -> increment_number_of_errors();
    switch(lc){
        case String:
            state = Init_acts;
            acts_.init_acts = li.string_index;
            break;

        case Kw_action:
            state = Action_sec;
            break;

        case Id:
            printf("Line %zu omits the keyword %%action.\n",
                msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            current_action_name_idx = li.ident_index;
            state = Act_def;
            break;

        case Colon:
            state = Maybe_final_acts;
            break;

        case Opened_fig_brack:
            state = Act_expr_beg;
            msc->back();
            break;

        default:
            msc->back();
    }
    return t;
}

bool NSI_parser::maybe_init_acts_proc()
{
    bool t = true;
    if(sec_begin == lc){
        printf("Line %zu unexpectedly found the keyword %s.\n",
               msc->lexem_begin_line_number(),
               (Kw_strings == sec_begin) ? "%strings" : "%numbers");
        return t;
    }
    switch(lc){
        case String:
            state = Init_acts;
            acts_.init_acts = li.string_index;
            break;

        case Kw_action:
            state = Action_sec;
            printf("Line %zu unexpectedly found the keyword %%action.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            current_action_name_idx = li.ident_index;
            break;

        case Id:
            printf("Line %zu omits the keyword %%action.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            current_action_name_idx = li.ident_index;
            state = Act_def;
            break;

        case Colon:
            state = Maybe_final_acts;
            break;

        case Opened_fig_brack:
            state = Act_expr_beg;
            msc->back();
            printf("Line %zu unexpectedly found {.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        default:
            t = false;
            msc->back();
    }
    return t;
}

bool NSI_parser::init_acts_proc()
{
    bool t = true;
    if(sec_begin == lc){
        printf("Line %zu unexpectedly found the keyword %s.\n",
               msc->lexem_begin_line_number(),
               (Kw_strings == sec_begin) ? "%strings" : "%numbers");
        return t;
    }
    switch(lc){
        case String:
            acts_.fin_acts = li.string_index;
            state = Final_acts;
            printf("Colon is omitted at line %zu.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Kw_action:
            state = Action_sec;
            printf("Line %zu unexpectedly found the keyword %%action.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            current_action_name_idx = li.ident_index;
            break;

        case Id:
            printf("Line %zu omits the keyword %%action.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            current_action_name_idx = li.ident_index;
            state = Act_def;
            break;

        case Colon:
            state = Maybe_final_acts;
            break;

        case Opened_fig_brack:
            state = Act_expr_beg;
            msc->back();
            printf("Line %zu unexpectedly found {.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        default:
            t = false;
            msc->back();
    }
    return t;
}

bool NSI_parser::maybe_final_acts_proc()
{
    bool t = true;
    if(sec_begin == lc){
        printf("Line %zu unexpectedly found the keyword %s.\n",
               msc->lexem_begin_line_number(),
               (Kw_strings == sec_begin) ? "%strings" : "%numbers");
        return t;
    }
    switch(lc){
        case String:
            acts_.fin_acts = li.string_index;
            state = Final_acts;
            break;

        case Kw_action:
            state = Action_sec;
            break;

        case Id:
            printf("Line %zu omits the keyword %%action.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            current_action_name_idx = li.ident_index;
            state = Act_def;
            break;

        case Colon:
            printf("Line %zu unexpectedly found the colon.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Opened_fig_brack:
            state = Act_expr_beg;
            msc->back();
            break;

        default:
            t = false;
            msc->back();
    }
    return t;
}

bool NSI_parser::final_acts_proc()
{
    bool t = true;
    if(sec_begin == lc){
        printf("Line %zu unexpectedly found the keyword %s.\n",
               msc->lexem_begin_line_number(),
              (Kw_strings == sec_begin) ? "%strings" : "%numbers");
        return t;
    }
    switch(lc){
        case String:
            printf("Line %zu unexpectedly found the string literal with "
                   "an action definition.\n", msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Kw_action:
            state = Action_sec;
            break;

        case Id:
            printf("Line %zu omits the keyword %%action.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            current_action_name_idx = li.ident_index;
            state = Act_def;
            break;

        case Colon:
            printf("Line %zu unexpectedly found the colon.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Opened_fig_brack:
            state = Act_expr_beg;
            msc->back();
            break;

        default:
            t = false;
            msc->back();
    }
    return t;
}

bool NSI_parser::action_sec_proc()
{
    bool t = true;
    if(sec_begin == lc){
        printf("Line %zu unexpectedly found the keyword %s.\n",
               msc->lexem_begin_line_number(),
               (Kw_strings == sec_begin) ? "%strings" : "%numbers");
        return t;
    }
    switch(lc){
        case String:
            printf("Line %zu unexpectedly found the string literal with "
                   "an action definition.\n", msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            state = Final_acts;
            break;

        case Kw_action:
            printf("Line %zu unexpectedly found the keyword %%action.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Id:
            current_action_name_idx = li.ident_index;
            state = Act_def;
            break;

        case Colon:
            printf("Line %zu unexpectedly found the colon.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Opened_fig_brack:
            state = Act_expr_beg;
            msc->back();
            printf("Line %zu unexpectedly found {.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        default:
            t = false;
            msc->back();
    }
    return t;
}

bool NSI_parser::act_expr_beg_proc()
{
    bool t = false;
    msc->back();
    return t;
}

bool NSI_parser::act_def_proc()
{
    bool t = true;
    if(sec_begin == lc){
        printf("Line %zu unexpectedly found the keyword %s.\n",
               msc->lexem_begin_line_number(),
               (Kw_strings == sec_begin) ? "%strings" : "%numbers");
        return t;
    }
    switch(lc){
        case String:
            add_action_definition(current_action_name_idx, li.string_index);
            state = Final_acts;
            break;

        case Kw_action:
            printf("Line %zu unexpectedly found the keyword %%action.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Id:
            printf("Line %zu unexpectedly found the identifier.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Colon:
            printf("Line %zu unexpectedly found the colon.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        case Opened_fig_brack:
            state = Act_expr_beg;
            msc->back();
            printf("Line %zu unexpectedly found {.\n",
                   msc->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;

        default:
            t = false;
            msc->back();
    }
    return t;
}