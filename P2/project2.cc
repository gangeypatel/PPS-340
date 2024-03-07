/*
 * Copyright (C) Mohsen Zohrevandi, 2017
 *               Rida Bazzi 2019
 * Do not share this file with anyone
 */
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <set>
#include "lexer.h"
#include <unordered_map>

using namespace std;

LexicalAnalyzer lexer;
unordered_map<string, int> tokenorder, nonterminals, terminals;
vector<pair<string, vector<string> > > grammar;
string EPSILON = "EPSILON";
const string DOLLAR = "$";
int grammarLen;
unordered_map<string,set<string> > firstmap, followmap;
string startsymbol;
set<string> generatingSymbols = {EPSILON};
set<string> reachableSymbols;
vector<bool> generatingGrammar, reachableGrammar;


struct TokenCompare
{
    bool operator ()(string a, string b)
    {
        return tokenorder[a]<tokenorder[b];
    }
} tokenCompare;


bool isNonTerminal(string lexeme)
{
    if(nonterminals.find(lexeme)!=nonterminals.end())
        return true;
    return false;
}

void addTokenToTokenOrder(string lexeme, int order)
{
    if(tokenorder.find(lexeme)==tokenorder.end())
                tokenorder[lexeme]=order;
}

void getRHS(vector<string> &rhs, int &order)
{
    Token t = lexer.GetToken();
    while(!(t.token_type==END_OF_FILE || t.token_type==HASH))
    {
        addTokenToTokenOrder(t.lexeme, order);
        rhs.push_back(t.lexeme);
        order++;
        t = lexer.GetToken();
    }

    if(rhs.size()==0)
    {
        rhs.push_back(EPSILON);
        if(tokenorder.find(EPSILON)==tokenorder.end())
        {
            addTokenToTokenOrder(EPSILON, order);
            order++;
        }
    }
    return;
}

void addAllTokensToMap()
{
    Token t = lexer.GetToken();
    bool lastSeenHash = true;
    int order = 1;

    while(t.token_type!=END_OF_FILE)
    {
        vector<string> rhsGrammar;
        pair<string, vector<string> > singleGrammarRule;
        //cout<<"-"<<t.lexeme<<"-"<<t.line_no<<endl;
        if(t.lexeme!="")
        {
            addTokenToTokenOrder(t.lexeme, order); //Adding LHS To tokenOrder
            singleGrammarRule.first=t.lexeme;
            nonterminals[t.lexeme]=1;
            order++;
            t = lexer.GetToken();
            if(t.token_type==ARROW)
            {
                getRHS(rhsGrammar, order);
                singleGrammarRule.second=rhsGrammar;
                grammar.push_back(singleGrammarRule);
            }
            else
            {
                cout<<"NOT ARROW-"<<t.lexeme<<"-"<<t.line_no<< endl;
                exit(1);
            }

        }
        //order++;
        t=lexer.GetToken();
    }

    unordered_map<string, int>::iterator itr = tokenorder.begin();

    while(itr!=tokenorder.end())
    {
        if(!isNonTerminal(itr->first))
        {
            terminals[itr->first]=1;
        }
        itr++;
    }

}

// read grammar
void ReadGrammar()
{
    addAllTokensToMap();
    
    grammarLen = grammar.size();
    startsymbol = grammar[0].first;
    //startsymbol = tokenorder.find("S")!=tokenorder.end() ? "S" : grammar[0].first;
    // TokenCompare tc;

    // sort(v.begin(), v.end(), tc);

    // for(int i=0;i<v.size();i++)
    // {
    //     cout<<v[i]<<endl;
    // }

    // for(int i=0;i<grammar.size();i++)
    // {
    //     cout<<grammar[i].first<<" -> ";
    //     for(int j=0; j<grammar[i].second.size();j++)
    //     {
    //         cout<<grammar[i].second[j]<<" ";
    //     }
    //     cout<<endl;
    // }

    //cout<<tokenorder.size()<<" "<<nonterminals.size()<<" "<<terminals.size()<<endl;
    //cout << "0\n";
}


