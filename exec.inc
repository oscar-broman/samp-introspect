#include "parser"

static stock
	gArrayLookup[32][3]; // Pointer, max indices, current index.

static stock const
	_EXEC_TAG_MISMATCH[] = "PARSER WARNING: Tag mismatch.",
	_EXEC_INCOMPATIBLE[] = "PARSER ERROR: Incompatible operands.",
	_EXEC_NOT_LVALUE[] = "PARSER ERROR: Not an lvalue.";

#define EXEC_TAG_MISMATCH print(_EXEC_TAG_MISMATCH)
#define EXEC_INCOMPATIBLE print(_EXEC_INCOMPATIBLE)
#define EXEC_NOT_LVALUE print(_EXEC_NOT_LVALUE)

stock ExecLine(line[], size = sizeof (line))
{
	if (Parser_SetInput(line, size))
	{
		new
			parse = Parser_BuildTree();
		
	}
}

static stock MI(const token[E_TOKEN])
{
	// Get a token as an integer.
	switch (token[E_TOKEN_TYPE])
	{
		case e_TOKEN_TYPE_VAR:
			return Exec_LoadAddress(token[E_TOKEN_SYM_PTR]);
		case e_TOKEN_TYPE_NUM:
			return token[E_TOKEN_NUM_VAL];
		case e_TOKEN_TYPE_BOOL:
			return token[E_TOKEN_BOOL_VAL] ? 1 : 0;
		case e_TOKEN_TYPE_FLOAT:
			return
				EXEC_TAG_MISMATCH,
				floatround(token[E_TOKEN_FLOAT_VAL]);
		case e_TOKEN_TYPE_FVAR:
			return
				EXEC_TAG_MISMATCH,
				floatround(Float:Exec_LoadAddress(token[E_TOKEN_SYM_PTR]));
	}
	return cellmin; // Almost NAN.
}

static stock Float:MF(const token[E_TOKEN])
{
	// Get a token as a float.
	switch (token[E_TOKEN_TYPE])
	{
		case e_TOKEN_TYPE_VAR:
			return
				EXEC_TAG_MISMATCH,
				float(Exec_LoadAddress(token[E_TOKEN_SYM_PTR]));
		case e_TOKEN_TYPE_NUM:
			return
				EXEC_TAG_MISMATCH,
				float(token[E_TOKEN_NUM_VAL]);
		case e_TOKEN_TYPE_BOOL:
			return token[E_TOKEN_BOOL_VAL] ? 1.0 : 0.0;
		case e_TOKEN_TYPE_FLOAT:
			return token[E_TOKEN_FLOAT_VAL];
		case e_TOKEN_TYPE_FVAR:
			return Float:Exec_LoadAddress(token[E_TOKEN_SYM_PTR]);
	}
	return Float:0x7FFFFFFF; // NAN.
}

static stock Exec_LoadAddress(var)
{
	#emit LREF.S.pri var
	#emit RETN
	return 0;
}

static stock Exec_WriteAddress(var, dat)
{
	#emit LOAD.S.pri dat
	#emit SREF.S.pri var
	#emit RETN
	return 0;
}

static stock e_TOKEN_TYPE:Exec_ResolveTags(const l[E_TOKEN], const r[E_TOKEN], &lr, &rr)
{
	new
		bool:rfloat = false;
	switch (l[E_TOKEN_TYPE])
	{
		case e_TOKEN_TYPE_VAR, e_TOKEN_TYPE_BOOL, e_TOKEN_TYPE_NUM: {}
		case e_TOKEN_TYPE_FVAR, e_TOKEN_TYPE_FLOAT: rfloat = true;
		default: return e_TOKEN_TYPE_NONE;
	}
	switch (r[E_TOKEN_TYPE])
	{
		case e_TOKEN_TYPE_VAR, e_TOKEN_TYPE_BOOL, e_TOKEN_TYPE_NUM: {}
		case e_TOKEN_TYPE_FVAR, e_TOKEN_TYPE_FLOAT: rfloat = true;
		default: return e_TOKEN_TYPE_NONE;
	}
	if (rfloat)
	{
		// Some floats.
		return
			lr = _:MF(l),
			rr = _:MF(r),
			e_TOKEN_TYPE_FLOAT;
	}
	else
	{
		return
			lr = MI(l),
			rr = MI(r),
			e_TOKEN_TYPE_NUM;
	}
}

