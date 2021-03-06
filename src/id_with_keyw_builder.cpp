/*
    File:    id_with_keyw_builder.cpp
    Created: 14 February 2018 at 08:32 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include <string>
#include <iterator>
#include <algorithm>
#include "../include/id_with_keyw_builder.h"
#include "../include/regexp_match.h"
#include "../include/u32strings_to_commands.h"
#include "../include/format.h"
#include "../include/attributed_char_trie.h"
#include "../include/add_category.h"
#include "../include/first_chars.h"
#include "../include/operations_with_sets.h"
#include "../include/jump_table_string_repres.h"
#include "../include/automata_repres_builder.h"
#include "../include/generate_separate_identifier_automat.h"
#include "../include/generate_separate_keywords_automaton.h"
#include "../include/u32strings_to_commands.h"
#include "../include/regexp1_or_regexp2.h"
#include "../include/list_to_columns.h"
#include "../include/char_conv.h"
#include "../include/idx_to_string.h"

using namespace std::string_literals;
using operations_with_sets::operator-;

// #define DEBUG_MODE

static const std::string ident_aut_name                  = "A_ident"s;
static const std::string ident_aut_proc_proto            = "bool ident_proc()"s;
static const std::string ident_aut_proc_ptr_fmt          = "&{0}::ident_proc"s;
static const std::string ident_aut_final_proc_proto      = "void ident_final_proc()"s;
static const std::string ident_aut_final_proc_ptr_fmt    = "&{0}::ident_final_proc"s;

static const std::string keyword_aut_name                   = "A_keyword"s;
static const std::string keyword_aut_proc_proto             = "bool keyword_proc()"s;
static const std::string keyword_aut_proc_ptr_fmt           = "&{0}::keyword_proc"s;
static const std::string keyword_aut_final_proc_proto       = "void keyword_final_proc()"s;
static const std::string keyword_aut_final_proc_ptr_fmt     = "&{0}::keyword_final_proc"s;

static const std::string keyword_begin_cat_name_by_default  = "KEYWORD_BEGIN"s;
static const std::string ident_begin_cat_name_by_default    = "IDENTIFIER_BEGIN"s;
static const std::string idkeyw_begin_cat_name_by_default   = "IDKEYWORD_BEGIN"s;
static const std::string after_keywords_cat_name_by_default = "AFTER_KEYWORDS"s;
static const std::string keyword_if_fmt = R"~(
    if(belongs({0}, char_categories)){{
        (loc->pcurrent_char)--;
        automaton = A_keyword;
        state     = -1;
        return t;
    }}
)~"s;

static const std::string keyw_jump_table_name               = "keyword_jump_table"s;
static const std::string keyword_init_table_name            = "init_table_for_keywords"s;

static const std::string keyword_aut_proc_impl_fmt          =
    R"~(bool {0}::keyword_proc(){{
    bool t = false;
    if(-1 == state){{
        state = get_init_state(ch, init_table_for_delimiters,
                               sizeof(init_table_for_keywords)/sizeof(State_for_char));
        token.code = keyword_jump_table[state].code;
        t = true;
        return t;
    }}
    Elem elem  = keyword_jump_table[state];
    token.code = elem.code;
    int y = search_char(ch, elem.symbols);
    if(y != THERE_IS_NO_CHAR){{
        state  =  elem.first_state + y;
        t      =  true;
        buffer += ch;
    }}else if(belongs({2}, char_categories)){{
        automaton  = A_ident;
        token.code = {3};
        t          = true;
    }}else{{
        {1}
    }}
    return t;
)~"s;

static const std::string postaction_fmt                     =
    R"~(    if(!t){{
        {0};
    }})~"s;

static const std::string keyword_aut_final_proc_fmt         =
    R"~(void {0}::keyword_final_proc(){{
    {1}token.code = keyword_jump_table[state].code;
}})~"s;

static const std::string ident_if_fmt = R"~(
    if(belongs({0}, char_categories)){{
        (loc->pcurrent_char)--;
        {1}
        automaton = A_ident;
        state     = -1;
        return t;
    }}
)~"s;

static const std::string add_ident_to_table                 =
    "\n        token.code = {0};"
    "\n        token.ids = ids_trie->insert(buffer);"s;

static const std::string ident_aut_final_proc_fmt           =
    R"~(void {0}::string_final_proc(){{
    {1}token.ident_index = ids->insert(buffer);
}})~"s;

static const std::string idkeyw_aut_name                    = "A_idkeyw"s;
static const std::string idkeyw_aut_proc_proto              = "bool idkeyw_proc()"s;
static const std::string idkeyw_aut_proc_ptr_fmt            = "&{0}::idkeyw_proc"s;
static const std::string idkeyw_aut_final_proc_proto        = "void idkeyw_final_proc()"s;
static const std::string idkeyw_aut_final_proc_ptr_fmt      = "&{0}::idkeyw_final_proc"s;
static const std::string idkeyw_aut_diagnostic_msg          =
    "At line %zu unexpectedly ended an identifier or a keyword.\\n"s;

static const std::string idkeyw_if_fmt = R"~(
    if(belongs({0}, char_categories)){{
        (loc->pcurrent_char)--;
        {1}
        automaton = A_idkeyw;
        state     = -1;
        return t;
    }}
)~"s;

static std::string keyword_automaton_impl_finals(const info_for_constructing::Info&  info)
{
    std::string result;
    auto keyword_postact = info.keywords_postaction;
    result           = fmt::format(keyword_aut_final_proc_fmt,
                                   info.names.name_of_scaner_class,
                                   keyword_postact.empty() ? keyword_postact :
                                                         (keyword_postact + '\n'));
    return result;
}

std::string add_category_wrapper(info_for_constructing::Info& info,
                                 const std::set<char32_t>&    s,
                                 const std::string&           default_name)
{
    auto cat_res = add_category(info, s, default_name);
    auto result  = cat_res.second;
    return result;
}

using Keyword_and_code = std::pair<std::u32string, std::string>;
/* The first element of this pair is a string representation of the keyword, and the
 * second element is a string representation of the identifier that is the
 * corresponding lexeme code. */

