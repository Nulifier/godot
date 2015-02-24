#ifndef VSCRIPT_STACK_H
#define VSCRIPT_STACK_H

#define VSCRIPT_STACK_DEBUG

class VScriptStack {

	Vector<Variant> m_stack;
	int m_stack_size;			// Always points one past the end of the stack. Ie. If an element is pushed, it is pushed at this index.

	void grow(int p_min_amt);

#ifdef VSCRIPT_STACK_DEBUG
	static int max_size_used = 0;
#endif

	inline int get_abs_index(int idx) const;

public:

	inline int get_top() const { return m_stack_size-1; }

	inline void push(const Variant& value);
	inline void pop(int n);
	inline const Variant& get(int p_idx) const;

	inline Variant::Type get_type(int p_idx) const;
	inline String get_type_name(int p_idx) const;
	inline bool is_type(int p_idx, Variant::Type p_type) const;
	inline bool is_type(int p_idx, const String& p_type) const;

	VScriptStack(int p_initial_size = 10);
};

void VScriptStack::grow(int p_min_amt)
{
	const int min_target_size = m_stack.size() + p_min_amt - 1;
	int new_size = m_stack.size();
	while (new_size < min_target_size)
		new_size *= 2;
	m_stack.resize(new_size);

#ifdef VSCRIPT_STACK_DEBUG
	if (min_target_size > max_size_used)
		max_size_used = min_target_size;
#endif
}

int VScriptStack::get_abs_index(int idx) const
{
	if (idx >= 0) {
		return idx;
	}
	else {
		return m_stack_size - idx;
	}
}

void VScriptStack::push(const Variant& value)
{
	// Check if we need to resize
	if (m_stack_size == m_stack.size()) {
		grow(1);
	}
	m_stack[m_stack_size++] = value;
}

void VScriptStack::pop(int n)
{
	// Reduce the stack size
	int old_stack_size = m_stack_size;
	m_stack_size = max(m_stack_size - n, 0);

	// Clear the pop-ed values
	for (int i = m_stack_size; i < old_stack_size; ++i) {
		m_stack[i] = Variant();
	}
}

const Variant& VScriptStack::get(int p_idx) const
{
	int idx = get_abs_index(p_idx);

	ERR_FAIL_INDEX_V(idx, m_stack_size, Variant());
}

Variant::Type VScriptStack::get_type(int p_idx) const
{
	return m_stack[get_abs_index(p_idx)].get_type();
}

String VScriptStack::get_type_name(int p_idx) const
{
	return Variant::get_type_name(m_stack[get_abs_index(p_idx)].get_type());
}

bool VScriptStack::is_type(int p_idx, Variant::Type p_type) const
{
	return m_stack[get_abs_index(p_idx)].get_type() == p_type;
}

inline bool VScriptStack::is_type(int p_idx, const String& p_type) const
{
	return Variant::get_type_name(m_stack[get_abs_index(p_idx)].get_type()) == p_type;
}

VScriptStack::VScriptStack(int p_initial_size /*= 10*/)
: m_stack_size(0)
{
	m_stack.resize(p_initial_size);
}

#endif // VSCRIPT_STACK_H