static stock e_TOKEN_TYPE:Exec_ResolveBool(const l[E_TOKEN], const r[E_TOKEN], &lr, &rr)
{
	switch (l[E_TOKEN_TYPE])
	{
		case e_TOKEN_TYPE_VAR  :
			lr = Exec_LoadAddress(l[E_TOKEN_SYM_PTR]) ? true : false;
		case e_TOKEN_TYPE_NUM  :
			lr = l[E_TOKEN_NUM_VAL] ? true : false;
		case e_TOKEN_TYPE_BOOL :
			lr = l[E_TOKEN_BOOL_VAL];
		case e_TOKEN_TYPE_FLOAT:
			lr = l[E_TOKEN_FLOAT_VAL] ? true : false;
		case e_TOKEN_TYPE_FVAR :
			lr = Float:Exec_LoadAddress(l[E_TOKEN_SYM_PTR]) ? true : false;
		default:
			return e_TOKEN_TYPE_NONE;
	}
	switch (r[E_TOKEN_TYPE])
	{
		case e_TOKEN_TYPE_VAR  :
			rr = Exec_LoadAddress(r[E_TOKEN_SYM_PTR]) ? true : false;
		case e_TOKEN_TYPE_NUM  :
			rr = r[E_TOKEN_NUM_VAL] ? true : false;
		case e_TOKEN_TYPE_BOOL :
			rr = r[E_TOKEN_BOOL_VAL];
		case e_TOKEN_TYPE_FLOAT:
			rr = r[E_TOKEN_FLOAT_VAL] ? true : false;
		case e_TOKEN_TYPE_FVAR :
			rr = Float:Exec_LoadAddress(r[E_TOKEN_SYM_PTR]) ? true : false;
		default:
			return e_TOKEN_TYPE_NONE;
	}
	return e_TOKEN_TYPE_BOOL;
}

static stock e_TOKEN_TYPE:Exec_ResolveInt(const l[E_TOKEN], const r[E_TOKEN], &lr, &rr)
{
	new
		e_TOKEN_TYPE:ret = e_TOKEN_TYPE_NUM;
	switch (l[E_TOKEN_TYPE])
	{
		case e_TOKEN_TYPE_VAR  :
			lr = Exec_LoadAddress(l[E_TOKEN_SYM_PTR]);
		case e_TOKEN_TYPE_NUM  :
			lr = l[E_TOKEN_NUM_VAL];
		case e_TOKEN_TYPE_BOOL :
			lr = _:l[E_TOKEN_BOOL_VAL];
		case e_TOKEN_TYPE_FLOAT:
			ret = e_TOKEN_TYPE_FLOAT,
			lr = _:l[E_TOKEN_FLOAT_VAL];
		case e_TOKEN_TYPE_FVAR :
			ret = e_TOKEN_TYPE_FLOAT,
			lr = _:Float:Exec_LoadAddress(l[E_TOKEN_SYM_PTR]);
		default:
			return e_TOKEN_TYPE_NONE;
	}
	switch (r[E_TOKEN_TYPE])
	{
		case e_TOKEN_TYPE_VAR  :
		{
			if (ret == e_TOKEN_TYPE_NUM) EXEC_TAG_MISMATCH;
			rr = Exec_LoadAddress(r[E_TOKEN_SYM_PTR]);
		}
		case e_TOKEN_TYPE_NUM  :
		{
			if (ret == e_TOKEN_TYPE_NUM) EXEC_TAG_MISMATCH;
			rr = r[E_TOKEN_NUM_VAL];
		}
		case e_TOKEN_TYPE_BOOL :
		{
			if (ret == e_TOKEN_TYPE_NUM) EXEC_TAG_MISMATCH;
			rr = _:r[E_TOKEN_BOOL_VAL];
		}
		case e_TOKEN_TYPE_FLOAT:
		{
			if (ret == e_TOKEN_TYPE_NUM) EXEC_TAG_MISMATCH;
			rr = _:r[E_TOKEN_FLOAT_VAL];
		}
		case e_TOKEN_TYPE_FVAR :
		{
			if (ret == e_TOKEN_TYPE_NUM) EXEC_TAG_MISMATCH;
			rr = _:Float:Exec_LoadAddress(r[E_TOKEN_SYM_PTR]);
		}
		default:
			return e_TOKEN_TYPE_NONE;
	}
	return ret;
}

