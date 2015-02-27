#include "register_types.h"

#include "behaviour_node.h"
#include "behaviour_tree.h"
#include "behaviour_node_types.h"

void register_behaviourtree_types()
{
	ObjectTypeDB::register_type<BehaviourTree>();
	ObjectTypeDB::register_type<BehaviourTreeInstance>();

	ObjectTypeDB::register_virtual_type<BehaviourNode>();
	ObjectTypeDB::register_virtual_type<BehaviourNodeLeaf>();
	ObjectTypeDB::register_virtual_type<BehaviourNodeCondition>();
	ObjectTypeDB::register_virtual_type<BehaviourNodeAction>();
	ObjectTypeDB::register_virtual_type<BehaviourNodeComposite>();

	ObjectTypeDB::register_type<BehaviourNodeRoot>();

	// Composites
	ObjectTypeDB::register_type<BehaviourNodeSequence>();
	ObjectTypeDB::register_type<BehaviourNodePriority>();
	ObjectTypeDB::register_type<BehaviourNodeMemSequence>();
	ObjectTypeDB::register_type<BehaviourNodeMemPriority>();

	// Decorators
	ObjectTypeDB::register_type<BehaviourNodeInverter>();
	ObjectTypeDB::register_type<BehaviourNodeSucceeder>();
	ObjectTypeDB::register_type<BehaviourNodeRepeater>();
	ObjectTypeDB::register_type<BehaviourNodeRepeatUntilFail>();
	ObjectTypeDB::register_type<BehaviourNodeMaxTime>();

	// Actions
	ObjectTypeDB::register_type<BehaviourNodeWait>();

	// Conditions
}

void unregister_behaviourtree_types()
{

}
