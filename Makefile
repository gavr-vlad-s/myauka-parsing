LINKER        = g++
LINKERFLAGS   =  
COMPILER      = g++
COMPILERFLAGS =  -std=c++14 -Wall -g
BIN           = myauka
LIBS          = -lboost_filesystem -lboost_system
vpath %.cpp src
vpath %.o build
OBJ           = myauka.o first_chars.o ndfa.o get_init_state.o groupped_dfa.o match.o main_scaner.o slr_act_expr_parser.o print_char32.o search_char.o aux_files_generate.o create_dir_if_not_exists.o sets_for_classes.o dfa.o print_commands.o format.o expr_scaner.o print_size_t.o comments_parser.o move_by_ext.o codes_and_newline.o error_count.o categories.o command2symbol.o aux_expr_scaner.o groupped_dfa_by_regexp.o minimal_dfa.o keyword_delimiter_parser.o num_str_id_parser.o expr_lr_tables.o symbol.o main_parser.o char_conv.o file_contents.o additions_parser.o char_trie.o fsize.o lexem_info_name_parser.o move_by_ext_from_curr_dir.o fuse_commands.o verify_collected_data.o
LINKOBJ       = build/myauka.o build/first_chars.o build/ndfa.o build/get_init_state.o build/groupped_dfa.o build/match.o build/main_scaner.o build/slr_act_expr_parser.o build/print_char32.o build/search_char.o build/aux_files_generate.o build/create_dir_if_not_exists.o build/sets_for_classes.o build/dfa.o build/print_commands.o build/format.o build/expr_scaner.o build/print_size_t.o build/comments_parser.o build/move_by_ext.o build/codes_and_newline.o build/error_count.o build/categories.o build/command2symbol.o build/aux_expr_scaner.o build/groupped_dfa_by_regexp.o build/minimal_dfa.o build/keyword_delimiter_parser.o build/num_str_id_parser.o build/expr_lr_tables.o build/symbol.o build/main_parser.o build/char_conv.o build/file_contents.o build/additions_parser.o build/char_trie.o build/fsize.o build/lexem_info_name_parser.o build/move_by_ext_from_curr_dir.o build/fuse_commands.o build/verify_collected_data.o

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