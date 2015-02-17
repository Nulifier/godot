#ifndef VSCRIPT_EDITOR_PLUGIN_H
#define VSCRIPT_EDITOR_PLUGIN_H

#include "tools/editor/editor_plugin.h"

class VScriptEditor;

class VScriptEditorPlugin : public EditorPlugin {
	OBJ_TYPE(VScriptEditorPlugin, EditorPlugin);
private:

	VScriptEditor* m_script_editor;
	EditorNode* m_editor;

public:
	String get_name() const { return "VScript"; }
	bool has_main_screen() const { return true; }
	void edit(Object* p_node);
	bool handles(Object* p_node) const;
	void make_visible(bool p_visible);

	VScriptEditorPlugin(EditorNode* p_node);
	~VScriptEditorPlugin();
};

#endif // VSCRIPT_EDITOR_PLUGIN_H