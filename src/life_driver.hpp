#ifndef LIFEPVP_LIFE_DRIVER_H
#define LIFEPVP_LIFE_DRIVER_H

#include <atomic>
#include <memory>

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/variant.hpp>

#include "engine_base.hpp"

#include "util/recurring_timer.hpp"

namespace godot {

class LifeDriver : public RefCounted {
	GDCLASS(LifeDriver, RefCounted)

public:
	LifeDriver() {
		m_timer = std::make_unique<RecurringTimer<>>([&]() { next_iteration(); });
	}

	enum EngineType {
		BASIC
	};

	void setup(size_t w, size_t h, const Variant &init_board, EngineType engine, const Variant &ruleset);

	void next_iteration();

	void start_auto_run();

	void stop_auto_run();

	void auto_run_set_interval(uint64_t interval_ms);

	void consume_updates(const Callable &cb);

protected:
	static void _bind_methods();

private:
	std::unique_ptr<lifepvp::engine::EngineBase> m_engine;
	std::atomic_bool is_busy = false;
	std::unique_ptr<RecurringTimer<>> m_timer;
	Dictionary m_updates;
	Dictionary m_iteration_updates;
	std::mutex m_updates_mutex;
};

} //namespace godot

VARIANT_ENUM_CAST(LifeDriver::EngineType);

#endif // LIFEPVP_LIFE_DRIVER_H