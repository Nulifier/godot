#include "vscript_editor.h"

//////////////////////////////////////////////////////////////////////////
// Editor View

void VScriptEditorView::_update_graph()
{
	// We don't want to double update
	if (m_block_update)
		return;

	// Clean up so we can repopulate
	for (Map<int, GraphNode*>::Element* element = m_node_map.front(); element; element = element->next()) {
		memdelete(element->get());
	}
	m_node_map.clear();

	// Check if our reference is good
	if (!m_script.is_valid())
		return;

	// Get a list of all the nodes from the script
	List<int> node_list;
	m_script->get_node_list(&node_list);

	// Create the nodes
	for (List<int>::Element* element = node_list.front(); element; element = element->next()) {
		_create_node(element->get());
	}

	// Create the connections
	m_graph_edit->clear_connections();

	List<VScript::Connection> connections;
	m_script->get_node_connections(&connections);
	for (List<VScript::Connection>::Element* element = connections.front(); element; element = element->next()) {

		// Make sure both the source and destination nodes exist
		ERR_CONTINUE(!m_node_map.has(element->get().src_id) || !m_node_map.has(element->get().dst_id));
		m_graph_edit->connect_node(m_node_map[element->get().src_id]->get_name(), element->get().src_slot, m_node_map[element->get().dst_id]->get_name(), element->get().dst_slot);
	}
}

void VScriptEditorView::_create_node(int p_id)
{
	GraphNode* gn = memnew(GraphNode);
	gn->set_show_close_button(true);

	// Colors of slots
	Color type_col[4] = {
		Color(0.9, 0.4, 1),
		Color(0.8, 1, 0.2),
		Color(1, 0.2, 0.2),
		Color(0, 1, 1)
	};

	// Switch based on node type
	switch (m_script->node_get_type(p_id)) {
	case VScript::NODE_COMMENT: {
		gn->set_title("Comment");
		TextEdit* te = memnew(TextEdit);
		te->set_custom_minimum_size(Size2(100, 100));
		gn->add_child(te);
		te->set_text(m_script->comment_node_get_text(p_id));
		te->connect("text_changed", this, "_comment_edited", varray(p_id, te));
	} break;
	}

	gn->connect("dragged", this, "_node_moved", varray(p_id));
	gn->connect("close_request", this, "_node_removed", varray(p_id), CONNECT_DEFERRED);
	m_graph_edit->add_child(gn);
	m_node_map[p_id] = gn;
	gn->set_offset(m_script->node_get_pos(p_id));
}

void VScriptEditorView::_node_removed(int p_id)
{
	ERR_FAIL_COND(!m_node_map.has(p_id));

	UndoRedo* ur = EditorNode::get_singleton()->get_undo_redo();
	ur->create_action("Remove VScript Graph Node");

	ur->add_do_method(m_script.ptr(), "node_remove", p_id);
	ur->add_undo_method(m_script.ptr(), "node_add", m_script->node_get_type(p_id), p_id);
	ur->add_undo_method(m_script.ptr(), "node_set_state", p_id, m_script->node_get_state(p_id));

	List<VScript::Connection> connections;
	m_script->get_node_connections(&connections);
	for (List<VScript::Connection>::Element* element = connections.front(); element; element = element->next()) {
		if (element->get().dst_id == p_id || element->get().src_id == p_id) {
			ur->add_undo_method(m_script.ptr(), "connect_node", element->get().src_id, element->get().src_slot, element->get().dst_id, element->get().dst_slot);
		}
	}

	ur->add_do_method(this, "_update_graph");
	ur->add_undo_method(this, "_update_graph");
	ur->commit_action();
}

void VScriptEditorView::_node_moved(const Vector2& p_from, const Vector2& p_to, int p_id)
{
	ERR_FAIL_COND(!m_node_map.has(p_id));
	UndoRedo* ur = EditorNode::get_singleton()->get_undo_redo();
	ur->create_action("Move VSCript Graph Node");
	ur->add_do_method(this, "_move_node", p_id, p_to);
	ur->add_undo_method(this, "_move_node", p_id, p_from);
	ur->commit_action();
}

void VScriptEditorView::_move_node(int p_id, const Vector2& p_to)
{
	ERR_FAIL_COND(!m_node_map.has(p_id));
	m_node_map[p_id]->set_offset(p_to);
	m_script->node_set_pos(p_id, p_to);
}

