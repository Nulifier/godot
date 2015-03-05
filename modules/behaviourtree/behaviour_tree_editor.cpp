#include "behaviour_tree_editor.h"
#include "tools/editor/editor_node.h"

void BehaviourTreeEditorView::_update_graph()
{
	// We don't want to double update
	if (m_block_update)
		return;

	// Clean up so we can repopulate
	for (Map<int, GraphNode*>::Element* element = m_node_map.front(); element; element = element->next()) {
		memdelete(element->get());
	}
	m_node_map.clear();

	// Check if the tree is valid
	if (!m_tree.is_valid())
		return;

	// Get a list of all the nodes from the tree
	List<int> node_list;
	m_tree->get_node_list(&node_list);

	// Create the nodes
	for (List<int>::Element* element = node_list.front(); element; element = element->next()) {
		_create_node(element->get());
	}

	// Connect the nodes
	m_graph_edit->clear_connections();
	for (List<int>::Element* element = node_list.front(); element; element = element->next()) {
		BehaviourNode* node = m_tree->get_node(element->get());
		ERR_CONTINUE(!node);
		BehaviourNodeDecorator* decorator = node->cast_to<BehaviourNodeDecorator>();
		BehaviourNodeComposite* composite = node->cast_to<BehaviourNodeComposite>();

		if (decorator && decorator->get_child_id() != BehaviourNode::INVALID_ID) {
			int child_id = decorator->get_child_id();
			ERR_CONTINUE(!m_tree->has_node(child_id));
			m_graph_edit->connect_node(m_node_map[element->get()]->get_name(), 0, m_node_map[child_id]->get_name(), 0);
		}
		else if (composite) {
			for (int i = 0; i < composite->get_num_children(); ++i) {
				m_graph_edit->connect_node(m_node_map[element->get()]->get_name(), 0, m_node_map[composite->get_child_id(i)]->get_name(), 0);
			}
		}
	}
}

void BehaviourTreeEditorView::_create_node(int p_id)
{
	const Color slot_color[SLOT_MAX] = {
		Color(1, 1, 1),	// Parent
		Color(1, 0, 0),	// Child
		Color(0, 0, 1),	// Children
	};

	// Types of nodes we can create:
	// - Composites (Multiple child slot)
	// - Decorators (Only 1 child slot)
	// - Actions/Conditions (only 1 parent slot)
	// - Root Node (No parent slot)

	ERR_FAIL_COND(!m_tree->has_node(p_id));
	BehaviourNode* node = m_tree->get_node(p_id);

	GraphNode* gn = memnew(GraphNode);

	// Common settings
	String type = node->get_type();
	if (type.begins_with("BehaviourNode"))
		type = type.replace_first("BehaviourNode", "");
	gn->set_title(type);
	gn->set_show_close_button(true);

	// Add properties
	PropertyEditor* editor = memnew(PropertyEditor);
	editor->hide_top_label();
	editor->set_show_categories(false);
	editor->edit(node);
	editor->set_custom_minimum_size(Size2(200, 200));
	editor->set_undo_redo(EditorNode::get_singleton()->get_undo_redo());
	gn->add_child(editor);

	if (node->is_type(BehaviourNodeRoot::get_type_static())) {
		// Type is root
		// No parent, has one child, no close button
		gn->set_show_close_button(false);
		gn->set_slot(0, false, 0, Color(), true, SLOT_CHILDREN, slot_color[SLOT_CHILD]);
	}
	else if (node->is_type(BehaviourNodeComposite::get_type_static())) {
		// Type is Composite
		// Has parent, has many children
		gn->set_slot(0, true, SLOT_CHILDREN, slot_color[SLOT_PARENT], true, SLOT_CHILDREN, slot_color[SLOT_CHILDREN]);
	}
	else if (node->is_type(BehaviourNodeDecorator::get_type_static())) {
		// Type is Decorator
		// Has parent, has one child
		gn->set_slot(0, true, SLOT_CHILDREN, slot_color[SLOT_PARENT], true, SLOT_CHILDREN, slot_color[SLOT_CHILD]);
	}
	else if (node->is_type(BehaviourNodeLeaf::get_type_static())) {
		// Type is Leaf
		// Has parent, has no children
		gn->set_slot(0, true, SLOT_CHILDREN, slot_color[SLOT_PARENT], false, 0, Color());
	}
	else {
		// We shouldn't end up here
		ERR_FAIL();
	}

	gn->connect("dragged", this, "_node_moved", varray(p_id));
	gn->connect("close_request", this, "_node_removed", varray(p_id), CONNECT_DEFERRED);
	m_graph_edit->add_child(gn);
	m_node_map[p_id] = gn;
	gn->set_offset(node->get_position());
}

