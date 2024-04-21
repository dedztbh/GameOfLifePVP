#include "life_driver.hpp"

#include <cstring>
#include <vector>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

#include "basic_engine.hpp"
#include "engine_base.hpp"

using namespace godot;

using namespace lifepvp::engine;

void LifeDriver::setup(const size_t w, const size_t h, const Variant &init_board, const EngineType engine) {
	const auto update_cell_cb = [&](size_t i, size_t j, uint8_t state) { emit_signal("update_cell", i, j, state); };
	const auto update_done_cb = [&]() { emit_signal("update_done"); };

	switch (engine) {
		case BASIC:
			if (init_board.get_type() == Variant::NIL) {
				std::vector<EngineBase::state_t> board(w * h);
				m_engine = std::make_unique<BasicEngine<>>(std::move(board), w, h, update_cell_cb, update_done_cb);
			} else if (init_board.get_type() == Variant::PACKED_BYTE_ARRAY) {
				m_engine = std::make_unique<BasicEngine<PackedByteArray>>(init_board, w, h, update_cell_cb, update_done_cb);
			} else {
				ERR_PRINT("Unknown init_board type");
			}
			break;

		default:
			ERR_PRINT("Unknown engine type");
			return;
	}
}

void LifeDriver::next_iteration() {
	if (m_engine) {
		m_engine->next_iteration();
	}
}

void LifeDriver::_bind_methods() {
	ADD_SIGNAL(MethodInfo("update_cell", PropertyInfo(Variant::INT, "i"), PropertyInfo(Variant::INT, "j"), PropertyInfo(Variant::INT, "state")));
	ADD_SIGNAL(MethodInfo("update_done"));

	ClassDB::bind_method(D_METHOD("setup", "w", "h", "init_board", "engine_type"), &LifeDriver::setup);
	ClassDB::bind_method(D_METHOD("next_iteration"), &LifeDriver::next_iteration);

	BIND_ENUM_CONSTANT(BASIC);
}
