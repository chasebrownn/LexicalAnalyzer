/*
 * Copyright (C) Rida Bazzi, 2019
 *
 * Do not share this file with anyone
 */

 /*
 * Chase Brown
 *
 * 1212869071
 *
 * Project 2
 */

#ifndef __PARSER_H__
#define __PARSER_H__
#define NO_DEST '#'
#define EPSILON '_'
#define CASH '$'

#include <string>
#include "lexer.h"

using namespace std;

 //  In the representation, first neighbor is the first node pointed to by a node and second neighbor
 //  is the second node pointed to by a node. first label and second label are the labels of the arrows 
 //  from the node to its neighbors. If a node has only one neighbor, then second neighbor will
 //  be NULL. If a node has no neighbors, then both first neighbor and second neighbor will be
 //  NULL.
struct REG_node {
    struct REG_node* first_neighbor;
    char first_label;
    struct REG_node* second_neighbor;
    char second_label;
};

struct REG {
    struct REG_node* start;
    struct REG_node* accept;
};

struct token {
    string token_id;
    struct REG token_graph;
};

struct full_input {
    string input;
    vector<token> grammar;
};

struct MGT {
    //string lexeme; // longest possible match
    string token_id; // token id
    int position;
};


void my_lexicalAnalyzer(vector<token>, string);
struct MGT my_GetToken(vector<token>, string, int);
int match(REG, string, int);
vector<REG_node*> iterate(REG_node*, vector<REG_node*>);
bool Match_One_Char(vector<REG_node*> path, char);

class Parser {
  public:
    void ConsumeAllInput();
    struct full_input parse_input(); 

  private:
    LexicalAnalyzer lexer;
    void syntax_error();
    Token expect(TokenType expected_type);

    vector<token> parse_tokens_section(vector<token>);
    vector<token> parse_token_list(vector<token>);
    struct token parse_token();
    struct REG parse_expr();
    
};

#endif

