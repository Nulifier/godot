#include "vscript.h"
#include "vscript_instance.h"
#include "vscript_language.h"
#include "scene/scene_string_names.h"

void VScript::get_node_list(List<int>* p_node_list) const
{
	// Copy list of node IDs into list
	Map<int, Node>::Element* element = m_node_map.front();
	while (element) {
		p_node_list->push_back(element->key());
		element = element->next();
	}
}

VScript::NodeType VScript::node_get_type(int p_id) const
{
	ERR_FAIL_COND_V(!m_node_map.has(p_id), NODE_TYPE_MAX);
	return m_node_map[p_id].type;
}

void VScript::node_add(NodeType p_node_type, int p_id)
{
	ERR_FAIL_COND(p_id == 0);
	ERR_FAIL_COND(m_node_map.has(p_id));
	ERR_FAIL_INDEX(p_node_type, NODE_TYPE_MAX);

	// Check to see if creating this node type is allowed (all are allowed so far)

	Node node;

	// Check to see if this is a node that only one can exist of (none so far)

	node.type = p_node_type;
	node.id = p_id;

	// Initialize the node with default values here
	switch (p_node_type)
	{
	case NODE_NOTIFICATION_EVENT:
		node.param1 = -1;	// Notification being watched
		break;
	case NODE_FUNCTION_EVENT:
		node.param1 = "";	// Function name
		break;
	case NODE_BOOL_CONST:
		node.param1 = false;// Constant value
		break;
	case NODE_INT_CONST:
		node.param1 = 0;	// Constant value
		break;
	case NODE_FLOAT_CONST:
		node.param1 = 0.0;	// Constant value
		break;
	case NODE_STRING_CONST:
		node.param1 = "";	// Constant value
		break;
	case NODE_PRINT:
		break;
	case NODE_COMMENT:
		break;
	case NODE_TYPE_MAX:
		break;
	}

	m_node_map[p_id] = node;
	_request_update();
}

void VScript::node_remove(int p_id)
{
	ERR_FAIL_COND(p_id == 0);
	ERR_FAIL_COND(!m_node_map.has(p_id));

	// Erase connections
	for (Map<int, Node>::Element* element = m_node_map.front(); element; element = element->next()) {
		if (element->key() == p_id);
			continue; // We don't care about self connections as the list will be removed

			for (Map<int, SourceSlot>::Element* connection = element->get().connections.front(); connection;) {
				Map<int, SourceSlot>::Element* next = connection->next();

				if (connection->get().id == p_id) {
					element->get().connections.erase(connection);
				}

				connection = next;
			}
	}

	// Erase the node
	m_node_map.erase(p_id);

	_request_update();
}

void VScript::node_set_pos(int p_id, const Point2& p_pos)
{
	ERR_FAIL_COND(!m_node_map.has(p_id));
	m_node_map[p_id].pos = p_pos;
	_request_update();
}

Point2 VScript::node_get_pos(int p_id) const
{
	ERR_FAIL_COND_V(!m_node_map.has(p_id), Vector2());
	return m_node_map[p_id].pos;
}

// This is just to save some typing
#define CHECK_GET_NODE(id, req_type) \
	ERR_FAIL_COND(!m_node_map.has(id)); \
	Node& n = m_node_map[(id)]; \
	ERR_FAIL_COND(n.type != (req_type))
#define CHECK_GET_NODE_V(id, req_type, def_val) \
	ERR_FAIL_COND_V(!m_node_map.has(id), def_val); \
	const Node& n = m_node_map[(id)]; \
	ERR_FAIL_COND_V(n.type != (req_type), def_val)

void VScript::notification_event_node_set_value(int p_id, int p_notification)
{
	CHECK_GET_NODE(p_id, NODE_NOTIFICATION_EVENT);
	n.param1 = p_notification;
	_request_update();
}

int VScript::notification_event_node_get_value(int p_id) const
{
	CHECK_GET_NODE_V(p_id, NODE_NOTIFICATION_EVENT, 0);
	return n.param1;
}

