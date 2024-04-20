#include "life_driver.h"

#include <vector>
#include <cstring>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>


#include "engine_base.h"
#include "basic_engine.h"

using namespace godot;

void LifeDriver::setup(const size_t w, const size_t h, const Variant& init_board, const EngineType engine) {

	const auto init_board_to_vec = [&]() {
		const PackedByteArray& array = std::forward<PackedByteArray>(init_board);
		static_assert(sizeof(*(array.ptr())) == sizeof(EngineBase::state_t));
		std::vector<EngineBase::state_t> vec(w * h);
		memcpy(vec.data(), array.ptr(), sizeof(EngineBase::state_t) * vec.size());
		return vec;
	};

	const auto get_board_vec = [&]() {
		switch (init_board.get_type()) {
			case Variant::NIL:
				return std::vector<EngineBase::state_t>(w * h);

			case Variant::PACKED_BYTE_ARRAY:
				return init_board_to_vec();
				break;

			default:
				ERR_PRINT("Unknown init_board type");
				return std::vector<EngineBase::state_t>{};
		}
	};

	switch (engine) {
		case BASIC:
			m_engine = std::make_unique<BasicEngine>(get_board_vec(), w, h, 
				[&](size_t i, size_t j, uint8_t state) {
					emit_signal("update_cell", i, j, state);
				},
				[&]() {
					emit_signal("update_done");
				}
			);
			break;

		default:
			ERR_PRINT("Unknown engine type");
			return;
	}
}

void LifeDriver::next_iteration() {
	m_engine->next_iteration();
}

void LifeDriver::_bind_methods() {
	ADD_SIGNAL(MethodInfo("update_cell", PropertyInfo(Variant::INT, "i"), PropertyInfo(Variant::INT, "j"), PropertyInfo(Variant::INT, "state")));
	ADD_SIGNAL(MethodInfo("update_done"));
	
	ClassDB::bind_method(D_METHOD("setup", "w", "h", "init_board", "engine_type"), &LifeDriver::setup);
	ClassDB::bind_method(D_METHOD("next_iteration"), &LifeDriver::next_iteration);

	BIND_ENUM_CONSTANT(BASIC);
}
