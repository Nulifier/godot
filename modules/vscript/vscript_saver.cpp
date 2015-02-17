#include "vscript_saver.h"
#include "vscript.h"
#include "os/file_access.h"

Error ResourceFormatSaverVScript::save(const String& p_path, const RES& p_resource, uint32_t p_flags /*= 0*/)
{
	Ref<VScript> vs_res = p_resource;
	ERR_FAIL_COND_V(vs_res.is_null(), ERR_INVALID_PARAMETER);

	String source = vs_res->get_source_code();

	Error err;
	FileAccess* file = FileAccess::open(p_path, FileAccess::WRITE, &err);

	if (err) {
		ERR_FAIL_COND_V(err, err);
	}

	file->store_string(source);

	file->close();
	memdelete(file);

	return OK;
}

void ResourceFormatSaverVScript::get_recognized_extensions(const RES& p_resource, List<String> *p_extensions) const
{
	if (recognize(p_resource)) {
		p_extensions->push_back("vs");
	}
}

bool ResourceFormatSaverVScript::recognize(const RES& p_resource) const
{
	return p_resource->cast_to<VScript>() != NULL;
}
