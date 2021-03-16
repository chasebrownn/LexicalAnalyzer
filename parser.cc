/*
 * Copyright (C) Rida Bazzi, 2020
 *
 * Do not share this file with anyone
 *
 * Do not post this file or derivatives of
 * of this file online
 *
 */
#include <iostream>
#include <cstdlib>
#include "parser.h"
#include <cstring>


/*
* Chase Brown
* 
* 1212869071
* 
* Project 2
*/

using namespace std;

void Parser::syntax_error()
{
    cout << "SYNTAX ERROR\n";
    exit(1);
}

// this function gets a token and checks if it is
// of the expected type. If it is, the token is
// returned, otherwise, synatx_error() is generated
// this function is particularly useful to match
// terminals in a right hand side of a rule.
// Written by Mohsen Zohrevandi
Token Parser::expect(TokenType expected_type)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error();
    return t;
}

//----------------------------------------------------------------------------------------------------------------------
//  PARSING
//----------------------------------------------------------------------------------------------------------------------

/* This parse argument function is a portion from project 1 and assisted me in correcting syntax errors
*   and was a reference in writing commands for this project such as using lexer.peek(), etc.

void Parser::parse_argument()
{
    Token token;

    token = lexer.peek(2);
    if (token.token_type == LPAREN)
    {
        parse_poly_eval();
        //return;
    }

    token = lexer.peek(1);
    if (token.token_type == ID)
    {
        token = expect(ID);
        //return;
    }
    else if (token.token_type == NUM)
    {
        token = expect(NUM);
        //return;
    }
}*/

// Function is meant for debugging to know which function is being called
// Prints when true
void debug_func(string str)
{
    if (!true)
    {

        cout << "/t" << str << endl;
    }
}

// This functnion is also meant for debugging. I call it to print tokens before I execute an expect()
// Prints when true
void debug_token(Token t)
{
    if (!true)
    {
        t.Print();
    }
}

// Parses the input into the tokens section and the INPUT_TEXT
// Also checks for end of file
// Returns a struct of full_input: containing (a) vector<token>
//                                            (b) string input;

full_input Parser::parse_input() // return a struct
{
    debug_func("parse_input"); // call of debug function

    Token t;

    struct full_input entry;
    vector<token> main_vector;

    main_vector = parse_tokens_section(main_vector);

    entry.grammar = main_vector;

    t = expect(INPUT_TEXT);

    entry.input = t.lexeme;

    t = expect(END_OF_FILE); // End of input

    return entry;
}

// Parse tokens section will break down into a parse token list and an expect a HASH
// to mark the end of the grammar line
// Returns a vector<token> that stores tokens. struct token contains (a) token_id
//                                                                   (b) token_graph (REG)
  
vector<token> Parser::parse_tokens_section(vector<token> main_vector)
{
    debug_func("parse_tokens_sections"); // call of debug function

    Token t;

    main_vector = parse_token_list(main_vector);

    t = expect(HASH);

    return main_vector;
}

// Parse token list will parse into individual tokens listed within the tokens section
vector<token> Parser::parse_token_list(vector<token> main_vector)
{
    debug_func("parse_token_list");

    Token t;

    main_vector.push_back(parse_token());

    t = lexer.peek(1);
    if (t.token_type == COMMA)
    {
        t = expect(COMMA);
        main_vector = parse_token_list(main_vector);
    }

    return main_vector;
}

// Parse token will break down into it's token ID (tok_k) along with it's expression aka grammar
token Parser::parse_token()
{
    debug_func("parse_token");

    Token t;
    struct token grammar;

    t = expect(ID);

    grammar.token_id = t.lexeme;
    //cout << t.lexeme << endl;

    debug_token(t);

    grammar.token_graph = parse_expr();

    return grammar;
}