void BehaviourTreeEditorView::_node_removed(int p_id)
{
	ERR_FAIL_COND(!m_node_map.has(p_id));

	UndoRedo* ur = EditorNode::get_singleton()->get_undo_redo();
	ur->create_action("Remove Behaviour Tree Node");

	// It was a child, we need to handle that
	int parent_id = m_tree->get_node(p_id)->get_parent_id();

	if (parent_id != BehaviourNode::INVALID_ID) {
		ur->add_do_method(m_tree.ptr(), "disconnect_nodes", parent_id, p_id);
	}

	ur->add_do_method(m_tree.ptr(), "remove_node", p_id);
	ur->add_undo_method(m_tree.ptr(), "add_node_by_id", p_id, m_tree->get_node(p_id)->get_type());
	ur->add_undo_method(m_tree.ptr(), "set_node_state", p_id, m_tree->get_node(p_id)->get_state());

	if (parent_id != BehaviourNode::INVALID_ID) {
		ur->add_undo_method(m_tree.ptr(), "connect_nodes", parent_id, p_id);
	}

	ur->add_do_method(this, "_update_graph");
	ur->add_undo_method(this, "_update_graph");
	ur->commit_action();
}

void BehaviourTreeEditorView::_node_moved(const Vector2& p_from, const Vector2& p_to, int p_id)
{
	ERR_FAIL_COND(!m_node_map.has(p_id));
	UndoRedo* ur = EditorNode::get_singleton()->get_undo_redo();
	ur->create_action("Move Behaviour Tree Node");
	ur->add_do_method(this, "_move_node", p_id, p_to);
	ur->add_undo_method(this, "_move_node", p_id, p_from);
	ur->commit_action();
}

void BehaviourTreeEditorView::_move_node(int p_id, const Vector2& p_to)
{
	ERR_FAIL_COND(!m_node_map.has(p_id));
	//m_node_map[p_id]->set_offset(p_to);		// Move the graph node
	m_tree->get_node(p_id)->set_position(p_to);
}

void BehaviourTreeEditorView::_connection_request(const String& p_from, int p_from_slot, const String& p_to, int p_to_slot)
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

	// Check if these nodes take children, etc.
	// If the parent is a decorator and it already has a child, this new one replaces it
	// If the parent is a composite, add to the list of children.

	BehaviourNode* from_node = m_tree->get_node(from_idx);
	BehaviourNodeDecorator* from_node_decorator = from_node->cast_to<BehaviourNodeDecorator>();

	// To Undo:
	// Disconnect this connection
	// Reconnect previous connection
	//	Decorator:
	//	- Connect previous child
	//	Composite:
	//	- Do Nothing

	// To Do:
	// Disconnect previous connections
	//	Decorator:
	//	- Already done for us
	//	Composite:
	//	- Doesn't need to be done
	// Connect this connection

	ur->create_action("Connect Graph Nodes");
	ur->add_undo_method(m_tree.ptr(), "disconnect_nodes", from_idx, to_idx);
	if (from_node_decorator) {
		// The parent is a decorator node, we need to reconnect the old child
		if (from_node_decorator->get_child_id() != BehaviourNode::INVALID_ID) {
			ur->add_undo_method(m_tree.ptr(), "connect_nodes", from_idx, from_node_decorator->get_child_id());
		}
	}
	ur->add_do_method(m_tree.ptr(), "connect_nodes", from_idx, to_idx);
	ur->add_do_method(this, "_update_graph");
	ur->add_undo_method(this, "_update_graph");
	ur->commit_action();
}

void BehaviourTreeEditorView::_disconnection_request(const String& p_from, int p_from_slot, const String& p_to, int p_to_slot)
{
	print_line("Disconnect: " + p_from + ", to: " + p_to);

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

	// Do Methods:
	// -	Disconnect
	// Undo Methods:
	// -	Connect

	ur->create_action("Disconnect Graph Nodes");
	ur->add_do_method(m_tree.ptr(), "disconnect_nodes", from_idx, to_idx);
	ur->add_undo_method(m_tree.ptr(), "connect_nodes", from_idx, to_idx);
	ur->add_do_method(this, "_update_graph");
	ur->add_undo_method(this, "_update_graph");
	ur->commit_action();
}

void BehaviourTreeEditorView::_notification(int p_notification)
{
	if (p_notification == NOTIFICATION_ENTER_TREE) {
		// Connect to any child controls
	}
}

void BehaviourTreeEditorView::_bind_methods()
{
	ObjectTypeDB::bind_method(_MD("_update_graph"), &BehaviourTreeEditorView::_update_graph);
	ObjectTypeDB::bind_method(_MD("_node_removed", "id"), &BehaviourTreeEditorView::_node_removed);
	ObjectTypeDB::bind_method(_MD("_node_moved", "from", "to", "id"), &BehaviourTreeEditorView::_node_moved);
	ObjectTypeDB::bind_method(_MD("_move_node", "id", "to"), &BehaviourTreeEditorView::_move_node);
	ObjectTypeDB::bind_method("_connection_request", &BehaviourTreeEditorView::_connection_request);
	ObjectTypeDB::bind_method("_disconnection_request", &BehaviourTreeEditorView::_disconnection_request);
}