#define EXEC_OP(%0\32;%9\32;%1,%7) switch(Exec_ResolveTags(%0,%1,lr,rr)) { \
	case e_TOKEN_TYPE_FLOAT:ret[E_TOKEN_TYPE]=e_TOKEN_TYPE_FLOAT,ret[E_TOKEN_FLOAT_VAL]=%7((Float:lr),(Float:rr)); \
	case e_TOKEN_TYPE_NUM:ret[E_TOKEN_TYPE]=e_TOKEN_TYPE_NUM,ret[E_TOKEN_NUM_VAL]=lr%9rr; \
	default:EXEC_INCOMPATIBLE;}

#define EXEC_CMP(%0\32;%9\32;%1,%7) switch(Exec_ResolveTags(%0,%1,lr,rr)) { \
	case e_TOKEN_TYPE_FLOAT:ret[E_TOKEN_TYPE]=e_TOKEN_TYPE_BOOL,ret[E_TOKEN_BOOL_VAL]=%7((Float:lr),(Float:rr)); \
	case e_TOKEN_TYPE_NUM:ret[E_TOKEN_TYPE]=e_TOKEN_TYPE_BOOL,ret[E_TOKEN_BOOL_VAL]=lr%9rr; \
	default:EXEC_INCOMPATIBLE;}

#define EXEC_BOOL(%0\32;%9\32;%1) if(Exec_ResolveBool(%0,%1,lr,rr)==e_TOKEN_TYPE_BOOL) \
	ret[E_TOKEN_TYPE]=e_TOKEN_TYPE_BOOL,ret[E_TOKEN_BOOL_VAL]=(bool:lr)%9(bool:rr); \
	else EXEC_INCOMPATIBLE;

#define EXEC_INT(%0\32;%9\32;%1) { new e_TOKEN_TYPE:ttt = Exec_ResolveInt(%0,%1,lr,rr); \
	if(ttt==e_TOKEN_TYPE_NONE) EXEC_INCOMPATIBLE; \
	else ret[E_TOKEN_TYPE]=ttt,ret[E_TOKEN_NUM_VAL]=lr%9rr; }

