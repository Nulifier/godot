#include "register_types.h"

#include "vscript_language.h"
#include "vscript.h"
#include "vscript_loader.h"
#include "vscript_saver.h"

VScriptLanguage* script_language_vscript = NULL;
ResourceFormatLoaderVScript* resource_loader_vscript = NULL;
ResourceFormatSaverVScript* resource_saver_vscript = NULL;

void register_vscript_types() {

	ObjectTypeDB::register_type<VScript>();

	script_language_vscript = memnew(VScriptLanguage);
	ScriptServer::register_language(script_language_vscript);

	resource_loader_vscript = memnew(ResourceFormatLoaderVScript);
	ResourceLoader::add_resource_format_loader(resource_loader_vscript);

	resource_saver_vscript = memnew(ResourceFormatSaverVScript);
	ResourceSaver::add_resource_format_saver(resource_saver_vscript);
}

void unregister_vscript_types() {

	if (script_language_vscript) {
		memdelete(script_language_vscript);
		script_language_vscript = NULL;
	}

	if (resource_loader_vscript) {
		memdelete(resource_loader_vscript);
		resource_loader_vscript = NULL;
	}

	if (resource_saver_vscript) {
		memdelete(resource_saver_vscript);
		resource_saver_vscript = NULL;
	}
}
