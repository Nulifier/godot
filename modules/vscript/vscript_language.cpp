#include "vscript_language.h"
#include "vscript.h"
#include "os/thread.h"

VScriptLanguage* VScriptLanguage::s_singleton = NULL;

String VScriptLanguage::get_name() const
{
	return "VScript";
}

void VScriptLanguage::init()
{

}

String VScriptLanguage::get_type() const
{
	return "VScript";
}

String VScriptLanguage::get_extension() const
{
	return "vs";
}

Error VScriptLanguage::execute_file(const String& p_path)
{
	/// @todo Implement executing VScript files.
	return OK;
}

void VScriptLanguage::finish()
{

}

String VScriptLanguage::get_template(const String& p_class_name, const String& p_base_class_name) const
{
	/// @todo Once the system is implemented, redo this
	return "";
}

bool VScriptLanguage::validate(const String& p_script, int &r_line_error, int &r_col_error, String& r_test_error, const String& p_path /*= ""*/, List<String> *r_functions /*= NULL*/) const
{
	/// @todo Implement validation of VScript files.
	return true;
}

Script* VScriptLanguage::create_script() const
{
	return memnew(VScript);
}

bool VScriptLanguage::has_named_classes() const
{
	return false;
}

int VScriptLanguage::find_function(const String& p_function, const String& p_code) const
{
	/// @todo Implement finding functions if it makes sense.
	return -1;
}

String VScriptLanguage::make_function(const String& p_class, const String& p_name, const StringArray& p_args) const
{
	// I don't think this will ever make sense for a visual scripting system
	return "";
}

String VScriptLanguage::debug_get_error() const
{
	return m_debug_error;
}

int VScriptLanguage::debug_get_stack_level_count() const
{
	return 1;
}

int VScriptLanguage::debug_get_stack_level_line(int p_level) const
{
	return -1;
}

String VScriptLanguage::debug_get_stack_level_function(int p_level) const
{
	return "";
}

String VScriptLanguage::debug_get_stack_level_source(int p_level) const
{
	return "";
}

void VScriptLanguage::debug_get_stack_level_locals(int p_level, List<String> *p_locals, List<Variant> *p_values, int p_max_subitems /*= -1*/, int p_max_depth /*= -1*/)
{
	return;
}

void VScriptLanguage::debug_get_stack_level_members(int p_level, List<String> *p_members, List<Variant> *p_values, int p_max_subitems /*= -1*/, int p_max_depth /*= -1*/)
{
	return;
}

void VScriptLanguage::debug_get_globals(List<String> *p_locals, List<Variant> *p_values, int p_max_subitems /*= -1*/, int p_max_depth /*= -1*/)
{
	return;
}

String VScriptLanguage::debug_parse_stack_level_expression(int p_level, const String& p_expression, int p_max_subitems /*= -1*/, int p_max_depth /*= -1*/)
{
	return "";
}

bool VScriptLanguage::debug_break(const String& p_error, bool p_allow_continue /*= true*/)
{
	if (ScriptDebugger::get_singleton() && Thread::get_caller_ID() == Thread::get_main_ID()) {
		// Set variables regarding calling function here
		m_debug_error = p_error;
		ScriptDebugger::get_singleton()->debug(this, p_allow_continue);
		return true;
	}
	else {
		return false;
	}
}

void VScriptLanguage::get_recognized_extensions(List<String> *p_extensions) const
{
	p_extensions->push_back("vs");
}

void VScriptLanguage::get_public_functions(List<MethodInfo> *p_functions) const
{
	return;
}

void VScriptLanguage::get_public_constants(List<Pair<String, Variant> > *p_constants) const
{
	return;
}

VScriptLanguage::VScriptLanguage()
{
	ERR_FAIL_COND(s_singleton);
	s_singleton = this;
}

VScriptLanguage::~VScriptLanguage()
{
	s_singleton = NULL;
}
