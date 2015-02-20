#ifndef VSCRIPT_INSTANCE_H
#define VSCRIPT_INSTANCE_H

#include "script_language.h"
#include "vscript.h"

class VScriptInstance : public ScriptInstance {
	friend class VScript;
private:

	Object* m_owner;
	Ref<VScript> m_script;

public:
	// Set a variable
	bool set(const StringName& p_name, const Variant& p_value);

	// Get a value
	bool get(const StringName& p_name, Variant &r_ret) const;
	void get_property_list(List<PropertyInfo> *p_properties) const;

	void get_method_list(List<MethodInfo> *p_list) const;
	bool has_method(const StringName& p_method) const;

	/**	Calls a method.
	 *	@param p_method The name of a method.
	 *	@param p_args An array of arguments.
	 *	@param p_argcount The number of arguments in p_args.
	 *	@param r_error An error return code. Variant::CallError::CALL_ERROR_INVALID_METHOD if the method doesn't exist.
	 */
	Variant call(const StringName& p_method, const Variant** p_args, int p_argcount, Variant::CallError& r_error);

	/**	Calls a method in this and all base classes.
	 *	@param p_method The name of a method.
	 *	@param p_args An array of arguments.
	 *	@param p_argcount The number of arguments in p_args.
	 */
	void call_multilevel(const StringName& p_method, const Variant** p_args, int p_argcount);

	/**	Calls a method in this and all base classes starting from the most basic.
	 *	This is usually used for initializers.
	 *	@param p_method The name of a method.
	 *	@param p_args An array of arguments.
	 *	@param p_argcount The number of arguments in p_args.
	 */
	void call_multilevel_reversed(const StringName& p_method, const Variant** p_args, int p_argcount);

	/**	Notifies the instance starting at the most derived.
	 *	@param p_notification The notification.
	 */
	void notification(int p_notification);

	Ref<Script> get_script() const;

	ScriptLanguage* get_language();

	VScriptInstance();
	~VScriptInstance();
};

#endif // VSCRIPT_INSTANCE_H
