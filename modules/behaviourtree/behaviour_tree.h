#ifndef BEHAVIOR_TREE_H
#define BEHAVIOR_TREE_H

#include "resource.h"
#include "behaviour_node.h"

class BehaviourTreeInstance;

/**	A behaviour tree resource.
 *	Instances are created of this tree and can be executed allowing reuse of this object.
 *	@see BehaviourTreeInstance
 */
class BehaviourTree : public Resource {
	OBJ_TYPE(BehaviourTree, Resource);
	RES_BASE_EXTENSION("bt");
	friend class BehaviourNode;	// For _get_new_id()

	/// The root of the tree.
	int m_tree_root_id;

	int m_next_id;				///< The next node id to be assigned.
	int _get_next_id() const;	///< Returns m_next_id for serialization.
	void _set_next_id(int id);	///< Sets m_next_id for serialization.

	Map<int, Ref<BehaviourNode>> m_node_map;

	void _set_nodes(const Dictionary& nodes);
	Dictionary _get_nodes() const;

protected:

	static void _bind_methods();

public:
	/// Gets the next valid id for a node.
	int get_new_id();

	/**	Adds a BehaviourNode to the tree.
	 *	@param p_node_type The class name of the node to add.
	 *	@returns The id of the newly created node.
	 */
	int add_node(const String& p_node_type);

	/**	Adds a BehaviourNode to the tree.
	 *	@param p_new_id The id of the node to create. Get this from get_new_id().
	 *	@param p_node_type The class name of the node to add.
	 */
	void add_node_by_id(int p_new_id, const String& p_node_type);

	/// Gets a BehaviourNode by id.
	BehaviourNode* get_node(int p_id);

	/// Checks if this tree has a node with a specified id.
	bool has_node(int p_id) const;

	/// Removes a node from this tree by id.
	void remove_node(int p_id);

	/// Sets the root of the tree.
	void set_root_id(int p_id);

	/// Gets the root id of the tree.
	int get_root_id() const { return m_tree_root_id; }

	/**	Creates and instance of this tree.
	 *	@param p_context A context object that can be used in the behaviour nodes.
	 *	@returns The newly created instance.
	 */
	Ref<BehaviourTreeInstance> create_instance(Object* p_context);

	BehaviourTree();
};

//////////////////////////////////////////////////////////////////////////
/// An instance of a BehaviourTree.
class BehaviourTreeInstance : public Reference {
	OBJ_TYPE(BehaviourTreeInstance, Reference);
	friend class BehaviourTree;
	friend class BehaviourNode;

	Object* m_context;										///< The context for this instance.
	Ref<BehaviourTree> m_tree;								///< A reference back to the parent tree.
	Map<StringName, Variant> m_inst_blackboard;				///< Used to hold variables for this instance.
	Map<int, Map<StringName, Variant>> m_node_blackboard;	///< Used to hold variables for each node.
	Set<const BehaviourNode*> m_open_nodes;					///< Keeps track of which nodes are open in between execute calls so they can be closed.
	Set<const BehaviourNode*> m_orphans;					///< Used in the execute function to temporarily keep track of which nodes weren't used this time. Member to avoid realloc.
	static const Variant nil;								///< Returned from get_inst_value and get_node_value when a value doesn't exist.

	// Functions called by BehaviourNode
	void _enter_node(const BehaviourNode& m_node);
	void _open_node(const BehaviourNode& m_node);
	void _tick_node(const BehaviourNode& m_node);
	void _close_node(const BehaviourNode& m_node);
	void _exit_node(const BehaviourNode& m_node);

	bool is_node_open(const BehaviourNode& m_node) const;	///< Checks if a node is considered open (close() hasn't been called after open()).

protected:

	static void _bind_methods();

public:

	/// Executes the tree starting from the root node.
	void execute();

	// Blackboard

	/**	Sets a value for the instance.
	 *	@param p_name The name of the value to set.
	 *	@param p_value The value to assign.
	 */
	void set_inst_value(const StringName& p_name, const Variant& p_value);

	/**	Gets a value from this instance.
	 *	@param p_name The name of the value to get.
	 *	@returns The value.
	 */
	const Variant& get_inst_value(const StringName& p_name) const;

	/**	Clears a value from this instance.
	 *	Used to avoid having lots of temporary variables.
	 *	@param p_name The name of the value to clear.
	 */
	void clear_inst_value(const StringName& p_name);

	/**	Sets a value for a node.
	 *	@param p_node_id The id of the node to set the value for.
	 *	@param p_name The name of the value to set.
	 *	@param p_value The value to assign.
	 */
	void set_node_value(int p_node_id, const StringName& p_name, const Variant& p_value);

	/**	Gets a value for a node.
	 *	@param p_node_id The id of the node to get the value for.
	 *	@param p_name The name of the value to get.
	 *	@returns The value.
	 */
	const Variant& get_node_value(int p_node_id, const StringName& p_name) const;

	/**	Clears a value for a node.
	 *	Used to avoid having lots of temporary variables.
	 *	@param p_node_id The id of the node to clear the value for.
	 *	@param p_name The name of the value to clear.
	 */
	void clear_node_value(int p_node_id, const StringName& p_name);

	/// Returns a reference to the tree that created this instance.
	Ref<BehaviourTree> get_tree() const;

	/// Sets the context for this instance.
	void set_context(Object* p_context);

	/// Gets the context for this instance.
	Object* get_context() const;

	BehaviourTreeInstance();
};

#endif // BEHAVIOUR_TREE_H
