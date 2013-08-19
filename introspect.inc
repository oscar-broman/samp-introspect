#include <a_samp>

#if !defined AMX_NAME
	#error Please #define AMX_NAME "amx_name.amx"
#endif

#if debug < 2
	#error The debug level must be 2 or 3
#endif

#if !defined MAX_GLOBAL_VARIABLES
	#define MAX_GLOBAL_VARIABLES 4096
#endif

#if !defined MAX_FUNCTIONS
	#define MAX_FUNCTIONS 2048
#endif

#if !defined MAX_TAGS
	#define MAX_TAGS 512
#endif

#if defined INTROSPECT_NATIVES
	#include "introspect-natives"
	
	#tryinclude "amx_assembly\dynamic_call"
	#tryinclude "amx\dynamic_call"

	#if !defined CallNative
		#error amx_assembly is required. Get it here: github.com/Zeex/amx_assembly
	#endif
#endif

static const s_AmxName[] = AMX_NAME;

enum {
	IDENT_VARIABLE  = 1,
	IDENT_REFERENCE = 2,
	IDENT_ARRAY     = 3,
	IDENT_REFARRAY  = 4,
	IDENT_FUNCTION  = 9
}

enum {
	VCLASS_GLOBAL,
	VCLASS_LOCAL,
	VCLASS_STATIC
}

enum E_VARIABLE {
	Address,
	Tag,
	Name[32],
	Dimensions,
	DimensionSize[3]
}

enum E_FUNCTION {
	Address,
	Name[48]
}

enum E_TAG {
	Tag,
	Name[32]
}

static
	s_Variables[MAX_GLOBAL_VARIABLES][E_VARIABLE],
	s_Functions[MAX_FUNCTIONS][E_FUNCTION],
//	s_Tags[MAX_TAGS][E_TAG],
	
	s_NumVariables,
	s_NumFunctions
//	s_NumTags
;

stock IntrospectInit() {
	new File:fp = fopen(s_AmxName, io_read);
	
	if (!fp) {
		printf("(introspect.inc) Unable to open \"%s\"", s_AmxName);
		
		return false;
	}
	
	// Go to the end of the AMX (0 contains the size excluding debug information)
	fseek(fp, freadcell(fp));
	
	new size         = freadcell(fp);
	new magic        = freadshort(fp);
	new file_version = freadbyte(fp);
	new amx_version  = freadbyte(fp);
	new flags        = freadshort(fp);
	new num_files    = freadshort(fp);
	new num_lines    = freadshort(fp);
	new num_symbols  = freadshort(fp);
	new num_tags     = freadshort(fp);
	new num_machines = freadshort(fp);
	new num_states   = freadshort(fp);
	
	#pragma unused num_states, num_machines, num_tags, flags, amx_version, file_version, magic, size
	
	// Skip the file table
	for (new i = 0; i < num_files; i++) {
		fseek(fp, 4, seek_current);
		while (freadbyte(fp)) {}
	}
	
	// Skip the line table
	fseek(fp, num_lines * 8, seek_current);
	
	// Read the symbol table
	for (new i = 0; i < num_symbols; i++) {
		new address = freadcell(fp);
		new tag = freadshort(fp);
		new codestart = freadcell(fp);
		new codeend = freadcell(fp);
		new ident = freadbyte(fp);
		new vclass = freadbyte(fp);
		new dim = freadshort(fp);
		new name[48]; freadstr(fp, name);
		new dim_size[4];
		
		#pragma unused codestart, codeend
		
		for (new j = 0; j < dim; j++) {
			freadshort(fp);
			
			dim_size[j] = freadcell(fp);
		}
		
		if (vclass != VCLASS_GLOBAL) {
			continue;
		}
		
		switch (ident) {
			case IDENT_VARIABLE,
			     IDENT_ARRAY: {
				s_Variables[s_NumVariables][Address] = address;
				s_Variables[s_NumVariables][Tag] = tag;
				s_Variables[s_NumVariables][Dimensions] = dim;
				s_Variables[s_NumVariables][DimensionSize][0] = dim_size[0];
				s_Variables[s_NumVariables][DimensionSize][1] = dim_size[1];
				s_Variables[s_NumVariables][DimensionSize][2] = dim_size[2];
				strunpack(s_Variables[s_NumVariables][Name], name, 32);
				
				s_NumVariables++;
			}
			
			case IDENT_FUNCTION: {
				s_Functions[s_NumFunctions][Address] = address;
				strunpack(s_Functions[s_NumFunctions][Name], name, 48);
				
				s_NumFunctions++;
			}
		}
	}
	
	
	fclose(fp);
	
	return true;
}