void VScriptEditorView::_connection_request(const String& p_from, int p_from_slot, const String& p_to, int p_to_slot)
{
	UndoRedo* ur = EditorNode::get_singleton()->get_undo_redo();

	// Find the node indices
	int from_idx = -1;
	int to_idx = -1;
	for (Map<int, GraphNode*>::Element* element = m_node_map.front(); element; element = element->next()) {
		if (p_from == element->get()->get_name())
			from_idx = element->key();
		if (p_to == element->get()->get_name())
			to_idx = element->key();
	}

	ERR_FAIL_COND(from_idx == -1);
	ERR_FAIL_COND(to_idx == -1);

	ur->create_action("Connect Graph Nodes");

	List<VScript::Connection> conns;
	m_script->get_node_connections(&conns);

	// Disconnect and reconnect dependencies
	ur->add_undo_method(m_script.ptr(), "disconnect_node", from_idx, p_from_slot, to_idx, p_to_slot);
	for (List<VScript::Connection>::Element* element = conns.front(); element; element = element->next()) {
		if (element->get().dst_id == to_idx && element->get().dst_slot == p_to_slot) {
			ur->add_do_method(m_script.ptr(), "disconnect_node", element->get().src_id, element->get().src_slot, element->get().dst_id, element->get().dst_slot);
			ur->add_undo_method(m_script.ptr(), "connect_node", element->get().src_id, element->get().src_slot, element->get().dst_id, element->get().dst_slot);
		}
	}

	ur->add_do_method(m_script.ptr(), "connect_node", from_idx, p_from_slot, to_idx, p_to_slot);
	ur->add_do_method(this, "_update_graph");
	ur->add_undo_method(this, "_update_graph");
	ur->commit_action();
}

void VScriptEditorView::_disconnection_request(const String& p_from, int p_from_slot, const String& p_to, int p_to_slot)
{
	UndoRedo* ur = EditorNode::get_singleton()->get_undo_redo();

	// Find the node indices
	int from_idx = -1;
	int to_idx = -1;
	for (Map<int, GraphNode*>::Element* element = m_node_map.front(); element; element = element->next()) {
		if (p_from == element->get()->get_name())
			from_idx = element->key();
		if (p_to == element->get()->get_name())
			to_idx = element->key();
	}

	ERR_FAIL_COND(from_idx == -1);
	ERR_FAIL_COND(to_idx == -1);

	if (!m_script->is_node_connected(from_idx, p_from_slot, to_idx, p_to_slot))
		return;	// Its not connected

	ur->create_action("Disconnect Graph Nodes");

	List<VScript::Connection> conns;
	m_script->get_node_connections(&conns);

	// Disconnect and reconnect dependencies
	ur->add_do_method(m_script.ptr(), "disconnect_node", from_idx, p_from_slot, to_idx, p_to_slot);
	ur->add_undo_method(m_script.ptr(), "connect_node", from_idx, p_from_slot, to_idx, p_to_slot);
	ur->add_do_method(this, "_update_graph");
	ur->add_undo_method(this, "_update_graph");
	ur->commit_action();
}

void VScriptEditorView::_script_updated()
{
	if (!m_script.is_valid()) {
		return;
	}

	switch (m_script->get_graph_error())
	{
	case VScript::GRAPH_OK:
		m_status->set_text("");
		break;
	}
}

void VScriptEditorView::_comment_edited(int p_id, Node* p_text_edit)
{
	UndoRedo* ur = EditorNode::get_singleton()->get_undo_redo();
	TextEdit* te = p_text_edit->cast_to<TextEdit>();
	ur->create_action("Change Comment", true);
	ur->add_do_method(m_script.ptr(), "comment_node_set_text", p_id, te->get_text());
	ur->add_undo_method(m_script.ptr(), "comment_node_set_text", p_id, m_script->comment_node_get_text(p_id));
	ur->add_do_method(this, "_update_graph");
	ur->add_undo_method(this, "_update_graph");
	m_block_update = true;
	ur->commit_action();
	m_block_update = false;
}

void VScriptEditorView::_notification(int p_what)
{
	if (p_what == NOTIFICATION_ENTER_TREE) {
		/// @todo Connect to any child controls
	}
}

void VScriptEditorView::_bind_methods()
{
	ObjectTypeDB::bind_method(_MD("_update_graph"), &VScriptEditorView::_update_graph);
	ObjectTypeDB::bind_method(_MD("_node_removed", "id"), &VScriptEditorView::_node_removed);
	ObjectTypeDB::bind_method(_MD("_node_moved", "from", "to", "id"), &VScriptEditorView::_node_moved);
	ObjectTypeDB::bind_method(_MD("_move_node", "id", "to"), &VScriptEditorView::_move_node);

	ObjectTypeDB::bind_method(_MD("_comment_edited"), &VScriptEditorView::_comment_edited);

	ObjectTypeDB::bind_method(_MD("_script_updated"), &VScriptEditorView::_script_updated);
}

