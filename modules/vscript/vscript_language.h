#ifndef VSCRIPT_LANGUAGE_H
#define VSCRIPT_LANGUAGE_H

#include "script_language.h"

/// The definition of the VScript language.
class VScriptLanguage : public ScriptLanguage {
private:

	static VScriptLanguage* s_singleton;

public:

	/// Returns the name of the scripting language
	String get_name() const;

	//////////////////////////////////////////////////////////////////////////
	// LANGUAGE FUNCTIONS

	/// Initializes the language.
	void init();

	/// Returns what type of language this is.
	String get_type() const;

	/// Returns what extension the scripts of this language use.
	String get_extension() const;

	/**	Executes a script file.
	 *	@note The GDScript implementation never implemented this function.
	 */
	Error execute_file(const String& p_path);

	/// Finalizes the language.
	void finish();

	//////////////////////////////////////////////////////////////////////////
	// EDITOR FUNCTIONS

	/// Fills a provided list with words that are reserved in the text editor.
	void get_reserved_words(List<String> *p_words) const {}

	/**	Fills a list with the strings that signify a comment.
	 *	If its a string that show the rest of the line as a comment simply put it in the string
	 *	If it has a beginning and end, separate them with a space.
	 */
	void get_comment_delimiters(List<String> *p_delimiters) const {}

	/**	Fills a list with the strings that signify a string.
	 */
	void get_string_delimiters(List<String> *p_delimiters) const {}

	/** Returns a string that will be populated as the default contents of a new script file.	
	 *	@param p_class_name The name of the newly created class.
	 *	@param p_base_class_name The name of the class this one is inheriting from.
	 */
	String get_template(const String& p_class_name, const String& p_base_class_name) const;

	/**	Validates a script file.
	 *	@param p_script The path to the script file.
	 *	@param r_line_error A return value of the line of the file the error is on.
	 *	@param r_col_error A return value for the column of the file the error is on.
	 *	@param r_test_error A return value for the error message.
	 *	@param p_path The self path, not sure what this means.
	 *	@param r_functions A pointer to a list of functions in this class.
	 *	@returns true if validation was successful, false if not.
	 */
	bool validate(const String& p_script, int &r_line_error, int &r_col_error, String& r_test_error, const String& p_path = "", List<String> *r_functions = NULL) const;

	/// Creates a new Script object.
	Script* create_script() const;

	/// Return true if this language has named classes, false if not.
	bool has_named_classes() const;

	/**	Finds a function in a chunk of code.
	 *	@param p_function The name of the function to find.
	 *	@param p_code The chunk of code to find the function in.
	 *	@returns The line that the function is on, or -1 if it can't be found.
	 */
	int find_function(const String& p_function, const String& p_code) const;

	/**	Makes a template for a function.
	 *	@param p_class The name of the class if this language uses them.
	 *	@param p_name The name of the function to create.
	 *	@param p_args A list of the arguments to this function.
	 *	@returns A String containing the function.
	 */
	String make_function(const String& p_class, const String& p_name, const StringArray& p_args) const;

	// This has a no-op body already from the base class.
	//Error complete_code(const String& p_code, const String& p_base_path, Object*p_owner, List<String>* r_options, String& r_call_hint);

	/**	Indents a block of code.
	 *	@param p_code The block of code to indent, it will also be returned through this parameter.
	 *	@param p_from_line The line to start indenting on, inclusive.
	 *	@param p_to_line The line to stop indenting on, inclusive.
	 */
	void auto_indent_code(String& p_code, int p_from_line, int p_to_line) const {}

	bool uses_text_editor() const { return false; }

	//////////////////////////////////////////////////////////////////////////
	// DEBUGGER FUNCTIONS

	String debug_get_error() const;

	int debug_get_stack_level_count() const;

	int debug_get_stack_level_line(int p_level) const;

	String debug_get_stack_level_function(int p_level) const;

	String debug_get_stack_level_source(int p_level) const;

	void debug_get_stack_level_locals(int p_level, List<String> *p_locals, List<Variant> *p_values, int p_max_subitems = -1, int p_max_depth = -1);

	void debug_get_stack_level_members(int p_level, List<String> *p_members, List<Variant> *p_values, int p_max_subitems = -1, int p_max_depth = -1);

	void debug_get_globals(List<String> *p_locals, List<Variant> *p_values, int p_max_subitems = -1, int p_max_depth = -1);

	String debug_parse_stack_level_expression(int p_level, const String& p_expression, int p_max_subitems = -1, int p_max_depth = -1);

	//////////////////////////////////////////////////////////////////////////
	// LOADER FUNCTIONS

	void get_recognized_extensions(List<String> *p_extensions) const;

	void get_public_functions(List<MethodInfo> *p_functions) const;

	void get_public_constants(List<Pair<String, Variant> > *p_constants) const;

	//void frame();

	inline static VScriptLanguage* get_singleton() { return s_singleton; }

	VScriptLanguage();
	~VScriptLanguage();
};

#endif // VSCRIPT_LANGUAGE_H
