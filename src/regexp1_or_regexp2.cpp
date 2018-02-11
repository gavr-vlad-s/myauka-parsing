/*
    File:    regexp1_or_regexp2.cpp
    Created: 05 February 2017 at 17:41 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
             vladimir.gavrilov@itmm.unn.ru
*/

#include "../include/regexp1_or_regexp2.h"

// enum class {

// };
//
// struct Command{
//     size_t action_name; /* The index of the identifier that is the name of
//                          * the action, in the prefix tree of identifiers. */
//     Command_name name;
//     union{
//         struct {
//             size_t first, second;
//         } args;
//         char32_t   c;
//         size_t     idx_of_set;
//     };
// };

Command_buffer regexp1_or_regexp2(const Command_buffer& a, const Command_buffer& b)
{
    if(a.empty()){
        return b;
    }
    if(b.empty()){
        return a;
    }

    Command_buffer result            = a;
    size_t         num_commands_in_a = a.size();
    size_t         or_first_arg      = num_commands_in_a - 1;

    Command_buffer temp              = b;
    for(auto& c : temp){
        switch(c.name){
            case Command_name::Or:      case Command_name::Concat:
            case Command_name::Multior: case Command_name::Multiconcat:
                c.args.first += num_commands_in_a; c.args.second += num_commands_in_a;
                break;
            case Command_name::Kleene:  case Command_name::Positive:
            case Command_name::Optional:
                c.args.first += num_commands_in_a;
                break;
            default:
                ;
        }
    }

    result.insert(result.end(), temp.begin(), temp.end());
    size_t         or_second_arg     = result.size() - 1;

    Command command;
    command.action_name     = 0;
    command.name            = Command_name::Or;
    command.args.first      = or_first_arg;
    command.args.second     = or_second_arg;
    result.push_back(command);

    return result;
}