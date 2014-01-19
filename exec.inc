#include "parser"

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

static stock Exec_LoadAddress(var)
{
	#emit LREF.S.pri var
	#emit RETN
	return 0;
}

static stock Float:MF(t, token[E_TOKEN])
{
	switch (t)
	{
		// Int constant.
		case 0b00: return float(token[E_TOKEN_NUM_VAL]);
		// Float constant.
		case 0b01: return token[E_TOKEN_FLOAT_VAL];
		// Int variable.
		case 0b10: return float(Exec_LoadAddress(token[E_TOKEN_SYM_PTR]));
		// Float variable.
		case 0b11: return Float:Exec_LoadAddress(token[E_TOKEN_SYM_PTR]);
	}
	return Float:0x7FFFFFFF; // NAN.
}

static stock MI(t, token[E_TOKEN])
{
	switch (t)
	{
		// Int constant.
		case 0b00: return token[E_TOKEN_NUM_VAL]);
		// Float constant.
		case 0b01: return floatround(token[E_TOKEN_FLOAT_VAL]);
		// Int variable.
		case 0b10: return Exec_LoadAddress(token[E_TOKEN_SYM_PTR]);
		// Float variable.
		case 0b11: return floatround(Float:Exec_LoadAddress(token[E_TOKEN_SYM_PTR]));
	}
	return cellmin; // Almost NAN...
}

static stock Exec_ResolveTag(token[E_TOKEN], &value)
{
	switch (token[E_TOKEN_TYPE])
	{
		case e_TOKEN_TYPE_NUM:
			return value =   token[E_TOKEN_NUM_VAL],   0b00;
		case e_TOKEN_TYPE_VAR:
			return value =   token[E_TOKEN_SYM_PTR],   0b10;
		case e_TOKEN_TYPE_FLOAT:
			return value = _:token[E_TOKEN_FLOAT_VAL], 0b01;
		case e_TOKEN_TYPE_FVAR:
			return value =   token[E_TOKEN_SYM_PTR],   0b11;
	}
	return -1;
}

static stock bool:Exec_CheckTags(l[E_TOKEN], r[E_TOKEN])
{
	switch (r[E_TOKEN_TYPE])
	{
		case e_TOKEN_TYPE_NONE,
		     e_TOKEN_TYPE_STRING,
		     e_TOKEN_TYPE_NATIVE,
		     e_TOKEN_TYPE_FUNC,
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
		     e_TOKEN_TYPE_OP,
		     e_TOKEN_TYPE_APP,
		     e_TOKEN_TYPE_ARR: return false;
	}
	return true;
}

