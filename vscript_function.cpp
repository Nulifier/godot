#include "vscript_function.h"

Variant VScriptFunction::call(VScriptInstance* p_instance, const Variant** p_args, int p_argcount, Variant::CallError& r_err, CallState* p_state /*= NULL*/)
{
	r_err.error = Variant::CallError::CALL_OK;

	VScriptInstance* instance;
	VScriptStack* stack;
	int ip = 0;
	Variant ret_value;

	if (p_state) {
		instance = p_state->instance;
		stack = p_state->stack;
		ip = p_state->instr_ptr;
	}
	else {
		instance = p_instance;
		stack = memnew(VScriptStack);
	}
	
	String err_text;
	bool exit_ok = false;

#ifdef DEBUG_ENABLED
#define CHECK_SPACE(m_space)\
	ERR_BREAK((ip+(m_space)) > m_code.size())
#else
#define CHECK_SPACE(m_space)
#endif

	// Run opcodes
	while (ip < m_code.size()) {
		Opcode last_opcode = m_code[ip];
		switch (m_code[ip]) {
			case OPCODE_PUSH_INT: {
				CHECK_SPACE(2);
				stack->push(m_code[ip + 1]);
				ip += 2;
			} continue;
			case OPCODE_PUSH_CONST: {
				CHECK_SPACE(2);
				const Variant& c = m_constants[m_code[ip + 1]];
				stack->push(c);
				ip += 2;
			} continue;
			default:
			{
				err_text = "Illegal opcode " + itos(m_code[ip]) + " at address " itos(ip);
			} break;
		}

		// Check for errors
		if (exit_ok)
			break;		// We returned from the function

		if (err_text.empty()) {
			err_text = "Internal Script Error! - opcode #" + itos(last_opcode) + " (report please).";
		}

		if (!VScriptLanguage::get_singleton()->debug_break(err_text, false) {
			_err_print_error("", "", 0, err_text.utf8().get_data());
		}

		break;
	}

	// Cleanup
	if (p_state) {
		p_state->instr_ptr = ip;
	}
	else {
		memdelete(stack);
	}

	return ret_value;
}
