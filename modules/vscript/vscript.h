#ifndef VSCRIPT_H
#define VSCRIPT_H

#include "script_language.h"

class VScript : public Script {
	OBJ_TYPE(VScript, Script);
	RES_BASE_EXTENSION("vs");

public:
	//////////////////////////////////////////////////////////////////////////
	// Node Graph Stuff

	// These are the different types of nodes supported
	// To add a node:
	// * Add it here
	// * Add it as a constant to _bind_methods
	// * Add it to VScript::node_add
	// * Add it to VScriptEditorView::_create_node
	// * Add it to VScriptEditor::node_names
	// * Add it to VScript::node_slot_info
	// If the node cannot be created:
	// * Add a check to  VScript::node_add
	// * Don't allow the name in VScriptEditor::_notification
	enum NodeType {
		NODE_COMMENT,
		NODE_TYPE_MAX
	};

	struct Connection {
		int src_id;
		int src_slot;
		int dst_id;
		int dst_slot;
	};

	// If you are adding a slot type that can take any input then fix the last check in VScript::connect_node
	enum SlotType {
		SLOT_MAX
	};

	enum SlotDir {
		SLOT_IN,
		SLOT_OUT
	};

	// If you're adding any error types, fix VScriptEditorView::_script_updated too
	enum GraphError {
		GRAPH_OK
	};

	void get_node_list(List<int>* p_node_list) const;
	NodeType node_get_type(int p_id) const;

	// Node manipulation
	void node_add(NodeType p_node_type, int p_id);
	void node_remove(int p_id);
	void node_set_pos(int p_id, const Point2& p_pos);
	Point2 node_get_pos(int p_id) const;

	// Type specific node manipulation
	void comment_node_set_text(int p_id, const String& p_comment);
	String comment_node_get_text(int p_id) const;

	// Connections
	Error connect_node(int p_src_id, int p_src_slot, int p_dst_id, int p_dst_slot);
	bool is_node_connected(int p_src_id, int p_src_slot, int p_dst_id, int p_dst_slot) const;
	void disconnect_node(int p_src_id, int p_src_slot, int p_dst_id, int p_dst_slot);

	void get_node_connections(List<Connection>* p_connections) const;

	void clear();

	Variant node_get_state(int p_id) const;
	void node_set_state(int p_id, const Variant& p_state);

	GraphError get_graph_error() const;	/// @todo Implement

	static int get_node_input_count(NodeType p_type);
	static int get_node_output_count(NodeType p_type);
	static SlotType get_node_input_type(NodeType p_type, int p_idx);
	static SlotType get_node_output_type(NodeType p_type, int p_idx);
	static const String& get_node_input_name(NodeType p_type, int p_idx);
	static const String& get_node_output_name(NodeType p_type, int p_idx);

private:

	struct SourceSlot {
		int id;
		int slot;
		bool operator==(const SourceSlot& p_slot) const {
			return id == p_slot.id && slot == p_slot.slot;
		}
	};

	struct Node {
		// If you add any parameters to this, make sure to add them to VScript::node_get_state and set_state
		Vector2 pos;
		NodeType type;
		Variant param1;
		Variant param2;
		int id;
		mutable int order;	// For sorting
		int sort_order;
		Map<int, SourceSlot> connections;	// This node is the destination as each slot can only have one source but can have multiple destinations
	};

	Map<int, Node> m_node_map;
	GraphError m_graph_error;

	struct NodeSlotInfo {
		enum { MAX_INS = 3, MAX_OUTS = 3 };
		NodeType type;
		const SlotType ins[MAX_INS];
		const SlotType outs[MAX_OUTS];
		const String in_names[MAX_INS];
		const String out_names[MAX_OUTS];
	};

	static const NodeSlotInfo node_slot_info[NODE_TYPE_MAX];

	Array _get_node_list() const;
	Array _get_node_connections() const;

private:
	//////////////////////////////////////////////////////////////////////////
	// Script Stuff

	bool m_is_valid;
	bool m_is_tool;

	Set<Object*> m_instances;

	String m_path;
	String m_source;

	Map<StringName, Variant> m_constants;

	bool m_needs_update;
	void _update_script();
	void _request_update();

	void _set_data(const Dictionary& p_data);
	Dictionary _get_data() const;

protected:

	static void _bind_methods();

public:

	void set_path(const String& p_path);


	/// Returns true if this script can has instances of it created, false if not.
	bool can_instance() const;

	/// Returns the type that this script inherits from.
	StringName get_instance_base_type() const;

	/// Create an instance of this script
	ScriptInstance* instance_create(Object *p_this);

	/// Return true if the supplied Object is an instance of this script.
	bool instance_has(const Object* p_this) const;

	/// Return true if this script has source code set.
	bool has_source_code() const;

	/// Gets the source code for this script.
	String get_source_code() const;

	/// Sets the source code for this script.
	void set_source_code(const String& p_code);

	/// Reloads this script from file.
	Error reload();

	/// Returns true if this script is to be run in the editor.
	bool is_tool() const;

	/// Returns the node type of this script.
	/// @note Not implemented by GDScript.
	String get_node_type() const;

	/// Returns the language used by this script.
	ScriptLanguage* get_language() const;
	//void update_exports() {}

	VScript();
};

VARIANT_ENUM_CAST(VScript::NodeType);
VARIANT_ENUM_CAST(VScript::SlotType);
VARIANT_ENUM_CAST(VScript::GraphError);

#endif // VSCRIPT_H