void VScript::function_event_node_set_value(int p_id, const String& p_func_name)
{
	CHECK_GET_NODE(p_id, NODE_FUNCTION_EVENT);
	n.param1 = p_func_name;
}

String VScript::function_event_node_get_value(int p_id) const
{
	CHECK_GET_NODE_V(p_id, NODE_FUNCTION_EVENT, String());
	return n.param1;
}

void VScript::bool_const_node_set_value(int p_id, bool p_value)
{
	CHECK_GET_NODE(p_id, NODE_BOOL_CONST);
	n.param1 = p_value;
	_request_update();
}

bool VScript::bool_const_node_get_value(int p_id) const
{
	CHECK_GET_NODE_V(p_id, NODE_BOOL_CONST, 0);
	return n.param1;
}

void VScript::int_const_node_set_value(int p_id, int p_value)
{
	CHECK_GET_NODE(p_id, NODE_INT_CONST);
	n.param1 = p_value;
	_request_update();
}

int VScript::int_const_node_get_value(int p_id) const
{
	CHECK_GET_NODE_V(p_id, NODE_INT_CONST, 0);
	return n.param1;
}

void VScript::float_const_node_set_value(int p_id, double p_value)
{
	CHECK_GET_NODE(p_id, NODE_FLOAT_CONST);
	n.param1 = p_value;
	_request_update();
}

double VScript::float_const_node_get_value(int p_id) const
{
	CHECK_GET_NODE_V(p_id, NODE_FLOAT_CONST, 0.0);
	return n.param1;
}

void VScript::string_const_node_set_value(int p_id, const String& p_string)
{
	CHECK_GET_NODE(p_id, NODE_STRING_CONST);
	n.param1 = p_string;
}

String VScript::string_const_node_get_value(int p_id) const
{
	CHECK_GET_NODE_V(p_id, NODE_STRING_CONST, String());
	return n.param1;
}

void VScript::comment_node_set_text(int p_id, const String& p_comment)
{
	CHECK_GET_NODE(p_id, NODE_COMMENT);
	n.param1 = p_comment;
}

String VScript::comment_node_get_text(int p_id) const
{
	CHECK_GET_NODE_V(p_id, NODE_COMMENT, String());
	return n.param1;
}

#undef CHECK_GET_NODE
#undef CHECK_GET_NODE_V

Error VScript::connect_node(int p_src_id, int p_src_slot, int p_dst_id, int p_dst_slot)
{
	ERR_FAIL_COND_V(p_src_id == p_dst_id, ERR_INVALID_PARAMETER);	// Can't connect to self
	ERR_FAIL_COND_V(!m_node_map.has(p_src_id), ERR_INVALID_PARAMETER);
	ERR_FAIL_COND_V(!m_node_map.has(p_dst_id), ERR_INVALID_PARAMETER);
	NodeType type_src = m_node_map[p_src_id].type;
	NodeType type_dst = m_node_map[p_dst_id].type;
	ERR_FAIL_INDEX_V(p_src_slot, get_node_output_count(type_src), ERR_INVALID_PARAMETER);
	ERR_FAIL_INDEX_V(p_dst_slot, get_node_input_count(type_dst), ERR_INVALID_PARAMETER);
	ERR_FAIL_COND_V(get_node_output_type(type_src, p_src_slot) != get_node_input_type(type_dst, p_dst_slot), ERR_INVALID_PARAMETER);

	SourceSlot ts;
	ts.id = p_src_id;
	ts.slot = p_src_slot;
	m_node_map[p_dst_id].connections[p_dst_slot] = ts;

	_request_update();

	return OK;
}

bool VScript::is_node_connected(int p_src_id, int p_src_slot, int p_dst_id, int p_dst_slot) const
{
	SourceSlot ts;
	ts.id = p_src_id;
	ts.slot = p_src_slot;
	return
		m_node_map.has(p_dst_id) &&
		m_node_map[p_dst_id].connections.has(p_dst_slot) &&
		m_node_map[p_dst_id].connections[p_dst_slot] == ts;
}