using Keywords_and_codes = std::vector<Keyword_and_code>;

struct Id_with_keyw_builder::Impl{
public:
    Impl(const Trie_for_set_of_char32ptr& sets,
         const Errors_and_tries&          et,
         const std::shared_ptr<Scope>&    scope) :
       sets_(sets), et_(et), scope_(scope), keyword_strings_() {};

    Impl()            = default;
    ~Impl()           = default;
    Impl(const Impl&) = default;

    void all_keywords_are_idents_case(info_for_constructing::Info& info);
    void not_all_keywords_are_idents_case(info_for_constructing::Info& info,
                                          const Command_buffer&        regexp,
                                          const Match_result&          t);
    void build_keyword_strings(const info_for_constructing::Info& info);
    Match_result check_keywords(const Command_buffer& regexp);
private:
    Trie_for_set_of_char32ptr   sets_;
    Errors_and_tries            et_;
    std::shared_ptr<Scope>      scope_;
    std::vector<std::u32string> keyword_strings_;
    std::set<char32_t>          idents_first_chars_whithout_keyws_first_chars_;

    using INFO = info_for_constructing::Info;
    Jumps_and_inits keywords_jumps(const INFO& info);
    std::string keyword_automaton_impl(INFO& info);
    void generate_glued_identifier_and_keywords_automata(INFO&                 info,
                                                         const Command_buffer& regexp);

    std::string build_table_of_keywords(const INFO& info);

    Keywords_and_codes build_keywords_with_codes(const INFO& info);
};

template<class InputIterator>
using elem_type = typename std::iterator_traits<InputIterator>::value_type::value_type;

template<class InputIterator>
using elems_set = std::set<elem_type<InputIterator>>;

template<class InputIterator>
auto first_elems(InputIterator first, InputIterator last) -> elems_set<InputIterator>
{
    using value_type = typename std::iterator_traits<InputIterator>::value_type;
    elems_set<InputIterator> result;
    for(auto it = first; it != last; ++it){
        value_type& t = *it;
        result.insert(t[0]);
    }
    return result;
}

