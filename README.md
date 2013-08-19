# introspect.inc

Load the AMX debug information during runtime. Access non-public variables and functions.

## Requirements

The script must be compiled with debug level 2 or 3.

Your amx-file must be placed in the `scriptfiles` directory. Its name must match `AMX_NAME`.

Alternatively, you can create a symbolic link to the scriptfiles directory (see [this](http://forum.sa-mp.com/showthread.php?t=458669)).

## Usage

To get the address of a function, use `GetFunctionAddress(name[])`.

To get information about a variable, use `GerVariableInfo(const name[], info[E_VARIABLE])`. `E_VARIABLE` is an enum with the following layout:

```
enum E_VARIABLE {
	Address,
	Tag,
	Name[32],
	Dimensions,
	DimensionSize[3]
}
```

## Run "simple statements"

An additional function will allow you to run Pawn-like statements such as the following:

```
g_SomeVariable = "string value"
g_SomeVariable = 123456
g_SomeVariable = 123.456

SomeFunction(123, 456.678, "hello")
SetPlayerHealth(2, 100.0)
SendRconCommand("echo hello!")
```

### Native functions

To run native functions, you must define `INTROSPECT_NATIVES` before including `introspect.inc`. You'll also need [amx_assembly](https://github.com/Zeex/amx_assembly).

When that statement is executed, the function will look for a variable called `g_SomeVariable` and set its value.

### Usage example:

```
new error[128];

if (RunSimpleStatement("g_SomeVariable = \"hello world!\"", error)) {
	print("The variable was modified!");
} else {
	printf("Error: %s", error);
}
```