void VScript::disconnect_node(int p_src_id, int p_src_slot, int p_dst_id, int p_dst_slot)
{
	if (is_node_connected(p_src_id, p_src_slot, p_dst_id, p_dst_slot)) {
		m_node_map[p_dst_id].connections.erase(p_dst_slot);
		_request_update();
	}
}

void VScript::get_node_connections(List<Connection>* p_connections) const
{
	// Iterate through the node map and build a list of all connections
	for (const Map<int, Node>::Element* destination = m_node_map.front(); destination; destination = destination->next()) {
		for (const Map<int, SourceSlot>::Element* source = destination->get().connections.front(); source; source = source->next()) {
			Connection c;
			c.dst_id = destination->key();
			c.dst_slot = source->key();
			c.src_id = source->get().id;
			c.src_slot = source->get().slot;
			p_connections->push_back(c);
		}
	}
}

void VScript::clear()
{
	m_node_map.clear();
	_request_update();
}

Variant VScript::node_get_state(int p_id) const
{
	ERR_FAIL_COND_V(!m_node_map.has(p_id), Variant());
	const Node& n = m_node_map[p_id];
	Dictionary s;
	s["pos"] = n.pos;
	s["param1"] = n.param1;
	s["param2"] = n.param2;
	return s;
}

void VScript::node_set_state(int p_id, const Variant& p_state)
{
	ERR_FAIL_COND(!m_node_map.has(p_id));
	Node& n = m_node_map[p_id];
	Dictionary d = p_state;
	ERR_FAIL_COND(!d.has("pos"));
	ERR_FAIL_COND(!d.has("param1"));
	ERR_FAIL_COND(!d.has("param2"));
	n.pos = d["pos"];
	n.param1 = d["param1"];
	n.param2 = d["param2"];
}

VScript::GraphError VScript::get_graph_error() const
{
	return m_graph_error;
}

const VScript::NodeSlotInfo VScript::node_slot_info[NODE_TYPE_MAX] = {
	// { Type, { ins }, { outs }, { in_names }, { out_names} },
	{ NODE_NOTIFICATION_EVENT, { SLOT_MAX }, { SLOT_EVENT, SLOT_MAX }, {}, { "Trigger" } },
	{ NODE_FUNCTION_EVENT, { SLOT_MAX }, { SLOT_EVENT, SLOT_MAX }, {}, { "Trigger" } },
	{ NODE_BOOL_CONST, { SLOT_MAX }, { SLOT_BOOL, SLOT_MAX }, {}, { "Value" } },
	{ NODE_INT_CONST, { SLOT_MAX }, { SLOT_INT, SLOT_MAX }, {}, { "Value" } },
	{ NODE_FLOAT_CONST, { SLOT_MAX }, { SLOT_FLOAT, SLOT_MAX }, {}, { "Value" } },
	{ NODE_STRING_CONST, { SLOT_MAX }, { SLOT_STRING, SLOT_MAX }, {}, { "Value" } },
	{ NODE_PRINT, { SLOT_EVENT, SLOT_STRING, SLOT_MAX }, { SLOT_MAX }, { "Print", "Message" }, {} },
	{ NODE_COMMENT, { SLOT_MAX }, { SLOT_MAX }, {}, {} },
};

int VScript::get_node_input_count(NodeType p_type)
{
	ERR_FAIL_INDEX_V(p_type, NODE_TYPE_MAX, 0);
	const NodeSlotInfo& nsi = node_slot_info[p_type];
	int count = 0;
	for (int i = 0; i < NodeSlotInfo::MAX_INS; ++i) {
		if (nsi.ins[i] == SLOT_MAX)
			break;
		++count;
	}
	return count;
}

int VScript::get_node_output_count(NodeType p_type)
{
	ERR_FAIL_INDEX_V(p_type, NODE_TYPE_MAX, 0);
	const NodeSlotInfo& nsi = node_slot_info[p_type];
	int count = 0;
	for (int i = 0; i < NodeSlotInfo::MAX_OUTS; ++i) {
		if (nsi.outs[i] == SLOT_MAX)
			break;
		++count;
	}
	return count;
}

