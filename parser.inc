#include "introspect"

#include <a_samp>
// Tokeniser.  Read text and convert it to tokens for simple processing.

enum e_TOKEN_OP
{
	e_TOKEN_OP_NONE,
	e_TOKEN_OP_ASSIGN, // '='
	e_TOKEN_OP_EQUALS, // '=='
	e_TOKEN_OP_LTE, // '<='
	e_TOKEN_OP_GTE, // '>='
	e_TOKEN_OP_LESS, // '<'
	e_TOKEN_OP_GREATER, // '>'
	e_TOKEN_OP_NOT, // '!'
	e_TOKEN_OP_NEQ, // '!='
	e_TOKEN_OP_INV, // '~'
	e_TOKEN_OP_INV_ASS, // '~='
	e_TOKEN_OP_ADD, // '+'
	e_TOKEN_OP_ADD_ASS, // '+='
	e_TOKEN_OP_SUB, // '-'
	e_TOKEN_OP_SUB_ASS, // '-='
	e_TOKEN_OP_MUL, // '*'
	e_TOKEN_OP_MUL_ASS, // '*='
	e_TOKEN_OP_DIV, // '/'
	e_TOKEN_OP_DIV_ASS, // '/='
	e_TOKEN_OP_MOD, // '%'
	e_TOKEN_OP_MOD_ASS, // '%='
	e_TOKEN_OP_XOR, // '^'
	e_TOKEN_OP_XOR_ASS, // '^='
	e_TOKEN_OP_LAND, // '&'
	e_TOKEN_OP_LAND_ASS, // '&='
	e_TOKEN_OP_LOR, // '|'
	e_TOKEN_OP_LOR_ASS, // '|='
	e_TOKEN_OP_RSHIFT, // '>>'
	e_TOKEN_OP_RSHIFT_ASS, // '>>='
	e_TOKEN_OP_SHIFT, // '>>>'
	e_TOKEN_OP_SHIFT_ASS, // '>>>='
	e_TOKEN_OP_LSHIFT, // '<<'
	e_TOKEN_OP_LSHIFT_ASS, // '<<='
	e_TOKEN_OP_AND, // '&&'
	e_TOKEN_OP_OR, // '||'
	e_TOKEN_OP_OP_BRACKET, // '('
	e_TOKEN_OP_CL_BRACKET, // ')'
	e_TOKEN_OP_OP_BRACE, // '{'
	e_TOKEN_OP_CL_BRACE, // '}'
	e_TOKEN_OP_OP_SQUARE, // '['
	e_TOKEN_OP_CL_SQUARE, // ']'
	e_TOKEN_OP_CL_INC, // '++'
	e_TOKEN_OP_CL_DEC, // '--'
	e_TOKEN_OP_CL_ELIPSIS, // '...'
	e_TOKEN_OP_CL_CONCAT, // '..'
	e_TOKEN_OP_CL_HASH, // '#'
	e_TOKEN_OP_CL_PAAMAYIM, // '::'
	e_TOKEN_OP_CL_DQUOTE, // '"'
	e_TOKEN_OP_CL_QUOTE, // '''
	e_TOKEN_OP_CL_SEMICOLON, // ';'
	e_TOKEN_OP_CL_COMMA, // ','
	e_TOKEN_OP_CL_POINT, // '.'
	e_TOKEN_OP_CL_COLON, // ':'
	e_TOKEN_OP_CL_QUESTION, // '?'
}

enum e_TOKEN_TYPE
{
	e_TOKEN_TYPE_NONE,
	e_TOKEN_TYPE_STRING,
	e_TOKEN_TYPE_FLOAT,
	// These two are distinct for simplicity.
	e_TOKEN_TYPE_NATIVE,
	e_TOKEN_TYPE_FUNC,
	e_TOKEN_TYPE_VAR,
	e_TOKEN_TYPE_NUM,
	e_TOKEN_TYPE_BOOL,
	e_TOKEN_TYPE_OP,
}

