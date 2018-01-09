/*
    File:    main_parser.cpp
    Created: 20 November 2017 at 18:26 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include <cstddef>
#include "../include/main_parser.h"
#include "../include/errors_and_tries.h"
#include "../include/char_trie.h"
#include "../include/scope.h"
#include "../include/expr_scaner.h"
// #include "../include/simple_regex_parser.h"
#include "../include/expr_lr_tables.h"
#include "../include/slr_act_expr_parser.h"
#include "../include/codes_and_newline.h"
#include "../include/trie_for_set.h"
#include "../include/used_automaton.h"
#include "../include/additions_parser.h"
#include "../include/lexem_info_name_parser.h"
#include "../include/keyword_delimiter_parser.h"
#include "../include/num_str_id_parser.h"
#include "../include/comments_parser.h"
#include "../include/collected_data.h"
#include "../include/verify_collected_data.h"
#include "../include/aux_files_generate.h"
#include "../include/collected_data_to_info.h"

static const char32_t* none_string    = U"None";
static const char32_t* unknown_string = U"Unknown";
static const char32_t* write_act_name = U"write";
static const char32_t* write_act_body = U"buffer += ch;";

// #define DEBUG_MODE
struct Parsers_ptrs{
public:
    Parsers_ptrs()                    = default;
    Parsers_ptrs(const Parsers_ptrs&) = default;
    Parsers_ptrs(const Location_ptr& loc);
    ~Parsers_ptrs()                   = default;

    Errors_and_tries                        et_;
    std::shared_ptr<Scope>                  scope_;
    Trie_for_set_of_char32ptr               char32_sets_;
    std::shared_ptr<Main_scaner>            msc_;
    std::shared_ptr<Expr_scaner>            expr_sc_;
    std::shared_ptr<SLR_act_expr_parser>    regexp_parser_;
    std::shared_ptr<Codes_and_newline>      codes_and_newline_;
    std::shared_ptr<KW_parser>              keywords_sec_parser_;
    std::shared_ptr<KW_parser>              delimiters_sec_parser_;
    std::shared_ptr<NSI_parser>             num_sec_parser_;
    std::shared_ptr<NSI_parser>             strs_sec_parser_;
    std::shared_ptr<NSI_parser>             id_definition_parser_;
    std::shared_ptr<Comments_parser>        comments_parser_;
    std::shared_ptr<Additions_parser>       additions_parser_;
    std::shared_ptr<Lexem_info_name_parser> lexem_info_name_parser_;
};

Parsers_ptrs::Parsers_ptrs(const Location_ptr& loc)
{
    et_.ec                  = std::make_shared<Error_count>();
    et_.ids_trie            = std::make_shared<Char_trie>();
    et_.strs_trie           = std::make_shared<Char_trie>();
#ifdef DEBUG_MODE
    printf("et_.ec        : %p\n", et_.ec.get());
    printf("et_.ids_trie  : %p\n", et_.ids_trie.get());
    printf("et_.strs_trie : %p\n", et_.strs_trie.get());
#endif
    scope_                  = std::make_shared<Scope>();
#ifdef DEBUG_MODE
    printf("scope_        : %p\n", scope_.get());
#endif
    char32_sets_            = std::make_shared<Trie_for_set_of_char32>(); //Trie_for_set_of_char32ptr();
#ifdef DEBUG_MODE
    printf("char32_sets_  : %p\n", char32_sets_.get());
#endif
    msc_                    = std::make_shared<Main_scaner>(loc, et_);
    expr_sc_                = std::make_shared<Expr_scaner>(loc, et_, char32_sets_);
    regexp_parser_          = std::make_shared<SLR_act_expr_parser>(expr_sc_,
                                                                    et_,
                                                                    scope_,
                                                                    expr_slr_tables);
    codes_and_newline_      = std::make_shared<Codes_and_newline>(scope_, et_, msc_);
    keywords_sec_parser_    = std::make_shared<Keyword_parser>(scope_, et_, msc_);
    delimiters_sec_parser_  = std::make_shared<Delimiter_parser>(scope_, et_, msc_);
    num_sec_parser_         = std::make_shared<Number_parser>(scope_, et_,
                                                              msc_,   regexp_parser_);
    strs_sec_parser_        = std::make_shared<String_parser>(scope_, et_,
                                                              msc_,   regexp_parser_);
    id_definition_parser_   = std::make_shared<Idents_parser>(scope_, et_,
                                                              msc_,   regexp_parser_);
    comments_parser_        = std::make_shared<Comments_parser>(et_, msc_);
    additions_parser_       = std::make_shared<Additions_parser>(et_, msc_);
    lexem_info_name_parser_ = std::make_shared<Lexem_info_name_parser>(et_, msc_);
}

enum class Name_sec{
    Scaner_name_sec, Lexem_code_name_sec, Id_name_sec
};

struct Main_parser::Main_parser_data{
public:
    Main_parser_data()                         = default;
    Main_parser_data(const Location_ptr& loc) : parsers_(Parsers_ptrs(loc)) {init();}
    Main_parser_data(const Main_parser_data&)  = default;
    ~Main_parser_data()                        = default;

    void parse();
    int get_number_of_errors() const
    {
        return parsers_.et_.ec->get_number_of_errors();
    };

    Parsers_ptrs    parsers_;
    Main_lexem_info li_;
    Main_lexem_code lc_;
    Collected_data  data_;

private:
    void init();

    /* The following functions process individual sections of the input text. In this
     * case, since the sections %scaner_name, %codes_type, and %ident_name have the
     * same structure, one function, name_section_handling, which takes the name of
     * the section and the place to which the index (in the prefix tree of identifiers)
     * of the corresponding identifier should be written, can be used to process
     * these three sections. */
    void name_section_handling(size_t& ind, Name_sec section);
    void set_default_name(size_t& ind, Name_sec section);

    bool id_is_defined(size_t idx);
    void token_fields_sec();
    void class_members_sec();
};