/*static*/ stock Exec_ExecToken(const t[E_TOKEN], const l[E_TOKEN], const r[E_TOKEN])
{
	static lr, rr;
	static const
		scAssign[E_TOKEN] = MAKE_PARSER_TOKEN(e_TOKEN_TYPE_OP, e_TOKEN_OP_ASSIGN, -1, -1);
	new
		ret[E_TOKEN] = EMPTY_PARSER_TOKEN;
	switch (t[E_TOKEN_TYPE])
	{
		case e_TOKEN_TYPE_OP       : switch (t[E_TOKEN_OP])
		{
			case e_TOKEN_OP_ASSIGN: switch (l[E_TOKEN_TYPE])
			{
				case e_TOKEN_TYPE_VAR:
					rr = MI(r),
					Exec_WriteAddress(l[E_TOKEN_SYM_PTR], rr),
					ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_NUM,
					ret[E_TOKEN_NUM_VAL] = rr;
				case e_TOKEN_TYPE_FVAR:
					rr = _:MF(r),
					Exec_WriteAddress(l[E_TOKEN_SYM_PTR], rr),
					ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_FLOAT,
					ret[E_TOKEN_FLOAT_VAL] = Float:rr;
				default: EXEC_NOT_LVALUE;
			}
			// Float versions are reversed.
			case e_TOKEN_OP_EQUALS : // '=='
				EXEC_CMP(l == r, 0 == floatcmp)
			case e_TOKEN_OP_LTE    : // '<='
				EXEC_CMP(l <= r, 0 >= floatcmp)
			case e_TOKEN_OP_GTE    : // '>='
				EXEC_CMP(l >= r, 0 <= floatcmp)
			case e_TOKEN_OP_LESS   : // '<'
				EXEC_CMP(l < r,  0 >  floatcmp)
			case e_TOKEN_OP_GREATER: // '>'
				EXEC_CMP(l > r,  0 <  floatcmp)
			case e_TOKEN_OP_NEQ    : // '!='
				EXEC_CMP(l != r, 0 != floatcmp)
			#define _EXEC_DO_ASSIGN(%0) case e_TOKEN_OP_%0_ASS: { \
				static const scMathOp[E_TOKEN] = MAKE_PARSER_TOKEN(e_TOKEN_TYPE_OP, e_TOKEN_OP_%0, -1, -1); \
				return ret = Exec_ExecToken(scMathOp, l, r), Exec_ExecToken(scAssign, l, ret); }
			// For these, run the code twice.
			_EXEC_DO_ASSIGN(INV)
			_EXEC_DO_ASSIGN(ADD)
			_EXEC_DO_ASSIGN(SUB)
			_EXEC_DO_ASSIGN(MUL)
			_EXEC_DO_ASSIGN(DIV)
			_EXEC_DO_ASSIGN(MOD)
			_EXEC_DO_ASSIGN(XOR)
			_EXEC_DO_ASSIGN(LAND)
			_EXEC_DO_ASSIGN(LOR)
			_EXEC_DO_ASSIGN(RSHIFT)
			_EXEC_DO_ASSIGN(SHIFT)
			_EXEC_DO_ASSIGN(LSHIFT)
			#undef _EXEC_DO_ASSIGN
			case e_TOKEN_OP_INV : switch (r[E_TOKEN_TYPE])
			{
				case e_TOKEN_TYPE_VAR  :
					ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_NUM,
					ret[E_TOKEN_NUM_VAL] = ~Exec_LoadAddress(r[E_TOKEN_SYM_PTR]);
				case e_TOKEN_TYPE_NUM  :
					ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_NUM,
					ret[E_TOKEN_NUM_VAL] = ~r[E_TOKEN_NUM_VAL];
				case e_TOKEN_TYPE_BOOL :
					ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_NUM,
					ret[E_TOKEN_NUM_VAL] = ~_:r[E_TOKEN_BOOL_VAL];
				case e_TOKEN_TYPE_FLOAT:
					ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_FLOAT,
					ret[E_TOKEN_FLOAT_VAL] = ~r[E_TOKEN_FLOAT_VAL];
				case e_TOKEN_TYPE_FVAR :
					ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_FLOAT,
					ret[E_TOKEN_FLOAT_VAL] = ~Float:Exec_LoadAddress(r[E_TOKEN_SYM_PTR]);
				default: Parser_Error("Unary inversion not applied to value.");
			}
			case e_TOKEN_OP_NOT : switch (r[E_TOKEN_TYPE])
			{
				case e_TOKEN_TYPE_VAR  :
					ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_BOOL,
					ret[E_TOKEN_BOOL_VAL] = !Exec_LoadAddress(r[E_TOKEN_SYM_PTR]);
				case e_TOKEN_TYPE_NUM  :
					ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_BOOL,
					ret[E_TOKEN_BOOL_VAL] = !r[E_TOKEN_NUM_VAL];
				case e_TOKEN_TYPE_BOOL :
					ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_BOOL,
					ret[E_TOKEN_BOOL_VAL] = !r[E_TOKEN_BOOL_VAL];
				case e_TOKEN_TYPE_FLOAT:
					ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_BOOL,
					ret[E_TOKEN_BOOL_VAL] = !r[E_TOKEN_FLOAT_VAL];
				case e_TOKEN_TYPE_FVAR :
					ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_BOOL,
					ret[E_TOKEN_BOOL_VAL] = !Float:Exec_LoadAddress(r[E_TOKEN_SYM_PTR]);
				default: Parser_Error("Unary negation not applied to value.");
			}
			case e_TOKEN_OP_NEG : switch (r[E_TOKEN_TYPE]) // PREFIX '-'
			{
				case e_TOKEN_TYPE_VAR  :
					ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_NUM,
					ret[E_TOKEN_NUM_VAL] = -Exec_LoadAddress(r[E_TOKEN_SYM_PTR]);
				case e_TOKEN_TYPE_NUM  :
					ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_NUM,
					ret[E_TOKEN_NUM_VAL] = -r[E_TOKEN_NUM_VAL];
				case e_TOKEN_TYPE_BOOL :
					ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_NUM,
					ret[E_TOKEN_NUM_VAL] = -_:r[E_TOKEN_BOOL_VAL];
				case e_TOKEN_TYPE_FLOAT:
					ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_FLOAT,
					ret[E_TOKEN_FLOAT_VAL] = r[E_TOKEN_FLOAT_VAL] ^ Float:cellmin;
				case e_TOKEN_TYPE_FVAR :
					ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_FLOAT,
					ret[E_TOKEN_FLOAT_VAL] = Float:Exec_LoadAddress(r[E_TOKEN_SYM_PTR]) ^ Float:cellmin;
				default:
					Parser_Error("Unary minus not applied to value.");
			}
			case e_TOKEN_OP_SUB    : // INFIX '-'
				EXEC_OP(l - r, floatsub)
			case e_TOKEN_OP_ADD    : // '+'
				EXEC_OP(l + r, floatadd)
			case e_TOKEN_OP_MUL    : // '*'
				EXEC_OP(l * r, floatmul)
			case e_TOKEN_OP_DIV    : // '/'
				EXEC_OP(l / r, floatdiv)
			case e_TOKEN_OP_MOD : // '%'
			{
				if (Exec_ResolveTags(l, r, lr, rr) == e_TOKEN_TYPE_NUM)
					ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_NUM,
					ret[E_TOKEN_NUM_VAL] = lr % rr;
				else EXEC_INCOMPATIBLE;
			}
			case e_TOKEN_OP_XOR    : // '^'
				EXEC_INT(l ^ r)
			case e_TOKEN_OP_LAND   : // INFIX '&'
				EXEC_INT(l & r)
			case e_TOKEN_OP_REF : switch (r[E_TOKEN_TYPE]) // PREFIX '&'
			{
				case e_TOKEN_TYPE_NUM  : TODO: Store data and take reference.
				case e_TOKEN_TYPE_BOOL : TODO: Store data and take reference.
				case e_TOKEN_TYPE_FLOAT: TODO: Store data and take reference.
				case e_TOKEN_TYPE_FVAR, e_TOKEN_TYPE_VAR:
					// Store the pointer as a number.
					ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_NUM,
					ret[E_TOKEN_NUM_VAL] = r[E_TOKEN_SYM_PTR];
				default:
					Parser_Error("References must be made to values.");
			}
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
			case e_TOKEN_OP_POST_INC: switch (l[E_TOKEN_TYPE])
			{
				case e_TOKEN_TYPE_VAR: // Suffix integer op.
					lr = Exec_LoadAddress(l[E_TOKEN_SYM_PTR]),
					Exec_WriteAddress(l[E_TOKEN_SYM_PTR], lr + 1),
					ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_NUM,
					ret[E_TOKEN_NUM_VAL] = lr;
				case e_TOKEN_TYPE_FVAR: // Suffix float op.
					lr = _:Float:Exec_LoadAddress(l[E_TOKEN_SYM_PTR]),
					Exec_WriteAddress(l[E_TOKEN_SYM_PTR], _:floatadd(Float:lr, 1.0)),
					ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_FLOAT,
					ret[E_TOKEN_FLOAT_VAL] = Float:lr;
				default: EXEC_NOT_LVALUE;
			}
			case e_TOKEN_OP_PRE_INC: switch (r[E_TOKEN_TYPE])
			{
				case e_TOKEN_TYPE_VAR: // Prefix integer op.
					rr = Exec_LoadAddress(r[E_TOKEN_SYM_PTR]) + 1,
					Exec_WriteAddress(r[E_TOKEN_SYM_PTR], rr),
					ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_NUM,
					ret[E_TOKEN_NUM_VAL] = rr;
				case e_TOKEN_TYPE_FVAR: // Prefix float op.
					rr = _:floatadd(Float:Exec_LoadAddress(r[E_TOKEN_SYM_PTR]), 1.0),
					Exec_WriteAddress(r[E_TOKEN_SYM_PTR], rr),
					ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_FLOAT,
					ret[E_TOKEN_FLOAT_VAL] = Float:rr;
				default: EXEC_NOT_LVALUE;
			}
			case e_TOKEN_OP_POST_DEC: switch (l[E_TOKEN_TYPE])
			{
				case e_TOKEN_TYPE_VAR: // Suffix integer op.
					lr = Exec_LoadAddress(l[E_TOKEN_SYM_PTR]),
					Exec_WriteAddress(l[E_TOKEN_SYM_PTR], lr - 1),
					ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_NUM,
					ret[E_TOKEN_NUM_VAL] = lr;
				case e_TOKEN_TYPE_FVAR: // Suffix float op.
					lr = _:Float:Exec_LoadAddress(l[E_TOKEN_SYM_PTR]),
					Exec_WriteAddress(l[E_TOKEN_SYM_PTR], _:floatsub(Float:lr, 1.0)),
					ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_FLOAT,
					ret[E_TOKEN_FLOAT_VAL] = Float:lr;
				default: EXEC_NOT_LVALUE;
			}
			case e_TOKEN_OP_PRE_DEC: switch (r[E_TOKEN_TYPE])
			{
				case e_TOKEN_TYPE_VAR: // Prefix integer op.
					rr = Exec_LoadAddress(r[E_TOKEN_SYM_PTR]) - 1,
					Exec_WriteAddress(r[E_TOKEN_SYM_PTR], rr),
					ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_NUM,
					ret[E_TOKEN_NUM_VAL] = rr;
				case e_TOKEN_TYPE_FVAR: // Prefix float op.
					rr = _:floatsub(Float:Exec_LoadAddress(r[E_TOKEN_SYM_PTR]), 1.0),
					Exec_WriteAddress(r[E_TOKEN_SYM_PTR], rr),
					ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_FLOAT,
					ret[E_TOKEN_FLOAT_VAL] = Float:rr;
				default: EXEC_NOT_LVALUE;
			}
			default: TODO: Whatever OP this is...
		}
		case e_TOKEN_TYPE_APP      :
		{
		}
		case e_TOKEN_TYPE_ARR      :
		{
			rr = MI(r);
			if (rr == cellmin) return Parser_Error("Invalid index."), ret;
			rr = l[E_TOKEN_SYM_PTR] + rr * 4; 
			switch (l[E_TOKEN_TYPE])
			{
				case e_TOKEN_TYPE_ARR@1: ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_VAR, ret[E_TOKEN_SYM_PTR] = rr;
				case e_TOKEN_TYPE_ARR@2: ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_ARR@1, ret[E_TOKEN_SYM_PTR] = Exec_LoadAddress(rr);
				case e_TOKEN_TYPE_ARR@3: ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_ARR@2, ret[E_TOKEN_SYM_PTR] = Exec_LoadAddress(rr);
				case e_TOKEN_TYPE_ARR@4: ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_ARR@3, ret[E_TOKEN_SYM_PTR] = Exec_LoadAddress(rr);
				case e_TOKEN_TYPE_ARR@5: ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_ARR@4, ret[E_TOKEN_SYM_PTR] = Exec_LoadAddress(rr);
				case e_TOKEN_TYPE_FARR@1: ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_FVAR, ret[E_TOKEN_SYM_PTR] = rr;
				case e_TOKEN_TYPE_FARR@2: ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_FARR@1, ret[E_TOKEN_SYM_PTR] = Exec_LoadAddress(rr);
				case e_TOKEN_TYPE_FARR@3: ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_FARR@2, ret[E_TOKEN_SYM_PTR] = Exec_LoadAddress(rr);
				case e_TOKEN_TYPE_FARR@4: ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_FARR@3, ret[E_TOKEN_SYM_PTR] = Exec_LoadAddress(rr);
				case e_TOKEN_TYPE_FARR@5: ret[E_TOKEN_TYPE] = e_TOKEN_TYPE_FARR@4, ret[E_TOKEN_SYM_PTR] = Exec_LoadAddress(rr);
				default: Parser_Error("Invalid subscript or not an array.");
			}
		}
		default: ret = t;
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

#undef EXEC_TAG_MISMATCH
#undef EXEC_INCOMPATIBLE
#undef EXEC_NOT_LVALUE

#if defined INTROSPECT_EXEC_TEST

#include "..\amx\amx_memory"

main()
{
	// These should all print "1".
	new
		l  [E_TOKEN],
		r  [E_TOKEN],
		op [E_TOKEN],
		res[E_TOKEN];
	
	MAKE_TOKEN(l, e_TOKEN_TYPE_NUM, 42),
	MAKE_TOKEN(r, e_TOKEN_TYPE_NUM, 11),
	MAKE_TOKEN(op, e_TOKEN_TYPE_OP, e_TOKEN_OP_ADD),
	res = Exec_ExecToken(op, l, r);
	printf("%d", COMPARE_PARSER_TOKEN(res, {e_TOKEN_TYPE_NUM, 53}));
	
	MAKE_TOKEN(l, e_TOKEN_TYPE_NUM, 42),
	MAKE_TOKEN(r, e_TOKEN_TYPE_NUM, 0),
	MAKE_TOKEN(op, e_TOKEN_TYPE_OP, e_TOKEN_OP_MUL),
	res = Exec_ExecToken(op, l, r);
	printf("%d", COMPARE_PARSER_TOKEN(res, {e_TOKEN_TYPE_NUM, 0}));
	
	MAKE_TOKEN(l, e_TOKEN_TYPE_NUM, 42),
	MAKE_TOKEN(r, e_TOKEN_TYPE_NUM, 11),
	MAKE_TOKEN(op, e_TOKEN_TYPE_OP, e_TOKEN_OP_MUL),
	res = Exec_ExecToken(op, l, r);
	printf("%d", COMPARE_PARSER_TOKEN(res, {e_TOKEN_TYPE_NUM, 462}));
	
	MAKE_TOKEN(l, e_TOKEN_TYPE_NUM, 42),
	MAKE_TOKEN(r, e_TOKEN_TYPE_NUM, 0),
	MAKE_TOKEN(op, e_TOKEN_TYPE_OP, e_TOKEN_OP_ADD),
	res = Exec_ExecToken(op, l, r);
	printf("%d", COMPARE_PARSER_TOKEN(res, {e_TOKEN_TYPE_NUM, 42}));
	
	MAKE_TOKEN(l, e_TOKEN_TYPE_NUM, 42),
	MAKE_TOKEN(r, e_TOKEN_TYPE_FLOAT, _:11.0),
	MAKE_TOKEN(op, e_TOKEN_TYPE_OP, e_TOKEN_OP_ADD),
	res = Exec_ExecToken(op, l, r);
	printf("%d", COMPARE_PARSER_TOKEN(res, {e_TOKEN_TYPE_FLOAT, _:53.0}));
	
	MAKE_TOKEN(l, e_TOKEN_TYPE_NUM, 42),
	MAKE_TOKEN(r, e_TOKEN_TYPE_FLOAT, _:0.0),
	MAKE_TOKEN(op, e_TOKEN_TYPE_OP, e_TOKEN_OP_ADD),
	res = Exec_ExecToken(op, l, r);
	printf("%d", COMPARE_PARSER_TOKEN(res, {e_TOKEN_TYPE_FLOAT, _:42.0}));
	
	MAKE_TOKEN(l, e_TOKEN_TYPE_NUM, 42),
	MAKE_TOKEN(r, e_TOKEN_TYPE_FLOAT, _:0.0),
	MAKE_TOKEN(op, e_TOKEN_TYPE_OP, e_TOKEN_OP_EQUALS),
	res = Exec_ExecToken(op, l, r);
	printf("%d", COMPARE_PARSER_TOKEN(res, {e_TOKEN_TYPE_BOOL, _:false}));
	
	MAKE_TOKEN(l, e_TOKEN_TYPE_NUM, 42),
	MAKE_TOKEN(r, e_TOKEN_TYPE_FLOAT, _:42.0),
	MAKE_TOKEN(op, e_TOKEN_TYPE_OP, e_TOKEN_OP_EQUALS),
	res = Exec_ExecToken(op, l, r);
	printf("%d", COMPARE_PARSER_TOKEN(res, {e_TOKEN_TYPE_BOOL, _:true}));
	
	MAKE_TOKEN(l, e_TOKEN_TYPE_NUM, 555),
	MAKE_TOKEN(r, e_TOKEN_TYPE_NUM, 101),
	MAKE_TOKEN(op, e_TOKEN_TYPE_OP, e_TOKEN_OP_GREATER),
	res = Exec_ExecToken(op, l, r);
	printf("%d", COMPARE_PARSER_TOKEN(res, {e_TOKEN_TYPE_BOOL, _:true}));
	
	MAKE_TOKEN(l, e_TOKEN_TYPE_NUM, 65),
	MAKE_TOKEN(r, e_TOKEN_TYPE_NUM, 101),
	MAKE_TOKEN(op, e_TOKEN_TYPE_OP, e_TOKEN_OP_GREATER),
	res = Exec_ExecToken(op, l, r);
	printf("%d", COMPARE_PARSER_TOKEN(res, {e_TOKEN_TYPE_BOOL, _:false}));
	
	MAKE_TOKEN(l, e_TOKEN_TYPE_NUM, 65),
	MAKE_TOKEN(r, e_TOKEN_TYPE_NUM, 0),
	MAKE_TOKEN(op, e_TOKEN_TYPE_OP, e_TOKEN_OP_AND),
	res = Exec_ExecToken(op, l, r);
	printf("%d", COMPARE_PARSER_TOKEN(res, {e_TOKEN_TYPE_BOOL, _:false}));
	
	MAKE_TOKEN(l, e_TOKEN_TYPE_NUM, 65),
	MAKE_TOKEN(r, e_TOKEN_TYPE_NUM, 0),
	MAKE_TOKEN(op, e_TOKEN_TYPE_OP, e_TOKEN_OP_OR),
	res = Exec_ExecToken(op, l, r);
	printf("%d", COMPARE_PARSER_TOKEN(res, {e_TOKEN_TYPE_BOOL, _:true}));
	
	MAKE_TOKEN(l, e_TOKEN_TYPE_NUM, 65),
	MAKE_TOKEN(r, e_TOKEN_TYPE_NUM, 33),
	MAKE_TOKEN(op, e_TOKEN_TYPE_OP, e_TOKEN_OP_AND),
	res = Exec_ExecToken(op, l, r);
	printf("%d", COMPARE_PARSER_TOKEN(res, {e_TOKEN_TYPE_BOOL, _:true}));
	
	MAKE_TOKEN(l, e_TOKEN_TYPE_NUM, 0),
	MAKE_TOKEN(r, e_TOKEN_TYPE_NUM, 0),
	MAKE_TOKEN(op, e_TOKEN_TYPE_OP, e_TOKEN_OP_OR),
	res = Exec_ExecToken(op, l, r);
	printf("%d", COMPARE_PARSER_TOKEN(res, {e_TOKEN_TYPE_BOOL, _:false}));
	
	MAKE_TOKEN(l, e_TOKEN_TYPE_NONE, 0),
	MAKE_TOKEN(r, e_TOKEN_TYPE_NUM, 7),
	MAKE_TOKEN(op, e_TOKEN_TYPE_OP, e_TOKEN_OP_NEG),
	res = Exec_ExecToken(op, l, r);
	printf("%d", COMPARE_PARSER_TOKEN(res, {e_TOKEN_TYPE_NUM, -7}));
	
	MAKE_TOKEN(l, e_TOKEN_TYPE_NONE, 0),
	MAKE_TOKEN(r, e_TOKEN_TYPE_FLOAT, _:88.43),
	MAKE_TOKEN(op, e_TOKEN_TYPE_OP, e_TOKEN_OP_NEG),
	res = Exec_ExecToken(op, l, r);
	printf("%d", COMPARE_PARSER_TOKEN(res, {e_TOKEN_TYPE_FLOAT, _:-88.43}));
	
	new testArr[55][66];
	
	MAKE_TOKEN(l, e_TOKEN_TYPE_ARR@2, ref(testArr)),
	MAKE_TOKEN(r, e_TOKEN_TYPE_NUM, 5),
	MAKE_TOKEN(op, e_TOKEN_TYPE_ARR, 0),
	res = Exec_ExecToken(op, l, r);
	printf("%d", COMPARE_PARSER_TOKEN(res, {e_TOKEN_TYPE_ARR@1, ReadAmxMemory(ref(testArr) + 20)}));
	
	new testVar;
	
	MAKE_TOKEN(l, e_TOKEN_TYPE_VAR, ref(testVar)),
	MAKE_TOKEN(r, e_TOKEN_TYPE_NUM, 7),
	MAKE_TOKEN(op, e_TOKEN_TYPE_OP, e_TOKEN_OP_ASSIGN),
	res = Exec_ExecToken(op, l, r);
	printf("%d", COMPARE_PARSER_TOKEN(res, {e_TOKEN_TYPE_NUM, 7}));
	printf("%d", testVar);
	
	new testA2[10];
	
	MAKE_TOKEN(l, e_TOKEN_TYPE_ARR@1, ref(testA2)),
	MAKE_TOKEN(r, e_TOKEN_TYPE_NUM, 5),
	MAKE_TOKEN(op, e_TOKEN_TYPE_ARR, 0),
	res = Exec_ExecToken(op, l, r);
	MAKE_TOKEN(r, e_TOKEN_TYPE_NUM, 42),
	MAKE_TOKEN(op, e_TOKEN_TYPE_OP, e_TOKEN_OP_ASSIGN),
	res = Exec_ExecToken(op, res, r);
	//printf("%d", COMPARE_PARSER_TOKEN(res, {e_TOKEN_TYPE_NUM, 7}));
	printf("%d", testA2[5]);
}

#endif