enum E_TOKEN
{
	e_TOKEN_TYPE:E_TOKEN_TYPE,
	E_TOKEN_NUM_VAL = 1,
	Float:E_TOKEN_FLOAT_VAL = 1,
	bool:E_TOKEN_BOOL_VAL = 1,
	e_TOKEN_OP:E_TOKEN_OP = 1,
	E_TOKEN_SYM_PTR = 1,
	E_TOKEN_NATIVE_IDX = 1,
}

/******************************************************************************\
	Parser_PrintOp

	Print an operator (or rather return it) given an identifier.

\******************************************************************************/
static stock Parser_PrintOp(e_TOKEN_OP:op)
{
	new
		ret[8];
	switch (op)
	{
		case e_TOKEN_OP_ASSIGN: ret = "=";
		case e_TOKEN_OP_EQUALS: ret = "==";
		case e_TOKEN_OP_LTE: ret = "<=";
		case e_TOKEN_OP_GTE: ret = ">=";
		case e_TOKEN_OP_LESS: ret = "<";
		case e_TOKEN_OP_GREATER: ret = ">";
		case e_TOKEN_OP_NOT: ret = "!";
		case e_TOKEN_OP_NEQ: ret = "!=";
		case e_TOKEN_OP_INV: ret = "~";
		case e_TOKEN_OP_INV_ASS: ret = "~=";
		case e_TOKEN_OP_ADD: ret = "+";
		case e_TOKEN_OP_ADD_ASS: ret = "+=";
		case e_TOKEN_OP_SUB: ret = "-";
		case e_TOKEN_OP_SUB_ASS: ret = "-=";
		case e_TOKEN_OP_MUL: ret = "*";
		case e_TOKEN_OP_MUL_ASS: ret = "*=";
		case e_TOKEN_OP_DIV: ret = "/";
		case e_TOKEN_OP_DIV_ASS: ret = "/=";
		case e_TOKEN_OP_MOD: ret = "%";
		case e_TOKEN_OP_MOD_ASS: ret = "%=";
		case e_TOKEN_OP_XOR: ret = "^";
		case e_TOKEN_OP_XOR_ASS: ret = "^=";
		case e_TOKEN_OP_LAND: ret = "&";
		case e_TOKEN_OP_LAND_ASS: ret = "&=";
		case e_TOKEN_OP_LOR: ret = "|";
		case e_TOKEN_OP_LOR_ASS: ret = "|=";
		case e_TOKEN_OP_RSHIFT: ret = ">>";
		case e_TOKEN_OP_RSHIFT_ASS: ret = ">>=";
		case e_TOKEN_OP_SHIFT: ret = ">>>";
		case e_TOKEN_OP_SHIFT_ASS: ret = ">>>=";
		case e_TOKEN_OP_LSHIFT: ret = "<<";
		case e_TOKEN_OP_LSHIFT_ASS: ret = "<<=";
		case e_TOKEN_OP_AND: ret = "&&";
		case e_TOKEN_OP_OR: ret = "||";
		case e_TOKEN_OP_OP_BRACKET: ret = "(";
		case e_TOKEN_OP_CL_BRACKET: ret = ")";
		case e_TOKEN_OP_OP_BRACE: ret = "{";
		case e_TOKEN_OP_CL_BRACE: ret = "}";
		case e_TOKEN_OP_OP_SQUARE: ret = "[";
		case e_TOKEN_OP_CL_SQUARE: ret = "]";
		case e_TOKEN_OP_CL_INC: ret = "++";
		case e_TOKEN_OP_CL_DEC: ret = "--";
		case e_TOKEN_OP_CL_ELIPSIS: ret = "...";
		case e_TOKEN_OP_CL_CONCAT: ret = "..";
		case e_TOKEN_OP_CL_HASH: ret = "#";
		case e_TOKEN_OP_CL_PAAMAYIM: ret = "::";
		case e_TOKEN_OP_CL_DQUOTE: ret = "\"";
		case e_TOKEN_OP_CL_QUOTE: ret = "\'";
		case e_TOKEN_OP_CL_SEMICOLON: ret = ";";
		case e_TOKEN_OP_CL_COMMA: ret = ",";
		case e_TOKEN_OP_CL_POINT: ret = ".";
		case e_TOKEN_OP_CL_COLON: ret = ":";
		case e_TOKEN_OP_CL_QUESTION: ret = "?";
		default: ret = "<none>";
	}
	return ret;
}