// Parse expr breaks the expression down in order to characterize the INPUT_TEXT to a specific token
REG Parser::parse_expr()
{
    debug_func("parse_expr");
    
    Token t;

    t = lexer.peek(1);

    // Expression must start with a char, left parenthesis, or an underscore (EPSILON)
    if (t.token_type == CHAR) // character state machine
    {
        t = expect(CHAR);
        debug_token(t);
        
        struct REG_node *node1 = new REG_node;
        struct REG_node *node2 = new REG_node;
        struct REG header;

        node1->first_label = t.lexeme[0];
        //cout << t.lexeme[0] << endl;
        node1->first_neighbor = node2;
        node1->second_label = NO_DEST;
        node1->second_neighbor = NULL;
        
        node2->first_label = NO_DEST;
        node2->first_neighbor = NULL;
        node2->second_label = NO_DEST;
        node2->second_neighbor = NULL;

        header.start = node1;
        header.accept = node2;

        return header;
    }
    else if (t.token_type == LPAREN)
    {
        t = expect(LPAREN);
        debug_token(t);

        struct REG first = parse_expr(); // After a left parenthesis, an expression is expected

        t = expect(RPAREN);
        debug_token(t);

        t = lexer.peek(1);

        // Followed by an expression (enclosed), we must have a DOT, STAR, or an OR
        if (t.token_type == DOT) // concatination
        {
            t = expect(DOT);
            debug_token(t);

            t = expect(LPAREN);
            debug_token(t);

            struct REG header;
            struct REG second = parse_expr();

            REG_node* node1 = first.start;
            REG_node* node2 = first.accept;
            REG_node* node3 = second.start;
            REG_node* node4 = second.accept;
            
            node2->first_neighbor = node3;
            node2->first_label = EPSILON;

            header.start = node1;
            header.accept = node4;

            t = expect(RPAREN);
            debug_token(t);

            return header;
        }
        else if (t.token_type == OR) // union
        {
            t = expect(OR);
            debug_token(t);

            t = expect(LPAREN);
            debug_token(t);

            struct REG header;
            struct REG second = parse_expr();

            REG_node* node1 = first.start;
            REG_node* node2 = first.accept;
            REG_node* node3 = second.start;
            REG_node* node4 = second.accept;

            struct REG_node* new_start = new REG_node;
            struct REG_node* new_accept = new REG_node;

            new_start->first_label = EPSILON;
            new_start->first_neighbor = node1;
            new_start->second_label = EPSILON;
            new_start->second_neighbor = node3;

            new_accept->first_label = NO_DEST;
            new_accept->first_neighbor = NULL;
            new_accept->second_label = NO_DEST;
            new_accept->second_neighbor = NULL;

            node2->first_label = EPSILON;
            node2->first_neighbor = new_accept;

            node4->first_label = EPSILON;
            node4->first_neighbor = new_accept;

            header.start = new_start;
            header.accept = new_accept;

            t = expect(RPAREN);
            debug_token(t);

            return header;
        }
        else if (t.token_type == STAR) // star
        {
            t = expect(STAR);
            debug_token(t);

            struct REG header;

            REG_node* node1 = first.start;
            REG_node* node2 = first.accept;

            struct REG_node* new_start = new REG_node;
            struct REG_node* new_accept = new REG_node;

            new_start->first_label = EPSILON;
            new_start->first_neighbor = node1;
            new_start->second_label = EPSILON;
            new_start->second_neighbor = new_accept;

            new_accept->first_label = NO_DEST;
            new_accept->first_neighbor = NULL;
            new_accept->second_label = NO_DEST;
            new_accept->second_neighbor = NULL;

            node2->first_label = EPSILON;
            node2->first_neighbor = new_accept;
            node2->second_neighbor = node1;
            node2->second_label = EPSILON;

            header.start = new_start;
            header.accept = new_accept;
            
            return header;
        }
        else
            syntax_error();
    }
    else if (t.token_type == UNDERSCORE)
    {
        t = expect(UNDERSCORE);
        //cout << "EPSILON IS NOOOOOT A TOKEN !!! " << t.lexeme << endl;

        struct REG_node* node1 = new REG_node;
        struct REG_node* node2 = new REG_node;
        struct REG header;

        node1->first_label = CASH;
        //cout << t.lexeme[0] << endl;
        node1->first_neighbor = node2;
        node1->second_label = NO_DEST;
        node1->second_neighbor = NULL;

        node2->first_label = NO_DEST;
        node2->first_neighbor = NULL;
        node2->second_label = NO_DEST;
        node2->second_neighbor = NULL;

        header.start = node1;
        header.accept = node2;

        return header;
        
        debug_token(t);
    }
    else
        syntax_error();
}


