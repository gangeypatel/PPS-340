#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <set>
#include "compiler.h"
#include "lexer.h"
using namespace std;

LexicalAnalyzer lexer;
extern string reserved[];
unordered_map<string,int> ValueMap;
InstructionNode *headInstructionNode =new InstructionNode();
InstructionNode *currentInstructionNode = headInstructionNode;

struct InstructionNode* parse_body();

struct InstructionNode* GetNewInstructionNode()
{
    struct InstructionNode* node = new InstructionNode();
    node->next=NULL;
    return node;
}

void parse_num_list(){
    Token t = lexer.GetToken();
    
    if(reserved[t.token_type]=="NUM"){
        inputs.push_back(stoi(t.lexeme));
        t = lexer.peek(1);
        
        if(reserved[t.token_type]=="NUM")
        {
            parse_num_list();
        }
        else if(reserved[t.token_type]=="END_OF_FILE")
        {
            
            return ;
        }
        else
        {
            cout<<"parse_input_section_1"<<endl;
            
        }
    }
    else{
        cout<<"parse_input_section_2"<<endl;
        
    }    
}

void parse_input_section(){
     
    parse_num_list();
    
    Token t = lexer.GetToken();

    if(reserved[t.token_type]=="END_OF_FILE")
    {
        return;
    }
    else
    {
        cout<<"parse_input_section_1"<<endl;
        
    }    

}
void parse_id_list()
{

    Token t = lexer.GetToken();
    if(reserved[t.token_type]=="ID")
    {
        ValueMap[t.lexeme] = next_available;
        mem[next_available] = 0;
        next_available++;

        t = lexer.peek(1);
        
        if(reserved[t.token_type]=="COMMA")
        {
            
            t = lexer.GetToken(); 
            parse_id_list();
        }
        else if(reserved[t.token_type]=="SEMICOLON")
        {
           
            return ;
        }
        else
        {
            cout<<"parse_id_list_1"<<endl;
        }
    }
    else
    {
        cout<<"parse_id_list_2"<<endl;
        
    }
  
}

void parse_var_section()
{
	// cout << "PARSING VAR SECTION" << endl;
    parse_id_list();

    Token t = lexer.GetToken();

    if(reserved[t.token_type]=="SEMICOLON")
    {	
    	Token t1 = lexer.peek(1);
    	
    	t1 = lexer.peek(1);
    	Token t2 = t1;
    	int popCounter = 0;
    	while(reserved[lexer.GetToken().token_type] != "RBRACE") {
    		popCounter++;
    		t1 = lexer.peek(1);
    		if(reserved[t1.token_type]== "NUM"){
    			
    			ValueMap[t1.lexeme] = next_available;
    			mem[ValueMap[t1.lexeme]] = stoi(t1.lexeme);
    			
    			next_available++;
    		}
    	}

    	lexer.UngetToken(popCounter+1);
    	

        return;
    }
    else
    {
        cout<<"parse_var_section_1"<<endl;
        
    }
   
}

struct InstructionNode* parse_input_statement()
{
    
    InstructionNode *inputnode = GetNewInstructionNode();
    inputnode->type= IN;

    Token t = lexer.GetToken();
    
    if(reserved[t.token_type] == "INPUT"){
        t = lexer.GetToken();
        
        inputnode->input_inst.var_index = ValueMap[t.lexeme];
        
        t = lexer.GetToken();
         
        if(reserved[t.token_type] == "SEMICOLON"){
            return inputnode;
        }
        else {
                 cout<<"parse_input_statement"<<endl;
                    
                }
    }
}

InstructionNode* parse_output_statement()
{
    
    // for (auto const& pair: ValueMap) {
    //     std::cout << pair.first << " : " <<"---"<< pair.second<< std::endl;
    // }
    InstructionNode *Node = GetNewInstructionNode();
    Node->type= OUT;

    Token t = lexer.GetToken();
    if(reserved[t.token_type] == "OUTPUT"){
        t = lexer.GetToken();
        // cout<<t.lexeme<<endl;
        // cout<<ValueMap[t.lexeme]<<endl;
        Node->output_inst.var_index = ValueMap[t.lexeme];
        t= lexer.GetToken();
        if(reserved[t.token_type] == "SEMICOLON"){
            return Node;
        }
        else {
            cout<<"parse_output_statement"<<endl;
            
        }
    }
}
int parse_primary()
{
    
    Token t = lexer.peek(1);
    
    if(reserved[t.token_type]=="ID" || reserved[t.token_type]=="NUM")
    {   t = lexer.GetToken();

        if(reserved[t.token_type] == "ID")
        {
            return ValueMap[t.lexeme];
        }
        else
        {
        
            return ValueMap[t.lexeme];
        }
        
        
    }
    else
    {
        cout<<"parse_primary_1"<<endl;
        
    }
    
}