// Task 1
void printTerminalsAndNoneTerminals()
{
    vector<string> nonterminalsVector, terminalsVector;

    for(unordered_map<string, int>::iterator itr = nonterminals.begin(); itr!=nonterminals.end(); itr++)
    {
        terminalsVector.push_back(itr->first);
    }

    sort(nonterminalsVector.begin(), nonterminalsVector.end(), tokenCompare);

    for(unordered_map<string, int>::iterator itr = terminals.begin(); itr!=terminals.end(); itr++)
    {
        nonterminalsVector.push_back(itr->first);
    }
    
    sort(terminalsVector.begin(), terminalsVector.end(), tokenCompare);

    for(int i=0;i<nonterminalsVector.size();i++)
    {
        cout<<terminalsVector[i]<<" ";
    }

    for(int i=0;i<terminalsVector.size();i++)
    {
        if(terminalsVector[i]!=EPSILON)
            cout<<nonterminalsVector[i]<<" ";
    }
    cout<<endl;
    //cout << "1\n";
}

bool hasAllGeneratingSymbols(vector<string> &rhs)
{
    int len=rhs.size();

    for(int i=0;i<len;i++)
    {
        if(generatingSymbols.find(rhs[i])==generatingSymbols.end())
            return false;
    }
    return true;
}

void getGeneratingSymbols()
{
    for(unordered_map<string, int>::iterator itr=terminals.begin();itr!=terminals.end();itr++)
    {
        generatingSymbols.insert(itr->first);
    }

    bool changed = true;

    while(changed)
    {
        changed = false;

        for(int i=0;i<grammarLen;i++)
        {
            string &lhs = grammar[i].first;
            vector<string> &rhs = grammar[i].second;
            if(generatingSymbols.find(lhs)==generatingSymbols.end() && hasAllGeneratingSymbols(rhs))
            {
                generatingSymbols.insert(lhs);
                changed = true;
            }
        }
    }
}

void getGeneratingGrammarRules()
{
    for(int i=0;i<grammarLen;i++)
    {
        generatingGrammar.push_back(false);
    }

    for(int i=0;i<grammarLen;i++)
    {
        string &lhs = grammar[i].first;
        vector<string> &rhs = grammar[i].second;
        generatingGrammar[i] = generatingSymbols.find(lhs)!=generatingSymbols.end() && hasAllGeneratingSymbols(rhs);
    }

}


void getReachableSymbols()
{
    reachableSymbols.insert(startsymbol);

    bool changed = true;

    while(changed)
    {
        changed = false;

        for(int i=0;i<grammarLen;i++)
        {
            if(generatingGrammar[i])
            {
                string &lhs = grammar[i].first;
                vector<string> &rhs = grammar[i].second;
                int rhsLen = rhs.size();

                if(reachableSymbols.find(lhs)!=reachableSymbols.end())   //Check if LHS is in reachableSymbols
                {
                    for(int j=0;j<rhsLen;j++)   //Check and add RHS to reachableSymbols
                    {
                        if(reachableSymbols.find(rhs[j])==reachableSymbols.end())
                        {
                            reachableSymbols.insert(rhs[j]);
                            changed = true;
                        }
                    }
                }
            }
        }
    }
}

bool hasAllReachableSymbols(vector<string> &rhs)
{
    int len=rhs.size();

    for(int i=0;i<len;i++)
    {
        if(reachableSymbols.find(rhs[i])==reachableSymbols.end())
            return false;
    }
    return true;
}

void getReachableGrammar()
{
    for(int i=0;i<grammarLen;i++)
    {
        reachableGrammar.push_back(false);
    }

    for(int i=0;i<grammarLen;i++)
    {
        if(generatingGrammar[i])
        {
            string &lhs = grammar[i].first;
            vector<string> &rhs = grammar[i].second;
            reachableGrammar[i] = reachableSymbols.find(lhs)!=reachableSymbols.end() && hasAllReachableSymbols(rhs);
        }
    }
}

