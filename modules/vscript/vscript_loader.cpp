#include "vscript_loader.h"
#include "vscript.h"

RES ResourceFormatLoaderVScript::load(const String &p_path, const String& p_original_path /*= ""*/)
{
	print_line("LOADING VSCRIPT");
	VScript* script = memnew(VScript);

	Ref<VScript> script_res(script);

	/// @todo Load the script
	// Check for errors

	script->set_path(p_original_path);
	script->reload();

	return script_res;
}

void ResourceFormatLoaderVScript::get_recognized_extensions(List<String> *p_extensions) const
{
	p_extensions->push_back("vs");
}

bool ResourceFormatLoaderVScript::handles_type(const String& p_type) const
{
	return (p_type == "Script") || (p_type == "VScript");
}

String ResourceFormatLoaderVScript::get_resource_type(const String &p_path) const
{
	String ext = p_path.extension().to_lower();
	if (ext == "vs") {
		return "VScript";
	}
	return "";
}
