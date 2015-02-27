#include "behaviour_node_types.h"
#include "behaviour_tree.h"
#include "os/os.h"

BehaviourNode::ReturnCode BehaviourNodeSequence::tick(Ref<BehaviourTreeInstance> p_instance) const
{
	for (int i = 0; i < get_num_children(); ++i) {
		ReturnCode status = get_child(i)->execute(p_instance);

		if (status != RET_SUCCESS) {
			return status;
		}
	}

	return RET_SUCCESS;
}

BehaviourNode::ReturnCode BehaviourNodePriority::tick(Ref<BehaviourTreeInstance> p_instance) const
{
	for (int i = 0; i < get_num_children(); ++i) {
		ReturnCode status = get_child(i)->execute(p_instance);

		if (status != RET_FAILURE) {
			return status;
		}
	}

	return RET_FAILURE;
}

void BehaviourNodeMemSequence::open(Ref<BehaviourTreeInstance> p_instance) const
{
	p_instance->set_node_value(get_id(), _SCS("runningChild"), 0);
}

BehaviourNode::ReturnCode BehaviourNodeMemSequence::tick(Ref<BehaviourTreeInstance> p_instance) const
{
	int start_child = p_instance->get_node_value(get_id(), _SCS("runningChild"));
	for (int i = start_child; i < get_num_children(); ++i) {
		ReturnCode status = get_child(i)->execute(p_instance);

		if (status != RET_SUCCESS) {
			if (status == RET_RUNNING) {
				p_instance->set_node_value(get_id(), _SCS("runningChild"), i);
			}
			return status;
		}
	}

	return RET_SUCCESS;
}

void BehaviourNodeMemSequence::close(Ref<BehaviourTreeInstance> p_instance) const
{
	p_instance->clear_node_value(get_id(), _SCS("runningChild"));
}

void BehaviourNodeMemPriority::open(Ref<BehaviourTreeInstance> p_instance) const
{
	p_instance->set_node_value(get_id(), _SCS("runningChild"), 0);
}

BehaviourNode::ReturnCode BehaviourNodeMemPriority::tick(Ref<BehaviourTreeInstance> p_instance) const
{
	int start_child = p_instance->get_node_value(get_id(), _SCS("runningChild"));
	for (int i = start_child; i < get_num_children(); ++i) {
		ReturnCode status = get_child(i)->execute(p_instance);

		if (status != RET_FAILURE) {
			if (status == RET_RUNNING) {
				p_instance->set_node_value(get_id(), _SCS("runningChild"), i);
			}
			return status;
		}
	}

	return RET_SUCCESS;
}

void BehaviourNodeMemPriority::close(Ref<BehaviourTreeInstance> p_instance) const
{
	p_instance->clear_node_value(get_id(), _SCS("runningChild"));
}

//////////////////////////////////////////////////////////////////////////

BehaviourNode::ReturnCode BehaviourNodeInverter::tick(Ref<BehaviourTreeInstance> p_instance) const
{
	Ref<BehaviourNode> child = get_child();

	if (child.is_null()) {
		return RET_ERROR;
	}

	ReturnCode status = child->execute(p_instance);

	if (status == RET_SUCCESS)
		status = RET_FAILURE;
	else if (status == RET_FAILURE)
		status = RET_SUCCESS;

	return status;
}

BehaviourNode::ReturnCode BehaviourNodeSucceeder::tick(Ref<BehaviourTreeInstance> p_instance) const
{
	Ref<BehaviourNode> child = get_child();

	if (child.is_null()) {
		return RET_ERROR;
	}

	ReturnCode status = child->execute(p_instance);

	if (status == RET_FAILURE)
		status = RET_SUCCESS;

	return status;
}

