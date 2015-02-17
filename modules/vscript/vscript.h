#ifndef VSCRIPT_H
#define VSCRIPT_H

#include "script_language.h"

class VScript : public Script {
	OBJ_TYPE(VScript, Script);
private:

	bool m_is_valid;
	bool m_is_tool;

	Set<Object*> m_instances;

	String m_path;
	String m_source;

	Map<StringName, Variant> m_constants;

public:

	void set_path(const String& p_path);


	/// Returns true if this script can has instances of it created, false if not.
	bool can_instance() const;

	/// Returns the type that this script inherits from.
	StringName get_instance_base_type() const;

	/// Create an instance of this script
	ScriptInstance* instance_create(Object *p_this);

	/// Return true if the supplied Object is an instance of this script.
	bool instance_has(const Object* p_this) const;

	/// Return true if this script has source code set.
	bool has_source_code() const;

	/// Gets the source code for this script.
	String get_source_code() const;

	/// Sets the source code for this script.
	void set_source_code(const String& p_code);

	/// Reloads this script from file.
	Error reload();

	/// Returns true if this script is to be run in the editor.
	bool is_tool() const;

	/// Returns the node type of this script.
	/// @note Not implemented by GDScript.
	String get_node_type() const;

	/// Returns the language used by this script.
	ScriptLanguage* get_language() const;
	//void update_exports() {}

	VScript();
};

#endif // VSCRIPT_H