#define Parser_Error(%0) printf("PARSER ERROR: " %0)

static stock
	ISI_gInputLine[512],
	ISI_gInputLen,
	ISI_gInputPtr;

static stock Parser_GetSymType(const name[], &e_TOKEN_TYPE:t, &ptr)
{
}

static stock Parser_Peek()
{
	return ISI_gInputLine[ISI_gInputPtr];
}

#define Parser_IsWhitespace((%0)) ('\0' <= (%0) <= ' ')

static stock Parser_SkipWhitespace()
{
	while ('\0' < ISI_gInputLine[ISI_gInputPtr] <= ' ') ++ISI_gInputPtr;
}

#define PARSER_DO_OP_1(%9,%0)          else if (p0 == %0) ret[E_TOKEN_OP] = (%9), len = 1;
#define PARSER_DO_OP_2(%9,%0,%1)       else if (p0 == %0 && p1 == %1) ret[E_TOKEN_OP] = (%9), len = 2;
#define PARSER_DO_OP_3(%9,%0,%1,%2)    else if (p0 == %0 && p1 == %1 && p2 == %2) ret[E_TOKEN_OP] = (%9), len = 3;
#define PARSER_DO_OP_4(%9,%0,%1,%2,%3) else if (p0 == %0 && p1 == %1 && p2 == %2 && p3 == %3) ret[E_TOKEN_OP] = (%9), len = 4;