// This function is simply to illustrate the GetToken() function
// you will not need it for your project and you can delete it
// the function also illustrates the use of peek()
void Parser::ConsumeAllInput()
{
    Token token;
    int i = 1;
    
    token = lexer.peek(i);
    token.Print();
    while (token.token_type != END_OF_FILE)
    {
        i = i+1;
        token = lexer.peek(i);
        token.Print();
    }
   
    token = lexer.GetToken();
    token.Print();
    while (token.token_type != END_OF_FILE)
    {
        token = lexer.GetToken();
        token.Print();
    }

}

//----------------------------------------------------------------------------------------------------------------------
//  LEXICAL ANALYZER
//----------------------------------------------------------------------------------------------------------------------


// match(REG r, string s, int p)
// 
// r is a REG , s is a string and p is a position in the string s.
// The function match() will return the longest possible lexeme starting from position p
// in the string s that matches the regular expression of the graph r.


void my_lexicalAnalyzer(vector<token> L, string s)
{
    int position = 2;
    struct MGT mgt;
    string match;

    //while (position < s.size())
    //{
        mgt = my_GetToken(L, s, position);

        match = s.substr(position, mgt.position-1);

        if (mgt.position != 99999)
        {
            cout << mgt.token_id << " , \"" << match << "\"" << endl;
        }

        position = mgt.position + 1;
    //}
}

// returns the token with the longest matching
// prefix together with its lexemeand updates the current position
MGT my_GetToken(vector<token> L, string s, int p)
{
    vector<MGT> matches;
    vector<MGT> epsilon_errors;
    struct MGT return_this;
    bool epsilon_error = false;

    return_this.position = 0;
    return_this.token_id = "";

    for (int i = 0; i < L.size(); i++)
    {
        return_this.position = match(L[i].token_graph, s, p);
        return_this.token_id = L[i].token_id;

        matches.push_back(return_this);
    }
    for (int i = 0; i < matches.size(); i++)
    {
        if (matches[i].position > return_this.position)
        {
            return_this.position = matches[i].position;
            return_this.token_id = matches[i].token_id;
        }
    }

    for (int i = 0; i < matches.size(); i++)
    {
        if (matches[i].position == 99999)
        {
            epsilon_error = true;
            epsilon_errors.push_back(matches[i]);
        }
    }
    if (epsilon_error)
    {
        cout << "EPSILON IS NOOOOOT A TOKEN !!!";
        for (int i = 0; i < epsilon_errors.size(); i++)
        {
            cout << " " << epsilon_errors[i].token_id;
        }
        cout << endl;
    }

    return return_this;
}