template<class InputIterator>
auto all_elems(InputIterator first, InputIterator last) -> elems_set<InputIterator>
{
    using value_type = typename std::iterator_traits<InputIterator>::value_type;
    elems_set<InputIterator> result;
    for(auto it = first; it != last; ++it){
        value_type& t = *it;
        for(const auto& elem : t){
            result.insert(elem);
        }
    }
    return result;
}

static const std::string keyword_list_fmt = R"~(struct Keyword_list_elem{{
    std::u32string keyword;
    {0}            kw_code;
}};

static const Keyword_list_elem kwlist[] = {{
{1}
}};

static constexpr ssize_t THERE_IS_NO_KEYWORD = -1;

static constexpr ssize_t NUM_OF_KEYWORDS = size(kwlist);

static ssize_t search_keyword(const std::u32string& finded_keyword)
{{
    ssize_t result      = THERE_IS_NO_KEYWORD;
    ssize_t low_bound   = 0;
    ssize_t upper_bound = NUM_OF_KEYWORDS - 1;
    ssize_t middle;
    while(low_bound <= upper_bound){{
        middle             = (low_bound + upper_bound) / 2;
        auto& curr_kw      = kwlist[middle].keyword;
        int compare_result = finded_keyword.compare(curr_kw);
        if(0 == compare_result){{
            return middle;
        }}
        if(compare_result < 0){{
            upper_bound = middle - 1;
        }}else{{
            low_bound   = middle + 1;
        }}
    }}
    return result;
}}

)~"s;

static const std::string kwlist_elem_fmt = R"~({{U"{0}", {1}}})~"s;
static constexpr size_t  INDENT_WIDTH    = 4;
static const std::string idkeyword_final_actions_fmt =
    R"~({0}ssize_t search_result = search_keyword(buffer);
        if(search_result != THERE_IS_NO_KEYWORD){{{{
            token.code = kwlist[search_result].kw_code;
        }}}}else{{{{
            token.code = {2}::{1};
            token.ids  = ids_trie->insert(buffer);
        }}}}
)~"s;

static const std::string idkeyw_final_proc_impl_fmt =
    R"~(void {0}::idkeyw_final_proc(){{
    if(!is_elem(state, final_states_for_idkeywords)){{
        printf("{1}", loc->current_line);
        en->increment_number_of_errors();
        {2}ssize_t search_result = search_keyword(buffer);
        if(search_result != THERE_IS_NO_KEYWORD){{
            token.code = kwlist[search_result].kw_code;
        }}else{{
            token.code = {4}::{3};
            token.ids  = ids_trie->insert(buffer);
        }}
    }}
}})~"s;

static const std::string spaces_after_actions = ";\n        "s;

std::string keyword_list(const Keywords_and_codes& kwcs)
{
    std::string              result;
    std::vector<std::string> kwl;
    for(const auto& k : kwcs){
        auto temp = fmt::format(kwlist_elem_fmt, u32string_to_utf8(k.first), k.second);
        kwl.push_back(temp);
    }

    Format f;
    f.indent                 = INDENT_WIDTH;
    f.number_of_columns      = 2;
    f.spaces_between_columns = 1;

    result = string_list_to_columns(kwl, f);
    return result;
}

Keywords_and_codes Id_with_keyw_builder::Impl::
    build_keywords_with_codes(const INFO& info)
{
    Keywords_and_codes result;
    result.reserve(keyword_strings_.size());
    size_t i = 0;
    for(size_t  kw_idx : info.kw_repres){
        auto  num_value_of_code   = (scope_->strsc[kw_idx]).code;
        auto  str_repres_for_code = fmt::format("{0}::{1}"s,
                                                info.names.codes_type_name,
                                                info.lexem_codes_names[num_value_of_code]);
        auto& keyword             = keyword_strings_[i];
        auto temp                 = std::make_pair(keyword, str_repres_for_code);
        result.push_back(temp);
        ++i;
    }
    return result;
}

std::string Id_with_keyw_builder::Impl::build_table_of_keywords(const INFO& info)
{
    std::string result;
    Keywords_and_codes kwcs = build_keywords_with_codes(info);
    std::sort(kwcs.begin(), kwcs.end(),
              [](const Keyword_and_code& k1, const Keyword_and_code& k2){
                  return k1.first < k2.first;
    });
    result = fmt::format(keyword_list_fmt,
                         info.names.codes_type_name,
                         keyword_list(kwcs));
    return result;
}