VScript::SlotType VScript::get_node_input_type(NodeType p_type, int p_idx)
{
	ERR_FAIL_INDEX_V(p_type, NODE_TYPE_MAX, SLOT_MAX);
	const NodeSlotInfo& nsi = node_slot_info[p_type];
	for (int i = 0; i < NodeSlotInfo::MAX_INS; ++i) {
		if (nsi.ins[i] == SLOT_MAX)
			break;
		if (i == p_idx)
			return nsi.ins[i];
	}

	ERR_FAIL_V(SLOT_MAX);
}

VScript::SlotType VScript::get_node_output_type(NodeType p_type, int p_idx)
{
	ERR_FAIL_INDEX_V(p_type, NODE_TYPE_MAX, SLOT_MAX);
	const NodeSlotInfo& nsi = node_slot_info[p_type];
	for (int i = 0; i < NodeSlotInfo::MAX_OUTS; ++i) {
		if (nsi.outs[i] == SLOT_MAX)
			break;
		if (i == p_idx)
			return nsi.outs[i];
	}

	ERR_FAIL_V(SLOT_MAX);
}

const char* VScript::get_node_input_name(NodeType p_type, int p_idx)
{
	// ERR_FAIL_INDEX_V(p_type, NODE_TYPE_MAX, SLOT_MAX);	// Taken care of in get_node_input_slot_count
	ERR_FAIL_INDEX_V(p_idx, get_node_input_count(p_type), "");

	return node_slot_info[p_type].in_names[p_idx];
}

const char* VScript::get_node_output_name(NodeType p_type, int p_idx)
{
	// ERR_FAIL_INDEX_V(p_type, NODE_TYPE_MAX, SLOT_MAX);	// Taken care of in get_node_output_slot_count
	ERR_FAIL_INDEX_V(p_idx, get_node_output_count(p_type), "");

	return node_slot_info[p_type].out_names[p_idx];
}

Array VScript::_get_node_list() const
{
	List<int> nodes;
	get_node_list(&nodes);
	Array arr(true);
	for (List<int>::Element* element = nodes.front(); element; element = element->next())
		arr.push_back(element->get());
	return arr;
}

Array VScript::_get_node_connections() const
{
	List<Connection> connections;
	get_node_connections(&connections);
	Array arr(true);
	for (List<Connection>::Element* element = connections.front(); element; element = element->next()) {
		Dictionary d(true);
		d["src_id"] = element->get().src_id;
		d["src_slot"] = element->get().src_slot;
		d["dst_id"] = element->get().dst_id;
		d["dst_slot"] = element->get().dst_slot;
		arr.push_back(d);
	}
	return arr;
}

void VScript::_update_script()
{
	// Iterate through all nodes, looking for Event nodes
	for (Map<int, Node>::Element* element = m_node_map.front(); element; element = element->next()) {
		NodeType type = element->get().type;
		if (type == NODE_NOTIFICATION_EVENT) {

		}
		else if (type == NODE_FUNCTION_EVENT) {

		}
	}



	m_needs_update = false;
	emit_signal(SceneStringNames::get_singleton()->updated);
}

void VScript::_request_update()
{
	if (m_needs_update)
		return;

	m_needs_update = true;
	call_deferred("_update_script");
}

void VScript::_set_data(const Dictionary& p_data)
{
	ERR_FAIL_COND(!p_data.has("nodes"));

	const Array& nodes = p_data["nodes"];
	ERR_FAIL_COND((nodes.size() % 6) != 0);

	m_node_map.clear();
	for (int i = 0; i < nodes.size(); i += 6) {
		Node n;
		n.id = nodes[i + 0];
		n.type = NodeType(int(nodes[i + 1]));
		n.pos = nodes[i + 2];
		n.param1 = nodes[i + 3];
		n.param2 = nodes[i + 4];

		Array conns = nodes[i + 5];
		ERR_FAIL_COND((conns.size() % 3) != 0);

		for (int j = 0; j < conns.size(); j += 3) {
			SourceSlot ss;
			int ls = conns[j + 0];
			ss.id = conns[j + 1];
			ss.slot = conns[j + 2];
			n.connections[ls] = ss;
		}

		m_node_map[n.id] = n;
	}

	_update_script();
}

