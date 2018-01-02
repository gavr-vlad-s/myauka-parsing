/*
    File:    main_scaner.cpp
    Created: 14 December 2015 at 15:25 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include <cstdlib>
#include <cstdio>
#include <map>
#include "../include/main_scaner.h"
#include "../include/belongs.h"
#include "../include/search_char.h"
#include "../include/get_init_state.h"

static const std::map<char32_t, uint8_t> categories_table = {
    {   1,   1},  {   2,   1},  {   3,   1},  {   4,   1},  {   5,   1},
    {   6,   1},  {   7,   1},  {   8,   1},  {   9,   1},  {  10,   1},
    {  11,   1},  {  12,   1},  {  13,   1},  {  14,   1},  {  15,   1},
    {  16,   1},  {  17,   1},  {  18,   1},  {  19,   1},  {  20,   1},
    {  21,   1},  {  22,   1},  {  23,   1},  {  24,   1},  {  25,   1},
    {  26,   1},  {  27,   1},  {  28,   1},  {  29,   1},  {  30,   1},
    {  31,   1},  {  32,   1},  {U'"', 128},  {U'%',   4},  {U',',  64},
    {U'0',  32},  {U'1',  32},  {U'2',  32},  {U'3',  32},  {U'4',  32},
    {U'5',  32},  {U'6',  32},  {U'7',  32},  {U'8',  32},  {U'9',  32},
    {U':',  64},  {U'A',  48},  {U'B',  48},  {U'C',  48},  {U'D',  48},
    {U'E',  48},  {U'F',  48},  {U'G',  48},  {U'H',  48},  {U'I',  48},
    {U'J',  48},  {U'K',  48},  {U'L',  48},  {U'M',  48},  {U'N',  48},
    {U'O',  48},  {U'P',  48},  {U'Q',  48},  {U'R',  48},  {U'S',  48},
    {U'T',  48},  {U'U',  48},  {U'V',  48},  {U'W',  48},  {U'X',  48},
    {U'Y',  48},  {U'Z',  48},  {U'_',  48},  {U'a',  56},  {U'b',  48},
    {U'c',  56},  {U'd',  56},  {U'e',  48},  {U'f',  48},  {U'g',  48},
    {U'h',  56},  {U'i',  56},  {U'j',  48},  {U'k',  56},  {U'l',  56},
    {U'm',  56},  {U'n',  56},  {U'o',  48},  {U'p',  48},  {U'q',  48},
    {U'r',  48},  {U's',  56},  {U't',  56},  {U'u',  48},  {U'v',  48},
    {U'w',  48},  {U'x',  48},  {U'y',  48},  {U'z',  48},  {U'{',  64},
    {U'}',  64},
};

uint64_t Main_scaner::get_categories_set(char32_t c){
    auto it = categories_table.find(c);
    if(it != categories_table.end()){
        return it->second;
    }else{
        return 1ULL << Other;
    }
}

/**
 * Element of transition table of automaton processing the keywords.
 */
struct Elem {
    /** A pointer to a string of characters that can be crossed. */
    char32_t*       symbols;
    /** lexeme code */
    Main_lexem_code code;
    /** If the current character matches symbols[0], then
     *  the transition to the state first_state;
     *  if the current character matches symbols[1], then
     *  the transition to the state first_state + 1;
     *  if the current character matches symbols[2], then
     *  the transition to the state first_state + 2,
     *  and so on. */
    uint16_t        first_state;
};
/* For the keyword processing automaton, the member state of the class Main_scaner
 * is the index of the element in the transition table, denoted below as
 * a_keyword_jump_table. */
