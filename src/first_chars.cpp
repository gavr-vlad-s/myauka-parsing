/*
    File:    first_chars.cpp
    Created: 13 January 2016 at 16:04 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/first_chars.h"
#include "../include/sets_for_classes.h"
#include "../include/categories.h"

Category command2category(const Command& com, const Trie_for_set_of_char32ptr& t)
{
    Category result;
//     switch(com.name){
//         case Cmd_char_def:
//             result.kind = Set_of_cs;
//             result.s    = Set_of_char({com.c});
//             break;
//         case Cmd_char_class_def:
//             result.s    = sets_for_char_classes[com.cls];
//             switch(com.cls){
//                 case C_ndq:
//                     result.kind = Not_double_quote;
//                     break;
//                 case C_nsq:
//                     result.kind = Not_single_quote;
//                     break;
//                 default:
//                     result.kind = Set_of_cs;
//             }
//             break;
//         default:
//             ;
//     }
    return result;
}


// enum class Command_name{
//     Or,                    Concat,  Kleene,     Positive,
//     Optional,              Char,    Char_class, Unknown,
//     Char_class_complement, Multior, Multiconcat
// };

Category first_charsR(const Command_buffer& buf,
                      size_t idx,
                      const Trie_for_set_of_char32ptr& t)
{
    Category  categ;
    Command   com    = buf[idx];
    switch(com.name){
        case Command_name::Or:
            return first_charsR(buf, com.args.first,  t) +
                   first_charsR(buf, com.args.second, t);
            break;
        case Command_name::Concat:   case Command_name::Kleene:
        case Command_name::Positive: case Command_name::Optional:
        case Command_name::Multiconcat:
            return first_charsR(buf, com.args.first, t);
            break;
        case Command_name::Multior:
            categ.kind = Category_kind::Set_of_cs;
            for(size_t i = com.args.first; i <= com.args.second; i++){
                categ = categ + command2category(buf[i], t);
            }
            return categ;
            break;
        default:
            return categ;
//
//         case Cmd_char_def: case Cmd_char_class_def:
//             return command2category(com);
//
//         case Cmd_multior:
//             categ.kind = Set_of_cs;
//
//         default:
//             return categ;
    }
}

Category first_chars(const Command_buffer& buf, const Trie_for_set_of_char32ptr& t)
{
    size_t last_elem_idx = buf.size() - 1;
    return first_charsR(buf, last_elem_idx, t);
}