void PrintReachableGrammar()
{
    for(int i=0;i<grammarLen;i++)
    {
        if(reachableGrammar[i])
        {
            string &lhs = grammar[i].first;
            vector<string> &rhs = grammar[i].second;
            int rhsLen = rhs.size();

            cout<<lhs<<" ->";

            for(int j=0;j<rhsLen;j++)
            {
                if(rhs[j]==EPSILON)
                {
                    cout<<" #"<<endl;
                    break;
                }
                else
                {
                    cout<<" "<<rhs[j];
                }
            }
            cout<<endl;
        }
    }
}

// Task 2
void RemoveUselessSymbols()
{
    getGeneratingSymbols();
    getGeneratingGrammarRules();
    getReachableSymbols();
    getReachableGrammar();
}

void addTerminalFirsts()
{
    for(unordered_map<string, int>::iterator itr = terminals.begin(); itr!=terminals.end(); itr++)
    {
        set<string> terminalSet{itr->first};
        firstmap[itr->first] = terminalSet;
    }

    
    for(unordered_map<string, int>::iterator itr = nonterminals.begin(); itr!=nonterminals.end(); itr++)
    {
        set<string> emptySet;
        firstmap[itr->first] = emptySet;
    }

    set<string> epsilonSet{EPSILON};
    firstmap[EPSILON] = epsilonSet;
}

bool hasEpsilonInFirstSet(string token)
{
    return !(firstmap[token].find(EPSILON)==firstmap[token].end());
}

bool addRHSFirstToLHSFirst(string lhs, string rhs, bool changed)
{
    //cout<<"IN addRHSFirstToLHSFirst: LHS "<<lhs<<" RHS "<<rhs<<endl;
    for (set<string>::iterator itr=firstmap[rhs].begin(); itr!=firstmap[rhs].end(); itr++)
    {
        //cout<<"RHS "<<*itr<<endl;
        if(*itr!=EPSILON && firstmap[lhs].find(*itr)==firstmap[lhs].end())
        {
            changed = true;
            firstmap[lhs].insert(*itr);
        }
    }
    return changed;
}


bool firstSetPass()
{
    bool changed = false;
    for(int a=0;a<grammarLen;a++)
    {
        string &lhs = grammar[a].first;
        vector<string> &rhs = grammar[a].second;
        int rhsLen = rhs.size();
        bool addEpsilonToFirst = true;
        //cout<<"GRAMMAR RULE NEW ###################"<<endl;
        for(int i=0;i<rhsLen;i++)
        {
            // if(rhsLen==1)
            // {
            //     cout<<"RHS VAL "<<rhs[i]<<endl;
            // }

            if(!isNonTerminal(rhs[i]))
            {
                //cout<<"TERMINAL LHS "<<lhs<<" RHS "<<rhs[i]<<endl;
                changed = addRHSFirstToLHSFirst(lhs, rhs[i], false) || changed;
                //cout<<"RHS["<<i<<"] "<<rhs[i]<<endl;
                addEpsilonToFirst = rhs[i]==EPSILON;
                break;
            }
            else
            {
                //cout<<"NON-TERMINAL LHS "<<lhs<<" RHS "<<rhs[i]<<endl;
                changed = addRHSFirstToLHSFirst(lhs, rhs[i], false) || changed;
                if(!hasEpsilonInFirstSet(rhs[i]))
                {
                    addEpsilonToFirst = false;
                    break;
                }
            }
        }

        if(addEpsilonToFirst)
        {
            if(firstmap[lhs].find(EPSILON)==firstmap[lhs].end())
            {
                firstmap[lhs].insert(EPSILON);
                changed = true;
            }
        }
    }
    return changed;
}

