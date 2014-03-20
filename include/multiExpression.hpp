#ifndef MULTIEXPRESSION_HPP
#define MULTIEXPRESSION_HPP

#include <list>

#include <boost/regex.hpp>

#include "expression.hpp"

using namespace std;

class MultiExpression
{

	public:
	/**
	  * todas as variaveis da expressao inicial correspondem a qualquer uma das subexpressoes
	  */
	MultiExpression(list<Expression> subexpressions,string expression);

	/**
    * se a frase satisfizer a expressao, faz o match entre as variaveis da frase com as da expressao
    * retorna uma lista dos matches feitos
    * se a frase nao satisfizer, throws eUnmatchedExpression
    */
	list<t_match > findAllSub(string phrase);

	string expression();
	string regexpression();

	protected:

	boost::regex regexp; //a expressao regular utilizada
	string exp;           //a string da expressao regular
	string regexStr;
	unsigned int numVars;//numero de variaveis da expressao principal
	unsigned int numSub; //numero de subexpressoes
	char *vars;          //os tipos das variaveis que existem na expressao principal
	unsigned char *subvars;       //os tipos das subexpressoes existentes

};

#endif // MULTIEXPRESSION_HPP