Dictionary VScript::_get_data() const
{
	Array nodes;
	int node_count = m_node_map.size();
	nodes.resize(node_count * 6);

	int idx = 0;
	for (Map<int, Node>::Element* element = m_node_map.front(); element; element = element->next()) {

		// Load in node data
		nodes[idx + 0] = element->key();
		nodes[idx + 1] = element->get().type;
		nodes[idx + 2] = element->get().pos;
		nodes[idx + 3] = element->get().param1;
		nodes[idx + 4] = element->get().param2;

		// Load in connection data
		Array connections;
		connections.resize(element->get().connections.size() * 3);
		int idx2 = 0;
		for (Map<int, SourceSlot>::Element* connection = element->get().connections.front(); connection; connection = connection->next()) {
			connections[idx2 + 0] = connection->key();
			connections[idx2 + 1] = connection->get().id;
			connections[idx2 + 2] = connection->get().slot;
			idx2 += 3;
		}

		nodes[idx + 5] = connections;
		idx += 6;
	}

	Dictionary data;
	data["nodes"] = nodes;
	return data;
}

void VScript::_bind_methods()
{
	ObjectTypeDB::bind_method(_MD("_update_script"), &VScript::_update_script);

	// Node methods
	ObjectTypeDB::bind_method(_MD("get_node_list"), &VScript::_get_node_list);
	ObjectTypeDB::bind_method(_MD("node_get_type"), &VScript::node_get_type);

	ObjectTypeDB::bind_method(_MD("node_add", "node_type", "id"), &VScript::node_add);
	ObjectTypeDB::bind_method(_MD("node_remove", "id"), &VScript::node_remove);
	ObjectTypeDB::bind_method(_MD("node_set_pos", "id", "pos"), &VScript::node_set_pos);
	ObjectTypeDB::bind_method(_MD("node_get_pos", "id"), &VScript::node_get_pos);

	ObjectTypeDB::bind_method(_MD("notification_event_node_set_value", "id", "value"), &VScript::notification_event_node_set_value);
	ObjectTypeDB::bind_method(_MD("notification_event_node_get_value", "id"), &VScript::notification_event_node_get_value);
	ObjectTypeDB::bind_method(_MD("function_event_node_set_value", "id", "func_name"), &VScript::function_event_node_set_value);
	ObjectTypeDB::bind_method(_MD("function_event_node_get_value", "id"), &VScript::function_event_node_set_value);
	ObjectTypeDB::bind_method(_MD("bool_const_node_set_value", "id", "value"), &VScript::bool_const_node_set_value);
	ObjectTypeDB::bind_method(_MD("bool_const_node_get_value", "id"), &VScript::bool_const_node_get_value);
	ObjectTypeDB::bind_method(_MD("int_const_node_set_value", "id", "value"), &VScript::int_const_node_set_value);
	ObjectTypeDB::bind_method(_MD("int_const_node_get_value", "id"), &VScript::int_const_node_get_value);
	ObjectTypeDB::bind_method(_MD("float_const_node_set_value", "id", "value"), &VScript::float_const_node_set_value);
	ObjectTypeDB::bind_method(_MD("float_const_node_get_value", "id"), &VScript::float_const_node_get_value);
	ObjectTypeDB::bind_method(_MD("string_const_node_set_value", "id", "string"), &VScript::string_const_node_set_value);
	ObjectTypeDB::bind_method(_MD("string_const_node_get_value", "id"), &VScript::string_const_node_get_value);
	ObjectTypeDB::bind_method(_MD("comment_node_set_text", "id", "text"), &VScript::comment_node_set_text);
	ObjectTypeDB::bind_method(_MD("comment_node_get_text", "id"), &VScript::comment_node_get_text);

	ObjectTypeDB::bind_method(_MD("connect_node", "src_id", "src_slot", "dst_id", "dst_slot"), &VScript::connect_node);
	ObjectTypeDB::bind_method(_MD("is_node_connected", "src_id", "src_slot", "dst_id", "dst_slot"), &VScript::is_node_connected);
	ObjectTypeDB::bind_method(_MD("disconnect_node", "src_id", "src_slot", "dst_id", "dst_slot"), &VScript::disconnect_node);

	ObjectTypeDB::bind_method(_MD("get_node_connections"), &VScript::_get_node_connections);

	ObjectTypeDB::bind_method(_MD("clear"), &VScript::clear);

	ObjectTypeDB::bind_method(_MD("node_set_state", "id", "state"), &VScript::node_set_state);
	ObjectTypeDB::bind_method(_MD("node_get_state:var", "id"), &VScript::node_get_state);

	ObjectTypeDB::bind_method(_MD("_set_data", "data"), &VScript::_set_data);
	ObjectTypeDB::bind_method(_MD("_get_data"), &VScript::_get_data);

	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "_data", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR), _SCS("_set_data"), _SCS("_get_data"));

	// Constants
	BIND_CONSTANT(NODE_NOTIFICATION_EVENT);
	BIND_CONSTANT(NODE_FUNCTION_EVENT);
	BIND_CONSTANT(NODE_BOOL_CONST);
	BIND_CONSTANT(NODE_INT_CONST);
	BIND_CONSTANT(NODE_FLOAT_CONST);
	BIND_CONSTANT(NODE_STRING_CONST);
	BIND_CONSTANT(NODE_PRINT);
	BIND_CONSTANT(NODE_COMMENT);

	BIND_CONSTANT(NODE_TYPE_MAX);

	BIND_CONSTANT(SLOT_EVENT);
	BIND_CONSTANT(SLOT_BOOL);
	BIND_CONSTANT(SLOT_INT);
	BIND_CONSTANT(SLOT_FLOAT);
	BIND_CONSTANT(SLOT_STRING);
	BIND_CONSTANT(SLOT_MAX);

	BIND_CONSTANT(GRAPH_OK);

	// Signals
	ADD_SIGNAL(MethodInfo("updated"));
}