static stock Parser_GetOp(ret[E_TOKEN])
{
	ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_OP;
	new
		len, // = ISI_gInputLen - ISI_gInputPtr,
		p0, p1, p2, p3;
	if ((p0 = ISI_gInputLine[ISI_gInputPtr + 0])
		&& (p1 = ISI_gInputLine[ISI_gInputPtr + 1])
			&& (p2 = ISI_gInputLine[ISI_gInputPtr + 2]))
				p3 = ISI_gInputLine[ISI_gInputPtr + 3];
	// Should really use a "trie" here, but don't - too complex for now.
	if (len) return ret; // Placeholder "if" - leave it!
	PARSER_DO_OP_4(e_TOKEN_OP_SHIFT_ASS, '>', '>', '>', '=')
	PARSER_DO_OP_3(e_TOKEN_OP_RSHIFT_ASS, '>', '>', '=')
	PARSER_DO_OP_3(e_TOKEN_OP_SHIFT, '>', '>', '>')
	PARSER_DO_OP_3(e_TOKEN_OP_LSHIFT_ASS, '<', '<', '=')
	PARSER_DO_OP_3(e_TOKEN_OP_CL_ELIPSIS, '.', '.', '.')
	PARSER_DO_OP_2(e_TOKEN_OP_EQUALS, '=', '=')
	PARSER_DO_OP_2(e_TOKEN_OP_LTE, '<', '=')
	PARSER_DO_OP_2(e_TOKEN_OP_GTE, '>', '=')
	PARSER_DO_OP_2(e_TOKEN_OP_NEQ, '!', '=')
	PARSER_DO_OP_2(e_TOKEN_OP_INV_ASS, '~', '=')
	PARSER_DO_OP_2(e_TOKEN_OP_ADD_ASS, '+', '=')
	PARSER_DO_OP_2(e_TOKEN_OP_SUB_ASS, '-', '=')
	PARSER_DO_OP_2(e_TOKEN_OP_MUL_ASS, '*', '=')
	PARSER_DO_OP_2(e_TOKEN_OP_DIV_ASS, '/', '=')
	PARSER_DO_OP_2(e_TOKEN_OP_MOD_ASS, '%', '=')
	PARSER_DO_OP_2(e_TOKEN_OP_XOR_ASS, '^', '=')
	PARSER_DO_OP_2(e_TOKEN_OP_LAND_ASS, '&', '=')
	PARSER_DO_OP_2(e_TOKEN_OP_LOR_ASS, '|', '=')
	PARSER_DO_OP_2(e_TOKEN_OP_RSHIFT, '>', '>')
	PARSER_DO_OP_2(e_TOKEN_OP_LSHIFT, '<', '<')
	PARSER_DO_OP_2(e_TOKEN_OP_AND, '&', '&')
	PARSER_DO_OP_2(e_TOKEN_OP_OR, '|', '|')
	PARSER_DO_OP_2(e_TOKEN_OP_CL_PAAMAYIM, ':', ':')
	PARSER_DO_OP_2(e_TOKEN_OP_CL_INC, '+', '+')
	PARSER_DO_OP_2(e_TOKEN_OP_CL_DEC, '-', '-')
	PARSER_DO_OP_2(e_TOKEN_OP_CL_CONCAT, '.', '.')
	PARSER_DO_OP_1(e_TOKEN_OP_ASSIGN, '=')
	PARSER_DO_OP_1(e_TOKEN_OP_LESS, '<')
	PARSER_DO_OP_1(e_TOKEN_OP_GREATER, '>')
	PARSER_DO_OP_1(e_TOKEN_OP_NOT, '!')
	PARSER_DO_OP_1(e_TOKEN_OP_INV, '~')
	PARSER_DO_OP_1(e_TOKEN_OP_ADD, '+')
	PARSER_DO_OP_1(e_TOKEN_OP_SUB, '-')
	PARSER_DO_OP_1(e_TOKEN_OP_MUL, '*')
	PARSER_DO_OP_1(e_TOKEN_OP_DIV, '/')
	PARSER_DO_OP_1(e_TOKEN_OP_MOD, '%')
	PARSER_DO_OP_1(e_TOKEN_OP_XOR, '^')
	PARSER_DO_OP_1(e_TOKEN_OP_LAND, '&')
	PARSER_DO_OP_1(e_TOKEN_OP_LOR, '|')
	PARSER_DO_OP_1(e_TOKEN_OP_OP_BRACKET, '(')
	PARSER_DO_OP_1(e_TOKEN_OP_CL_BRACKET, ')')
	PARSER_DO_OP_1(e_TOKEN_OP_OP_BRACE, '{')
	PARSER_DO_OP_1(e_TOKEN_OP_CL_BRACE, '}')
	PARSER_DO_OP_1(e_TOKEN_OP_OP_SQUARE, '[')
	PARSER_DO_OP_1(e_TOKEN_OP_CL_SQUARE, ']')
	PARSER_DO_OP_1(e_TOKEN_OP_CL_HASH, '#')
	PARSER_DO_OP_1(e_TOKEN_OP_CL_DQUOTE, '"')
	PARSER_DO_OP_1(e_TOKEN_OP_CL_QUOTE, '\'')
	PARSER_DO_OP_1(e_TOKEN_OP_CL_SEMICOLON, ';')
	PARSER_DO_OP_1(e_TOKEN_OP_CL_COMMA, ',')
	PARSER_DO_OP_1(e_TOKEN_OP_CL_POINT, '.')
	PARSER_DO_OP_1(e_TOKEN_OP_CL_COLON, ':')
	PARSER_DO_OP_1(e_TOKEN_OP_CL_QUESTION, '?')
	if (len)
	{
		// Found an op.
		ISI_gInputPtr += len;
	}
	else
	{
		Parser_Error("Unexpected input at (%d).", ISI_gInputPtr);
		// Skip one character and try again.
		++ISI_gInputPtr;
	}
	return ret;
}

#undef PARSER_DO_OP_1
#undef PARSER_DO_OP_2
#undef PARSER_DO_OP_3
#undef PARSER_DO_OP_4

static stock Parser_GetNextToken(ret[E_TOKEN])
{
	ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_NONE,
	Parser_SkipWhitespace();
	switch (Parser_Peek())
	{
		case '\0': return ret;
		case '0' .. '9': Parser_GetNumber(ret);
		case 'a' .. 'z', 'A' .. 'Z', '_', '@': Parser_GetSymbol(ret);
//		case '"': Parser_GetString(ret);
		case '\'': Parser_GetChar(ret);
		default: Parser_GetOp(ret);
	}
	return ret;
}

static stock Parser_GetHex(ch)
{
	switch (ch)
	{
		case '0' .. '9': return ch - '0';
		case 'a' .. 'f': return ch - 'a' + 10;
		case 'A' .. 'F': return ch - 'A' + 10;
	}
	return -1;
}

