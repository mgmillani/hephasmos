

#include <boost/regex.hpp>

#include "stringer.hpp"
#include "expression.hpp"
#include "multiExpression.hpp"

#include "defs.hpp"
#define ERR_OFF
#include "debug.hpp"

	/**
	  * todas as variaveis da expressao inicial correspondem a qualquer uma das subexpressoes
	  */
	MultiExpression::MultiExpression(list<Expression> subexpressions,string expression)
	{
		//determina quais sao as variaveis das subexpressoes
		this->numSub = subexpressions.size();
		this->subvars = (unsigned char *)malloc(this->numSub);
		list<Expression>::iterator et;
		unsigned int i=0;
		for(et=subexpressions.begin() ; et!=subexpressions.end() ; et++,i++)
		{
			this->subvars[i] = et->vars[0];
		}

		//determina o numero de variaveis da expressao
		unsigned int amount=0;
		bool escape = false;
		for(i=0 ; i<expression.size() ; i++)
		{
			if(escape)
			{
				escape = false;
				continue;
			}
			else if(expression[i]=='\\')
				escape = true;
			else if(isReserved(expression[i]))
				amount++;

		}
		this->numVars = amount;

		//determina quais sao as variaveis da expressao principal
		this->vars = (char *)malloc(amount);

		//cria a expressao regular
		string regexStr = "[[:blank:]]*";
		escape = false;
		amount = 0;
		for(i=0 ; i<expression.size() ; i++)
		{
			char c= expression[i];
			if(c=='\\')
			{
				escape = true;
				continue;
			}
			else if(stringIn(c,".[{}()\\*+?|^$"))
			{
				regexStr += '\\';
				regexStr += c;
			}
			//as variaveis sao qualquer sequencia de a-zA-Z0-9 ou _
			else if(isReserved(c) && !escape)
			{
				//pode ser qualquer uma das subexpressoes
				regexStr += "(";
				//para cada subexpressao, adiciona a sua expressao regular
				unsigned int max = subexpressions.size();
				unsigned int count = 0;
				list<Expression>::iterator  it;
				for(it=subexpressions.begin() ; count<max-1; it++,count++)
				{
					regexStr += "(?:" + it->regexpression() + ")|";
				}
				regexStr += "(?:" + it->regexpression() + ")";
				regexStr += ")";
				//guarda o tipo da variavel
				this->vars[amount++] = c;
			}
			else
			{
				regexStr += c;
			}
			//qualquer sequencia (0 ou mais) de brancos
			regexStr += "(?:[[:blank:]]*)";
			escape = false;
		}

		this->regexStr = regexStr;
		this->regexp = boost::regex(regexStr,boost::regex::icase);

	}

	/**
    * se a frase satisfizer a expressao, faz o match entre as variaveis da frase com as da expressao
    * retorna uma lista dos matches feitos
    * se a frase nao satisfizer, throws eUnmatchedExpression
    */
	list<t_match > MultiExpression::findAllSub(string phrase)
	{
		//usa a regex
		boost::match_results<const char*> what;
		boost::regex_match(phrase.c_str(),what,this->regexp);
		if(what[0].matched==0)
		{
			throw(eUnmatchedExpression);
		}

		unsigned int i,t;
		list<t_match> vars;

		for(t=0,i=2 ; i<what.size() ; t++,i+=this->numSub*3+1)
		{
			// type of the element on the top expression
			e_type type = varToNum(this->vars[t]);
			t_match m;
			for(int j=0 ; j<TYPE_TOTAL ; j++)
				m.subtype[j] = 0;
			//determina o subtipo do elemento encontrado

			for(unsigned int j=0,k=0 ; j<this->numSub ; j++,k+=3)
			{
				if(what[i+k].matched)
				{
					m.element = what[i+k];
					m.operation = what[i+k+1];
					m.operand = what[i+k+2];

					e_type subtype = varToNum(this->subvars[j]);
					subtype = mostStrictType(type,subtype);
					if(subtype != TYPE_NONE)
						m.subtype[subtype] = 1;
					m.indexes.push_back(j);
				}
			}
			vars.push_back(m);
		}

		return vars;
	}

	string MultiExpression::expression()
	{
		return this->exp;
	}
	string MultiExpression::regexpression()
	{
		return this->regexStr;
	}