void PrintFirstSets()
{
    vector<string> firstSetOutput;
    for(unordered_map<string,set<string> >::iterator itr = firstmap.begin(); itr!=firstmap.end(); itr++)
    {
        if(isNonTerminal(itr->first))
        {
            firstSetOutput.push_back(itr->first);
            // vector<string> firstSet;
            // for(set<string>::iterator itrset = itr->second.begin(); itrset!=itr->second.end(); itrset++)
            // {
            //     if(*itrset == EPSILON)
            //     {
            //         firstSet.push_back("#");
            //     }
            //     else
            //     {
            //         firstSet.push_back(*itrset);
            //     }
            // }

            // sort(firstSet.begin(), firstSet.end(), tokenCompare);
            // firstSetOutput.push_back(make_pair(itr->first, firstSet));
        }
    }

    sort(firstSetOutput.begin(), firstSetOutput.end(), tokenCompare);

    for(int i=0;i<firstSetOutput.size();i++)
    {
        vector<string> firstSet;
        for(set<string>::iterator itrset = firstmap[firstSetOutput[i]].begin(); itrset!=firstmap[firstSetOutput[i]].end(); itrset++)
        {
            if(*itrset == EPSILON)
            {
                firstSet.push_back("#");
            }
            else
            {
                firstSet.push_back(*itrset);
            }
        }
        sort(firstSet.begin(), firstSet.end(), tokenCompare);
        int len = firstSet.size();
        cout<<"FIRST("<<firstSetOutput[i]<<") = {";
        for(int j=0;j<len;j++)
        {
            cout<<" "<<firstSet[j];
            if(j!=len-1)
            {
                cout<<",";
            }
        }
        cout<<" }"<<endl;
    }

}
// Task 3
void CalculateFirstSets()
{
    addTerminalFirsts();
    
    bool changed = true;

    while(changed)
    {
        changed = firstSetPass();
    }

    //cout << "3\n";
}


bool addFirstToFollowForSingleNonterminal(string lhs, string rhs, bool changed)
{
    //cout<<"IN addRHSFirstToLHSFirst: LHS "<<lhs<<" RHS "<<rhs<<endl;
    for (set<string>::iterator itr=firstmap[rhs].begin(); itr!=firstmap[rhs].end(); itr++)
    {
        //cout<<"RHS "<<*itr<<endl;
        if(*itr!=EPSILON && followmap[lhs].find(*itr)==followmap[lhs].end())
        {
            changed = true;
            followmap[lhs].insert(*itr);
        }
    }
    return changed;
}

bool addLHSFollowToRHSFollow(string lhs, string rhs, bool changed)
{
    for (set<string>::iterator itr=followmap[lhs].begin(); itr!=followmap[lhs].end(); itr++)
    {
        //cout<<"RHS "<<*itr<<endl;
        if(followmap[rhs].find(*itr)==followmap[rhs].end())
        {
            // if(*itr == DOLLAR && rhs == "A")
            // {
            //     cout<<lhs<<" "<<rhs<<endl;
            // }
            changed = true;
            followmap[rhs].insert(*itr);
        }
    }
    return changed;
}

bool addFirstsToFollow()
{
    bool changed = false;
    for(int i=0;i<grammarLen;i++)
    {
        vector<string> &rhs = grammar[i].second;
        int rhsLen = rhs.size();
        for(int j=0;j<rhsLen;j++)
        {
            for(int k=j+1;k<rhsLen && isNonTerminal(rhs[j]);k++)
            {
                changed = addFirstToFollowForSingleNonterminal(rhs[j], rhs[k], false) || changed;
                if(!isNonTerminal(rhs[k]) || !hasEpsilonInFirstSet(rhs[k]))
                {
                    break;
                }
            }
        }
    }

    return changed;
}

void addDollarToStartFollowSet()
{
    set<string> dollarSet{DOLLAR};
    followmap[startsymbol]=dollarSet;
}

void initializeFollowSetsOfNonterminal()
{
    for(unordered_map<string, int>::iterator itr = nonterminals.begin();itr!=nonterminals.end();itr++)
    {
        set<string> emptySet;
        followmap[itr->first] = emptySet;
    }
}