void Main_parser::Main_parser_data::init()
{
    Id_attributes iattr;
    iattr.kind                            = Code_of_lexem;
    size_t idx                            = parsers_.et_.ids_trie->insert(none_string);
    iattr.code                            = 0;
    data_.codes_.push_back(idx);
    parsers_.scope_->idsc[idx]            = iattr;
    idx                                   = parsers_.et_.ids_trie->insert(unknown_string);
    iattr.code                            = 1;
    data_.codes_.push_back(idx);
    parsers_.scope_->idsc[idx]            = iattr;

    iattr.kind                            = Action_name;
    idx                                   = parsers_.et_.ids_trie->insert(write_act_name);
    data_.indeces_.write_action_name_idx  = idx;
    size_t body_idx                       = parsers_.et_.strs_trie->insert(write_act_body);
    iattr.act_string                      = body_idx;
    parsers_.scope_->idsc[idx]            = iattr;
    Str_attributes sattr;
    sattr.kind                            = Action_definition;
    sattr.code                            = 0;
    parsers_.scope_->strsc[body_idx] = sattr;
}


static const Main_lexem_code expected_keywords[] = {
    Kw_scaner_name, Kw_codes_type, Kw_ident_name
};

static const Id_kind name_attribute[] = {
    Scaner_name, Codes_type_name, Lexem_ident_name
};

static const char* expected_keywords_strings[] = {
    "%scaner_name", "%codes_type", "%ident_name"
};

static const char32_t* default_names[] = {
    U"Scaner", U"Lexem_code", U"Id"
};

static const char* messages[] = {
    "Line %zu repeatedly defines the identifier ",
    "Keyword %s expected at line %zu.\n",
    "Keyword %%token_fields is expected at line %zu.\n",
    "A string literal with the added lexeme fields is expected at line %zu.\n",
    "Keyword %%class_members is expected at line %zu.\n",
    "A string literal with the added class members is expected at line %zu.\n"
};

enum class Message_id{
    Id_repeated,                  Keyword_expected,       Token_fields_expected,
    Added_lexeme_fields_expected, Class_members_expected, Added_class_members_expected
};

bool Main_parser::Main_parser_data::id_is_defined(size_t idx){
    bool is_def = false;
    auto s = parsers_.scope_->idsc.find(idx);
    if(s != parsers_.scope_->idsc.end()){
        printf(messages[static_cast<size_t>(Message_id::Id_repeated)],
               parsers_.msc_->lexem_begin_line_number());
        parsers_.et_.ids_trie->print(idx); printf("\n");
        parsers_.et_.ec->increment_number_of_errors();
        is_def = true;
    }
    return is_def;
}