static const Elem a_keyword_jump_table[] = {
    {const_cast<char32_t*>(U"c"),  M_Kw_action,          1},   // 0:   %a...
    {const_cast<char32_t*>(U"t"),  M_Kw_action,          2},   // 1:   %ac...
    {const_cast<char32_t*>(U"i"),  M_Kw_action,          3},   // 2:   %act...
    {const_cast<char32_t*>(U"o"),  M_Kw_action,          4},   // 3:   %acti...
    {const_cast<char32_t*>(U"n"),  M_Kw_action,          5},   // 4:   %actio...
    {const_cast<char32_t*>(U""),   Kw_action,            0},   // 5:   %action

    {const_cast<char32_t*>(U"lo"), M_Kw_codes,           7},   // 6:   %c...
    {const_cast<char32_t*>(U"a"),  M_Kw_class_members,   9},   // 7:   %cl...
    {const_cast<char32_t*>(U"dm"), M_Kw_codes,           10},  // 8:   %co...
    {const_cast<char32_t*>(U"s"),  M_Kw_class_members,   12},  // 9:   %cla...
    {const_cast<char32_t*>(U"e"),  M_Kw_codes,           13},  // 10:  %cod...
    {const_cast<char32_t*>(U"m"),  M_Kw_comments,        14},  // 11:  %com...
    {const_cast<char32_t*>(U"s"),  M_Kw_class_members,   15},  // 12:  %clas...
    {const_cast<char32_t*>(U"s"),  M_Kw_codes,           16},  // 13:  %code...
    {const_cast<char32_t*>(U"e"),  M_Kw_comments,        17},  // 14:  %comm...
    {const_cast<char32_t*>(U"_"),  M_Kw_class_members,   18},  // 15:  %class...
    {const_cast<char32_t*>(U"_"),  Kw_codes,             19},  // 16:  %codes...
    {const_cast<char32_t*>(U"n"),  M_Kw_comments,        20},  // 17:  %comme...
    {const_cast<char32_t*>(U"m"),  M_Kw_class_members,   21},  // 18:  %class_...
    {const_cast<char32_t*>(U"t"),  M_Kw_codes_type,      22},  // 19:  %codes_...
    {const_cast<char32_t*>(U"t"),  M_Kw_comments,        23},  // 20:  %commen...
    {const_cast<char32_t*>(U"e"),  M_Kw_class_members,   24},  // 21:  %class_m...
    {const_cast<char32_t*>(U"y"),  M_Kw_codes_type,      25},  // 22:  %codes_t...
    {const_cast<char32_t*>(U"s"),  M_Kw_comments,        26},  // 23:  %comment...
    {const_cast<char32_t*>(U"m"),  M_Kw_class_members,   27},  // 24:  %class_me...
    {const_cast<char32_t*>(U"p"),  M_Kw_codes_type,      28},  // 25:  %codes_ty...
    {const_cast<char32_t*>(U""),   Kw_comments,          0},   // 26:  %comments
    {const_cast<char32_t*>(U"b"),  M_Kw_class_members,   29},  // 27:  %class_mem...
    {const_cast<char32_t*>(U"e"),  M_Kw_codes_type,      30},  // 28:  %codes_typ...
    {const_cast<char32_t*>(U"e"),  M_Kw_class_members,   31},  // 29:  %class_memb...
    {const_cast<char32_t*>(U""),   Kw_codes_type,        0},   // 30:  %codes_type
    {const_cast<char32_t*>(U"r"),  M_Kw_class_members,   32},  // 31:  %class_membe...
    {const_cast<char32_t*>(U"s"),  M_Kw_class_members,   33},  // 32:  %class_member...
    {const_cast<char32_t*>(U""),   Kw_class_members,     0},   // 33:  %class_members

    {const_cast<char32_t*>(U"e"),  M_Kw_delimiters,      35},  // 34:  %d...
    {const_cast<char32_t*>(U"l"),  M_Kw_delimiters,      36},  // 35:  %de...
    {const_cast<char32_t*>(U"i"),  M_Kw_delimiters,      37},  // 36:  %del...
    {const_cast<char32_t*>(U"m"),  M_Kw_delimiters,      38},  // 37:  %deli...
    {const_cast<char32_t*>(U"i"),  M_Kw_delimiters,      39},  // 38:  %delim...
    {const_cast<char32_t*>(U"t"),  M_Kw_delimiters,      40},  // 39:  %delimi...
    {const_cast<char32_t*>(U"e"),  M_Kw_delimiters,      41},  // 40:  %delimit...
    {const_cast<char32_t*>(U"r"),  M_Kw_delimiters,      42},  // 41:  %delimite...
    {const_cast<char32_t*>(U"s"),  M_Kw_delimiters,      43},  // 42:  %delimiter...
    {const_cast<char32_t*>(U""),   Kw_delimiters,        0},   // 43:  %delimiters

    {const_cast<char32_t*>(U"e"),  M_Kw_header_additions,45},  // 44:  %h...
    {const_cast<char32_t*>(U"a"),  M_Kw_header_additions,46},  // 45:  %he...
    {const_cast<char32_t*>(U"d"),  M_Kw_header_additions,47},  // 46:  %hea...
    {const_cast<char32_t*>(U"e"),  M_Kw_header_additions,48},  // 47:  %head...
    {const_cast<char32_t*>(U"r"),  M_Kw_header_additions,49},  // 48:  %heade...
    {const_cast<char32_t*>(U"_"),  M_Kw_header_additions,50},  // 49:  %header...
    {const_cast<char32_t*>(U"a"),  M_Kw_header_additions,51},  // 50:  %header_...
    {const_cast<char32_t*>(U"d"),  M_Kw_header_additions,52},  // 51:  %header_a...
    {const_cast<char32_t*>(U"d"),  M_Kw_header_additions,53},  // 52:  %header_ad...
    {const_cast<char32_t*>(U"i"),  M_Kw_header_additions,54},  // 53:  %header_add...
    {const_cast<char32_t*>(U"t"),  M_Kw_header_additions,55},  // 54:  %header_addi...
    {const_cast<char32_t*>(U"i"),  M_Kw_header_additions,56},  // 55:  %header_addit...
    {const_cast<char32_t*>(U"o"),  M_Kw_header_additions,57},  // 56:  %header_additi...
    {const_cast<char32_t*>(U"n"),  M_Kw_header_additions,58},  // 57:  %header_additio...
    {const_cast<char32_t*>(U"s"),  M_Kw_header_additions,59},  // 58:  %header_addition...
    {const_cast<char32_t*>(U""),   Kw_header_additions,  0},   // 59:  %header_additions

    {const_cast<char32_t*>(U"dm"), M_Kw_idents,          61},  // 60:  %i...
    {const_cast<char32_t*>(U"e"),  M_Kw_idents,          63},  // 61:  %id...
    {const_cast<char32_t*>(U"p"),  M_Kw_impl_additions,  64},  // 62:  %im...
    {const_cast<char32_t*>(U"n"),  M_Kw_idents,          65},  // 63:  %ide...
    {const_cast<char32_t*>(U"l"),  M_Kw_impl_additions,  66},  // 64:  %imp...
    {const_cast<char32_t*>(U"t"),  M_Kw_idents,          67},  // 65:  %iden...
    {const_cast<char32_t*>(U"_"),  M_Kw_impl_additions,  68},  // 66:  %impl...
    {const_cast<char32_t*>(U"_s"), M_Kw_idents,          69},  // 67:  %ident...
    {const_cast<char32_t*>(U"a"),  M_Kw_impl_additions,  71},  // 68:  %impl_...
    {const_cast<char32_t*>(U"n"),  M_Kw_ident_name,      72},  // 69:  %ident_...
    {const_cast<char32_t*>(U""),   Kw_idents,            0},   // 70:  %idents
    {const_cast<char32_t*>(U"d"),  M_Kw_impl_additions,  73},  // 71:  %impl_a...
    {const_cast<char32_t*>(U"a"),  M_Kw_ident_name,      74},  // 72:  %ident_n...
    {const_cast<char32_t*>(U"d"),  M_Kw_impl_additions,  75},  // 73:  %impl_ad...
    {const_cast<char32_t*>(U"m"),  M_Kw_ident_name,      76},  // 74:  %ident_na...
    {const_cast<char32_t*>(U"i"),  M_Kw_impl_additions,  77},  // 75:  %impl_add...
    {const_cast<char32_t*>(U"e"),  M_Kw_ident_name,      78},  // 76:  %ident_nam...
    {const_cast<char32_t*>(U"t"),  M_Kw_impl_additions,  79},  // 77:  %impl_addi...
    {const_cast<char32_t*>(U""),   Kw_ident_name,        0},   // 78:  %ident_name
    {const_cast<char32_t*>(U"i"),  M_Kw_impl_additions,  80},  // 79:  %impl_addit...
    {const_cast<char32_t*>(U"o"),  M_Kw_impl_additions,  81},  // 80:  %impl_additi...
    {const_cast<char32_t*>(U"n"),  M_Kw_impl_additions,  82},  // 81:  %impl_additio...
    {const_cast<char32_t*>(U"s"),  M_Kw_impl_additions,  83},  // 82:  %impl_addition...
    {const_cast<char32_t*>(U""),   Kw_impl_additions,  0},     // 83:  %impl_additions

    {const_cast<char32_t*>(U"e"),  M_Kw_keywords,        85},  // 84:  %k...
    {const_cast<char32_t*>(U"y"),  M_Kw_keywords,        86},  // 85:  %ke...
    {const_cast<char32_t*>(U"w"),  M_Kw_keywords,        87},  // 86:  %key...
    {const_cast<char32_t*>(U"o"),  M_Kw_keywords,        88},  // 87:  %keyw...
    {const_cast<char32_t*>(U"r"),  M_Kw_keywords,        89},  // 88:  %keywo...
    {const_cast<char32_t*>(U"d"),  M_Kw_keywords,        90},  // 89:  %keywor...
    {const_cast<char32_t*>(U"s"),  M_Kw_keywords,        91},  // 90:  %keyword...
    {const_cast<char32_t*>(U""),   Kw_keywords,          0},   // 91:  %keywords

    {const_cast<char32_t*>(U"e"),  M_Kw_lexem_info_name, 93},  // 92:  %l...
    {const_cast<char32_t*>(U"x"),  M_Kw_lexem_info_name, 94},  // 93:  %le...
    {const_cast<char32_t*>(U"e"),  M_Kw_lexem_info_name, 95},  // 94:  %lex...
    {const_cast<char32_t*>(U"m"),  M_Kw_lexem_info_name, 96},  // 95:  %lexe...
    {const_cast<char32_t*>(U"_"),  M_Kw_lexem_info_name, 97},  // 96:  %lexem...
    {const_cast<char32_t*>(U"i"),  M_Kw_lexem_info_name, 98},  // 97:  %lexem_...
    {const_cast<char32_t*>(U"n"),  M_Kw_lexem_info_name, 99},  // 98:  %lexem_i...
    {const_cast<char32_t*>(U"f"),  M_Kw_lexem_info_name, 100}, // 99:  %lexem_in...
    {const_cast<char32_t*>(U"o"),  M_Kw_lexem_info_name, 101}, // 100: %lexem_inf...
    {const_cast<char32_t*>(U"_"),  M_Kw_lexem_info_name, 102}, // 101: %lexem_info...
    {const_cast<char32_t*>(U"n"),  M_Kw_lexem_info_name, 103}, // 102: %lexem_info_...
    {const_cast<char32_t*>(U"a"),  M_Kw_lexem_info_name, 104}, // 103: %lexem_info_n...
    {const_cast<char32_t*>(U"m"),  M_Kw_lexem_info_name, 105}, // 104: %lexem_info_na...
    {const_cast<char32_t*>(U"e"),  M_Kw_lexem_info_name, 106}, // 105: %lexem_info_nam...
    {const_cast<char32_t*>(U""),   Kw_lexem_info_name,   0},   // 106: %lexem_info_name

    {const_cast<char32_t*>(U"u"),  M_Kw_multilined,      108}, // 107: %m...
    {const_cast<char32_t*>(U"l"),  M_Kw_multilined,      109}, // 108: %mu...
    {const_cast<char32_t*>(U"t"),  M_Kw_multilined,      110}, // 109: %mul...
    {const_cast<char32_t*>(U"i"),  M_Kw_multilined,      111}, // 110: %mult...
    {const_cast<char32_t*>(U"l"),  M_Kw_multilined,      112}, // 111: %multi...
    {const_cast<char32_t*>(U"i"),  M_Kw_multilined,      113}, // 112: %multil...
    {const_cast<char32_t*>(U"n"),  M_Kw_multilined,      114}, // 113: %multili...
    {const_cast<char32_t*>(U"e"),  M_Kw_multilined,      115}, // 114: %multilin...
    {const_cast<char32_t*>(U"d"),  M_Kw_multilined,      116}, // 115: %multiline...
    {const_cast<char32_t*>(U""),   Kw_multilined,        0},   // 116: %multilined

    {const_cast<char32_t*>(U"eu"), M_Kw_numbers,         118}, // 117: %n...
    {const_cast<char32_t*>(U"sw"), M_Kw_nested,          120}, // 118: %ne...
    {const_cast<char32_t*>(U"m"),  M_Kw_numbers,         122}, // 119: %nu...
    {const_cast<char32_t*>(U"t"),  M_Kw_nested,          123}, // 120: %nes...
    {const_cast<char32_t*>(U"l"),  M_Kw_newline_is_lexem,124}, // 121: %new...
    {const_cast<char32_t*>(U"b"),  M_Kw_numbers,         125}, // 122: %num...
    {const_cast<char32_t*>(U"e"),  M_Kw_nested,          126}, // 123: %nest...
    {const_cast<char32_t*>(U"i"),  M_Kw_newline_is_lexem,127}, // 124: %newl...
    {const_cast<char32_t*>(U"e"),  M_Kw_numbers,         128}, // 125: %numb...
    {const_cast<char32_t*>(U"d"),  M_Kw_nested,          129}, // 126: %neste...
    {const_cast<char32_t*>(U"n"),  M_Kw_newline_is_lexem,130}, // 127: %newli...
    {const_cast<char32_t*>(U"r"),  M_Kw_numbers,         131}, // 128: %numbe...
    {const_cast<char32_t*>(U""),   Kw_nested,            0},   // 129: %nested
    {const_cast<char32_t*>(U"e"),  M_Kw_newline_is_lexem,132}, // 130: %newlin...
    {const_cast<char32_t*>(U"s"),  M_Kw_numbers,         133}, // 131: %number...
    {const_cast<char32_t*>(U"_"),  M_Kw_newline_is_lexem,134}, // 132: %newline...
    {const_cast<char32_t*>(U""),   Kw_numbers,           0},   // 133: %numbers
    {const_cast<char32_t*>(U"i"),  M_Kw_newline_is_lexem,135}, // 134: %newline_...
    {const_cast<char32_t*>(U"s"),  M_Kw_newline_is_lexem,136}, // 135: %newline_i...
    {const_cast<char32_t*>(U"_"),  M_Kw_newline_is_lexem,137}, // 136: %newline_is...
    {const_cast<char32_t*>(U"l"),  M_Kw_newline_is_lexem,138}, // 137: %newline_is_...
    {const_cast<char32_t*>(U"e"),  M_Kw_newline_is_lexem,139}, // 138: %newline_is_l...
    {const_cast<char32_t*>(U"x"),  M_Kw_newline_is_lexem,140}, // 139: %newline_is_le...
    {const_cast<char32_t*>(U"e"),  M_Kw_newline_is_lexem,141}, // 140: %newline_is_lex...
    {const_cast<char32_t*>(U"m"),  M_Kw_newline_is_lexem,142}, // 141: %newline_is_lexe...
    {const_cast<char32_t*>(U""),   Kw_newline_is_lexem,  0},   // 142: %newline_is_lexem

    {const_cast<char32_t*>(U"cit"),M_Kw_scaner_name,     144}, // 143: %s...
    {const_cast<char32_t*>(U"a"),  M_Kw_scaner_name,     147}, // 144: %sc...
    {const_cast<char32_t*>(U"n"),  M_Kw_single_lined,    148}, // 145: %si...
    {const_cast<char32_t*>(U"r"),  M_Kw_strings,         149}, // 146: %st...
    {const_cast<char32_t*>(U"n"),  M_Kw_scaner_name,     150}, // 147: %sca...
    {const_cast<char32_t*>(U"g"),  M_Kw_single_lined,    151}, // 148: %sin...
    {const_cast<char32_t*>(U"i"),  M_Kw_strings,         152}, // 149: %str...
    {const_cast<char32_t*>(U"e"),  M_Kw_scaner_name,     153}, // 150: %scan...
    {const_cast<char32_t*>(U"l"),  M_Kw_single_lined,    154}, // 151: %sing...
    {const_cast<char32_t*>(U"n"),  M_Kw_strings,         155}, // 152: %stri...
    {const_cast<char32_t*>(U"r"),  M_Kw_scaner_name,     156}, // 153: %scane...
    {const_cast<char32_t*>(U"e"),  M_Kw_single_lined,    157}, // 154: %singl...
    {const_cast<char32_t*>(U"g"),  M_Kw_strings,         158}, // 155: %strin...
    {const_cast<char32_t*>(U"_"),  M_Kw_scaner_name,     159}, // 156: %scaner...
    {const_cast<char32_t*>(U"_"),  M_Kw_single_lined,    160}, // 157: %single...
    {const_cast<char32_t*>(U"s"),  M_Kw_strings,         161}, // 158: %string...
    {const_cast<char32_t*>(U"n"),  M_Kw_scaner_name,     162}, // 159: %scaner_...
    {const_cast<char32_t*>(U"l"),  M_Kw_single_lined,    163}, // 160: %single_...
    {const_cast<char32_t*>(U""),   Kw_strings,           0},   // 161: %strings
    {const_cast<char32_t*>(U"a"),  M_Kw_scaner_name,     164}, // 162: %scaner_n...
    {const_cast<char32_t*>(U"i"),  M_Kw_single_lined,    165}, // 163: %single_l...
    {const_cast<char32_t*>(U"m"),  M_Kw_scaner_name,     166}, // 164: %scaner_na...
    {const_cast<char32_t*>(U"n"),  M_Kw_single_lined,    167}, // 165: %single_li...
    {const_cast<char32_t*>(U"e"),  M_Kw_scaner_name,     168}, // 166: %scaner_nam...
    {const_cast<char32_t*>(U"e"),  M_Kw_single_lined,    169}, // 167: %single_lin...
    {const_cast<char32_t*>(U""),   Kw_scaner_name,       0},   // 168: %scaner_name
    {const_cast<char32_t*>(U"d"),  M_Kw_single_lined,    170}, // 169: %single_line...
    {const_cast<char32_t*>(U""),   Kw_single_lined,      0},   // 170: %single_lined

    {const_cast<char32_t*>(U"o"),  M_Kw_token_fields,    172}, // 171: %t...
    {const_cast<char32_t*>(U"k"),  M_Kw_token_fields,    173}, // 172: %to...
    {const_cast<char32_t*>(U"e"),  M_Kw_token_fields,    174}, // 173: %tok...
    {const_cast<char32_t*>(U"n"),  M_Kw_token_fields,    175}, // 174: %toke...
    {const_cast<char32_t*>(U"_"),  M_Kw_token_fields,    176}, // 175: %token...
    {const_cast<char32_t*>(U"f"),  M_Kw_token_fields,    177}, // 176: %token_...
    {const_cast<char32_t*>(U"i"),  M_Kw_token_fields,    178}, // 177: %token_f...
    {const_cast<char32_t*>(U"e"),  M_Kw_token_fields,    179}, // 178: %token_fi...
    {const_cast<char32_t*>(U"l"),  M_Kw_token_fields,    180}, // 179: %token_fie...
    {const_cast<char32_t*>(U"d"),  M_Kw_token_fields,    181}, // 180: %token_fiel...
    {const_cast<char32_t*>(U"s"),  M_Kw_token_fields,    182}, // 181: %token_field...
    {const_cast<char32_t*>(U""),   Kw_token_fields,      0}    // 182: %token_fields
};