stock GetFunctionAddress(const name[]) {
	for (new i = 0; i < s_NumFunctions; i++) {
		if (!strcmp(name, s_Functions[i][Name])) {
			return s_Functions[i][Address];
		}
	}
	
	return -1;
}

stock GerVariableInfo(const name[], info[E_VARIABLE]) {
	for (new i = 0; i < s_NumVariables; i++) {
		if (!strcmp(name, s_Variables[i][Name])) {
			info[Address] = s_Variables[i][Address];
			info[Tag] = s_Variables[i][Tag];
			info[Dimensions] = s_Variables[i][Dimensions];
			info[DimensionSize][0] = s_Variables[i][DimensionSize][0];
			info[DimensionSize][1] = s_Variables[i][DimensionSize][1];
			info[DimensionSize][2] = s_Variables[i][DimensionSize][2];
			strunpack(info[Name], s_Variables[i][Name], 32);
			
			return true;
		}
	}
	
	return false;
}

// Read a single cell
static freadcell(File:fp) {
	new buf[1];
	
	fblockread(fp, buf);
	
	return buf[0];
}

// Read a 16-bit integer
static freadshort(File:fp) {
	new buf[1];
	
	fblockread(fp, buf);
	fseek(fp, -2, seek_current);
	
	return buf[0] & 0xFFFF;
}

// Read a single byte
static freadbyte(File:fp) {
	return fgetchar(fp, 0, false);
}

// Read a zero-terminated string
static freadstr(File:fp, buf[], maxlength = sizeof(buf)) {
	new i = 0, c;
	
	buf[i] = '\0';
	
	while ((c = fgetchar(fp, 0))) {
		if (i + 1 < maxlength) {
			buf[i++] = c;
		}
	}
	
	buf[i] = '\0';
	
	return i;
}

static ReadSymbol(const input[], &index, output[], maxlength = sizeof(output)) {
	new i = index, j = 0;
	
	if ('0' <= input[i] <= '9') {
		return false;
	}
	
	for (new c; (c = input[i]); i++) {
		if (j + 1 >= maxlength) {
			return false;
		}
		
		switch (c) {
			case 'a' .. 'z',
			     'A' .. 'Z',
			     '0' .. '9',
			     '@', '_': {
				output[j++] = c;
				
				continue;
			}
			
			default: {
				break;
			}
		}
	}
	
	if (!j) {
		return false;
	}
	
	output[j] = '\0';
	
	index = i;
	
	return j;
}

static ReadString(const input[], &index, output[], maxlength = sizeof(output)) {
	new in_esc = false, i = 0, c;
	
	for (index++; (c = input[index]); index++) {
		if (i + 1 >= maxlength) {
			break;
		}
		
		if (c == '\\') {
			if (in_esc) {
				in_esc = false;
				
				output[i++] = '\\';
			} else {
				in_esc = true;
			}
		} else if (in_esc) {
			switch (c) {
				case '"': output[i++] = c;
				case 'a': output[i++] = '\a';
				case 'b': output[i++] = '\b';
				case 'e': output[i++] = '\e';
				case 'r': output[i++] = '\r';
				case 'n': output[i++] = '\n';
				case 't': output[i++] = '\t';
				case 'v': output[i++] = '\v';
			}
			
			in_esc = false;
		} else {
			if ( c == '"') {
				output[i] = '\0';
				index++;
				
				return true;
			} else {
				output[i++] = c;
			}
		}
	}
	
	output[i] = '\0';
	
	return false;
}

static ReadNumber(const input[], &index, &output, &type) {
	new i, c, had_num = false;
	
	type = 'i';
	
	for (i = index; (c = input[i]); i++) {
		if (i == 0 && c == '-') {
			continue;
		}
		
		switch (c) {
			case '0' .. '9': {
				had_num = true;
				
				continue;
			}
			
			case '.': {
				if (!had_num) {
					return false;
				}
				
				type = 'f';
			}
			
			case '-', '+', 'e': {
				if (!had_num) {
					return false;
				}
				
				continue;
			}
			
			default: {
				break;
			}
		}
	}
	
	if (!had_num) {
		return false;
	}
	
	if (type == 'f') {
		output = _:floatstr(input[index]);
	} else {
		output = strval(input[index]);
	}
	
	index = i;
	
	return true;
}

static SkipSpaces(const buf[], &index) {
	while ('\0' < buf[index] <= ' ') {
		index++;
	}
}