void Main_parser::Main_parser_data::set_default_name(size_t& ind, Name_sec section)
{
    size_t          section_as_int = static_cast<size_t>(section);
    char32_t*       default_name   = const_cast<char32_t*>(default_names[section_as_int]);
    Id_kind         attribute      = name_attribute[section_as_int];
    Id_attributes   iattr;

    size_t idx = parsers_.et_.ids_trie->insert(default_name);
    iattr.kind = attribute;
    parsers_.scope_->idsc[idx] = iattr;
    ind        = idx;
}

void Main_parser::Main_parser_data::name_section_handling(size_t& ind, Name_sec section)
{
    size_t          section_as_int   = static_cast<size_t>(section);
    Main_lexem_code expected_keyword = expected_keywords[section_as_int];
    Id_kind         attribute        = name_attribute[section_as_int];
    Id_attributes   iattr;

    lc_ = (li_ = parsers_.msc_-> current_lexem()).code;
    if(Id == lc_){
        printf(messages[static_cast<size_t>(Message_id::Keyword_expected)],
               expected_keywords_strings[section_as_int],
               parsers_.msc_->lexem_begin_line_number());
        parsers_.et_.ec->increment_number_of_errors();
        iattr.kind = attribute;
        size_t idx = li_.ident_index;
        if(id_is_defined(idx)){return;};
        parsers_.scope_->idsc[idx] = iattr; ind = idx;
        return;
    }
    if(lc_ != expected_keyword){
        parsers_.msc_->back();
        if(Name_sec::Id_name_sec != section){
            set_default_name(ind, section);
        }
        return;
    }

    lc_ = (li_ = parsers_.msc_-> current_lexem()).code;
    if(Id == lc_){
        iattr.kind = attribute;
        size_t idx = li_.ident_index;
        if(id_is_defined(idx)){return;};
        parsers_.scope_->idsc[idx] = iattr; ind = idx;
        if(Name_sec::Id_name_sec == section){
            data_.aut_data_.set_of_used_automata |= 1ULL << Id_aut;
        };
        return;
    }

    parsers_.msc_->back();
    if(Name_sec::Id_name_sec != section){
        set_default_name(ind, section);
    };
    return;
}

void Main_parser::Main_parser_data::token_fields_sec(){
    lc_ = (li_ = parsers_.msc_-> current_lexem()).code;
    if(Kw_token_fields != lc_){
        if(String == lc_){
            printf(messages[static_cast<size_t>(Message_id::Token_fields_expected)],
                   parsers_.msc_->lexem_begin_line_number());
            parsers_.et_.ec->increment_number_of_errors();
            data_.indeces_.token_fields_idx = li_.string_index;
        }else{
            parsers_.msc_->back();
        };
        return;
    }

    lc_ = (li_ = parsers_.msc_-> current_lexem()).code;
    if(String == lc_){
        data_.indeces_.token_fields_idx = li_.string_index;
    }else{
        printf(messages[static_cast<size_t>(Message_id::Added_lexeme_fields_expected)],
               parsers_.msc_->lexem_begin_line_number());
        parsers_.et_.ec->increment_number_of_errors();
        parsers_.msc_->back();
    }
}

void Main_parser::Main_parser_data::class_members_sec()
{
    lc_ = (li_ = parsers_.msc_-> current_lexem()).code;
    if(Kw_class_members != lc_){
        if(String == lc_){
            printf(messages[static_cast<size_t>(Message_id::Class_members_expected)],
                   parsers_.msc_->lexem_begin_line_number());
            parsers_.et_.ec->increment_number_of_errors();
            data_.indeces_.class_members_idx = li_.string_index;
        }else{
            parsers_.msc_->back();
        };
        return;
    }

    lc_ = (li_ = parsers_.msc_-> current_lexem()).code;
    if(String == lc_){
        data_.indeces_.class_members_idx = li_.string_index;
    }else{
        printf(messages[static_cast<size_t>(Message_id::Added_class_members_expected)],
               parsers_.msc_->lexem_begin_line_number());
        parsers_.et_.ec->increment_number_of_errors();
        parsers_.msc_->back();
    }
}