/******************************************************************************\
	Parser_DoHex

	Given an input string we KNOW starts with a hex number, return that number.

\******************************************************************************/
static stock Parser_DoHex(ret[E_TOKEN])
{
	new
		ptr = ISI_gInputPtr + 2,
		cur,
		num = Parser_GetHex(ISI_gInputLine[ptr]);
	if (num == -1) Parser_Error("Invalid HEX number.");
	while ((cur = Parser_GetHex(ISI_gInputLine[++ptr])) != -1)
	{
		num = (num << 4) | cur;
	}
	//new
	//	ret[E_TOKEN] = {e_TOKEN_TYPE_NUM, 0};
	return
		ret[E_TOKEN_NUM_VAL] = num,
		ISI_gInputPtr = ptr,
		1;
}

static stock Parser_DoDecimal(ret[E_TOKEN])
{
	new
		ptr = ISI_gInputPtr,
		cur,
		num = ISI_gInputLine[ptr] - '0';
	if (0 <= num <= 9)
	{
		while (0 <= (cur = ISI_gInputLine[++ptr] - '0') <= 9)
		{
			num = num * 10 + cur;
		}
		return
			ret[E_TOKEN_NUM_VAL] = num,
			ISI_gInputPtr = ptr,
			1;
	}
	else return Parser_Error("Invalid HEX number.");
}

static stock Parser_GetBinary(ch)
{
	switch (ch)
	{
		case '0': return 0;
		case '1': return 1;
	}
	return -1;
}

static stock Parser_DoBinary(ret[E_TOKEN])
{
	new
		ptr = ISI_gInputPtr + 2,
		cur,
		num = Parser_GetBinary(ISI_gInputLine[ptr]);
	if (num == -1) Parser_Error("Invalid Binary number.");
	while ((cur = Parser_GetBinary(ISI_gInputLine[++ptr])) != -1)
	{
		num = (num << 1) | cur;
	}
	//new
	//	ret[E_TOKEN] = {e_TOKEN_TYPE_NUM, 0};
	ret[E_TOKEN_NUM_VAL] = num;
	ISI_gInputPtr = ptr;
	return 1;
}

static stock Parser_GetOctal(ch)
{
	switch (ch)
	{
		case '0' .. '7': return ch - '0';
	}
	return -1;
}

static stock Parser_DoOctal(ret[E_TOKEN])
{
	new
		ptr = ISI_gInputPtr + 1,
		cur,
		num = Parser_GetOctal(ISI_gInputLine[ptr]);
	if (num == -1) Parser_Error("Invalid Octal number.");
	while ((cur = Parser_GetOctal(ISI_gInputLine[++ptr])) != -1)
	{
		num = (num << 3) | cur;
	}
	//new
	//	ret[E_TOKEN] = {e_TOKEN_TYPE_NUM, 0};
	ret[E_TOKEN_NUM_VAL] = num;
	ISI_gInputPtr = ptr;
	return 1;
}

/******************************************************************************\
	Parser_Pow10

	Very quickly raise some number by a given exponent (i.e. n *= 10^e).

\******************************************************************************/
static stock Float:Parser_Pow10(Float:n, e)
{
	static const
		Float:scPow[] = {1.0, 10.0, 100.0, 1000.0, 10000.0, 100000.0, 1000000.0};
	while (e > 0)
	{
		n *= scPow[e % sizeof (scPow)];
		e -= sizeof (scPow);
	}
	return n;
}

static stock Parser_DoFloat(ret[E_TOKEN])
{
	ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_FLOAT;
	new
		part = 0,
		ndp = 0,
		dp = 0,
		parts[3],
		//numPart1, // Before ".".
		//numPart2, // After ".".
		//numPart3, // After "e".
		ptr = ISI_gInputPtr - 1,
		ch;
	for ( ; ; )
	{
		switch ((ch = ISI_gInputLine[++ptr]))
		{
			case '0' .. '9': parts[part] = parts[part] * 10 + (ch - '0'), ++dp;
			case '.':
			{
				if (part == 0) part = 1, dp = 0;
				else break;
			}
			case 'e', 'E':
			{
				if (part == 2) break;
				else part = 2, ndp = dp;
			}
			default: break;
		}
	}
	if (ndp) dp = ndp;
	// At this point we have 3 integers - the whole part, the fractional part,
	// and the exponent:
	//
	//  4.6e6 == 4600000 == 4, 6, 6
	//
	ret[E_TOKEN_FLOAT_VAL] = Parser_Pow10(float(parts[0]) + float(parts[1]) / Parser_Pow10(1.0, dp), parts[2]);
	ISI_gInputPtr = ptr;
	return 1;
}

