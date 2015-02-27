#ifndef BEHAVIOUR_NODE_TYPES_H
#define BEHAVIOUR_NODE_TYPES_H

#include "behaviour_node.h"

//////////////////////////////////////////////////////////////////////////
// Composites

/** Executes all it's children until one fails.
 *	Returns RET_SUCCESS if all succeed, RET_FAILURE if one fails.
 */
class BehaviourNodeSequence : public BehaviourNodeComposite {
	OBJ_TYPE(BehaviourNodeSequence, BehaviourNodeComposite);
protected:
	ReturnCode tick(Ref<BehaviourTreeInstance> p_instance) const;
};

/**	Executes all it's children until one succeeds.
 *	Returns RET_SUCCESS if one succeeds, RET_FAILURE if none do.
 */
class BehaviourNodePriority : public BehaviourNodeComposite {
	OBJ_TYPE(BehaviourNodePriority, BehaviourNodeComposite);
protected:
	ReturnCode tick(Ref<BehaviourTreeInstance> p_instance) const;
};

/** Executes all it's children until one fails, remembering what child its on between runs.
 *	Returns RET_SUCCESS if all succeed, RET_FAILURE if one fails.
 */
class BehaviourNodeMemSequence : public BehaviourNodeComposite {
	OBJ_TYPE(BehaviourNodeMemSequence, BehaviourNodeComposite);
protected:
	void open(Ref<BehaviourTreeInstance> p_instance) const;
	ReturnCode tick(Ref<BehaviourTreeInstance> p_instance) const;
	void close(Ref<BehaviourTreeInstance> p_instance) const;
};

/**	Executes all it's children until one succeeds, remember what child its on between runs.
 *	Returns RET_SUCCESS if one succeeds, RET_FAILURE if none do.
 */
class BehaviourNodeMemPriority : public BehaviourNodeComposite {
	OBJ_TYPE(BehaviourNodeMemPriority, BehaviourNodeComposite);
protected:
	void open(Ref<BehaviourTreeInstance> p_instance) const;
	ReturnCode tick(Ref<BehaviourTreeInstance> p_instance) const;
	void close(Ref<BehaviourTreeInstance> p_instance) const;
};

//////////////////////////////////////////////////////////////////////////
// Decorators

/// Inverts the success/failure of its child.
class BehaviourNodeInverter : public BehaviourNodeDecorator {
	OBJ_TYPE(BehaviourNodeInverter, BehaviourNodeDecorator);
protected:
	ReturnCode tick(Ref<BehaviourTreeInstance> p_instance) const;
};

/// Always returns success. Running and error states are passed through.
class BehaviourNodeSucceeder : public BehaviourNodeDecorator {
	OBJ_TYPE(BehaviourNodeSucceeder, BehaviourNodeDecorator);
protected:
	ReturnCode tick(Ref<BehaviourTreeInstance> p_instance) const;
};

/// Repeats its child a specified number of times.
class BehaviourNodeRepeater : public BehaviourNodeDecorator {
	OBJ_TYPE(BehaviourNodeRepeater, BehaviourNodeDecorator);

	int m_times_repeat;
protected:
	static void _bind_methods();

	void open(Ref<BehaviourTreeInstance> p_instance) const;
	ReturnCode tick(Ref<BehaviourTreeInstance> p_instance) const;
	void close(Ref<BehaviourTreeInstance> p_instance) const;

public:
	/// Sets the number of times to repeat.
	void set_times_repeat(int p_times);

	/// Gets the number of times to repeat.
	int get_times_repeat() const;

	BehaviourNodeRepeater();
};

class BehaviourNodeRepeatUntilFail : public BehaviourNodeDecorator {
	OBJ_TYPE(BehaviourNodeRepeatUntilFail, BehaviourNodeDecorator);

	int m_times_repeat;
	bool m_repeat_forever;
protected:
	static void _bind_methods();

	void open(Ref<BehaviourTreeInstance> p_instance) const;
	ReturnCode tick(Ref<BehaviourTreeInstance> p_instance) const;
	void close(Ref<BehaviourTreeInstance> p_instance) const;

public:
	/// Sets the number of times to repeat.
	void set_times_repeat(int p_times);

	/// Gets the number of times to repeat.
	int get_times_repeat() const;

	/// Sets if this node will repeat forever until a failure.
	void set_repeat_forever(bool p_forever);

	/// Checks if this node will repeat forever.
	bool get_repeat_forever() const;

	BehaviourNodeRepeatUntilFail();
};

class BehaviourNodeMaxTime : public BehaviourNodeDecorator {
	OBJ_TYPE(BehaviourNodeMaxTime, BehaviourNodeDecorator);

	int64_t m_max_time;
protected:
	static void _bind_methods();

	void open(Ref<BehaviourTreeInstance> p_instance) const;
	ReturnCode tick(Ref<BehaviourTreeInstance> p_instance) const;
	void close(Ref<BehaviourTreeInstance> p_instance) const;

public:
	void set_max_time(double seconds);
	double get_max_time() const;

	BehaviourNodeMaxTime();
};

//////////////////////////////////////////////////////////////////////////
// Actions

class BehaviourNodeWait : public BehaviourNodeAction {
	OBJ_TYPE(BehaviourNodeWait, BehaviourNodeAction);

	int64_t m_time_delay;

protected:
	static void _bind_methods();

	void open(Ref<BehaviourTreeInstance> p_instance) const;
	ReturnCode tick(Ref<BehaviourTreeInstance> p_instance) const;
	void close(Ref<BehaviourTreeInstance> p_instance) const;

public:

	void set_time_delay(double seconds);
	double get_time_delay() const;

	BehaviourNodeWait();
};

#endif // BEHAVIOUR_NODE_TYPES_H