int parse_operator()
{
    Token t = lexer.GetToken();
    
    if(reserved[t.token_type]=="PLUS" || reserved[t.token_type]=="MINUS" || reserved[t.token_type]=="MULT" || reserved[t.token_type]=="DIV")
    {
        switch (t.token_type)
        {
            case PLUS:
                return OPERATOR_PLUS;
            case MINUS:
                return OPERATOR_MINUS;
            case MULT:
                return OPERATOR_MULT;
            case DIV:
                return OPERATOR_DIV;
            default:
                return OPERATOR_NONE;
        }
    }
    else
    {
        cout<<"parse_operator_1"<<endl;
        
    }
    

} 
void parse_expression(InstructionNode *InstructionNode)
{
    InstructionNode->assign_inst.operand1_index = parse_primary();
    InstructionNode->assign_inst.op = (ArithmeticOperatorType)parse_operator();
    InstructionNode->assign_inst.operand2_index = parse_primary();
   
}

struct InstructionNode* parse_assign_statement()
{
    InstructionNode *Node = GetNewInstructionNode();
    Node->type=ASSIGN;
        
    Token t = lexer.GetToken();
    if(reserved[t.token_type]=="ID")
    {
       
        Node->assign_inst.left_hand_side_index = ValueMap[t.lexeme];
       
        t = lexer.GetToken();
        
        
        if(reserved[t.token_type]=="EQUAL")
        {

            t = lexer.GetToken();
         
            Token t2 = lexer.GetToken();
        
            lexer.UngetToken(1);
            
            lexer.UngetToken(1);
            
        
            if(reserved[t2.token_type]=="PLUS" || reserved[t2.token_type]=="MINUS" || reserved[t2.token_type]=="MULT" || reserved[t2.token_type]=="DIV")
            {
                
                parse_expression(Node);

                t2 = lexer.GetToken();
                if(reserved[t2.token_type]=="SEMICOLON")
                {
                    return Node;
                }
                else
                {
                    cout<<"parse_assign_statement_4"<<endl;
                    
                }
            }
            else if(reserved[t.token_type]=="ID" || reserved[t.token_type]=="NUM")
            {
                
                Node->assign_inst.operand1_index = parse_primary();
                Node->assign_inst.op = OPERATOR_NONE;
                t = lexer.GetToken();

                if(reserved[t2.token_type]=="SEMICOLON")
                {
                    return Node;
                }
                else
                {
                    cout<<"parse_assign_statement_5"<<endl;
                    
                }
            }
            else
            {
                cout<<"parse_assign_statement_1"<<endl;
                
            }
        }
        else
        {
            cout<<"parse_assign_statement_2"<<endl;
            
        }
    }
    else
    {
        cout<<"parse_assign_statement_3"<<endl;
        
    }
}
int parse_rel_operator()
{
    Token t = lexer.GetToken(); 
    if(reserved[t.token_type]=="GREATER" || reserved[t.token_type]=="LESS" || reserved[t.token_type]=="NOTEQUAL")
    {
        switch (t.token_type)
        {
            case GREATER:
                return CONDITION_GREATER;
            case LESS:
                return CONDITION_LESS;
            case NOTEQUAL:
                return CONDITION_NOTEQUAL;
            default:
                return 8888;
        }
    }
    else
    {
        cout<<"parse_rel_operator_1"<<endl;
    }
}

void parse_condition(InstructionNode *InstructionNode)
{
	
    InstructionNode->cjmp_inst.operand1_index = parse_primary();
    
    InstructionNode->cjmp_inst.condition_op = (ConditionalOperatorType) parse_rel_operator();
   
    InstructionNode->cjmp_inst.operand2_index = parse_primary();
    
}
struct InstructionNode* parse_if_statement()
{
    Token t = lexer.GetToken();
   