int match(REG r, string s, int p)
{   
    // Determine longest possible substring of s
    // If there is no match, return false ?
    // Note that match need not return an actual substring, it just 
        // needs to return a position p’ corresponding to the end of the
        // substring.The substring would be between p and p’

    //char parse_this = s[p];
    vector<REG_node*> reachable_nodes;
    //vector<REG_node*> path;

    struct REG_node* q = r.start;

    reachable_nodes = iterate(q, reachable_nodes);

    for (int i = 0; i < reachable_nodes.size(); i++)
    {
        if (reachable_nodes[i] == r.accept || reachable_nodes[i]->first_label == CASH)
        {
            return 99999;
        }
    }

    //cout << reachable_nodes.size() << endl;
    //cout << s[p] << endl;

    bool match = true;
    bool first_match = true;
    bool match_found = false;
    //char prev = '#';

    vector<REG_node*> char_match;
    vector<REG_node*> middle_man;

    //cout << "DEBUG DEBUG" << endl;

    char c = s[p];
    
    while ((match == true) && (p < (s.size() - 1)))
    { 
        for (int i = 0; i < reachable_nodes.size(); i++)
        {
            if (reachable_nodes[i]->first_label == c) // || reachable_nodes[i]->second_label == c
            {
                match_found = true;
                //char_match.push_back(reachable_nodes[i]);
                //char_match.push_back(reachable_nodes[i]->first_neighbor);
                
                //char_match.insert(reachable_nodes.end(), char_match.begin(), char_match.end());

                if (first_match)
                {
                    char_match = iterate(reachable_nodes[i]->first_neighbor, char_match);
                    //char_match.push_back(reachable_nodes[i]);
                    first_match = false;
                }
                else
                {
                    middle_man = iterate(reachable_nodes[i]->first_neighbor, middle_man);
                    char_match.insert(char_match.end(), middle_man.begin(), middle_man.end());
                }               
            }
            
        }

        if (!match_found)
        {
            match = false;
        }

        //replace reachable nodes with char match
        reachable_nodes.clear();
        reachable_nodes = char_match;

        //for (int i = 0; i < char_match.size(); i++)
        //{
        //    reachable_nodes[i] = char_match[i];
        //}

        char_match.clear();
        middle_man.clear();

        //prev = c;
        match_found = false;
        first_match = true;
        p++;
        c = s[p];
    }
   
    return p-1;
}

vector<REG_node*> iterate(REG_node* q, vector<REG_node*> reachable_nodes)
{
    reachable_nodes.push_back(q);
    //cout << "Push" << endl;

    if (q->first_neighbor != NULL && q->first_label == EPSILON)
    {
        reachable_nodes = iterate(q->first_neighbor, reachable_nodes);
        //reachable_nodes.push_back(q);
    }
    if (q->second_neighbor != NULL && q->second_label == EPSILON)
    {     
        reachable_nodes = iterate(q->second_neighbor, reachable_nodes);
        //reachable_nodes.push_back(q);
    }

    return reachable_nodes;
}


int main()
{
	// note: the parser class has a lexer object instantiated in it. You should not be declaring
    // a separate lexer object. You can access the lexer object in the parser functions as shown in the
    // example method Parser::ConsumeAllInput
    // If you declare another lexer object, lexical analysis will not work correctly
    Parser parser;

    //g++ -g -Wall inputbuf.cc lexer.cc parser.cc

    struct full_input entry;

    entry = parser.parse_input();
    //parser.ConsumeAllInput();

    string input_string = entry.input; // Input string s
    vector<token> tokens = entry.grammar; // List L

    my_lexicalAnalyzer(tokens, input_string);
}

/*

    How to Implement a Solution
    
    The main difficulty in coming up with a solution is to transform a given list of token names and
    their regular expression descriptions into a my GetToken() function for the given list of tokens.
    This transformation will be done in three high-level steps:
    
    
    1. Transform regular expressions into REGs. The goal here is to parse a regular expression
    description and generate a graph that represents the regular expression1. The generated
    graph will have a specific format and I will describe below how to generate it. I will call it a
    regular expression graph, or REG for short.
    
    
    2. Write a function --- match(r,s,p) ---, where r is a REG , s is a string and p is a position in the
    string s. The function match will return the longest possible lexeme starting from position p
    in the string s that matches the regular expression of the graph r.
    
    
    3. Write a class my LexicalAnalyzer(list,s) where list is a list of structures of the form
    {token name, reg pointer} and s is an input string. my LexicalAnalyzer stores the list of
    structures and keeps track of the part of the input string that has been processed. The
    class my LexicalAnalyzer has a method my GetToken(). For every call of my GetToken(),
    match(r,s,p) is called for every REG r in the list starting from the current position p
    maintained in my LexicalAnalyzer. my GetToken() returns the token with the longest matching
    prefix together with its lexeme and updates the current position. If the longest matching
    prefix matches more than one token, the matched token that is listed first in the list of tokens
    is returned.

*/
