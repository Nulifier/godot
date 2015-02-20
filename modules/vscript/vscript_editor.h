#ifndef VSCRIPT_EDITOR_H
#define VSCRIPT_EDITOR_H

#include "scene/gui/graph_edit.h"
#include "scene/gui/box_container.h"
#include "tools/editor/editor_node.h"
#include "vscript.h"

class VScriptEditorView : public Node {
	OBJ_TYPE(VScriptEditorView, Node);

	// GUI
	GraphEdit* m_graph_edit;
	Label* m_status;
	Map<int, GraphNode*> m_node_map;
	bool m_block_update;

	Ref<VScript> m_script;

	void _update_graph();
	void _create_node(int p_id);

	// Event Handlers
	void _node_removed(int p_id);
	void _node_moved(const Vector2& p_from, const Vector2& p_to, int p_id);
	void _move_node(int p_id, const Vector2& p_to);

	void _connection_request(const String& p_from, int p_from_slot, const String& p_to, int p_to_slot);
	void _disconnection_request(const String& p_from, int p_from_slot, const String& p_to, int p_to_slot);

	void _script_updated();

	// Node type Event Handlers
	void _notification_event_changed(double p_value, int p_id);
	void _function_event_changed(const String& text, int p_id);
	void _bool_const_changed(bool p_pressed, int p_id);
	void _int_const_changed(double p_value, int p_id);
	void _float_const_changed(double p_value, int p_id);
	void _string_const_changed(int p_id, Node* p_text_edit);
	void _comment_edited(int p_id, Node* p_text_edit);

protected:

	void _notification(int p_what);
	static void _bind_methods();

public:

	void add_node(int p_type);
	GraphEdit* get_graph_edit() { return m_graph_edit; }
	void set_script(Ref<VScript> p_script);

	VScriptEditorView();
};

class VScriptEditor : public VBoxContainer {
	OBJ_TYPE(VScriptEditor, VBoxContainer);
private:

	EditorNode* m_editor;
	MenuButton* m_menu;
	VScriptEditorView* m_graph_editor;

	static const char* node_names[VScript::NODE_TYPE_MAX];

	void _add_node(int p_type);

	void set_enable(bool enable);

protected:

	void _notification(int p_what);
	static void _bind_methods();

public:

	void edit(Ref<VScript> p_script);

	VScriptEditor(EditorNode* p_editor = NULL);
};

#endif // VSCRIPT_EDITOR_H