void Id_with_keyw_builder::Impl::
    generate_glued_identifier_and_keywords_automata(INFO&                 info,
                                                    const Command_buffer& regexp)
{
    Automaton_constructing_info result;
    auto regexp_for_keywords =
        u32strings_to_commands(keyword_strings_.begin(), keyword_strings_.end());
    /* Next, we set action_name = write in all commands of keywords regexp, and
     * glue an identifier regexp and keywords regexp. */
    for(auto& com : regexp_for_keywords){
        com.action_name = info.write_action_name_idx;
    }
    auto glued_regexp        = regexp1_or_regexp2(regexp, regexp_for_keywords);
    auto idkeyw_first_chars  = first_chars(glued_regexp, sets_).s;

    result.name               = idkeyw_aut_name;
    result.proc_proto         = idkeyw_aut_proc_proto;
    auto& scaner_name         = info.names.name_of_scaner_class;
    result.proc_ptr           = fmt::format(idkeyw_aut_proc_ptr_fmt, scaner_name);

    auto idkeyw_beg_cat_name = add_category_wrapper(info,
                                                    idkeyw_first_chars,
                                                    idkeyw_begin_cat_name_by_default);
    auto idkeyw_if           = fmt::format(idkeyw_if_fmt,
                                           idkeyw_beg_cat_name,
                                           info.identifier_preactions);
    info.ifs_of_start_procs.push_back(idkeyw_if);
    result.final_proc_proto  = idkeyw_aut_final_proc_proto;
    auto keywords_table      = build_table_of_keywords(info);
    Str_data_for_automaton      f;
    f.automata_name          = idkeyw_aut_name;
    f.proc_name              = "idkeyw_proc"s;
    f.category_name_prefix   = "IDKEYWORD"s;
    f.diagnostic_msg         = idkeyw_aut_diagnostic_msg;
    f.final_states_set_name  = "final_states_for_idkeywords"s;
    auto& id_postacts        = info.identifier_postactions;
    auto idkeyw_postactions  = id_postacts.empty() ?
                               id_postacts :
                               (id_postacts + spaces_after_actions);
    f.final_actions          = fmt::format(idkeyword_final_actions_fmt,
                                           idkeyw_postactions,
                                           info.names.ident_name,
                                           info.names.codes_type_name);
    Automata_repres_builder repres_builder {f, sets_, et_, scope_};
    result.proc_impl         = keywords_table +
                               repres_builder.build_repres(info, glued_regexp);
    result.final_proc_ptr    = fmt::format(idkeyw_aut_final_proc_ptr_fmt,
                                           scaner_name);
    auto final_proc_impl     = fmt::format(idkeyw_final_proc_impl_fmt,
                                           scaner_name,
                                           idkeyw_aut_diagnostic_msg,
                                           info.keywords_postaction,
                                           info.names.ident_name,
                                           info.names.codes_type_name);
    result.final_proc_impl   = final_proc_impl;
    info.automata_info.push_back(result);
}

void Id_with_keyw_builder::Impl::
    not_all_keywords_are_idents_case(info_for_constructing::Info& info,
                                     const Command_buffer&        regexp,
                                     const Match_result&          t)
{
    using operations_with_sets::operator*;
    auto first_chars_for_keywords =
        first_elems(keyword_strings_.begin(), keyword_strings_.end());
    auto first_chars_for_idents   = first_chars(regexp, sets_).s;
    auto common_chars             = first_chars_for_keywords * first_chars_for_idents;
    if((!t.is_any) && common_chars.empty()){
        generate_separate_identifier_automat(info, et_, sets_, scope_);
        generate_separate_keywords_automaton(info, et_, scope_);
    }else{
        generate_glued_identifier_and_keywords_automata(info, regexp);
    }
}

