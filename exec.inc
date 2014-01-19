#include "parser"

stock ExecLine(line[], size = sizeof (line))
{
	if (Parser_SetInput(line, size))
	{
		new
			parse = Parser_BuildTree();
		
	}
}

enum e_EXEC_RESULT_TAG
{
	e_EXEC_RESULT_TAG_FAIL = -1,
	e_EXEC_RESULT_TAG__, // _ (default). // 0
	e_EXEC_RESULT_TAG_Float,             // 1
	e_EXEC_RESULT_TAG_bool,              // 2
	e_EXEC_RESULT_TAG_string             // 3
}

enum E_EXEC_RESULT
{
	e_EXEC_RESULT_TAG:E_EXEC_RESULT_TAG,
	E_EXEC_RESULT_VAL
}

static stock Float:MF(token[E_EXEC_RESULT])
{
	// "Make Float".
	return (token[E_EXEC_RESULT_TAG] == e_EXEC_RESULT_TAG_Float) ? token[E_TOKEN_FLOAT_VAL] : float(token[E_TOKEN_INT_VAL]);
}

static stock _:MI(token[E_EXEC_RESULT])
{
	// "Make Int".
	return (token[E_EXEC_RESULT_TAG] == e_EXEC_RESULT_TAG_Float) ? floatround(token[E_TOKEN_FLOAT_VAL]) : token[E_TOKEN_INT_VAL];
}

static stock bool:CheckTags(l[E_TOKEN], r[E_TOKEN])
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

static stock e_TOKEN_TYPE:ResolveTags(l[E_TOKEN], r[E_TOKEN], &lr, &rr)
{
	new
		e_TOKEN_TYPE:rt = r[E_TOKEN_TYPE];
	switch (rt)
	{
		case e_TOKEN_TYPE_NONE,
		     e_TOKEN_TYPE_STRING,
		     e_TOKEN_TYPE_NATIVE,
		     e_TOKEN_TYPE_FUNC,
		     e_TOKEN_TYPE_VAR,
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

// #define EXEC_OP(%0:%9:%1) switch(_:%0[E_EXEC_RESULT_TAG]|(_:%1[E_EXEC_RESULT_TAG]<<8)){\
	// case 0x000:0x200:0x002:ret[E_EXEC_RESULT_TAG]=e_EXEC_RESULT_TAG_Float,ret[E_EXEC_RESULT_VAL]=(MI(%0)%9MI(%1));\
	// case 0x202:ret[E_EXEC_RESULT_TAG]=e_EXEC_RESULT_TAG_bool,ret[E_EXEC_RESULT_VAL]=_:(bool:MI(%0)%9bool:MI(%1));\
	// case 0x001:0x201:0x101:0x100:0x102:ret[E_EXEC_RESULT_TAG]=e_EXEC_RESULT_TAG_Float,ret[E_EXEC_RESULT_VAL]=_:(MF(%0)%9MF(%1));\
	// default:return ret;}

// #define EXEC_BOOL(%0:%9:%1) switch(_:%0[E_EXEC_RESULT_TAG]|(_:%1[E_EXEC_RESULT_TAG]<<8)){\
	// case 0x000:0x200:0x002:ret[E_EXEC_RESULT_TAG]=e_EXEC_RESULT_TAG_Float,ret[E_EXEC_RESULT_VAL]=(MI(%0)%9MI(%1));\
	// case 0x202:ret[E_EXEC_RESULT_TAG]=e_EXEC_RESULT_TAG_bool,ret[E_EXEC_RESULT_VAL]=_:(bool:MI(%0)%9bool:MI(%1));\
	// case 0x001:0x201:0x101:0x100:0x102:ret[E_EXEC_RESULT_TAG]=e_EXEC_RESULT_TAG_Float,ret[E_EXEC_RESULT_VAL]=_:(MF(%0)%9MF(%1));\
	// default:return ret;}

#define EXEC_OP(%0\32;%9\32;%1) switch(ResolveTags(%0,%1,lr,rr)) { \
	case e_TOKEN_TYPE_FLOAT:ret[E_TOKEN_TYPE]=e_TOKEN_TYPE_FLOAT,ret[E_TOKEN_FLOAT_VAL]=(Float:lr)%9(Float:rr); \
	case e_TOKEN_TYPE_NUM:ret[E_TOKEN_TYPE]=e_TOKEN_TYPE_NUM,ret[E_TOKEN_NUM_VAL]=lr%9rr; \
	default:Parser_Error("Incompatible operands.");}

#define EXEC_CMP(%0\32;%9\32;%1) switch(ResolveTags(%0,%1,lr,rr)) { \
	case e_TOKEN_TYPE_FLOAT:ret[E_TOKEN_TYPE]=e_TOKEN_TYPE_BOOL,ret[E_TOKEN_BOOL_VAL]=(Float:lr)%9(Float:rr); \
	case e_TOKEN_TYPE_NUM:ret[E_TOKEN_TYPE]=e_TOKEN_TYPE_BOOL,ret[E_TOKEN_BOOL_VAL]=lr%9rr; \
	default:Parser_Error("Incompatible operands.");}

#define EXEC_BOOL(%0\32;%9\32;%1) if(CheckTags(%0,%1)) \
	ret[E_TOKEN_TYPE]=e_TOKEN_TYPE_BOOL,ret[E_TOKEN_BOOL_VAL]=%0[E_TOKEN_BOOL_VAL]%9%1[E_TOKEN_BOOL_VAL]; \
	else Parser_Error("Incompatible operands.");

#define EXEC_INT(%0\32;%9\32;%1) if(CheckTags(%0,%1)) \
	ret[E_TOKEN_TYPE]=e_TOKEN_TYPE_NUM,ret[E_TOKEN_NUM_VAL]=%0[E_TOKEN_NUM_VAL]%9%1[E_TOKEN_NUM_VAL]; \
	else Parser_Error("Incompatible operands.");

stock ExecToken(t[E_TOKEN], l[E_TOKEN], r[E_TOKEN])
{
	static lr, rr;
	new
		ret[E_EXEC_RESULT] = {e_EXEC_RESULT_TAG_FAIL, 0};
	switch (t[E_TOKEN_TYPE])
	{
		case e_TOKEN_TYPE_NONE     : {}
		case e_TOKEN_TYPE_STRING   : 
		case e_TOKEN_TYPE_FLOAT    : ret[E_EXEC_RESULT_TAG] = e_EXEC_RESULT_TAG_Float: ret[E_EXEC_RESULT_VAL] = _:t[E_TOKEN_FLOAT_VAL];
		// These two are distinct for simplicity.
		case e_TOKEN_TYPE_NATIVE   : 
		case e_TOKEN_TYPE_FUNC     : 
		case e_TOKEN_TYPE_VAR      : 
		case e_TOKEN_TYPE_NUM      : ret[E_EXEC_RESULT_VAL] = t[E_TOKEN_NUM_VAL];
		case e_TOKEN_TYPE_BOOL     : ret[E_EXEC_RESULT_TAG] = e_EXEC_RESULT_TAG_bool: ret[E_EXEC_RESULT_VAL] = _:t[E_TOKEN_BOOL_VAL];
		case e_TOKEN_TYPE_OP       :
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

