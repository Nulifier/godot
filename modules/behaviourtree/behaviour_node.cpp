#include "behaviour_node.h"
#include "behaviour_tree.h"

/// Used to sort children in BehaviourNodeComposite.
struct BehaviourNodeCompare {
	BehaviourTree* tree;

	inline bool operator()(int lhs_id, int rhs_id) const
	{
		Ref<BehaviourNode> lhs = tree->get_node(lhs_id);
		Ref<BehaviourNode> rhs = tree->get_node(rhs_id);
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

void BehaviourNode::_set_tree(BehaviourTree* p_tree)
{
	m_tree.set_obj(p_tree);
}

void BehaviourNode::_set_id(int p_id)
{
	m_id = p_id;
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

	BIND_CONSTANT(INVALID_ID);
}

BehaviourNode::ReturnCode BehaviourNode::execute(Ref<BehaviourTreeInstance> m_instance)
{
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

BehaviourTree* BehaviourNode::get_tree() const
{
	Variant ref = m_tree.get_ref();
	ERR_FAIL_COND_V(ref.get_type() == Variant::NIL, NULL);
	Ref<BehaviourTree> tree = ref;
	return tree.ptr();
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
	: m_id(INVALID_ID)
{
}

//////////////////////////////////////////////////////////////////////////

void BehaviourNodeComposite::_set_children(const Array& p_children)
{
	m_stop_sort = true;
	clear_children();
	for (int i = 0; i < p_children.size(); ++i) {
		add_child(p_children[i]);
	}
	m_stop_sort = false;
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
	if (!m_stop_sort) {
		// Get and check the size of the children list
		int len = m_children.size();
		if (len == 0)
			return;

		// Get the data pointer
		int* data = &m_children[0];

		// Setup the sorter
		SortArray<int, BehaviourNodeCompare> sorter;
		sorter.compare.tree = get_tree();

		// Sort
		sorter.sort(data, len);
	}
}

void BehaviourNodeComposite::_bind_methods()
{
	ObjectTypeDB::bind_method(_MD("_set_children", "children"), &BehaviourNodeComposite::_set_children);
	ObjectTypeDB::bind_method(_MD("_get_children"), &BehaviourNodeComposite::_get_children);
	ObjectTypeDB::bind_method(_MD("_sort_children"), &BehaviourNodeComposite::_sort_children);

	ObjectTypeDB::bind_method(_MD("add_child", "child_id"), &BehaviourNodeComposite::add_child);
	ObjectTypeDB::bind_method(_MD("remove_child", "child_id"), &BehaviourNodeComposite::remove_child);
	ObjectTypeDB::bind_method(_MD("has_child", "child_id"), &BehaviourNodeComposite::has_child);
	ObjectTypeDB::bind_method(_MD("get_child", "index"), &BehaviourNodeComposite::get_child);
	ObjectTypeDB::bind_method(_MD("get_num_children"), &BehaviourNodeComposite::get_num_children);

	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "_children", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR), _SCS("_set_children"), _SCS("_get_children"));
}

void BehaviourNodeComposite::add_child(int p_child_id)
{
	ERR_FAIL_COND(has_child(p_child_id));

	m_children.push_back(p_child_id);
	
	// Connect to the child's postion_changed signal so we can resort when the position changes
	get_tree()->get_node(p_child_id)->connect("position_changed", this, "_sort_children");

	// Resort the children
	_sort_children();
}

void BehaviourNodeComposite::remove_child(int p_child_id)
{
	// Find child
	for (int i = 0; i < m_children.size(); ++i) {
		if (m_children[i] == p_child_id) {
			m_children.remove(i);

			// Disconnect from the child's position_changed signal
			get_tree()->get_node(p_child_id)->disconnect("position_changed", this, "_sort_children");
			break;
		}
	}
}

bool BehaviourNodeComposite::has_child(int p_child_id) const
{
	for (int i = 0; i < m_children.size(); ++i) {
		if (m_children[i] == p_child_id) {
			return true;
		}
	}
	return false;
}

void BehaviourNodeComposite::clear_children()
{
	// Disconnect from all of their position_changed signals.
	BehaviourTree* tree = get_tree();
	for (int i = 0; i < m_children.size(); ++i) {
		tree->get_node(m_children[i])->disconnect("position_changed", this, "_sort_children");
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

BehaviourNodeComposite::BehaviourNodeComposite()
	: m_stop_sort(false)
{
}

//////////////////////////////////////////////////////////////////////////

void BehaviourNodeDecorator::_bind_methods()
{
	ObjectTypeDB::bind_method(_MD("set_child_id", "child_id"), &BehaviourNodeDecorator::set_child_id);
	ObjectTypeDB::bind_method(_MD("get_child_id"), &BehaviourNodeDecorator::get_child_id);
	ObjectTypeDB::bind_method(_MD("get_child:BehaviourNode"), &BehaviourNodeDecorator::get_child);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "_child_id", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR), _SCS("set_child_id"), _SCS("get_child_id"));
}

void BehaviourNodeDecorator::set_child_id(int p_child_id)
{
	m_child_id = p_child_id;
}

int BehaviourNodeDecorator::get_child_id() const
{
	return m_child_id;
}

Ref<BehaviourNode> BehaviourNodeDecorator::get_child() const
{
	ERR_FAIL_COND_V(m_child_id == INVALID_ID, Ref<BehaviourNode>());
	return get_tree()->get_node(m_child_id);
}

BehaviourNodeDecorator::BehaviourNodeDecorator()
	: m_child_id(INVALID_ID)
{
}

//////////////////////////////////////////////////////////////////////////

BehaviourNode::ReturnCode BehaviourNodeRoot::tick(Ref<BehaviourTreeInstance> p_instance) const
{
	Ref<BehaviourNode> child = get_child();

	if (child.is_null())
		return RET_ERROR;

	return child->execute(p_instance);
}
