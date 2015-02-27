#include "behaviour_node.h"
#include "behaviour_tree.h"

/// Used to sort children in BehaviourNodeComposite.
struct BehaviourNodeCompare {
	inline bool operator()(const Ref<BehaviourNode>& lhs, const Ref<BehaviourNode>& rhs) const
	{
		return lhs->get_position().y < rhs->get_position().y;
	}
};

void BehaviourNode::_enter(Ref<BehaviourTreeInstance> p_instance) const
{
	p_instance->_enter_node(*this);
	enter(p_instance);
}

void BehaviourNode::_open(Ref<BehaviourTreeInstance> p_instance) const
{
	p_instance->_open_node(*this);
	open(p_instance);
}

BehaviourNode::ReturnCode BehaviourNode::_tick(Ref<BehaviourTreeInstance> p_instance) const
{
	p_instance->_tick_node(*this);
	return tick(p_instance);
}

void BehaviourNode::_close(Ref<BehaviourTreeInstance> p_instance) const
{
	p_instance->_close_node(*this);
	close(p_instance);
}

void BehaviourNode::_exit(Ref<BehaviourTreeInstance> p_instance) const
{
	p_instance->_exit_node(*this);
	exit(p_instance);
}

void BehaviourNode::_set_id(int id)
{
	m_id = id;
}

void BehaviourNode::_bind_methods()
{
	ObjectTypeDB::bind_method(_MD("_set_id", "id"), &BehaviourNode::_set_id);
	ObjectTypeDB::bind_method(_MD("get_id"), &BehaviourNode::get_id);

	ObjectTypeDB::bind_method(_MD("set_position", "position"), &BehaviourNode::set_position);
	ObjectTypeDB::bind_method(_MD("get_position"), &BehaviourNode::get_position);

	ObjectTypeDB::bind_method(_MD("execute", "instance:BehaviourTreeInstance"), &BehaviourNode::execute);

	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "position"), _SCS("set_position"), _SCS("get_position"));
	ADD_PROPERTY(PropertyInfo(Variant::INT, "id", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR), _SCS("_set_id"), _SCS("get_id"));

	// Used by composite nodes to resort children.
	ADD_SIGNAL(MethodInfo("position_changed"));

	BIND_CONSTANT(RET_SUCCESS);
	BIND_CONSTANT(RET_FAILURE);
	BIND_CONSTANT(RET_RUNNING);
	BIND_CONSTANT(RET_ERROR);
}

BehaviourNode::ReturnCode BehaviourNode::execute(Ref<BehaviourTreeInstance> m_instance)
{
	// Get an id if we don't already have one
	if (m_id < 0) {
		m_id = m_instance->get_tree()->_get_new_id();
	}

	_enter(m_instance);
	
	// Open the node if it isn't already open
	if (!m_instance->is_node_open(*this)) {
		_open(m_instance);
	}

	ReturnCode status = _tick(m_instance);

	// If this node isn't still running, close it
	if (status != RET_RUNNING) {
		_close(m_instance);
	}

	_exit(m_instance);

	return status;
}

int BehaviourNode::get_id() const
{
	return m_id;
}

void BehaviourNode::set_position(const Vector2& p_position)
{
	m_position = p_position;
	emit_signal("position_changed");
}

const Vector2& BehaviourNode::get_position() const
{
	return m_position;
}

BehaviourNode::BehaviourNode()
	: m_id(-1)
{
}

//////////////////////////////////////////////////////////////////////////

void BehaviourNodeComposite::_set_children(const Array& p_children)
{
	m_children.clear();
	for (int i = 0; i < p_children.size(); ++i) {
		m_children.push_back(p_children[i]);
	}
	_sort_children();
}

Array BehaviourNodeComposite::_get_children() const
{
	Array arr;
	for (int i = 0; i < m_children.size(); ++i) {
		arr.push_back(m_children[i]);
	}
	return arr;
}