    if(reserved[t.token_type]=="IF")
    {
    	InstructionNode* noopStatement = GetNewInstructionNode();
    	noopStatement->type=NOOP;

   		InstructionNode* Node = GetNewInstructionNode();
   		Node->type=CJMP;

   		
        parse_condition(Node);
        Node->cjmp_inst.target = noopStatement;
        InstructionNode* bodyStatement = parse_body();
        Node->next = bodyStatement;
        while(bodyStatement->next!=NULL)
        {
           
            bodyStatement=bodyStatement->next;

        }

        bodyStatement->next = noopStatement;
        

        return Node;
    }
    else
    {
        cout<<"parse_if_statement_1"<<endl;
        
    }
}

struct InstructionNode* parse_while_statement()
{
   

    Token t = lexer.GetToken();
    if(reserved[t.token_type]=="WHILE")
    {
        InstructionNode* Node = GetNewInstructionNode();
        Node->type=CJMP;

        parse_condition(Node);
        InstructionNode* bodyStatement = parse_body();
        Node->next = bodyStatement;

        InstructionNode* gotostatement = GetNewInstructionNode();
        gotostatement->type = JMP;
        
        InstructionNode* noopStatement = GetNewInstructionNode();
        noopStatement->type=NOOP;

        gotostatement->next = noopStatement;

        gotostatement->jmp_inst.target = Node;
        Node->cjmp_inst.target= noopStatement;
        
        while(bodyStatement->next!=NULL)
        {
          
            bodyStatement=bodyStatement->next;
        }

        bodyStatement->next = gotostatement;

        return Node;
    }
    else
    {
        cout<<"parse_while_statement_1"<<endl;
    }
}

struct InstructionNode* parse_for_statement()
{
    Token t = lexer.GetToken();

    
    if(reserved[t.token_type]=="FOR")
    {
        InstructionNode* Node = GetNewInstructionNode();
        InstructionNode* initialAssignStatement=NULL, *incrementStatement=NULL, *bodyStatement=NULL;
        Node->type=CJMP;
        t = lexer.GetToken();
        if(reserved[t.token_type]=="LPAREN")
        {
            initialAssignStatement = parse_assign_statement();
            parse_condition(Node);

            t = lexer.GetToken();
            if(reserved[t.token_type]=="SEMICOLON")
            {
                incrementStatement = parse_assign_statement();
                t = lexer.GetToken();
                if(reserved[t.token_type]=="RPAREN")
                {
                    
                    InstructionNode* gotostatement = GetNewInstructionNode();
                    gotostatement->type = JMP;
        
                    InstructionNode* noopStatement = GetNewInstructionNode();
                    noopStatement->type=NOOP;

                    gotostatement->next = noopStatement;
                    bodyStatement = parse_body();
                    Node->next = bodyStatement;

                    while(bodyStatement->next!=NULL)
                    {
                        bodyStatement=bodyStatement->next;
                    }
                    bodyStatement->next = incrementStatement;
                    bodyStatement = bodyStatement->next;
                    bodyStatement->next = gotostatement;

                    initialAssignStatement->next = Node;

                    return initialAssignStatement;
                }
                else
                {
                    cout<<"parse_for_statement_1"<<endl;
                   
                }
            }
            else
            {
                cout<<"parse_for_statement_2"<<endl;
                
            }
        }
        else
        {
            cout<<"parse_for_statement_3"<<endl;
        }
    }
    else
    {
        cout<<"parse_for_statement_4"<<endl;
    }
}

struct StatementNode* parse_switch_stmt() 
{
	Token t;
	int var ;
	InstructionNode* noopstart = GetNewInstructionNode();
        noopstart->next = NULL;
        noopstart->type = NOOP; 
	InstructionNode* noopend = GetNewInstructionNode(); 
        noopend->next = NULL;
        noopend->type = NOOP; 