void Main_parser::Main_parser_data::parse()
{
    data_.postactions_.delimiters_postaction = 0;
    data_.postactions_.keywords_postaction   = 0;
    while((lc_ = (li_ = parsers_.msc_-> current_lexem()).code)){
        if(Unknown == lc_){
            continue;
        };
        parsers_.msc_->back();
        switch(lc_){
            case Kw_scaner_name:
                name_section_handling(data_.indeces_.scaner_name_idx,
                                      Name_sec::Scaner_name_sec);
                break;
            case Kw_codes_type:
                name_section_handling(data_.indeces_.codes_type_name_idx,
                                      Name_sec::Lexem_code_name_sec);
                break;
            case Kw_ident_name:
                name_section_handling(data_.indeces_.ident_name_idx,
                                      Name_sec::Id_name_sec);
                break;
            case Kw_token_fields:
                token_fields_sec();
                break;
            case Kw_class_members:
                class_members_sec();
                break;
            case Kw_newline_is_lexem:
                parsers_.codes_and_newline_->newline_is_lexem_sec();
                data_.newline_is_lexem_ = true;
                break;
            case Kw_codes: case Comma: case Id:
                parsers_.codes_and_newline_->codes_sec(data_.codes_,
                                                       data_.last_code_val_);
                break;
            case Kw_header_additions:
                data_.indeces_.header_additions_idx =
                    parsers_.additions_parser_->compile(Header_or_impl::Header);
                break;
            case Kw_impl_additions:
                data_.indeces_.impl_additions_idx   =
                    parsers_.additions_parser_->compile(Header_or_impl::Impl);
                break;
            case Kw_lexem_info_name:
                data_.indeces_.lexem_info_name_idx  =
                    parsers_.lexem_info_name_parser_->compile();
                break;
            case Kw_delimiters:
                data_.postactions_.delimiters_postaction =
                    parsers_.delimiters_sec_parser_->compile(data_.del_repres_,
                                                             data_.codes_,
                                                             data_.last_code_val_);
                data_.aut_data_.set_of_used_automata |= 1ULL << Delimiter_aut;
                break;
            case Kw_keywords:
                data_.postactions_.keywords_postaction =
                    parsers_.keywords_sec_parser_->compile(data_.kw_repres_,
                                                           data_.codes_,
                                                           data_.last_code_val_);
                data_.aut_data_.set_of_used_automata |= 1ULL << Keyword_aut;
                break;
            case Kw_numbers:       case Kw_action:
            case Opened_fig_brack: case Closed_fig_brack:
                parsers_.num_sec_parser_->compile(data_.numbers_regexp_,
                                                  data_.acts_for_numbers_);
                data_.aut_data_.set_of_used_automata |= 1ULL << Number_aut;
                break;
            case Kw_strings:
                parsers_.strs_sec_parser_->compile(data_.strings_regexp_,
                                                   data_.acts_for_strings_);
                data_.aut_data_.set_of_used_automata |= 1ULL << String_aut;
                break;
            case Kw_idents:
                parsers_.id_definition_parser_->compile(data_.idents_regexp_,
                                                        data_.acts_for_idents_);
                break;
            case Kw_comments: case Kw_nested: case Kw_multilined: case Kw_single_lined:
                std::tie(data_.indeces_.mark_of_single_lined,
                         data_.indeces_.mark_of_multilined_begin,
                         data_.indeces_.mark_of_multilined_end,
                         data_.indeces_.multilined_is_nested) =
                    parsers_.comments_parser_->compile();
                data_.aut_data_.set_of_used_automata |= 1ULL << Comment_aut;
                break;
            default:
                ;
        }
    }
}

Main_parser::Main_parser(const Location_ptr& loc) :
    impl_(std::make_unique<Main_parser_data>(loc)) {
#ifdef DEBUG_MODE
        printf("impl_ pointer: %p\n", impl_.get());
#endif
}

void Main_parser::compile()
{
    impl_->parse();
    if(!verify(impl_->data_, impl_->parsers_.et_, impl_->parsers_.char32_sets_)){
        impl_->parsers_.et_.ec->print();
        return;
    }
    auto info = collected_data_to_info(impl_->data_, impl_->parsers_.et_);
    aux_files_generate();
}

Main_parser::~Main_parser() = default;

int Main_parser::get_number_of_errors() const
{
    return impl_->get_number_of_errors();
}