stock RunSimpleStatement(const statement[], error[] = "", errlen = sizeof(error)) {
	new sym[32], idx = 0;
	
	SkipSpaces(statement, idx);
	
	if (!ReadSymbol(statement, idx, sym)) {
		strunpack(error, "Invalid symbol given", errlen);
		
		return false;
	}
	
	SkipSpaces(statement, idx);
	
	if (statement[idx] == '=') {
		new info[E_VARIABLE];
		
		if (!GerVariableInfo(sym, info)) {
			format(error, errlen, "Invalid variable: %s", sym);
			
			return false;
		}
		
		idx++, SkipSpaces(statement, idx);
		
		switch (statement[idx]) {
			case '"': {
				new buf[512];
				
				if (!ReadString(statement, idx, buf)) {
					strunpack(error, "Invalid string literal", errlen);
					
					return false;
				}
				
				new addr = info[Address];
				new len = info[DimensionSize][0];
				
				if (!len) {
					len = sizeof(buf);
				}{}
				
				#emit PUSH.S   len
				#emit PUSH.ADR buf
				#emit PUSH.S   addr
				#emit PUSH.C   12
				#emit SYSREQ.C strunpack
				#emit STACK    16
				
				return true;
			}
			
			case '-', '0' .. '9': {
				new type, value;
				
				if (!ReadNumber(statement, idx, value, type)) {
					strunpack(error, "Invalid number", errlen);
					
					return false;
				}
				
				new addr = info[Address];
				
				#emit LOAD.S.pri value
				#emit SREF.S.pri addr
				
				return true;
			}
			
			default: {
				strunpack(error, "Invalid value in assignment (expects string or number)", errlen);
				
				return false;
			}
		}
	} else if (statement[idx] == '(') {
		idx++;
		
		new addr = 0;
		
		#if defined INTROSPECT_NATIVES
			new is_native = false;
			
			if (-1 != (addr = GetNativeIndexFromName(sym))) {
				is_native = true;
			}
		#endif
			
		if (addr == -1) {
			addr = GetFunctionAddress(sym);
		}
		
		if (addr == -1) {
			format(error, errlen, "Invalid function: %s", sym);
			
			return false;
		}
		
		new
			arg[16],
			arg_type[16],
			args = 0,
			buf[2048],
			buf_idx = 0,
			buf_adr;
		
		#emit ADDR.pri   buf
		#emit STOR.S.pri buf_adr
		
		while (statement[idx]) {
			SkipSpaces(statement, idx);
			
			switch (statement[idx]) {
				case ')': {
					break;
				}
				
				case ',': {
					idx++;
					continue;
				}
				
				case '"': {
					if (!ReadString(statement, idx, buf[buf_idx], sizeof(buf) - buf_idx)) {
						strunpack(error, "Invalid string literal", errlen);
					
						return false;
					}
					
					arg_type[args] = 's';
					arg[args] = buf_adr + buf_idx;
					arg[args] = buf_adr + buf_idx * 4;
					
					args++;
					
					buf_idx += strlen(buf[buf_idx]) + 1;
				}
				
				case '-', '0' .. '9': {
					new type, value;
				
					if (!ReadNumber(statement, idx, value, type)) {
						strunpack(error, "Invalid number", errlen);
					
						return false;
					}
					
					arg_type[args] = 'i';
					arg[args] = value;
					
					args++;
				}
			}
		}
		
		#if defined INTROSPECT_NATIVES
			if (is_native) {
				new arg_bytes = 4 + args * 4;
				
				while (--args >= 0) {
					#emit ADDR.pri arg
					#emit LOAD.S.alt args
					#emit SHL.C.alt 2
					#emit ADD
					#emit PUSH.pri
				}
				
				#emit PUSH.S addr
				#emit PUSH.S arg_bytes
		
				#emit LCTRL 6
				#emit ADD.C 28
				#emit PUSH.pri
				#emit CONST.pri CallNative
				#emit SCTRL 6
				
				return true;
			}
		#endif
		
		new value, arg_bytes = args * 4;
		
		while (--args >= 0) {
			value = arg[args];
			#emit PUSH.S value
		}
		
		#emit PUSH.S arg_bytes
		
		#emit LCTRL 6
		#emit ADD.C 28
		#emit PUSH.pri
		#emit LOAD.S.pri addr
		#emit SCTRL 6
		
		return true;
	}
	
	strunpack(error, "Invalid simple statement", errlen);
	
	return false;
}