/******************************************************************************\
	Parser_GetNumber

	Parse an unknown number, this could be: hex, octal, binary, decimal, or a
	float (including exponents).  Loop through assuming its decimal, and if we
	find concrete evidence to the contrary then reparse it as that type instead.

\******************************************************************************/
static stock Parser_GetNumber(ret[E_TOKEN])
{
	new
		num = 0,
		ch,
		ptr = ISI_gInputPtr;
	ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_NUM;
	if (ISI_gInputLine[ptr] == '0')
	{
		// Hex, binary, or octal (or 0).
		switch ((ch = ISI_gInputLine[++ptr]))
		{
			case 'x', 'X': return Parser_DoHex(ret); // Hex
			case 'b', 'B': return Parser_DoBinary(ret); // Binary
			case 'e', 'E', '.': return Parser_DoFloat(ret); // Float
			case '0' .. '9':
			{
				new
					bool:oct = ('0' <= ch <= '7');
				num = ch - '0';
				for ( ; ; )
				{
					switch ((ch = ISI_gInputLine[++ptr]))
					{
						case '0' .. '7': num = num * 10 + (ch - '0'), oct &= true;
						case 'e', 'E', '.': return Parser_DoFloat(ret); // Float
						case '8', '9': num = num * 10 + (ch - '0'), oct = false;
						default: break;
					}
				}
				if (oct) return Parser_DoOctal(ret);
			}
		}
	}
	else
	{
		// Decimal or float.
		// Lookahead.
		num = ISI_gInputLine[ptr] - '0';
		for ( ; ; )
		{
			switch ((ch = ISI_gInputLine[++ptr]))
			{
				case '0' .. '9': num = num * 10 + (ch - '0');
				case 'e', 'E', '.': return Parser_DoFloat(ret);
				default: break;
			}
		}
	}
	ret[E_TOKEN_NUM_VAL] = num;
	// Decimal (default).
	ISI_gInputPtr = ptr;
	return 1;
}

static stock Parser_GetString()
{
	new
		ret[E_TOKEN] = {e_TOKEN_TYPE_STRING, 0};
	// Decimal (default).
	ISI_gInputPtr = 0;
	return ret;
}

static stock Parser_IsSymbolCharacter(ch)
{
	return ('0' <= ch <= '9') || ('a' <= ch <= 'z') || ('A' <= ch <= 'Z') || ch == '_' || ch == '@';
}

static stock Parser_GetSymbol(ret[E_TOKEN])
{
	ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_NONE;
	// Find the end of the symbol (already have the first character).
	new
		ptr = ISI_gInputPtr + 1;
	while (Parser_IsSymbolCharacter(ISI_gInputLine[ptr])) ++ptr;
	// Store the next character so we can blank it and get the symbol.
	new
		tmp = ISI_gInputLine[ptr];
	ISI_gInputLine[ptr] = '\0';
	// TODO: Add constants:
	#if defined INTROSPECT_CONSTANTS
		if (Parser_GetConstant(ISI_gInputLine[ISI_gInputPtr], ret))
		{
			// Pass it floats and integers from things like "MAX_PLAYERS".
			goto Parser_GetSymbol_return;
		}
	#endif
	// Get the variable pointer.
	{
		new
			symDat[E_VARIABLE];
		if (GetVariableInfo(ISI_gInputLine[ISI_gInputPtr], symDat))
		{
			ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_VAR,
			ret[E_TOKEN_SYM_PTR] = symDat[Address];
			goto Parser_GetSymbol_return;
		}
	}
	// Get the function pointer.
	if ((ret[E_TOKEN_SYM_PTR] = GetFunctionAddress(ISI_gInputLine[ISI_gInputPtr])) != -1)
	{
		ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_FUNC;
		goto Parser_GetSymbol_return;
	}
	// Get the native index.
	#if defined INTROSPECT_NATIVES
		if ((ret[E_TOKEN_NATIVE_IDX] = GetNativeIndexFromName(ISI_gInputLine[ISI_gInputPtr])) != -1)
		{
			ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_NATIVE;
			goto Parser_GetSymbol_return;
		}
	#endif
	Parser_Error("Unknown symbol: %s", ISI_gInputLine[ISI_gInputPtr]);
Parser_GetSymbol_return:
	return
		ISI_gInputLine[(ISI_gInputPtr = ptr)] = tmp,
		1;
}