void BehaviourNodeRepeater::_bind_methods()
{
	ObjectTypeDB::bind_method(_MD("set_times_repeat", "times"), &BehaviourNodeRepeater::set_times_repeat);
	ObjectTypeDB::bind_method(_MD("get_times_repeat"), &BehaviourNodeRepeater::get_times_repeat);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "times_repeat", PROPERTY_HINT_RANGE, "0,10000,1"), _SCS("set_times_repeat"), _SCS("get_times_repeat"));
}

void BehaviourNodeRepeater::open(Ref<BehaviourTreeInstance> p_instance) const
{
	p_instance->set_node_value(get_id(), "i", 0);
}

BehaviourNode::ReturnCode BehaviourNodeRepeater::tick(Ref<BehaviourTreeInstance> p_instance) const
{
	Ref<BehaviourNode> child = get_child();

	if (child.is_null()) {
		return RET_ERROR;
	}

	int i = p_instance->get_node_value(get_id(), "i");
	ReturnCode status = RET_SUCCESS;

	while (i < m_times_repeat) {
		status = child->execute(p_instance);

		if (status == RET_SUCCESS)
			++i;
		else {
			p_instance->set_node_value(get_id(), "i", i);
			return status;
		}
	}

	return status;
}

void BehaviourNodeRepeater::close(Ref<BehaviourTreeInstance> p_instance) const
{
	p_instance->clear_node_value(get_id(), "i");
}

void BehaviourNodeRepeater::set_times_repeat(int p_times)
{
	ERR_FAIL_COND(p_times < 0);
	m_times_repeat = p_times;
}

int BehaviourNodeRepeater::get_times_repeat() const
{
	return m_times_repeat;
}

BehaviourNodeRepeater::BehaviourNodeRepeater()
	: m_times_repeat(1)
{
}

void BehaviourNodeRepeatUntilFail::_bind_methods()
{
	ObjectTypeDB::bind_method(_MD("set_times_repeat", "times"), &BehaviourNodeRepeatUntilFail::set_times_repeat);
	ObjectTypeDB::bind_method(_MD("get_times_repeat"), &BehaviourNodeRepeatUntilFail::get_times_repeat);

	ObjectTypeDB::bind_method(_MD("set_repeat_forever", "forever"), &BehaviourNodeRepeatUntilFail::set_repeat_forever);
	ObjectTypeDB::bind_method(_MD("get_repeat_forever"), &BehaviourNodeRepeatUntilFail::get_repeat_forever);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "times_repeat", PROPERTY_HINT_RANGE, "0,10000,1"), _SCS("set_times_repeat"), _SCS("get_times_repeat"));
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "repeat_forever"), _SCS("set_repeat_forever"), _SCS("get_repeat_forever"));
}

void BehaviourNodeRepeatUntilFail::open(Ref<BehaviourTreeInstance> p_instance) const
{
	p_instance->set_node_value(get_id(), "i", 0);
}

BehaviourNode::ReturnCode BehaviourNodeRepeatUntilFail::tick(Ref<BehaviourTreeInstance> p_instance) const
{
	Ref<BehaviourNode> child = get_child();

	if (child.is_null()) {
		return RET_ERROR;
	}

	int i = p_instance->get_node_value(get_id(), "i");
	ReturnCode status = RET_SUCCESS;

	while (true) {
		ReturnCode status = child->execute(p_instance);

		if (status == RET_FAILURE)
			break;
	}

	return RET_SUCCESS;
}

void BehaviourNodeRepeatUntilFail::close(Ref<BehaviourTreeInstance> p_instance) const
{

}

void BehaviourNodeRepeatUntilFail::set_times_repeat(int p_times)
{
	ERR_FAIL_COND(p_times < 0);
	m_times_repeat = p_times;
}

int BehaviourNodeRepeatUntilFail::get_times_repeat() const
{
	return m_times_repeat;
}

void BehaviourNodeRepeatUntilFail::set_repeat_forever(bool p_forever)
{
	m_repeat_forever = p_forever;
}