Main_scaner::Automaton_proc Main_scaner::procs[] = {
    &Main_scaner::start_proc,     &Main_scaner::unknown_proc,
    &Main_scaner::id_proc,        &Main_scaner::keyword_proc,
    &Main_scaner::delimiter_proc, &Main_scaner::string_proc
};

Main_scaner::Final_proc Main_scaner::finals[] = {
    &Main_scaner::none_final_proc,
    &Main_scaner::unknown_final_proc,
    &Main_scaner::id_final_proc,
    &Main_scaner::keyword_final_proc,
    &Main_scaner::delimiter_final_proc,
    &Main_scaner::string_final_proc
};

bool Main_scaner::start_proc(){
    bool t = true;
    state = -1;
    /* For an automaton that processes a lexeme, the state with the number (-1) is
     * the state in which this automaton is initialized. */
    if(belongs(Spaces, char_categories)){
        loc->current_line += U'\n' == ch;
        return t;
    }
    lexem_begin_line = loc->current_line;
    if(belongs(Percent, char_categories)){
        automaton = A_keyword; token.code = Unknown;
    }else if(belongs(Id_begin, char_categories)){
        automaton = A_id; buffer = U""; buffer += ch;
        token.code = Id;
    }else if(belongs(Delimiters, char_categories)){
        automaton = A_delimiter; token.code = Unknown;
        (loc->pcurrent_char)--;
    }else if(belongs(Double_quote, char_categories)){
         automaton = A_string; token.code = String;
         buffer = U""; (loc->pcurrent_char)--;
    }else{
        automaton = A_unknown; token.code = Unknown;
    }
    return t;
}

