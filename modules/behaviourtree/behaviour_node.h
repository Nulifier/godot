#ifndef BEHAVIOUR_NODE_H
#define BEHAVIOUR_NODE_H

#include "resource.h"

class BehaviourTreeInstance;

/// The base class for all behaviour nodes.
class BehaviourNode : public Resource {
	OBJ_TYPE(BehaviourNode, Resource);
	friend class BehaviourTree;				// Calls _set_id()
	friend class BehaviourTreeInstance;		// Calls _close()
	friend class BehaviourNodeComposite;	// Calls _set_parent_id()
	friend class BehaviourNodeDecorator;	// Calls _set_parent_id()
public:
	/// The return codes from tick().
	enum ReturnCode {
		RET_SUCCESS,	///< The node's operation was a success.
		RET_FAILURE,	///< The node's operation failed.
		RET_RUNNING,	///< The node is still running.
		RET_ERROR		///< There was an error running the node. This means that the nodes were configured wrong.
	};

	static const int INVALID_ID = -1;

private:

	WeakRef m_tree;
	int m_id;					///< The id of this node.
	int m_parent;
	Vector2 m_position;			///< The position of this node in the graph. The y coordinate is also used for sorting children.

	void _enter(Ref<BehaviourTreeInstance> p_instance) const;
	void _open(Ref<BehaviourTreeInstance> p_instance) const;
	ReturnCode _tick(Ref<BehaviourTreeInstance> p_instance) const;
	void _close(Ref<BehaviourTreeInstance> p_instance) const;
	void _exit(Ref<BehaviourTreeInstance> p_instance) const;

	void _set_tree(BehaviourTree* p_tree);	///< Sets the parent tree, used by BehaviourTree
	void _set_id(int p_id);	///< Sets the id. Used by BehaviourTree and serialization.
	void _set_parent_id(int p_parent) { m_parent = p_parent; }

protected:

	static void _bind_methods();

	/// Called every time this node is executed, before the other functions.
	virtual void enter(Ref<BehaviourTreeInstance> p_instance) const {}

	/// Called when the node is opened. If the node returns RET_RUNNING, it will be kept open until it returns another value or the tree switched paths.
	virtual void open(Ref<BehaviourTreeInstance> p_instance) const {}

	/// Called every time this node is executed. This is the real implementation of the node. The return code will be passed up the tree.
	virtual ReturnCode tick(Ref<BehaviourTreeInstance> p_instance) const = 0;

	/// Called when the node does not return RET_RUNNING or when the tree switches paths and doesn't execute this node on an execute() call.
	virtual void close(Ref<BehaviourTreeInstance> p_instance) const {}

	/// Called every time this node is executed, after the other functions.
	virtual void exit(Ref<BehaviourTreeInstance> p_instance) const {}

public:

	/**	Gets the id of this node.
	 *	@note This is not initialized until the first execute() call.
	 */
	int get_id() const;

	/// Gets the parent id of this node or INVALID_ID if it doesn't have one.
	int get_parent_id() const { return m_parent; }

	BehaviourTree* get_tree() const;

	/**	Sets the position of this node in the graph.
	 *	@note The y coordinate is used for sorting children.
	 */
	void set_position(const Vector2& p_position);

	/// Gets the position of this node in the graph.
	const Vector2& get_position() const;

	/// Gets a copy of all the properties of this node for use in UndoRedo.
	Dictionary get_state() const;

	/// Sets all the properties of this node for use in UndoRedo.
	void set_state(const Dictionary& p_state);

	/**	Executes this node.
	 *	@param p_instance A reference to the instance that this node is in.
	 */
	ReturnCode execute(Ref<BehaviourTreeInstance> p_instance);

	BehaviourNode();
	virtual ~BehaviourNode() {}
};

//////////////////////////////////////////////////////////////////////////

/// A node with no children. This is used to implement actions and conditions.
class BehaviourNodeLeaf : public BehaviourNode {
	OBJ_TYPE(BehaviourNodeLeaf, BehaviourNode);
};

/// A node that checks a condition and returns a status.
class BehaviourNodeCondition : public BehaviourNodeLeaf {
	OBJ_TYPE(BehaviourNodeCondition, BehaviourNodeLeaf);
};

/// A node that performs an action and returns a status based on the success of that action.
class BehaviourNodeAction : public BehaviourNodeLeaf {
	OBJ_TYPE(BehaviourNodeAction, BehaviourNodeLeaf);
};

/// A node that can have multiple children and executes each of them.
class BehaviourNodeComposite : public BehaviourNode {
	OBJ_TYPE(BehaviourNodeComposite, BehaviourNode);
	friend class BehaviourNode;	// Calls _sort_children()

	Vector<int> m_children;			///< The children of this node.

	void _set_children(const Array& p_children);	///< Sets the children of this node, used for serialization.
	Array _get_children() const;					///< Gets the children of this node, used for serialization.

	/**	Sorts the children based on the y value of their position.
	 *	This is automatically called when the position changes or a child is added or removed.
	 */
	void _sort_children();
	bool m_stop_sort;

protected:

	static void _bind_methods();

public:
	/// Adds a child to this node.
	void add_child(int p_child_id);

	/// Removes a child from this node.
	void remove_child(int p_child_id);

	/// Checks if this node has a specific child.
	bool has_child(int p_child_id) const;

	/// Clears all children from this node.
	void clear_children();

	// Iteration
	Ref<BehaviourNode> get_child(int idx) const;	// Note, this is the index, not the id
	int get_child_id(int idx) const;
	int get_num_children() const;

	BehaviourNodeComposite();
};

/// A node that only has one child.
class BehaviourNodeDecorator : public BehaviourNode {
	OBJ_TYPE(BehaviourNodeDecorator, BehaviourNode);

	int m_child_id;

protected:

	static void _bind_methods();

public:

	void set_child_id(int p_child_id);
	int get_child_id() const;
	Ref<BehaviourNode> get_child() const;

	BehaviourNodeDecorator();
};

//////////////////////////////////////////////////////////////////////////
// We want this here so we don't need to include all the types into behaviour_tree.h

/// The root node of every tree. Simply executes its child (that sounds harsher than it is).
class BehaviourNodeRoot : public BehaviourNodeDecorator {
	OBJ_TYPE(BehaviourNodeRoot, BehaviourNodeDecorator);
	friend class BehaviourTree;
protected:
	ReturnCode tick(Ref<BehaviourTreeInstance> p_instance) const;
};

VARIANT_ENUM_CAST(BehaviourNode::ReturnCode);

#endif // BEHAVIOUR_NODE_H