bool followSetPass()
{

    bool changed = false;
    for(int i=0;i<grammarLen;i++)
    {
        string &lhs = grammar[i].first;
        vector<string> &rhs = grammar[i].second;
        int rhsLen = rhs.size();

        for(int j=rhsLen-1;j>=0;j--)
        {
            if(isNonTerminal(rhs[j]))
            {
                changed = addLHSFollowToRHSFollow(lhs, rhs[j], false) || changed;
            }
            if(!isNonTerminal(rhs[j]) || !hasEpsilonInFirstSet(rhs[j]))
            {
                break;
            }
        }
    }
    return changed;
}

void printFollowSets()
{
    vector<string> followSetOutput;
    for(unordered_map<string,set<string> >::iterator itr = followmap.begin(); itr!=followmap.end(); itr++)
    {
        if(isNonTerminal(itr->first))
        {
            followSetOutput.push_back(itr->first);
            // vector<string> firstSet;
            // for(set<string>::iterator itrset = itr->second.begin(); itrset!=itr->second.end(); itrset++)
            // {
            //     if(*itrset == EPSILON)
            //     {
            //         firstSet.push_back("#");
            //     }
            //     else
            //     {
            //         firstSet.push_back(*itrset);
            //     }
            // }

            // sort(firstSet.begin(), firstSet.end(), tokenCompare);
            // firstSetOutput.push_back(make_pair(itr->first, firstSet));
        }
    }

    sort(followSetOutput.begin(), followSetOutput.end(), tokenCompare);

    for(int i=0;i<followSetOutput.size();i++)
    {
        vector<string> followSet;
        for(set<string>::iterator itrset = followmap[followSetOutput[i]].begin(); itrset!=followmap[followSetOutput[i]].end(); itrset++)
        {
                followSet.push_back(*itrset);
        }
        sort(followSet.begin(), followSet.end(), tokenCompare);
        int len = followSet.size();
        cout<<"FOLLOW("<<followSetOutput[i]<<") = {";
        for(int j=0;j<len;j++)
        {
            cout<<" "<<followSet[j];
            if(j!=len-1)
            {
                cout<<",";
            }
        }
        cout<<" }"<<endl;
    }
}

// Task 4
void CalculateFollowSets()
{
    initializeFollowSetsOfNonterminal();
    addDollarToStartFollowSet();
    addFirstsToFollow();
    bool changed = true;

    while(changed)
    {
        changed = followSetPass();
    }


    // for(unordered_map<string,set<string>>::iterator itr = followmap.begin();itr!=followmap.end();itr++)
    // {
    //     cout<<itr->first<<" --> ";
    //     for(set<string>::iterator itr2 = itr->second.begin();itr2!=itr->second.end();itr2++)
    //     {
    //         cout<<*itr2<<" ";
    //     }
    //     cout<<endl;
    // }

    //cout << "4\n";
}


bool IntersectionOfFirstAndFollow(string term)
{
    set<string> firstOfA = firstmap[term], followOfA = followmap[term];

    for(set<string>::iterator itr = firstOfA.begin();itr!=firstOfA.end();itr++)
    {
        if(followOfA.find(*itr)!=followOfA.end())
            return true;
    }
    return false;
}

bool IntersectionOfTwoFirsts(set<string> firstOfA, set<string> firstOfB)
{
    
    for(set<string>::iterator itr = firstOfA.begin();itr!=firstOfA.end();itr++)
    {
        if(firstOfB.find(*itr)!=firstOfB.end())
            return true;
    }
    return false;
}