static const char* keyword_strings[] = {
    "%action",             "%class_members",   "%codes",
    "%codes_type",         "%comments",        "%delimiters",
    "%ident_name",         "%idents",          "%keywords",
    "%multilined",         "%nested",          "%newline_is_lexem",
    "%numbers",            "%scaner_name",     "%single_lined",
    "%strings",            "%token_fields",    "%header_additions",
    "%impl_additions",     "%lexem_info_name"
};

void Main_scaner::correct_keyword(){
    /* This function corrects the lexeme code, which is most likely a keyword, and
     * displays the necessary diagnostics. */
    if(token.code >= M_Kw_action){
        int y = token.code - M_Kw_action;
        printf("Line %zu expects %s.\n",
               loc->current_line, keyword_strings[y]);
        token.code = static_cast<Main_lexem_code>(y + Kw_action);
        en -> increment_number_of_errors();
    }
}

Main_lexem_info Main_scaner::current_lexem(){
    automaton = A_start; token.code = None;
    lexem_begin = loc->pcurrent_char;
    bool t = true;
    while((ch = *(loc->pcurrent_char)++)){
        char_categories = get_categories_set(ch); //categories_table[ch];
        t = (this->*procs[automaton])();
        if(!t){
            /* We get here only if the lexeme has already been read. At the same time,
             * the current automaton reads the character immediately after the end of
             * the lexeme read, based on this symbol, it is decided that the lexeme has
             * been read and the transition to the next character has been made.
             * Therefore, in order to not miss the first character of the next lexeme,
             * we need to decrease the pcurrent_char pointer by one. */
            (loc->pcurrent_char)--;
            if(Id == token.code){
                /* If the current lexeme is an identifier, then this identifier must
                 * be written to the identifier table. */
                token.ident_index = ids -> insert(buffer);
            }else if(String == token.code){
                /* If the current token is a string literal, then it must be written
                 * to the string literal table. */
                token.string_index = strs -> insert(buffer);
            }else if(A_keyword == automaton){
                /* If we finish processing the keyword, then we need to adjust its code,
                 * and, perhaps, output the diagnostics.*/
                correct_keyword();
            }
            return token;
        }
    }
    /* Here we can be, only if we have already read all the processed text. In this case,
     * the pointer to the current symbol indicates a byte, which is immediately after the
     * zero character, which is a sign of the end of the text. To avoid entering subsequent
     * calls outside the text, we need to go back to the null character. */
    (loc->pcurrent_char)--;
    /* Further, since we are here, the end of the current token (perhaps unexpected) has
     *not yet been processed. It is necessary to perform this processing, and, probably,
     * to display any diagnostics. */
    (this->*finals[automaton])();
    return token;
}

