LINKER        = g++
LINKERFLAGS   =  
COMPILER      = g++
COMPILERFLAGS =  -std=c++14 -Wall -g
BIN           = myauka
LIBS          = -lboost_filesystem -lboost_system
vpath %.cpp src
vpath %.o build
OBJ           = myauka.o verify_collected_data.o symbol.o slr_act_expr_parser.o sets_for_classes.o search_char.o print_size_t.o print_commands.o print_char32.o num_str_id_parser.o ndfa.o move_by_ext_from_curr_dir.o move_by_ext.o minimal_dfa.o match.o main_scaner.o main_parser.o lexem_info_name_parser.o keyword_delimiter_parser.o groupped_dfa_by_regexp.o groupped_dfa.o get_init_state.o fuse_commands.o fsize.o first_chars.o file_contents.o expr_scaner.o expr_lr_tables.o error_count.o dfa.o create_dir_if_not_exists.o comments_parser.o command2symbol.o codes_and_newline.o char_trie.o char_conv.o categories.o aux_expr_scaner.o additions_parser.o
LINKOBJ       = build/myauka.o build/verify_collected_data.o build/symbol.o build/slr_act_expr_parser.o build/sets_for_classes.o build/search_char.o build/print_size_t.o build/print_commands.o build/print_char32.o build/num_str_id_parser.o build/ndfa.o build/move_by_ext_from_curr_dir.o build/move_by_ext.o build/minimal_dfa.o build/match.o build/main_scaner.o build/main_parser.o build/lexem_info_name_parser.o build/keyword_delimiter_parser.o build/groupped_dfa_by_regexp.o build/groupped_dfa.o build/get_init_state.o build/fuse_commands.o build/fsize.o build/first_chars.o build/file_contents.o build/expr_scaner.o build/expr_lr_tables.o build/error_count.o build/dfa.o build/create_dir_if_not_exists.o build/comments_parser.o build/command2symbol.o build/codes_and_newline.o build/char_trie.o build/char_conv.o build/categories.o build/aux_expr_scaner.o build/additions_parser.o

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