Match_result Id_with_keyw_builder::Impl::check_keywords(const Command_buffer& regexp)
{
    auto result = match_regexp(regexp,                   sets_,
                               keyword_strings_.begin(), keyword_strings_.end());
#ifdef DEBUG_MODE
    bool t = regexp_match(regexp,                   sets_,
                          keyword_strings_.begin(), keyword_strings_.end());
    if(t){
        puts("All keywords are identifiers.");
    }else{
        puts("There is keyword that is not identifier.");
    }
#endif
    return result;
}

void Id_with_keyw_builder::Impl::
    build_keyword_strings(const info_for_constructing::Info&  info)
{
    for(size_t kw_idx : info.kw_repres){
        auto keyword = et_.strs_trie->get_string(kw_idx);
        keyword_strings_.push_back(keyword);
    }
#ifdef DEBUG_MODE
    puts("Keywords are:");
    for(const auto& s : keyword_strings_){
        auto su8 = u32string_to_utf8(s);
        puts(su8.c_str());
    }
    puts("*******************************************");
#endif
}

Jumps_and_inits Id_with_keyw_builder::Impl::
    keywords_jumps(const info_for_constructing::Info&  info)
{
    Attributed_char_trie        atrie;
    size_t counter = 0;
    for(size_t kw_idx : info.kw_repres){
        Attributed_cstring atrib_cstr;
        atrib_cstr.str       = const_cast<char32_t*>(keyword_strings_[counter].c_str());
        atrib_cstr.attribute = (scope_->strsc[kw_idx]).code;
        atrie.insert(attributed_cstring2string(atrib_cstr, 1));
        counter++;
    }

    Jumps_and_inits jmps = atrie.jumps(); /* We built a sketch for
                                             the transition table. */
    return jmps;
}

std::string Id_with_keyw_builder::Impl::
    keyword_automaton_impl(info_for_constructing::Info&  info)
{
    std::string result;
    std::set<char32_t>          first_chars_for_keywords;
    first_chars_for_keywords = first_elems(keyword_strings_.begin(), keyword_strings_.end());
    idents_first_chars_whithout_keyws_first_chars_ =
        first_chars(info.regexps.idents, sets_).s - first_chars_for_keywords;
    auto symbols_after_keywords =
        symbols_after_last_chars_of_strings(info.regexps.idents,      sets_,
                                            keyword_strings_.begin(), keyword_strings_.end());
    auto jmps = keywords_jumps(info);
    /* Now we need to add the desired text to the implementation of the start automaton
     * and generate a function that handles the delimiters. */
    auto keyword_begin_cat_name          = add_category_wrapper(info,
                                                                first_chars_for_keywords,
                                                                keyword_begin_cat_name_by_default);
    auto keyword_if                      = fmt::format(keyword_if_fmt,
                                                       keyword_begin_cat_name);
    info.ifs_of_start_procs.push_back(keyword_if);
    auto keyword_postact                     = info.keywords_postaction;
    std::string postaction_code_in_proc  = keyword_postact.empty() ?
                                           std::string() :
                                           fmt::format(postaction_fmt, keyword_postact);

    auto jump_table_repres = jump_table_string_repres(info,
                                                      jmps,
                                                      keyw_jump_table_name,
                                                      keyword_init_table_name);
    auto after_keywords_cat_name          = add_category_wrapper(info,
                                                                 symbols_after_keywords.s,
                                                                 after_keywords_cat_name_by_default);
    result = jump_table_repres +
             fmt::format(keyword_aut_proc_impl_fmt,
                         info.names.name_of_scaner_class,
                         postaction_code_in_proc,
                         after_keywords_cat_name,
                         info.names.ident_name);
    return result;
}

static std::string ident_automaton_impl_finals(const info_for_constructing::Info&  info)
{
    std::string result;
    auto ident_postact = info.identifier_preactions;
    result             = fmt::format(ident_aut_final_proc_fmt,
                                     info.names.name_of_scaner_class,
                                     ident_postact.empty() ? ident_postact :
                                                             (ident_postact + '\n'));
    return result;
}