bool Main_scaner::unknown_proc(){
    return belongs(Other, char_categories);
}

bool Main_scaner::id_proc(){
    bool t = belongs(Id_body, char_categories);
    /* The variable t is true if the identifier is not yet
     * fully read, and false otherwise. */
    if(t){
        buffer += ch;
    }
    return t;
}

/* This array consists of pairs of the form (state, character) and is used to
 * initialize the keyword processing automaton. The sense of the element of the
 * array is this: if the current character in the state (-1) coincides with the
 * second component of the element, then work begins with the state that is the
 * first component of the element. Consider, for example, the element {6, U'c '}.
 * If the current character coincides with the second component of this element,
 * then work begins with the state being the first component, i.e. from state 6.
 * The array must be sorted in ascending order of the second component. */
static const State_for_char init_table_for_keywords[] = {
    {0,   U'a'}, {6,   U'c'}, {34,  U'd'}, {44,  U'h'},
    {60,  U'i'}, {84,  U'k'}, {92,  U'l'}, {107, U'm'},
    {117, U'n'}, {143, U's'}, {171, U't'}
};

bool Main_scaner::keyword_proc(){
    bool t = false;
    if(state != -1){
        Elem elem = a_keyword_jump_table[state];
        token.code = elem.code;
        int y = search_char(ch, elem.symbols);
        if(y != THERE_IS_NO_CHAR){
            state = elem.first_state + y; t = true;
        }
        return t;
    }
    if(belongs(After_percent, char_categories)){
        state = get_init_state(ch, init_table_for_keywords,
                               sizeof(init_table_for_keywords)/
                               sizeof(State_for_char));
        token.code = a_keyword_jump_table[state].code;
        t = true;
    }else{
        printf("In line %zu, one of the following symbols is expected: "
               "a, c, d, i, k, m, n, s, t.\n",
               loc->current_line);
        en -> increment_number_of_errors();
    }
    return t;
}

