#include "behaviour_tree.h"

int BehaviourTree::_get_next_id() const
{
	return m_next_id;
}

void BehaviourTree::_set_next_id(int p_id)
{
	m_next_id = p_id;
}

void BehaviourTree::_set_nodes(const Dictionary& nodes)
{
	for (const Variant* key = nodes.next(); key; key = nodes.next(key)) {
		m_node_map[*key] = nodes[key];
	}
}

Dictionary BehaviourTree::_get_nodes() const
{
	Dictionary nodes;
	for (Map<int, Ref<BehaviourNode>>::Element* e = m_node_map.front(); e; e = e->next()) {
		nodes[e->key()] = e->get();
	}
	return nodes;
}

void BehaviourTree::_bind_methods()
{
	ObjectTypeDB::bind_method(_MD("_set_next_id", "id"), &BehaviourTree::_set_next_id);
	ObjectTypeDB::bind_method(_MD("_get_next_id"), &BehaviourTree::_get_next_id);

	ObjectTypeDB::bind_method("_set_nodes", &BehaviourTree::_set_nodes);
	ObjectTypeDB::bind_method("_get_nodes", &BehaviourTree::_get_nodes);

	ObjectTypeDB::bind_method(_MD("get_new_id"), &BehaviourTree::get_new_id);
	ObjectTypeDB::bind_method(_MD("add_node", "node_type"), &BehaviourTree::add_node);
	ObjectTypeDB::bind_method(_MD("add_node_by_id", "new_id", "node_type"), &BehaviourTree::add_node_by_id);
	ObjectTypeDB::bind_method(_MD("get_node:BehaviourNode", "id"), &BehaviourTree::get_node);
	ObjectTypeDB::bind_method(_MD("has_node", "id"), &BehaviourTree::has_node);
	ObjectTypeDB::bind_method(_MD("remove_node", "id"), &BehaviourTree::remove_node);

	ObjectTypeDB::bind_method(_MD("set_root_id", "root_id"), &BehaviourTree::set_root_id);
	ObjectTypeDB::bind_method(_MD("get_root_id"), &BehaviourTree::get_root_id);

	ObjectTypeDB::bind_method(_MD("create_instance", "self"), &BehaviourTree::create_instance);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "_next_id", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR), _SCS("_set_next_id"), _SCS("_get_next_id"));
	ADD_PROPERTY(PropertyInfo(Variant::INT, "_root_id", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR), _SCS("set_root_id"), _SCS("get_root_id"));
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "_nodes", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR), _SCS("_set_nodes"), _SCS("_get_nodes"));
}

int BehaviourTree::get_new_id()
{
	return m_next_id++;
}

int BehaviourTree::add_node(const String& p_node_type)
{
	int id = get_new_id();
	add_node_by_id(id, p_node_type);
	return id;
}

void BehaviourTree::add_node_by_id(int p_new_id, const String& p_node_type)
{
	ERR_FAIL_COND(has_node(p_new_id));

	Object* c = ObjectTypeDB::instance(p_node_type);

	ERR_FAIL_COND(!c);
	BehaviourNode* node = c->cast_to<BehaviourNode>();
	ERR_FAIL_COND(!node);

	node->_set_id(p_new_id);
	node->_set_tree(this);

	m_node_map.insert(p_new_id, node);
}

BehaviourNode* BehaviourTree::get_node(int p_id)
{
	Map<int, Ref<BehaviourNode>>::Element* e = m_node_map.find(p_id);
	ERR_FAIL_COND_V(!e, NULL);
	return e->get().ptr();
}

bool BehaviourTree::has_node(int p_id) const
{
	return m_node_map.has(p_id);
}

void BehaviourTree::remove_node(int p_id)
{
	Map<int, Ref<BehaviourNode>>::Element* e = m_node_map.find(p_id);
	ERR_FAIL_COND(!e);
	m_node_map.erase(e);
}

void BehaviourTree::set_root_id(int p_id)
{
	ERR_FAIL_COND(!has_node(p_id));
	m_tree_root_id = p_id;
}

Ref<BehaviourTreeInstance> BehaviourTree::create_instance(Object* p_context)
{
	// Create an instance
	BehaviourTreeInstance* instance = memnew(BehaviourTreeInstance);

	// Set the context and the parent tree
	instance->m_context = p_context;
	instance->m_tree = Ref<BehaviourTree>(this);

	return instance;
}

BehaviourTree::BehaviourTree()
	: m_next_id(0), m_tree_root_id(BehaviourNode::INVALID_ID)
{
	// Create the root node
	set_root_id(add_node("BehaviourNodeRoot"));
}

//////////////////////////////////////////////////////////////////////////

const Variant BehaviourTreeInstance::nil;

