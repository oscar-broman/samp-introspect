#include <a_samp>

#define AMX_NAME "introspect_demo.amx"
#include "introspect"

new g_LocalVariable = 5;
new Float:g_LocalArray[123][456];

stock SomeFunction() {
	
}

main() {
	IntrospectInit();
	SomeFunction();
	
	g_LocalVariable = g_LocalVariable++;
	g_LocalArray[1][2] = g_LocalArray[3][4] + 5.6;
	
	new info[E_VARIABLE];
	
	if (GerVariableInfo("g_LocalVariable", info)) {
		print(info[Name]);
		printf("  Address: %08x", info[Address]);
		printf("  Tag: %04x", info[Tag]);
		printf("  Dimensions: %d", info[Dimensions]);
		printf("  Dimension sizes: %d %d %d", info[DimensionSize][0], info[DimensionSize][1], info[DimensionSize][2]);
	} else {
		print("Variable not found.");
	}
	
	if (GerVariableInfo("g_LocalArray", info)) {
		print(info[Name]);
		printf("  Address: %08x", info[Address]);
		printf("  Tag: %04x", info[Tag]);
		printf("  Dimensions: %d", info[Dimensions]);
		printf("  Dimension sizes: %d %d %d", info[DimensionSize][0], info[DimensionSize][1], info[DimensionSize][2]);
	} else {
		print("Variable not found.");
	}
	
	new address = GetFunctionAddress("SomeFunction");
	
	if (address) {
		print("SomeFunction");
		printf("  Address: %08x", address);
	} else {
		print("Function not found.");
	}
}