bool Main_scaner::delimiter_proc(){
    bool t = false;
    switch(ch){
        case U',':
            token.code = Comma;
            break;
        case U':':
            token.code = Colon;
            break;
        case U'{':
            token.code = Opened_fig_brack;
            break;
        case U'}':
            token.code = Closed_fig_brack;
            break;
    }
    (loc->pcurrent_char)++;
    return t;
}

enum {Begin_string = -1, String_body, End_string};
/* These are the state names of the string literals processing automaton. */

bool Main_scaner::string_proc(){
/* This function implements a finite state machine for processing string literals.
 * String literals allowed by Myauka can be described by a regular expression
 *         b(a|bb)*b (1),
 * where b is a double quotation mark ("), and a is any character other than a
 * double quotation mark. In other words, string literals are enclosed in double
 * quotes, and if in a string literal it is necessary to specify such a
 * quotation mark, then it must be doubled. Moreover, from the above regular
 * expression it follows that a string literal can occupy several lines of
 * the source text.
 *
 * Further, if we construct a NFA (a nondeterministic finite state machine) by a
 * this regular expression, then we construct a corresponding DFA (deterministic
 * finite state machine) from the constructed NFA and minimize the resulting DFA,
 * then we obtain a DFA with the following transition table:
 *
 * --------------------------------------------
 * |     state    | a           | b           |
 * --------------------------------------------
 * | Begin_string |             | String_body |
 * --------------------------------------------
 * | String_body  | String_body | End_string  |
 * --------------------------------------------
 * | End_string   |             | String_body |
 * --------------------------------------------
 *
 * In this table, the Begin_string state is initial, and the End_string state
 * is the end state. */
    bool t = true;
    switch(state){
        case Begin_string:
            state = String_body;
            break;
        case String_body:
            if(ch != U'\"'){
                buffer += ch;
            }else{
                state = End_string;
            }
            break;
        case End_string:
            if(U'\"' == ch){
                buffer += ch; state = String_body;
            }else{
                t = false;
            }
            break;
    }
    return t;
}

void Main_scaner::none_final_proc(){
    /* This subroutine will be called if, after reading the input text, it
     * turned out to be in the A_start automaton. Then we do not need to
     * do anything. */
}

void Main_scaner::unknown_final_proc(){
    /* This subroutine will be called if, after reading the input text, it
     * turned out to be in the A_unknown automaton. Then we do not need to
     * do anything. */
}

void Main_scaner::id_final_proc(){
    /* This function will be called if, after reading the input stream, it
     *appeared in the identifier processing automaton, the A_id. Then the
     * identifier must be written to the identifier table. */
    token.ident_index = ids -> insert(buffer);
}

void Main_scaner::keyword_final_proc(){
    token.code = a_keyword_jump_table[state].code;
    correct_keyword();
}

void Main_scaner::delimiter_final_proc(){
}

void Main_scaner::string_final_proc(){
    /* This function will be called if, after reading the input stream, it appeared in
     * the string literals processing automaton. If in this case we are not in the
     * final state of this automaton, then we must display the diagnosis. */
    token.string_index = strs -> insert(buffer);
    if(state != End_string){
        printf("Unexpected end of string literal in line %zu.\n",
               loc->current_line);
        en -> increment_number_of_errors();
    }
}