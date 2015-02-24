#ifndef VSCRIPT_FUNCTION_H
#define VSCRIPT_FUNCTION_H

#include "vscript_stack.h"

class VScriptInstance;

class VScriptFunction {

	/*
	Each opcode has an indicator like this [-o, +p, x]
	The first field, o, is how many elements the function pops from the stack. The second field, p, is how many elements the function
	pushes onto the stack. (Any function always pushes its results after popping its arguments.) A field in the form x|y means the
	function can push (or pop) x or y elements, depending on the situation; an interrogation mark '?' means that we cannot know how
	many elements the function pops/pushes by looking only at its arguments (e.g., they may depend on what is on the stack). The
	third field, x, tells whether the function may throw errors: '-' means the function never throws any error; 'm' means the function
	may throw an error only due to not enough memory; 'e' means the function may throw other kinds of errors; 'v' means the function
	may throw an error on purpose.
	*/
	enum Opcode {
		// Push
		OPCODE_PUSH_INT,		// OP, INT				[-0, +1, -]
		OPCODE_PUSH_CONST,		// OP, CONST_IDX		[-0, +1, -]
		OPCODE_PUSH_TEMP,		// OP, TEMP_IDX			[-0, +1, -]
		OPCODE_PUSH_MEMBER,		// OP, NAME_IDX			[-0, +1, -]
	};

	Vector<Variant> m_constants;
	Vector<StringName> m_names;		// This table contains all the strings that specify member values or functions
	Vector<int> m_code;

public:
	struct CallState {
		VScriptInstance* instance;
		VScriptStack stack;
		int instr_ptr;
	};

	Variant call(VScriptInstance* p_instance, const Variant** p_args, int p_argcount, Variant::CallError& r_err, CallState* p_state = NULL);
};

#endif // VSCRIPT_FUNCTION_H