void BehaviourTreeInstance::_enter_node(const BehaviourNode& m_node)
{
	// If we see it in this function it means that it was executed again this tick and isn't an orphan
	m_orphans.erase(&m_node);

	// call debug functions
}

void BehaviourTreeInstance::_open_node(const BehaviourNode& m_node)
{
	m_open_nodes.insert(&m_node);

	// call debug functions
}

void BehaviourTreeInstance::_tick_node(const BehaviourNode& m_node)
{
	// call debug functions
}

void BehaviourTreeInstance::_close_node(const BehaviourNode& m_node)
{
	// call debug functions

	m_open_nodes.erase(&m_node);
}

void BehaviourTreeInstance::_exit_node(const BehaviourNode& m_node)
{
	// call debug functions
}

bool BehaviourTreeInstance::is_node_open(const BehaviourNode& m_node) const
{
	return m_open_nodes.has(&m_node);
}

void BehaviourTreeInstance::_bind_methods()
{
	ObjectTypeDB::bind_method(_MD("execute"), &BehaviourTreeInstance::execute);

	ObjectTypeDB::bind_method(_MD("set_inst_value", "name:StringName", "value"), &BehaviourTreeInstance::set_inst_value);
	ObjectTypeDB::bind_method(_MD("get_inst_value", "name:StringName"), &BehaviourTreeInstance::get_inst_value);
	ObjectTypeDB::bind_method(_MD("clear_inst_value", "name:StringName"), &BehaviourTreeInstance::clear_inst_value);

	ObjectTypeDB::bind_method(_MD("set_node_value", "node_id", "name:StringName", "value"), &BehaviourTreeInstance::set_node_value);
	ObjectTypeDB::bind_method(_MD("get_node_value", "node_id", "name:StringName"), &BehaviourTreeInstance::get_node_value);
	ObjectTypeDB::bind_method(_MD("clear_node_value", "node_id", "name:StringName"), &BehaviourTreeInstance::clear_node_value);

	ObjectTypeDB::bind_method(_MD("get_tree:BehaviourTree"), &BehaviourTreeInstance::get_tree);
	ObjectTypeDB::bind_method(_MD("set_context", "context"), &BehaviourTreeInstance::set_context);
	ObjectTypeDB::bind_method(_MD("get_context"), &BehaviourTreeInstance::get_context);
}

void BehaviourTreeInstance::execute()
{
	int root_id = m_tree->get_root_id();
	if (m_tree->has_node(root_id)) {
		// Make a copy of last tick's nodes
		// We will remove the node from this copy each time we see it in the _enter() function.
		// This means its still being executed and isn't an orphan.
		m_orphans = m_open_nodes;

		// Execute the tree
		m_tree->get_node(root_id)->execute(this);

		// Close all the orphans
		for (Set<const BehaviourNode*>::Element* E = m_orphans.front(); E; E = E->next()) {
			E->get()->_close(this);
		}
		//m_orphans.clear(); // This isn't needed but if memory becomes an issue it could be
	}
}

void BehaviourTreeInstance::set_inst_value(const StringName& p_name, const Variant& p_value)
{
	m_inst_blackboard[p_name] = p_value;
}

const Variant& BehaviourTreeInstance::get_inst_value(const StringName& p_name) const
{
	// Check if the value exists
	const Map<StringName, Variant>::Element* e = m_inst_blackboard.find(p_name);
	if (e)
		return e->get();
	else
		return nil;
}

void BehaviourTreeInstance::clear_inst_value(const StringName& p_name)
{
	m_inst_blackboard.erase(p_name);
}

void BehaviourTreeInstance::set_node_value(int p_node_id, const StringName& p_name, const Variant& p_value)
{
	m_node_blackboard[p_node_id][p_name] = p_value;
}

const Variant& BehaviourTreeInstance::get_node_value(int p_node_id, const StringName& p_name) const
{
	// Check if the value exists
	const Map<int, Map<StringName, Variant>>::Element* e = m_node_blackboard.find(p_node_id);
	if (e) {
		const Map<StringName, Variant>::Element* f = e->get().find(p_name);
		if (f)
			return f->get();
	}

	return nil;
}

void BehaviourTreeInstance::clear_node_value(int p_node_id, const StringName& p_name)
{
	Map<int, Map<StringName, Variant>>::Element* e = m_node_blackboard.find(p_node_id);
	if (e) {
		e->get().erase(p_name);
	}
}

Ref<BehaviourTree> BehaviourTreeInstance::get_tree() const
{
	return m_tree;
}

void BehaviourTreeInstance::set_context(Object* p_owner)
{
	m_context = p_owner;
}

Object* BehaviourTreeInstance::get_context() const
{
	return m_context;
}

BehaviourTreeInstance::BehaviourTreeInstance()
	: m_context(NULL)
{
}