static stock e_TOKEN_TYPE:Exec_ResolveTags(l[E_TOKEN], r[E_TOKEN], &lr, &rr)
{
	switch (l[E_TOKEN_TYPE])
	{
		case e_TOKEN_TYPE_VAR  : switch (r[E_TOKEN_TYPE])
		{
			case e_TOKEN_TYPE_VAR  : return
				lr = Exec_LoadAddress(l[E_TOKEN_SYM_PTR]),
				rr = Exec_LoadAddress(r[E_TOKEN_SYM_PTR]),
				e_TOKEN_TYPE_NUM;
			case e_TOKEN_TYPE_NUM  : return
				lr = Exec_LoadAddress(l[E_TOKEN_SYM_PTR]),
				rr = r[E_TOKEN_NUM_VAL],
				e_TOKEN_TYPE_NUM;
			case e_TOKEN_TYPE_BOOL : return
				lr = Exec_LoadAddress(l[E_TOKEN_SYM_PTR]),
				rr = _:r[E_TOKEN_BOOL_VAL],
				e_TOKEN_TYPE_NUM;
			case e_TOKEN_TYPE_FLOAT: return
				lr = _:float(Exec_LoadAddress(l[E_TOKEN_SYM_PTR])),
				rr = _:r[E_TOKEN_FLOAT_VAL],
				e_TOKEN_TYPE_FLOAT;
			case e_TOKEN_TYPE_FVAR : return
				lr = _:float(Exec_LoadAddress(l[E_TOKEN_SYM_PTR])),
				rr = _:Float:Exec_LoadAddress(r[E_TOKEN_SYM_PTR]),
				e_TOKEN_TYPE_FLOAT;
		}
		case e_TOKEN_TYPE_NUM  : switch (r[E_TOKEN_TYPE])
		{
			case e_TOKEN_TYPE_VAR  : return
				lr = l[E_TOKEN_NUM_VAL],
				rr = Exec_LoadAddress(r[E_TOKEN_SYM_PTR]),
				e_TOKEN_TYPE_NUM;
			case e_TOKEN_TYPE_NUM  : return
				lr = l[E_TOKEN_NUM_VAL],
				rr = r[E_TOKEN_NUM_VAL],
				e_TOKEN_TYPE_NUM;
			case e_TOKEN_TYPE_BOOL : return
				lr = l[E_TOKEN_NUM_VAL],
				rr = _:r[E_TOKEN_BOOL_VAL],
				e_TOKEN_TYPE_NUM;
			case e_TOKEN_TYPE_FLOAT: return
				lr = _:float(l[E_TOKEN_NUM_VAL]),
				rr = _:r[E_TOKEN_FLOAT_VAL],
				e_TOKEN_TYPE_FLOAT;
			case e_TOKEN_TYPE_FVAR : return
				lr = _:float(l[E_TOKEN_NUM_VAL]),
				rr = _:Float:Exec_LoadAddress(r[E_TOKEN_SYM_PTR]),
				e_TOKEN_TYPE_FLOAT;
		}
		case e_TOKEN_TYPE_BOOL : switch (r[E_TOKEN_TYPE])
		{
			case e_TOKEN_TYPE_VAR  : return
				lr = _:l[E_TOKEN_BOOL_VAL],
				rr = Exec_LoadAddress(r[E_TOKEN_SYM_PTR]),
				e_TOKEN_TYPE_NUM;
			case e_TOKEN_TYPE_NUM  : return
				lr = _:l[E_TOKEN_BOOL_VAL],
				rr = r[E_TOKEN_NUM_VAL],
				e_TOKEN_TYPE_NUM;
			case e_TOKEN_TYPE_BOOL : return
				lr = _:l[E_TOKEN_BOOL_VAL],
				rr = _:r[E_TOKEN_BOOL_VAL],
				e_TOKEN_TYPE_BOOL;
			case e_TOKEN_TYPE_FLOAT: return
				lr = _:float(_:l[E_TOKEN_BOOL_VAL]),
				rr = _:r[E_TOKEN_FLOAT_VAL],
				e_TOKEN_TYPE_FLOAT;
			case e_TOKEN_TYPE_FVAR : return
				lr = _:float(_:l[E_TOKEN_BOOL_VAL]),
				rr = _:Float:Exec_LoadAddress(r[E_TOKEN_SYM_PTR]),
				e_TOKEN_TYPE_FLOAT;
		}
		case e_TOKEN_TYPE_FLOAT: switch (r[E_TOKEN_TYPE])
		{
			case e_TOKEN_TYPE_VAR  : return
				lr = _:l[E_TOKEN_FLOAT_VAL],
				rr = _:float(Exec_LoadAddress(r[E_TOKEN_SYM_PTR])),
				e_TOKEN_TYPE_FLOAT;
			case e_TOKEN_TYPE_NUM  : return
				lr = _:l[E_TOKEN_FLOAT_VAL],
				rr = _:float(r[E_TOKEN_NUM_VAL]),
				e_TOKEN_TYPE_FLOAT;
			case e_TOKEN_TYPE_BOOL : return
				lr = _:l[E_TOKEN_FLOAT_VAL],
				rr = _:float(_:r[E_TOKEN_BOOL_VAL]),
				e_TOKEN_TYPE_FLOAT;
			case e_TOKEN_TYPE_FLOAT: return
				lr = _:l[E_TOKEN_FLOAT_VAL],
				rr = _:r[E_TOKEN_FLOAT_VAL],
				e_TOKEN_TYPE_FLOAT;
			case e_TOKEN_TYPE_FVAR : return
				lr = _:l[E_TOKEN_FLOAT_VAL],
				rr = _:Float:Exec_LoadAddress(r[E_TOKEN_SYM_PTR]),
				e_TOKEN_TYPE_FLOAT;
		}
		case e_TOKEN_TYPE_FVAR : switch (r[E_TOKEN_TYPE])
		{
			case e_TOKEN_TYPE_VAR  : return
				lr = _:Float:Exec_LoadAddress(l[E_TOKEN_SYM_PTR]),
				rr = _:float(Exec_LoadAddress(r[E_TOKEN_SYM_PTR])),
				e_TOKEN_TYPE_FLOAT;
			case e_TOKEN_TYPE_NUM  : return
				lr = _:Float:Exec_LoadAddress(l[E_TOKEN_SYM_PTR]),
				rr = _:float(r[E_TOKEN_NUM_VAL]),
				e_TOKEN_TYPE_FLOAT;
			case e_TOKEN_TYPE_BOOL : return
				lr = _:Float:Exec_LoadAddress(l[E_TOKEN_SYM_PTR]),
				rr = _:float(_:r[E_TOKEN_BOOL_VAL]),
				e_TOKEN_TYPE_FLOAT;
			case e_TOKEN_TYPE_FLOAT: return
				lr = _:Float:Exec_LoadAddress(l[E_TOKEN_SYM_PTR]),
				rr = _:r[E_TOKEN_FLOAT_VAL],
				e_TOKEN_TYPE_FLOAT;
			case e_TOKEN_TYPE_FVAR : return
				lr = _:Float:Exec_LoadAddress(l[E_TOKEN_SYM_PTR]),
				rr = _:Float:Exec_LoadAddress(r[E_TOKEN_SYM_PTR]),
				e_TOKEN_TYPE_FLOAT;
		}
	}
	return e_TOKEN_TYPE_NONE;
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

/*static*/stock Exec_ExecToken(t[E_TOKEN], l[E_TOKEN], r[E_TOKEN])
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

#if defined INTROSPECT_EXEC_TEST

main()
{
	new
		l [E_TOKEN],
		r [E_TOKEN],
		op[E_TOKEN];
	l  = MAKE_TOKEN(e_TOKEN_TYPE_NUM, 42),
	r  = MAKE_TOKEN(e_TOKEN_TYPE_NUM, 11),
	op = MAKE_TOKEN(e_TOKEN_TYPE_OP, e_TOKEN_OP_ADD);
	
	
	
	
	
	l  = MAKE_TOKEN(e_TOKEN_TYPE_NUM, 42),
	r  = MAKE_TOKEN(e_TOKEN_TYPE_NUM, 11),
	op = MAKE_TOKEN(e_TOKEN_TYPE_OP, e_TOKEN_OP_ADD);
}

#endif