/******************************************************************************\
	Parser_GetChar

	Gets a number by decoding a single character enclosed in ''s.  This includes
	various escape sequences, decimals, and hex numbers.

\******************************************************************************/
static stock Parser_GetChar(ret[E_TOKEN])
{
	ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_NUM;
	new
		ch;
	switch ((ch = ISI_gInputLine[++ISI_gInputPtr]))
	{
		case '\\':
		{
			switch (ISI_gInputLine[++ISI_gInputPtr])
			{
				case '\0':
				{
					return Parser_Error("Unclosed character literal.");
				}
				case 'n': ret[E_TOKEN_NUM_VAL] = '\n';
				case '\\': ret[E_TOKEN_NUM_VAL] = '\\';
				case '\'': ret[E_TOKEN_NUM_VAL] = '\'';
				case 't': ret[E_TOKEN_NUM_VAL] = '\t';
				case '"': ret[E_TOKEN_NUM_VAL] = '\"';
				case 'a': ret[E_TOKEN_NUM_VAL] = '\a';
				case 'b': ret[E_TOKEN_NUM_VAL] = '\b';
				case 'v': ret[E_TOKEN_NUM_VAL] = '\v';
				case 'r': ret[E_TOKEN_NUM_VAL] = '\r';
				case '0' .. '9':
				{
					Parser_DoDecimal(ret);
					if (ISI_gInputLine[ISI_gInputPtr] != ';') goto Parser_GetChar_no_increment;
				}
				case 'x', 'X':
				{
					ISI_gInputPtr -= 1,
					Parser_DoHex(ret);
					if (ISI_gInputLine[ISI_gInputPtr] != ';') goto Parser_GetChar_no_increment;
				}
			}
		}
		case '\0':
		{
			return Parser_Error("Unclosed character literal.");
		}
		default:
		{
			ret[E_TOKEN_NUM_VAL] = ch;
		}
	}
	++ISI_gInputPtr;
Parser_GetChar_no_increment:
	if (ISI_gInputLine[ISI_gInputPtr] != '\'') return Parser_Error("Unclosed character literal.");
	return 1;
}

#if defined INTROSPECT_PARSER_TEST

// "P"arser "T"est "N"umber.
#define PTN(%1) ISI_gInputLine=#%1,ISI_gInputPtr=0,Parser_GetNextToken(ret),printf("%s: \"%s\"",(ret[E_TOKEN_TYPE]==e_TOKEN_TYPE_NUM&&ret[E_TOKEN_NUM_VAL]==%1)?("PASS"):("FAIL"),ISI_gInputLine)
// "P"arser "T"est "F"loat.
#define PTF(%1) ISI_gInputLine=#%1,ISI_gInputPtr=0,Parser_GetNextToken(ret),printf("%s: \"%s\"",(ret[E_TOKEN_TYPE]==e_TOKEN_TYPE_FLOAT&&ret[E_TOKEN_FLOAT_VAL]==%1)?("PASS"):("FAIL"),ISI_gInputLine)
// "P"arser "T"est "O"ctal (no native PAWN representation).
#define PTO(%0,%1) ISI_gInputLine=#%0,ISI_gInputPtr=0,Parser_GetNextToken(ret),printf("%s: \"%s\"",(ret[E_TOKEN_TYPE]==e_TOKEN_TYPE_NUM&&ret[E_TOKEN_NUM_VAL]==%1)?("PASS"):("FAIL"),ISI_gInputLine)

