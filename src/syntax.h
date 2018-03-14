//
// Created by Milan Suk on 14.03.18.
//

#ifndef MATRIXCALC_SYNTAX_H
#define MATRIXCALC_SYNTAX_H

#include "lexer.h"

typedef struct abstract_syntax_tree_node {
    struct abstract_syntax_tree_node* left;
    Token token;
    struct abstract_syntax_tree_node* right;
} AbstractSyntaxTreeNode;

typedef struct parser_state {
    FILE* file;
    Token token;
} ParserState;

AbstractSyntaxTreeNode* ast(Token token, AbstractSyntaxTreeNode* left, AbstractSyntaxTreeNode* right);
AbstractSyntaxTreeNode* ast_create();
AbstractSyntaxTreeNode* ast_create_valued(Token token);
AbstractSyntaxTreeNode* ast_parse(FILE*);

void eat(ParserState* parser_state, State token_state);
AbstractSyntaxTreeNode* factor(ParserState* parser_state);
AbstractSyntaxTreeNode* term(ParserState* parser_state);
AbstractSyntaxTreeNode* expr(ParserState* parser_state);

#endif //MATRIXCALC_SYNTAX_H
