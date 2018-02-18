LINKER        = g++
LINKERFLAGS   =  
COMPILER      = g++
COMPILERFLAGS =  -std=c++14 -Wall -g
BIN           = myauka
LIBS          = -lboost_filesystem -lboost_system
vpath %.cpp src
vpath %.o build
OBJ           = myauka.o first_chars.o ndfa.o automata_repres_builder.o get_init_state.o regexp1_or_regexp2.o groupped_dfa.o generate_separate_keywords_automaton.o match.o add_category.o main_scaner.o u32string_to_commands.o slr_act_expr_parser.o print_char32.o search_char.o aux_files_generate.o implement_delimiter_automaton.o create_dir_if_not_exists.o sets_for_classes.o dfa.o get_act_repres.o collected_data_to_info.o print_commands.o implement_string_automaton.o format.o expr_scaner.o print_size_t.o list_to_columns.o jump_table_string_repres.o comments_parser.o attributed_char_trie.o move_by_ext.o codes_and_newline.o idx_to_string.o error_count.o categories.o command2symbol.o aux_expr_scaner.o groupped_dfa_by_regexp.o minimal_dfa.o keyword_delimiter_parser.o num_str_id_parser.o generate_separate_identifier_automat.o automata_repres.o expr_lr_tables.o symbol.o main_parser.o implement_ident_and_keywords_automata.o char_conv.o file_contents.o id_with_keyw_builder.o additions_parser.o char_trie.o implement_none_automaton.o generate_both_keyword_and_ident_automata.o implement_unknown_automaton.o fsize.o lexem_info_name_parser.o move_by_ext_from_curr_dir.o str_repres_for_set_of_size_t_const.o implement_number_automaton.o implement_automata.o fuse_commands.o verify_collected_data.o
LINKOBJ       = build/myauka.o build/first_chars.o build/ndfa.o build/automata_repres_builder.o build/get_init_state.o build/regexp1_or_regexp2.o build/groupped_dfa.o build/generate_separate_keywords_automaton.o build/match.o build/add_category.o build/main_scaner.o build/u32string_to_commands.o build/slr_act_expr_parser.o build/print_char32.o build/search_char.o build/aux_files_generate.o build/implement_delimiter_automaton.o build/create_dir_if_not_exists.o build/sets_for_classes.o build/dfa.o build/get_act_repres.o build/collected_data_to_info.o build/print_commands.o build/implement_string_automaton.o build/format.o build/expr_scaner.o build/print_size_t.o build/list_to_columns.o build/jump_table_string_repres.o build/comments_parser.o build/attributed_char_trie.o build/move_by_ext.o build/codes_and_newline.o build/idx_to_string.o build/error_count.o build/categories.o build/command2symbol.o build/aux_expr_scaner.o build/groupped_dfa_by_regexp.o build/minimal_dfa.o build/keyword_delimiter_parser.o build/num_str_id_parser.o build/generate_separate_identifier_automat.o build/automata_repres.o build/expr_lr_tables.o build/symbol.o build/main_parser.o build/implement_ident_and_keywords_automata.o build/char_conv.o build/file_contents.o build/id_with_keyw_builder.o build/additions_parser.o build/char_trie.o build/implement_none_automaton.o build/generate_both_keyword_and_ident_automata.o build/implement_unknown_automaton.o build/fsize.o build/lexem_info_name_parser.o build/move_by_ext_from_curr_dir.o build/str_repres_for_set_of_size_t_const.o build/implement_number_automaton.o build/implement_automata.o build/fuse_commands.o build/verify_collected_data.o

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after

clean: clean-custom 
	rm -f ./build/*.o
	rm -f ./build/$(BIN)

.cpp.o:
	$(COMPILER) -c $< -o $@ $(COMPILERFLAGS) 
	mv $@ ./build

$(BIN):$(OBJ)
	$(LINKER) -o $(BIN) $(LINKOBJ) $(LIBS) $(LINKERFLAGS)
	mv $(BIN) ./build