#define TEST_OP(%1) ISI_gInputLine=#%1,ISI_gInputPtr=0,Parser_GetNextToken(ret),printf("%s: \"%s\"",(ret[E_TOKEN_TYPE]==e_TOKEN_TYPE_OP&&!strcmp(Parser_PrintOp(ret[E_TOKEN_OP]),#%1))?("PASS"):("FAIL"),ISI_gInputLine)

main()
{
	IntrospectInit();
	print("\n-------------------------");
	print(" Introspect parser tests");
	print("-------------------------\n");
	new
		ret[E_TOKEN];
	// Test numbers.
	PTN(70);
	PTN(0800);
	PTN(0x45);
	PTN(0b11);
	PTN(0b01010101);
	PTN(2345);
	PTF(5.5);
	PTF(9.5e1);
	PTN(77);
	PTF(66.123);
	PTF(0.1);
	PTO(0500, 320);
	PTO(01234, 668);
	// These are a little awkward, because we are simulating run-time input.
	PTN('f');
	PTO('\\n', '\n');
	PTN('7');
	PTN('@');
	PTN('_');
	PTO("'\\x45;'", '\x45;');
	PTO('\\01', '\01');
	PTO("'\\x11;'", '\x11;');
	// Especially these two...
	PTO('\\\\', '\\');
	PTO('\\\'', '\'');
	TEST_OP("=");
	TEST_OP("==");
	TEST_OP("<=");
	TEST_OP(">=");
	TEST_OP("<");
	TEST_OP(">");
	TEST_OP("!");
	TEST_OP("!=");
	TEST_OP("~");
	TEST_OP("~=");
	TEST_OP("+");
	TEST_OP("+=");
	TEST_OP("-");
	TEST_OP("-=");
	TEST_OP("*");
	TEST_OP("*=");
	TEST_OP("/");
	TEST_OP("/=");
	TEST_OP("%");
	TEST_OP("%=");
	TEST_OP("^");
	TEST_OP("^=");
	TEST_OP("&");
	TEST_OP("&=");
	TEST_OP("|");
	TEST_OP("|=");
	TEST_OP(">>");
	TEST_OP(">>=");
	TEST_OP(">>>");
	TEST_OP(">>>=");
	TEST_OP("<<");
	TEST_OP("<<=");
	TEST_OP("&&");
	TEST_OP("||");
	TEST_OP("(");
	TEST_OP(")");
	TEST_OP("{");
	TEST_OP("}");
	TEST_OP("[");
	TEST_OP("]");
	TEST_OP("++");
	TEST_OP("--");
	TEST_OP("...");
	TEST_OP("..");
	TEST_OP("#");
	TEST_OP("::");
	//TEST_OP("\"");
	//TEST_OP("\'");
	/*ISI_gInputLine = "70 5.5 0800 0100 0x345 0b11 0.888 0.888e3";
	new
		ret[E_TOKEN];
	printf("%d", e_TOKEN_TYPE_NUM);
	Parser_GetNextToken(ret);
	printf("T = %d %d", ret[E_TOKEN_TYPE], ret[E_TOKEN_NUM_VAL]);
	Parser_GetNextToken(ret);
	printf("T = %d %.1f", ret[E_TOKEN_TYPE], ret[E_TOKEN_FLOAT_VAL]);
	Parser_GetNextToken(ret);
	printf("T = %d %04d", ret[E_TOKEN_TYPE], ret[E_TOKEN_NUM_VAL]);
	Parser_GetNextToken(ret);
	printf("T = %d %04x", ret[E_TOKEN_TYPE], ret[E_TOKEN_NUM_VAL]);
	Parser_GetNextToken(ret);
	printf("T = %d 0x%x", ret[E_TOKEN_TYPE], ret[E_TOKEN_NUM_VAL]);
	Parser_GetNextToken(ret);
	printf("T = %d 0b%b", ret[E_TOKEN_TYPE], ret[E_TOKEN_NUM_VAL]);
	Parser_GetNextToken(ret);
	printf("T = %d %f", ret[E_TOKEN_TYPE], ret[E_TOKEN_FLOAT_VAL]);
	Parser_GetNextToken(ret);
	printf("T = %d %f", ret[E_TOKEN_TYPE], ret[E_TOKEN_FLOAT_VAL]);*/
}

#endif
