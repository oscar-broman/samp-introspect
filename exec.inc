#include "parser"

#if !defined TODO
	#define TODO { new Todo; }
#endif

static stock
	gFloatTagIndex = -1;

stock ExecLine(line[], size = sizeof (line))
{
	if (gFloatTagIndex == -1)
	{
		gFloatTagIndex = GetTagIdx("Float");
	}
	if (Parser_SetInput(line, size))
	{
		new
			parse = Parser_BuildTree();
		
	}
}

static stock bool:Exec_CheckTags(l[E_TOKEN], r[E_TOKEN])
{
	switch (r[E_TOKEN_TYPE])
	{
		case e_TOKEN_TYPE_NONE,
		     e_TOKEN_TYPE_STRING,
		     e_TOKEN_TYPE_NATIVE,
		     e_TOKEN_TYPE_FUNC,
		     e_TOKEN_TYPE_VAR,
		     e_TOKEN_TYPE_OP,
		     e_TOKEN_TYPE_APP,
		     e_TOKEN_TYPE_ARR: return false;
	}
	switch (l[E_TOKEN_TYPE])
	{
		case e_TOKEN_TYPE_NONE,
		     e_TOKEN_TYPE_STRING,
		     e_TOKEN_TYPE_NATIVE,
		     e_TOKEN_TYPE_FUNC,
		     e_TOKEN_TYPE_VAR,
		     e_TOKEN_TYPE_OP,
		     e_TOKEN_TYPE_APP,
		     e_TOKEN_TYPE_ARR: return false;
	}
	return true;
}

static stock Float:MF(t, token[E_TOKEN])
{
	switch (t)
	{
		case 0b00: return float(token[E_TOKEN_FLOAT_VAL]);// Int   constant.
		case 0b01: return float(token[E_TOKEN_FLOAT_VAL]);// Float constant.
		case 0b10: return float(token[E_TOKEN_FLOAT_VAL]);// Int   variable.
		case 0b11: return Float(token[E_TOKEN_FLOAT_VAL]);// Float variable.
	}
}

static stock e_TOKEN_TYPE:Exec_ResolveTag(token[E_TOKEN], &value, e_TOKEN_TYPE:tag)
{
	switch (token[E_TOKEN_TYPE])
	{
		case e_TOKEN_TYPE_NONE,
		     e_TOKEN_TYPE_STRING,
		     e_TOKEN_TYPE_NATIVE,
		     e_TOKEN_TYPE_FUNC,
		     e_TOKEN_TYPE_OP,
		     e_TOKEN_TYPE_APP,
		     e_TOKEN_TYPE_ARR:
				return e_TOKEN_TYPE_NONE;
		case e_TOKEN_TYPE_FLOAT:
				return value = token[E_TOKEN_FLOAT_VAL], e_TOKEN_TYPE_FLOAT;
		case e_TOKEN_TYPE_FVAR:
				
	}
}

static stock e_TOKEN_TYPE:Exec_ResolveTags(l[E_TOKEN], r[E_TOKEN], &lr, &rr)
{
	new
		e_TOKEN_TYPE:tokenType = r[E_TOKEN_TYPE];,
		tokenVal;
	switch (tokenType)
	{
		case e_TOKEN_TYPE_FLOAT,
			 e_TOKEN_TYPE_FVAR: tokenType = Exec_ResolveTag(l, tokenVal, e_TOKEN_TYPE_FLOAT);
		case e_TOKEN_TYPE_NONE,
		     e_TOKEN_TYPE_STRING,
		     e_TOKEN_TYPE_NATIVE,
		     e_TOKEN_TYPE_FUNC,
		     e_TOKEN_TYPE_OP,
		     e_TOKEN_TYPE_APP,
		     e_TOKEN_TYPE_ARR: return e_TOKEN_TYPE_NONE;
	}
	// We have checked that one side is valid.  Is the other side as well?
	switch (l[E_TOKEN_TYPE])
	{
		case e_TOKEN_TYPE_NUM, e_TOKEN_TYPE_BOOL:
		{
			if (rt == e_TOKEN_TYPE_FLOAT) return
				lr = _:float(l[E_TOKEN_NUM_VAL]),
				rr = _:r[E_TOKEN_FLOAT_VAL],
				e_TOKEN_TYPE_FLOAT;
			else return
				lr = l[E_TOKEN_NUM_VAL],
				rr = r[E_TOKEN_NUM_VAL],
				e_TOKEN_TYPE_NUM;
		}
		case e_TOKEN_TYPE_FLOAT:
		{
			return
				lr = _:l[E_TOKEN_FLOAT_VAL],
				rr = _:((rt == e_TOKEN_TYPE_FLOAT) ? r[E_TOKEN_FLOAT_VAL] : float(r[E_TOKEN_NUM_VAL])),
				e_TOKEN_TYPE_FLOAT;
		}
	}
	return e_TOKEN_TYPE_NONE;
}

/*
Return values:
	0b0001 - Right Float
	0b0010 - Right Var
	0b0100 - Left  Float
	0b1000 - Left  Var

Combine the numbers to get the final result.
*/

if (ret & 0b0101)
{
	
}