void VScriptEditorView::add_node(int p_type)
{
	// Find new id for node
	List<int> existing;
	m_script->get_node_list(&existing);
	existing.sort();
	int new_id = 1;
	for (List<int>::Element* element = existing.front(); element; element = element->next()) {
		// If this is the last element or if the element id after this one is not one more than this one (ie. a gap) then use the next id
		if (!element->next() || (element->get() + 1 != element->next()->get())) {
			new_id = element->get() + 1;
			break;
		}
	}

	// Try to find a starting position thats not overlapping an existing node
	Vector2 init_offset(20, 20);
	while (true) {
		bool valid = true;
		for (List<int>::Element* element = existing.front(); element; element = element->next()) {
			Vector2 pos = m_script->node_get_pos(element->get());
			// If they are overlapping, offset again and go through the list again
			if (init_offset == pos) {
				init_offset += Vector2(20, 20);
				valid = false;
				break;
			}
		}

		if (valid)
			break;
	}

	// Create the node
	UndoRedo* ur = EditorNode::get_singleton()->get_undo_redo();
	ur->create_action("Add VScript Graph Node");
	ur->add_do_method(m_script.ptr(), "node_add", p_type, new_id);
	ur->add_do_method(m_script.ptr(), "node_set_pos", new_id, init_offset);
	ur->add_undo_method(m_script.ptr(), "node_remove", new_id);
	ur->add_do_method(this, "_update_graph");
	ur->add_undo_method(this, "_update_graph");
	ur->commit_action();
}

void VScriptEditorView::set_script(Ref<VScript> p_script)
{
	// Disconnect from the updated signal of the old one and connect to the new one
	if (m_script.is_valid()) {
		m_script->disconnect("updated", this, "_script_updated");
	}
	m_script = p_script;
	if (m_script.is_valid()) {
		m_script->connect("updated", this, "_script_updated");
	}
	_update_graph();
	_script_updated();
}

VScriptEditorView::VScriptEditorView()
: m_graph_edit(NULL), m_status(NULL), m_block_update(false)
{
	// Create the graph editor
	m_graph_edit = memnew(GraphEdit);

	// Create the status displayer
	m_status = memnew(Label);
	m_graph_edit->get_top_layer()->add_child(m_status);
	m_status->set_pos(Vector2(5, 5));
	m_status->add_color_override("font_color_shadow", Color(0, 0, 0));
	m_status->add_color_override("font_color", Color(1, 0.4, 0.3));
	m_status->add_constant_override("shadow_as_outline", 1);
	m_status->add_constant_override("shadow_offset_x", 2);
	m_status->add_constant_override("shadow_offset_y", 2);
	m_status->set_text("");
}

//////////////////////////////////////////////////////////////////////////
// Editor

// The format for adding is: "IconName:VisibleName", end that string with a ':' if there should be a seperator after it
const char* VScriptEditor::node_names[VScript::NODE_TYPE_MAX] = {
	"GraphComment:Comment"
};

void VScriptEditor::_add_node(int p_type)
{
	m_graph_editor->add_node(p_type);
}

void VScriptEditor::set_enable(bool enable)
{
	if (enable) {
		m_menu->set_disabled(false);
	}
	else {
		m_menu->set_disabled(true);
	}
}

void VScriptEditor::_notification(int p_what)
{
	if (p_what == NOTIFICATION_ENTER_TREE) {
		for (int i = 0; i < VScript::NODE_TYPE_MAX; ++i) {
			// Check if we are allowed creating the node (All are allowed so far)
			// if (i == NODE_NOT_ALLOWED)
			//     continue;

			String nn = node_names[i];
			String icon = nn.get_slice(":", 0);
			String visible = nn.get_slice(":", 1);

			bool add_seperator = false;
			if (nn.ends_with(":")) {
				add_seperator = true;
			}

			m_menu->get_popup()->add_icon_item(get_icon(icon, "EditorIcons"), visible, i);
			if (add_seperator)
				m_menu->get_popup()->add_separator();
		}

		m_menu->get_popup()->connect("item_pressed", this, "_add_node");
	}
}

void VScriptEditor::_bind_methods()
{
	ObjectTypeDB::bind_method(_MD("_add_node"), &VScriptEditor::_add_node);
}

void VScriptEditor::edit(Ref<VScript> p_script)
{
	m_graph_editor->set_script(p_script);
	set_enable(p_script.is_valid());
}

VScriptEditor::VScriptEditor(EditorNode* p_editor /*= NULL*/)
: m_editor(p_editor), m_menu(NULL), m_graph_editor(NULL)
{
	// Create the menu bar
	HBoxContainer* hbc = memnew(HBoxContainer);
	m_menu = memnew(MenuButton);
	m_menu->set_text("Add Node..");
	hbc->add_child(m_menu);
	add_child(hbc);

	// Create graph editor
	m_graph_editor = memnew(VScriptEditorView);
	add_child(m_graph_editor);

	// We need this to have a border
	PanelContainer* pc = memnew(PanelContainer);
	pc->set_v_size_flags(SIZE_EXPAND_FILL);
	add_child(pc);

	m_graph_editor->get_graph_edit()->set_name("VScript");
	pc->add_child(m_graph_editor->get_graph_edit());

	m_graph_editor->get_graph_edit()->connect("connection_request", m_graph_editor, "_connection_request");
	m_graph_editor->get_graph_edit()->connect("disconnection_request", m_graph_editor, "_disconnection_request");
	m_graph_editor->get_graph_edit()->set_right_disconnects(true);

	set_custom_minimum_size(Size2(100, 300));

	set_enable(false);
}