    t = lexer.GetToken();
	if(reserved[t.token_type]=="SWITCH"){
        
        Token t2 = lexer.GetToken();
        if(reserved[t2.token_type]=="ID"){
            var = ValueMap[t2.lexeme];
            t2 = lexer.GetToken();
            if(reserved[t2.token_type]=="LBRACE")
            {
                InstructionNode* prev = noopstart; 
                t2 = lexer.peek(1);
                while (reserved[t2.token_type] == "CASE") {
		            Token t3;

                    InstructionNode* statement = GetNewInstructionNode();
                    statement->type = CJMP;

                    InstructionNode* noop1 = GetNewInstructionNode();
                            noop1->next = NULL;
                            noop1->type = NOOP; 
                    statement->next = noop1;

                    statement->cjmp_inst.operand1_index = var;
		            statement->cjmp_inst.condition_op = CONDITION_NOTEQUAL;
                    t3 = lexer.GetToken();
                if(reserved[t3.token_type]=="CASE"){
                    statement->cjmp_inst.operand2_index = ValueMap[t3.lexeme];
                    t3 = lexer.GetToken();
                    if(reserved[t3.token_type]=="COLON"){
                        InstructionNode* bodystat = parse_body();
                        statement->cjmp_inst.target = bodystat;
		                
                        statement->next = statement->next;
                        
                        InstructionNode* goto_stmt = GetNewInstructionNode();
                        goto_stmt->type = JMP;
                        goto_stmt->jmp_inst.target = noopend;
                        goto_stmt->next = statement->next;

                        while (bodystat->next != NULL) 
                        {
                            bodystat = bodystat->next;
	                        bodystat->next = goto_stmt;
                        }

                        prev->next = statement;
                        prev = statement->next;
                    t = lexer.peek(1);
                    }
                t = lexer.peek(1);
                    if (reserved[t.token_type] == "DEFAULT") {
                        Token t2 = lexer.GetToken();
                        if (reserved[t2.token_type] == "COLON"){
                            InstructionNode* body = parse_body();
                            	InstructionNode* noop2 = GetNewInstructionNode(); 
                                    noop2->next = NULL;
                                    noop2->type = NOOP;
                        }
            
	                }
                }
                    
            }
        }
        }
    }
}    
struct InstructionNode* parse_statement()
{
     Token t = lexer.peek(1);
    
    if(reserved[t.token_type]=="ID")
    {
        return parse_assign_statement();
        
    }
    else if(reserved[t.token_type]=="INPUT")
    {
        return parse_input_statement();
    }
       
    else if(reserved[t.token_type]=="OUTPUT")
    {
        return parse_output_statement();
    }
    else if(reserved[t.token_type]=="IF")
    {
    	
        return parse_if_statement();
    }
    else if(reserved[t.token_type]=="WHILE")
    {
        
        return parse_while_statement();
    }
    else if(reserved[t.token_type]=="FOR")
    {
        
        return parse_for_statement();
    }
    else if(reserved[t.token_type]=="SWITCH")
    {
        
        return parse_switch_statement();
    }
    
}

struct InstructionNode* parse_statement_list()
{
   
    InstructionNode* currStatement = parse_statement();

    InstructionNode* listStatements, *temp1=currStatement;
    
        Token t=lexer.GetToken();
    
   
    if(reserved[t.token_type]=="ID" || reserved[t.token_type]=="INPUT" || reserved[t.token_type]=="OUTPUT" || reserved[t.token_type]=="IF" || reserved[t.token_type]=="WHILE"|| reserved[t.token_type]=="FOR" ||reserved[t.token_type]=="SWITCH"  )
    {
        lexer.UngetToken(1);
        listStatements = parse_statement_list();

            while(temp1->next!=NULL)
            {
                temp1=temp1->next;
            }
            temp1->next = listStatements;

        return currStatement;
    }
    else if(reserved[t.token_type] == "RBRACE")
    {   
        lexer.UngetToken(1);
        return currStatement;
    }
    else
    {
        cout<<"parse_statement_list_1"<<endl;
        
    }
    
}

struct InstructionNode* parse_body()
{
  
    Token t = lexer.peek(1);
    
    if(reserved[t.token_type]=="LBRACE")
    {
        t = lexer.GetToken();
        InstructionNode* statementList = parse_statement_list();
        t = lexer.GetToken();

        if(reserved[t.token_type]=="RBRACE")
        {
            t = lexer.peek(1);
            
            if(reserved[t.token_type]=="NUM"){
                parse_input_section();
            }
            return statementList;
        }
        else
        {
            cout<<"parse_body_1"<<endl;
        }
    }
}


struct InstructionNode* parse_program()
{
    
    Token t = lexer.peek(1);

    if(reserved[t.token_type]=="ID")
    {
        parse_var_section();

        InstructionNode* body = parse_body();
    
        return body;        
    }
    
    else
    {
        cout<<"parse_program_1"<<endl;
        
    }
    
    
}
struct InstructionNode * parse_generate_intermediate_representation()
{

        return parse_program();

}