void BehaviourNodeComposite::_sort_children()
{
	m_children.sort_custom<BehaviourNodeCompare>();
}

void BehaviourNodeComposite::_bind_methods()
{
	ObjectTypeDB::bind_method(_MD("_set_children", "children"), &BehaviourNodeComposite::_set_children);
	ObjectTypeDB::bind_method(_MD("_get_children"), &BehaviourNodeComposite::_get_children);
	ObjectTypeDB::bind_method(_MD("_sort_children"), &BehaviourNodeComposite::_sort_children);

	ObjectTypeDB::bind_method(_MD("add_child", "child"), &BehaviourNodeComposite::add_child);
	ObjectTypeDB::bind_method(_MD("remove_child", "child"), &BehaviourNodeComposite::remove_child);
	ObjectTypeDB::bind_method(_MD("has_child", "child"), &BehaviourNodeComposite::has_child);
	ObjectTypeDB::bind_method(_MD("sort_children"), &BehaviourNodeComposite::_sort_children);
	ObjectTypeDB::bind_method(_MD("get_child", "index"), &BehaviourNodeComposite::get_child);
	ObjectTypeDB::bind_method(_MD("get_num_children"), &BehaviourNodeComposite::get_num_children);

	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "_children", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR), _SCS("_set_children"), _SCS("_get_children"));
}

void BehaviourNodeComposite::add_child(Ref<BehaviourNode> m_node)
{
	ERR_FAIL_COND(has_child(m_node));

	m_children.push_back(m_node);
	
	// Connect to the child's postion_changed signal so we can resort when the position changes
	m_node->connect("position_changed", this, "_sort_children");
}

void BehaviourNodeComposite::remove_child(Ref<BehaviourNode> m_node)
{
	// Find child
	for (int i = 0; i < m_children.size(); ++i) {
		if (m_children[i] == m_node) {
			m_children.remove(i);

			// Disconnect from the child's position_changed signal
			m_node->disconnect("position_changed", this, "_sort_children");
			break;
		}
	}
}

bool BehaviourNodeComposite::has_child(const Ref<BehaviourNode>& m_node) const
{
	for (int i = 0; i < m_children.size(); ++i) {
		if (m_children[i] == m_node) {
			return true;
		}
	}
	return false;
}

void BehaviourNodeComposite::clear_children()
{
	// Disconnect from all of their position_changed signals.
	for (int i = 0; i < m_children.size(); ++i) {
		m_children[i]->disconnect("position_changed", this, "_sort_children");
	}

	m_children.clear();
}

Ref<BehaviourNode> BehaviourNodeComposite::get_child(int idx) const
{
	ERR_FAIL_INDEX_V(idx, m_children.size(), Ref<BehaviourNode>());
	return m_children[idx];
}

int BehaviourNodeComposite::get_num_children() const
{
	return m_children.size();
}

//////////////////////////////////////////////////////////////////////////

void BehaviourNodeDecorator::_bind_methods()
{
	ObjectTypeDB::bind_method(_MD("set_child", "child:BehaviourNode"), &BehaviourNodeDecorator::set_child);
	ObjectTypeDB::bind_method(_MD("get_child:BehaviourNode"), &BehaviourNodeDecorator::get_child);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "child", PROPERTY_HINT_RESOURCE_TYPE, "BehaviourNode"), _SCS("set_child"), _SCS("get_child"));
}

void BehaviourNodeDecorator::set_child(const Ref<BehaviourNode>& p_child)
{
	m_child = p_child;
}

Ref<BehaviourNode> BehaviourNodeDecorator::get_child() const
{
	return m_child;
}

//////////////////////////////////////////////////////////////////////////

BehaviourNode::ReturnCode BehaviourNodeRoot::tick(Ref<BehaviourTreeInstance> p_instance) const
{
	Ref<BehaviourNode> child = get_child();

	if (child.is_null())
		return RET_ERROR;

	return child->execute(p_instance);
}