void Id_with_keyw_builder::Impl::
    all_keywords_are_idents_case(info_for_constructing::Info& info)
{
    info.needed_Elem          = true;
    Automaton_constructing_info result;
    result.name               = keyword_aut_name;
    result.proc_proto         = keyword_aut_proc_proto;
    auto& scaner_name         = info.names.name_of_scaner_class;
    result.proc_ptr           = fmt::format(keyword_aut_proc_ptr_fmt, scaner_name);
    result.proc_impl          = keyword_automaton_impl(info);
    result.final_proc_proto   = keyword_aut_final_proc_proto;
    result.final_proc_ptr     = fmt::format(keyword_aut_final_proc_ptr_fmt,
                                            scaner_name);
    result.final_proc_impl    = keyword_automaton_impl_finals(info);
    info.automata_info.push_back(result);

    /* Next we add a idetifier automaton. */
    result.name               = ident_aut_name;
    result.proc_proto         = ident_aut_proc_proto;
    result.proc_ptr           = fmt::format(ident_aut_proc_ptr_fmt, scaner_name);
    auto ident_begin_cat_name =
        add_category_wrapper(info,
                             idents_first_chars_whithout_keyws_first_chars_,
                             ident_begin_cat_name_by_default);
    result.final_proc_proto   = ident_aut_final_proc_proto;
    auto ident_if             = fmt::format(ident_if_fmt,
                                            ident_begin_cat_name,
                                            info.identifier_preactions);
    info.ifs_of_start_procs.push_back(ident_if);

    Str_data_for_automaton      f;
    f.automata_name           = ident_aut_name;
    f.proc_name               = "ident_proc"s;
    f.category_name_prefix    = "IDENTIFIER"s;
    f.diagnostic_msg          = "At line %zu unexpectedly ended an identifier.\\n";
    f.final_states_set_name   = "final_states_for_idents";
    f.final_actions           = info.identifier_postactions +
                                fmt::format(add_ident_to_table, info.names.ident_name);
    Automata_repres_builder repres_builder {f, sets_, et_, scope_};
    result.proc_impl          = repres_builder.build_repres(info,
                                                            info.regexps.idents);
    result.final_proc_ptr     = fmt::format(ident_aut_final_proc_ptr_fmt, scaner_name);
    result.final_proc_impl    = ident_automaton_impl_finals(info);
    info.automata_info.push_back(result);
}

// void Id_with_keyw_builder::some_keywords_are_not_idents_case(info_for_constructing::Info& info)
// {}
//
// void Id_with_keyw_builder::all_keywords_are_not_idents_case(info_for_constructing::Info& info)
// {}

/*
 * If any keyword is an identifier, then
 *      1) we create a keywords automaton;
 *      2) we create an identifiers automaton;
 *      3) if the keywords automaton finds the character
 *         for which there are no transitions, then
 *          a) if the current character is admissible for an identifier automaton,
 *             then we go to the identifier automaton;
 *          b) otherwise, we returns a lexeme code from a transition table of
 *             the keywords automaton.
 *
 * If there are keywords that are not identifiers, then we glue the identifier
 * regexp and keywords in one regexp. Next, we create a minimal DFA by this glued
 * regexp, and generate an implementation of the automaton. If the automaton finished,
 * then we search the contents of the member buffer into a table of keywords, using
 * a binary search algorithm.
 */
void Id_with_keyw_builder::build(info_for_constructing::Info& info)
{
    auto& id_regexp                = info.regexps.idents;
    impl_->build_keyword_strings(info);

    auto t = impl_->check_keywords(id_regexp);
#ifdef DEBUG_MODE
    printf("t.is_all is %s\n", t.is_all ? "true" : "false");
#endif
    if(t.is_all){
        impl_->all_keywords_are_idents_case(info);
    }else{
        impl_->not_all_keywords_are_idents_case(info, id_regexp, t);
    }
}

Id_with_keyw_builder::~Id_with_keyw_builder() = default;

Id_with_keyw_builder::Id_with_keyw_builder() : impl_(std::make_unique<Impl>()) {}

Id_with_keyw_builder::Id_with_keyw_builder(const Trie_for_set_of_char32ptr& sets,
                                           const Errors_and_tries&          et,
                                           const std::shared_ptr<Scope>&    scope) :
    impl_(std::make_unique<Impl>(sets, et, scope)) {}
