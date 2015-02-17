#ifndef VSCRIPT_SAVER_H
#define VSCRIPT_SAVER_H

#include "io/resource_saver.h"

class ResourceFormatSaverVScript : public ResourceFormatSaver {
public:
	Error save(const String& p_path, const RES& p_resource, uint32_t p_flags = 0);
	void get_recognized_extensions(const RES& p_resource, List<String> *p_extensions) const;
	bool recognize(const RES& p_resource) const;
};

#endif // VSCRIPT_SAVER_H