void BehaviourTreeEditorView::add_node(const String& p_node_type)
{
	List<int> existing;
	m_tree->get_node_list(&existing);

	// Try to find a starting point that isn't overlapping an existing node
	Vector2 init_offset(20, 20);
	while (true) {
		bool valid = true;
		for (List<int>::Element* element = existing.front(); element; element = element->next()) {
			Vector2 pos = m_tree->get_node(element->get())->get_position();
			// If they are overlapping, offset and go through the list again.
			if (init_offset == pos) {
				init_offset += Vector2(20, 20);
				valid = false;
				break;
			}
		}

		if (valid)
			break;
	}

	// Get the new id
	int new_id = m_tree->get_new_id();

	// Create the node
	UndoRedo* ur = EditorNode::get_singleton()->get_undo_redo();
	ur->create_action("Add Behaviour Tree Node");
	ur->add_do_method(m_tree.ptr(), "add_node_by_id", new_id, p_node_type);
	ur->add_do_method(m_tree.ptr(), "set_node_pos", new_id, init_offset);
	ur->add_undo_method(m_tree.ptr(), "remove_node", new_id);
	ur->add_do_method(this, "_update_graph");
	ur->add_undo_method(this, "_update_graph");
	ur->commit_action();
}

void BehaviourTreeEditorView::set_tree(Ref<BehaviourTree> p_tree)
{
	// Disconnect signals
	m_tree = p_tree;
	// Connect signals
	_update_graph();
	// _tree_updated -> to get any errors from script
}

BehaviourTreeEditorView::BehaviourTreeEditorView()
	: m_block_update(false)
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

void BehaviourTreeEditor::_enable(bool p_enable)
{
	m_create_node->set_disabled(!p_enable);
}

void BehaviourTreeEditor::_show_create_dialog()
{
	// Create the dialog if needed
	if (!m_create_dialog) {
		m_create_dialog = memnew(CreateDialog);
		m_create_dialog->set_base_type("BehaviourNode");
		m_create_dialog->connect("create", this, "_create_node");
		add_child(m_create_dialog);
	}

	m_create_dialog->popup_centered_ratio();
}

void BehaviourTreeEditor::_create_node()
{
	String node_type = m_create_dialog->get_selected_type();
	ERR_FAIL_COND(node_type.empty());

	m_graph_editor->add_node(node_type);
}

void BehaviourTreeEditor::_notification(int p_what)
{
	if (p_what == NOTIFICATION_ENTER_TREE) {
		// Add the node types to the menu
	}
}

void BehaviourTreeEditor::_bind_methods()
{
	ObjectTypeDB::bind_method(_MD("_show_create_dialog"), &BehaviourTreeEditor::_show_create_dialog);
	ObjectTypeDB::bind_method(_MD("_create_node"), &BehaviourTreeEditor::_create_node);
	ObjectTypeDB::bind_method(_MD("edit", "tree"), &BehaviourTreeEditor::edit);
}

void BehaviourTreeEditor::edit(Ref<BehaviourTree> p_tree)
{
	m_graph_editor->set_tree(p_tree);
	_enable(p_tree.is_valid());
}

BehaviourTreeEditor::BehaviourTreeEditor(EditorNode* p_node)
	: m_create_dialog(NULL)
{
	// Create the menu bar
	HBoxContainer* hbc = memnew(HBoxContainer);
	
	m_create_node = memnew(Button);
	m_create_node->set_text("Create");
	m_create_node->set_flat(true);
	m_create_node->connect("pressed", this, "_show_create_dialog");
	hbc->add_child(m_create_node);

	add_child(hbc);

	// Create the graph editor
	m_graph_editor = memnew(BehaviourTreeEditorView);
	add_child(m_graph_editor);

	// Create a nice border
	PanelContainer* pc = memnew(PanelContainer);
	pc->set_v_size_flags(SIZE_EXPAND_FILL);
	add_child(pc);

	/// @todo Move this into the right class
	m_graph_editor->get_graph_edit()->set_name("Behaviour Tree");
	pc->add_child(m_graph_editor->get_graph_edit());

	m_graph_editor->get_graph_edit()->connect("connection_request", m_graph_editor, "_connection_request");
	m_graph_editor->get_graph_edit()->connect("disconnection_request", m_graph_editor, "_disconnection_request");
	m_graph_editor->get_graph_edit()->set_right_disconnects(true);

	set_custom_minimum_size(Size2(100, 300));

	_enable(false);
}

//////////////////////////////////////////////////////////////////////////

void BehaviourTreePlugin::make_visible(bool p_visible)
{
	if (p_visible) {
		m_tree_editor->show();
	}
	else {
		m_tree_editor->hide();
	}
}

void BehaviourTreePlugin::edit(Object *p_object)
{
	m_tree_editor->edit(p_object->cast_to<BehaviourTree>());
}

bool BehaviourTreePlugin::handles(Object *p_object) const
{
	return p_object->is_type("BehaviourTree");
}

BehaviourTreePlugin::BehaviourTreePlugin(EditorNode* p_node)
	: m_editor(p_node), m_tree_editor(NULL)
{
	m_tree_editor = memnew(BehaviourTreeEditor(m_editor));
	m_editor->get_viewport()->add_child(m_tree_editor);
	m_tree_editor->set_area_as_parent_rect();
	m_tree_editor->hide();
}

BehaviourTreePlugin::~BehaviourTreePlugin()
{
}
