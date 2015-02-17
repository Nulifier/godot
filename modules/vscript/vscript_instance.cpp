#include "vscript_instance.h"
#include "vscript_language.h"

bool VScriptInstance::set(const StringName& p_name, const Variant& p_value)
{
	return false;
}

bool VScriptInstance::get(const StringName& p_name, Variant &r_ret) const
{
	return false;
}

void VScriptInstance::get_property_list(List<PropertyInfo> *p_properties) const
{
	// Do nothing
}

void VScriptInstance::get_method_list(List<MethodInfo> *p_list) const
{
	// Do nothing
}

bool VScriptInstance::has_method(const StringName& p_method) const
{
	return false;
}

Variant VScriptInstance::call(const StringName& p_method, const Variant** p_args, int p_argcount, Variant::CallError& r_error)
{
	// No function calling supported yet
	return Variant();
}

void VScriptInstance::call_multilevel(const StringName& p_method, const Variant** p_args, int p_argcount)
{
	// No function calling supported yet
}

void VScriptInstance::call_multilevel_reversed(const StringName& p_method, const Variant** p_args, int p_argcount)
{
	// No function calling supported yet
}

void VScriptInstance::notification(int p_notification)
{
	// No notifications supported yet
}

Ref<Script> VScriptInstance::get_script() const
{
	return m_script;
}

ScriptLanguage* VScriptInstance::get_language()
{
	return VScriptLanguage::get_singleton();
}

VScriptInstance::VScriptInstance()
: m_owner(NULL)
{

}

VScriptInstance::~VScriptInstance()
{

}
