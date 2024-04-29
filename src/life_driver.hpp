#ifndef LIFEPVP_LIFE_DRIVER_H
#define LIFEPVP_LIFE_DRIVER_H

#include <atomic>
#include <memory>

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/variant.hpp>

#include "engine_base.hpp"

namespace godot {

class LifeDriver : public RefCounted {
	GDCLASS(LifeDriver, RefCounted)

public:
	enum EngineType {
		BASIC
	};

	void setup(const size_t w, const size_t h, const Variant &init_board, const EngineType engine, const Variant &ruleset);

	void next_iteration();

protected:
	static void _bind_methods();

private:
	std::unique_ptr<lifepvp::engine::EngineBase> m_engine;
	std::atomic_bool is_busy = false;
};

} //namespace godot

VARIANT_ENUM_CAST(LifeDriver::EngineType);

#endif // LIFEPVP_LIFE_DRIVER_H