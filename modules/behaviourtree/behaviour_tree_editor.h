#ifndef BEHAVIOUR_TREE_EDITOR_H
#define BEHAVIOUR_TREE_EDITOR_H

#include "tools/editor/editor_plugin.h"
#include "scene/gui/box_container.h"
#include "scene/gui/menu_button.h"
#include "scene/gui/graph_edit.h"
#include "scene/gui/button.h"
#include "tools/editor/create_dialog.h"
#include "behaviour_tree.h"

class BehaviourTreeEditorView : public Node {
	OBJ_TYPE(BehaviourTreeEditorView, Node);

	enum SlotType {
		SLOT_PARENT = 0,
		SLOT_CHILD,
		SLOT_CHILDREN,
		SLOT_MAX,
	};

	// Gui
	GraphEdit* m_graph_edit;
	Label* m_status;

	// Context
	Ref<BehaviourTree> m_tree;
	Map<int, GraphNode*> m_node_map;
	bool m_block_update;

	void _update_graph();
	void _create_node(int p_id);

	void _node_removed(int p_id);
	void _node_moved(const Vector2& p_from, const Vector2& p_to, int p_id);
	void _move_node(int p_id, const Vector2& p_to);

	void _connection_request(const String& p_from, int p_from_slot, const String& p_to, int p_to_slot);
	void _disconnection_request(const String& p_from, int p_from_slot, const String& p_to, int p_to_slot);

protected:

	void _notification(int p_notification);
	static void _bind_methods();

public:

	void add_node(const String& p_node_type);
	GraphEdit* get_graph_edit() { return m_graph_edit; }
	void set_tree(Ref<BehaviourTree> p_tree);

	BehaviourTreeEditorView();
};

//////////////////////////////////////////////////////////////////////////

class BehaviourTreeEditor : public VBoxContainer {
	OBJ_TYPE(BehaviourTreeEditor, VBoxContainer);

	EditorNode* m_editor;
	Button* m_create_node;
	CreateDialog* m_create_dialog;
	BehaviourTreeEditorView* m_graph_editor;

	void _enable(bool p_enable);
	void _show_create_dialog();
	void _create_node();

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:

	void edit(Ref<BehaviourTree> p_tree);

	BehaviourTreeEditor(EditorNode* p_node);
};

//////////////////////////////////////////////////////////////////////////

class BehaviourTreePlugin : public EditorPlugin {
	OBJ_TYPE(BehaviourTreePlugin, EditorPlugin);

	BehaviourTreeEditor* m_tree_editor;
	EditorNode* m_editor;

public:
	String get_name() const { return "Behaviour Tree"; }
	bool has_main_screen() const { return true; }
	void make_visible(bool p_visible);
	void edit(Object *p_object);
	bool handles(Object *p_object) const;

	BehaviourTreePlugin(EditorNode* p_node);
	~BehaviourTreePlugin();
};

#endif // BEHAVIOUR_TREE_EDITOR_H