#define EXEC_OP(%0\32;%9\32;%1) switch(Exec_ResolveTags(%0,%1,lr,rr)) { \
	case e_TOKEN_TYPE_FLOAT:ret[E_TOKEN_TYPE]=e_TOKEN_TYPE_FLOAT,ret[E_TOKEN_FLOAT_VAL]=(Float:lr)%9(Float:rr); \
	case e_TOKEN_TYPE_NUM:ret[E_TOKEN_TYPE]=e_TOKEN_TYPE_NUM,ret[E_TOKEN_NUM_VAL]=lr%9rr; \
	default:Parser_Error("Incompatible operands.");}

#define EXEC_CMP(%0\32;%9\32;%1) switch(Exec_ResolveTags(%0,%1,lr,rr)) { \
	case e_TOKEN_TYPE_FLOAT:ret[E_TOKEN_TYPE]=e_TOKEN_TYPE_BOOL,ret[E_TOKEN_BOOL_VAL]=(Float:lr)%9(Float:rr); \
	case e_TOKEN_TYPE_NUM:ret[E_TOKEN_TYPE]=e_TOKEN_TYPE_BOOL,ret[E_TOKEN_BOOL_VAL]=lr%9rr; \
	default:Parser_Error("Incompatible operands.");}

#define EXEC_BOOL(%0\32;%9\32;%1) if(Exec_CheckTags(%0,%1)) \
	ret[E_TOKEN_TYPE]=e_TOKEN_TYPE_BOOL,ret[E_TOKEN_BOOL_VAL]=%0[E_TOKEN_BOOL_VAL]%9%1[E_TOKEN_BOOL_VAL]; \
	else Parser_Error("Incompatible operands.");

#define EXEC_INT(%0\32;%9\32;%1) if(Exec_CheckTags(%0,%1)) \
	ret[E_TOKEN_TYPE]=e_TOKEN_TYPE_NUM,ret[E_TOKEN_NUM_VAL]=%0[E_TOKEN_NUM_VAL]%9%1[E_TOKEN_NUM_VAL]; \
	else Parser_Error("Incompatible operands.");

static stock Exec_ExecToken(t[E_TOKEN], l[E_TOKEN], r[E_TOKEN])
{
	static lr, rr;
	new
		ret[E_TOKEN] = EMPTY_PARSER_TOKEN;
	switch (t[E_TOKEN_TYPE])
	{
		case e_TOKEN_TYPE_NONE     : {}
		case e_TOKEN_TYPE_STRING   : TODO
		case e_TOKEN_TYPE_FLOAT    : ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_FLOAT, ret[E_TOKEN_FLOAT_VAL] = t[E_TOKEN_FLOAT_VAL];
		// These two are distinct for simplicity.
		case e_TOKEN_TYPE_NATIVE   : TODO
		case e_TOKEN_TYPE_FUNC     : TODO
		case e_TOKEN_TYPE_VAR      : TODO
		case e_TOKEN_TYPE_NUM      : ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_NUM, ret[E_TOKEN_NUM_VAL] = t[E_TOKEN_NUM_VAL];
		case e_TOKEN_TYPE_BOOL     : ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_BOOL, ret[E_TOKEN_BOOL_VAL] = t[E_TOKEN_BOOL_VAL];
		case e_TOKEN_TYPE_OP       : switch (t[E_TOKEN_OP])
		{
			case e_TOKEN_OP_EQUALS : // '=='
				EXEC_CMP(l == r)
			case e_TOKEN_OP_LTE    : // '<='
				EXEC_CMP(l <= r)
			case e_TOKEN_OP_GTE    : // '>='
				EXEC_CMP(l >= r)
			case e_TOKEN_OP_LESS   : // '<'
				EXEC_CMP(l < r)
			case e_TOKEN_OP_GREATER: // '>'
				EXEC_CMP(l > r)
			case e_TOKEN_OP_NEQ    : // '!='
				EXEC_CMP(l != r)
			// case e_TOKEN_OP_NOT : // '!'
				// EXEC_OP(l ! r)
			// case e_TOKEN_OP_SUB : // '-'
				// EXEC_OP(l - r)
			case e_TOKEN_OP_ADD    : // '+'
				EXEC_OP(l + r)
			case e_TOKEN_OP_MUL    : // '*'
				EXEC_OP(l * r)
			case e_TOKEN_OP_DIV    : // '/'
				EXEC_OP(l / r)
			// case e_TOKEN_OP_MOD : // '%'
				// EXEC_OP(l % r)
			case e_TOKEN_OP_XOR    : // '^'
				EXEC_INT(l ^ r)
			case e_TOKEN_OP_LAND   : // '&'
				EXEC_INT(l & r)
			case e_TOKEN_OP_LOR    : // '|'
				EXEC_INT(l | r)
			case e_TOKEN_OP_RSHIFT : // '>>'
				EXEC_INT(l >> r)
			case e_TOKEN_OP_SHIFT  : // '>>>'
				EXEC_INT(l >>> r)
			case e_TOKEN_OP_LSHIFT : // '<<'
				EXEC_INT(l << r)
			case e_TOKEN_OP_AND    : // '&&'
				EXEC_BOOL(l && r)
			case e_TOKEN_OP_OR     : // '||'
				EXEC_BOOL(l || r)
		}
		case e_TOKEN_TYPE_APP      :
		{
		}
		case e_TOKEN_TYPE_ARR      :
		{
		}
	}
	return ret;
	// new
		// l[E_EXEC_RESULT]:
		// r[E_EXEC_RESULT];
}

#undef EXEC_OP
#undef EXEC_CMP
#undef EXEC_BOOL
#undef EXEC_INT

