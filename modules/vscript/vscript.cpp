#include "vscript.h"
#include "vscript_instance.h"
#include "vscript_language.h"

void VScript::set_path(const String& p_path)
{
	m_path = p_path;
}

bool VScript::can_instance() const
{
	// Based off of GDScript
	return m_is_valid || (!m_is_tool && !ScriptServer::is_scripting_enabled());
}

StringName VScript::get_instance_base_type() const
{
	/// @todo Implement for VScript once inheritance is implemented.
	return StringName();
}

ScriptInstance* VScript::instance_create(Object *p_this)
{
	// If this isn't a tool and scripting isn't enabled (ie. in Editor mode), then don't instance the script
	if (!m_is_tool && !ScriptServer::is_scripting_enabled()) {
#ifdef TOOLS_ENABLED
		/// @todo Create a placeholder VScriptInstance when placeholders exist.
		return NULL;
#else
		return NULL;
#endif
	}

	// Scripting is enabled or this is a tool

	// Create the script
	VScriptInstance* instance = memnew(VScriptInstance);
	instance->m_script = Ref<VScript>(this);
	instance->m_owner = p_this;
	instance->m_owner->set_script_instance(instance);
	// DO VARIOUS THINGS TO CONSTRUCT OBJECT

	// Initialize and Construct
	m_instances.insert(instance->m_owner);

	// Call initializer
	// Check for errors and erase if there are

	// Return the instance
	return instance;
}

bool VScript::instance_has(const Object* p_this) const
{
	return m_instances.has((Object*)p_this);
}

bool VScript::has_source_code() const
{
	return m_source != "";
}

String VScript::get_source_code() const
{
	return m_source;
}

void VScript::set_source_code(const String& p_code)
{
	/// @todo Set a dirty flag once we implement compiling or maybe just exporting
	m_source = p_code;
}

Error VScript::reload()
{
	return OK;
}

bool VScript::is_tool() const
{
	return m_is_tool;
}

String VScript::get_node_type() const
{
	// This is never called in the current implementation of godot
	print_line("VScript::get_node_type() was called");
	return "";
}

ScriptLanguage* VScript::get_language() const
{
	return VScriptLanguage::get_singleton();
}

VScript::VScript()
: m_is_valid(false), m_is_tool(false)
{

}