set<string> calculateFirstSetForSingleRule(int grammarIdx)
{
    string &lhs = grammar[grammarIdx].first;
    vector<string> &rhs = grammar[grammarIdx].second;
    int rhsLen = rhs.size();
    bool addEpsilonToFirst = true;

    set<string> firstSetOfLHS;
    for(int i=0;i<rhsLen;i++)
    {
        if(!isNonTerminal(rhs[i]))
        {
            firstSetOfLHS.insert(firstmap[rhs[i]].begin(), firstmap[rhs[i]].end());
            // addRHSFirstToLHSFirst(lhs, rhs[i], false);
            firstSetOfLHS.erase(EPSILON);
            addEpsilonToFirst = rhs[i]==EPSILON;
            break;
        }
        else
        {
            firstSetOfLHS.insert(firstmap[rhs[i]].begin(), firstmap[rhs[i]].end());
            // addRHSFirstToLHSFirst(lhs, rhs[i], false);
            firstSetOfLHS.erase(EPSILON);
            if(!hasEpsilonInFirstSet(rhs[i]))
            {
                addEpsilonToFirst = false;
                break;
            }
        }
        //cout<<"SET SIZE "<< firstSetOfLHS.size()<<endl;
    }

    if(addEpsilonToFirst)
    {
        firstSetOfLHS.insert(EPSILON);
    }

    return firstSetOfLHS;
}


bool CheckIntersectionForNonTerminal(vector<int> &v)
{
    int len=v.size();

    for(int i=0;i<len;i++)
    {
        set<string> rhs1 = calculateFirstSetForSingleRule(v[i]);
        for(int j=i+1;j<len;j++)
        {
            //vector<string> rhs2 = grammar[v[j]].second;
            set<string> rhs2 = calculateFirstSetForSingleRule(v[j]);
            if(IntersectionOfTwoFirsts(rhs1, rhs2))
            {
                return true;
            }
        }
    }
    
    return false;
}

// Task 5
void CheckIfGrammarHasPredictiveParser()
{
    // cout<<"tokenorder"<<endl;
    // for(unordered_map<string, int>::iterator itr = tokenorder.begin();itr!=tokenorder.end();itr++)
    // {
    //     cout<<itr->first<<" ";
    // }
    // cout<<endl;
    
    // cout<<"reachableSymbols"<<endl;
    // for(set<string>::iterator itr = reachableSymbols.begin();itr!=reachableSymbols.end();itr++)
    // {
    //     cout<<*itr<<" ";
    // }
    // cout<<endl;

    if(reachableSymbols.size() != tokenorder.size())
    {
        cout<<"NO"<<endl;
        return;
    }

    for(unordered_map<string, int>::iterator itr = nonterminals.begin();itr!=nonterminals.end();itr++)
    {
        vector<int> v;
        if(hasEpsilonInFirstSet(itr->first) && IntersectionOfFirstAndFollow(itr->first))
        {
            cout<<"NO"<<endl;
            return;
        }

        for(int i=0;i<grammarLen;i++)
        {
            if(grammar[i].first==itr->first)
            {
                v.push_back(i);
            }
        }

        if(v.size()>1 && CheckIntersectionForNonTerminal(v))
        {
            cout<<"NO"<<endl;
            return;
        }

    }

    cout<<"YES"<<endl;
    
}
    
int main (int argc, char* argv[])
{
    int task;

    if (argc < 2)
    {
        cout << "Error: missing argument\n";
        return 1;
    }

    /*
       Note that by convention argv[0] is the name of your executable,
       and the first argument to your program is stored in argv[1]
     */

    task = atoi(argv[1]);

    
    ReadGrammar();  // Reads the input grammar from standard input
                    // and represent it internally in data structures
                    // ad described in project 2 presentation file

    switch (task) {
        case 1: printTerminalsAndNoneTerminals();
            break;

        case 2:
            RemoveUselessSymbols();
            PrintReachableGrammar();
            break;

        case 3:
            CalculateFirstSets();
            PrintFirstSets();
            break;

        case 4:
            CalculateFirstSets();
            CalculateFollowSets();
            printFollowSets();
            break;

        case 5:
            CalculateFirstSets();
            CalculateFollowSets();
            RemoveUselessSymbols();
            CheckIfGrammarHasPredictiveParser();
            break;

        default:
            cout << "Error: unrecognized task number " << task << "\n";
            break;
    }
    return 0;
}