bool BehaviourNodeRepeatUntilFail::get_repeat_forever() const
{
	return m_repeat_forever;
}

BehaviourNodeRepeatUntilFail::BehaviourNodeRepeatUntilFail()
	: m_times_repeat(0), m_repeat_forever(true)
{
}

void BehaviourNodeMaxTime::_bind_methods()
{
	ObjectTypeDB::bind_method(_MD("set_max_time", "seconds"), &BehaviourNodeMaxTime::set_max_time);
	ObjectTypeDB::bind_method(_MD("get_max_time"), &BehaviourNodeMaxTime::get_max_time);
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "max_time", PROPERTY_HINT_RANGE, "0,86400,0.01"), _SCS("set_max_time"), _SCS("get_max_time"));
}

void BehaviourNodeMaxTime::open(Ref<BehaviourTreeInstance> p_instance) const
{
	p_instance->set_node_value(get_id(), _SCS("startTime"), OS::get_singleton()->get_ticks_usec());
}

BehaviourNode::ReturnCode BehaviourNodeMaxTime::tick(Ref<BehaviourTreeInstance> p_instance) const
{
	Ref<BehaviourNode> child = get_child();

	if (child.is_null()) {
		return RET_ERROR;
	}

	// Get the start and current time
	int64_t start = p_instance->get_node_value(get_id(), _SCS("startTime"));
	int64_t now = OS::get_singleton()->get_ticks_usec();

	ReturnCode status = child->execute(p_instance);
	if ((now - start) > m_max_time) {
		return RET_FAILURE;
	}

	return status;
}

void BehaviourNodeMaxTime::close(Ref<BehaviourTreeInstance> p_instance) const
{
	p_instance->clear_node_value(get_id(), _SCS("startTime"));
}

void BehaviourNodeMaxTime::set_max_time(double seconds)
{
	m_max_time = static_cast<int64_t>(seconds * 1000000.0);
}

double BehaviourNodeMaxTime::get_max_time() const
{
	return static_cast<double>(m_max_time) / 1000000.0;
}

BehaviourNodeMaxTime::BehaviourNodeMaxTime()
	: m_max_time(0)
{
}

//////////////////////////////////////////////////////////////////////////

void BehaviourNodeWait::_bind_methods()
{
	ObjectTypeDB::bind_method(_MD("set_time_delay", "seconds"), &BehaviourNodeWait::set_time_delay);
	ObjectTypeDB::bind_method(_MD("get_time_delay"), &BehaviourNodeWait::get_time_delay);
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "time_delay", PROPERTY_HINT_RANGE, "0,86400,0.01"), _SCS("set_time_delay"), _SCS("get_time_delay"));
}

void BehaviourNodeWait::open(Ref<BehaviourTreeInstance> p_instance) const
{
	p_instance->set_node_value(get_id(), _SCS("startTime"), OS::get_singleton()->get_ticks_usec());
}

BehaviourNode::ReturnCode BehaviourNodeWait::tick(Ref<BehaviourTreeInstance> p_instance) const
{
	// Get the start and current time
	int64_t start = p_instance->get_node_value(get_id(), _SCS("startTime"));
	int64_t now = OS::get_singleton()->get_ticks_usec();

	// Check if we are past the delay
	if ((now - start) < m_time_delay)
		return BehaviourNode::RET_RUNNING;
	else
		return RET_SUCCESS;
}

void BehaviourNodeWait::close(Ref<BehaviourTreeInstance> p_instance) const
{
	p_instance->clear_node_value(get_id(), _SCS("startTime"));
}

void BehaviourNodeWait::set_time_delay(double seconds)
{
	m_time_delay = static_cast<int64_t>(seconds * 1000000.0);
}

double BehaviourNodeWait::get_time_delay() const
{
	return static_cast<double>(m_time_delay) / 1000000.0;
}

BehaviourNodeWait::BehaviourNodeWait()
	: m_time_delay(0)
{
}
