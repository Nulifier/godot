#ifndef VSCRIPT_LOADER_H
#define VSCRIPT_LOADER_H

#include "io/resource_loader.h"

class ResourceFormatLoaderVScript : public ResourceFormatLoader {
public:

	RES load(const String &p_path, const String& p_original_path = "");
	void get_recognized_extensions(List<String> *p_extensions) const;
	bool handles_type(const String& p_type) const;
	String get_resource_type(const String &p_path) const;
};

#endif
