#ifndef LIFEPVP_ENGINE_BASE_H
#define LIFEPVP_ENGINE_BASE_H

#include <cstdint>
#include <functional>

namespace lifepvp::engine {

class EngineBase {
public:
	using state_t = uint8_t;
	using update_cb_t = std::function<void(size_t, size_t, uint8_t)>;
	using done_cb_t = std::function<void()>;

	EngineBase(const size_t w,
			const size_t h,
			const update_cb_t update_cb,
			const done_cb_t done_cb) :
			W(w), H(h), m_update_cb(update_cb), m_done_cb(done_cb) {}

	virtual ~EngineBase() = default;
	virtual void next_iteration() = 0;
	virtual state_t get_state(size_t, size_t) = 0;

protected:
	const size_t W;
	const size_t H;
	const update_cb_t m_update_cb;
	const done_cb_t m_done_cb;
};

} //namespace lifepvp::engine

#endif