void VScript::set_path(const String& p_path)
{
	m_path = p_path;
}

bool VScript::can_instance() const
{
	// Based off of GDScript
	return m_is_valid || (!m_is_tool && !ScriptServer::is_scripting_enabled());
}

StringName VScript::get_instance_base_type() const
{
	/// @todo Implement for VScript once inheritance is implemented.
	return StringName();
}

ScriptInstance* VScript::instance_create(Object *p_this)
{
	// If this isn't a tool and scripting isn't enabled (ie. in Editor mode), then don't instance the script
	if (!m_is_tool && !ScriptServer::is_scripting_enabled()) {
#ifdef TOOLS_ENABLED
		/// @todo Create a placeholder VScriptInstance when placeholders exist.
		return NULL;
#else
		return NULL;
#endif
	}

	// Scripting is enabled or this is a tool

	// Create the script
	VScriptInstance* instance = memnew(VScriptInstance);
	instance->m_script = Ref<VScript>(this);
	instance->m_owner = p_this;
	instance->m_owner->set_script_instance(instance);
	// DO VARIOUS THINGS TO CONSTRUCT OBJECT

	// Initialize and Construct
	m_instances.insert(instance->m_owner);

	// Call initializer
	// Check for errors and erase if there are

	// Return the instance
	return instance;
}

bool VScript::instance_has(const Object* p_this) const
{
	return m_instances.has((Object*)p_this);
}

bool VScript::has_source_code() const
{
	return m_source != "";
}

String VScript::get_source_code() const
{
	return m_source;
}

void VScript::set_source_code(const String& p_code)
{
	/// @todo Set a dirty flag once we implement compiling or maybe just exporting
	m_source = p_code;
}

Error VScript::reload()
{

	return OK;
}

bool VScript::is_tool() const
{
	return m_is_tool;
}

String VScript::get_node_type() const
{
	// This is never called in the current implementation of godot
	print_line("VScript::get_node_type() was called");
	return "";
}

ScriptLanguage* VScript::get_language() const
{
	return VScriptLanguage::get_singleton();
}

VScript::VScript()
: m_is_valid(false), m_is_tool(false), m_needs_update(false), m_graph_error(GRAPH_OK)
{

}
