#include "vscript_editor_plugin.h"
#include "vscript.h"
#include "vscript_editor.h"

void VScriptEditorPlugin::edit(Object* p_node)
{
	m_script_editor->edit(p_node->cast_to<VScript>());
}

bool VScriptEditorPlugin::handles(Object* p_object) const
{
	return p_object->is_type("VScript");
}

void VScriptEditorPlugin::make_visible(bool p_visible)
{
	if (p_visible) {
		m_script_editor->show();
	}
	else {
		m_script_editor->hide();
	}
}

VScriptEditorPlugin::VScriptEditorPlugin(EditorNode* p_editor)
: m_editor(p_editor), m_script_editor(NULL)
{
	m_script_editor = memnew(VScriptEditor(p_editor));
	m_editor->get_viewport()->add_child(m_script_editor);
	m_script_editor->set_area_as_parent_rect();
	m_script_editor->hide();
}

VScriptEditorPlugin::~VScriptEditorPlugin()
{

}
