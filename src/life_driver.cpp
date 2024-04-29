#include "life_driver.hpp"

#include <tuple>
#include <vector>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

#include "basic_engine.hpp"
#include "engine_base.hpp"

using namespace godot;

using namespace lifepvp::engine;

// setup_engine_with_ruleset(tuple{bools...}, f) is same as f([]() { return Ruleset{bools...}; })
// Except bools need not be constexpr for Ruleset{bools...} to be constexpr.
template <class Ruleset, size_t I, bool... bools>
auto setup_engine_with_ruleset(const auto &tup, const auto f) requires(I == 0) {
	return f([]() { return Ruleset{ bools... }; });
}

template <class Ruleset, size_t I, bool... bools>
auto setup_engine_with_ruleset(const auto &tup, const auto f) requires(0 < I && I <= std::tuple_size_v<std::remove_cvref_t<decltype(tup)>>) {
	if (std::get<I - 1>(tup)) {
		return setup_engine_with_ruleset<Ruleset, I - 1, bools..., true>(tup, f);
	} else {
		return setup_engine_with_ruleset<Ruleset, I - 1, bools..., false>(tup, f);
	}
}

void LifeDriver::setup(const size_t w, const size_t h, const Variant &init_board, const EngineType engine, const Variant &ruleset) {
	const auto update_cell_cb = [&](size_t i, size_t j, uint8_t state) { emit_signal("update_cell", i, j, state); };

	if (engine == BASIC) {
		// workaround for not being able to pass constexpr Ruleset directly
		const auto make_engine = [&](const auto get_ruleset) {
			if (init_board.get_type() == Variant::NIL) {
				m_engine = std::make_unique<BasicEngine<get_ruleset()>>(std::vector<EngineBase::state_t>(w * h), w, h, update_cell_cb);
			} else if (init_board.get_type() == Variant::PACKED_BYTE_ARRAY) {
				m_engine = std::make_unique<BasicEngine<get_ruleset(), PackedByteArray>>(init_board, w, h, update_cell_cb);
			} else {
				ERR_PRINT("Unknown init_board type, should be either null or PackedByteArray");
			}
		};
		if (ruleset.get_type() == Variant::NIL) {
			make_engine([]() { return BasicEngineRuleset{}; });
		} else if (ruleset.get_type() == Variant::DICTIONARY) {
			const Dictionary &dict = ruleset;
			const auto tup = std::make_tuple<bool>(dict.get(String("wrap_around"), false));
			setup_engine_with_ruleset<BasicEngineRuleset, std::tuple_size_v<decltype(tup)>>(tup, make_engine);
		} else {
			ERR_PRINT("Unknown ruleset type, should be either null or Dictionary");
		}
	} else {
		ERR_PRINT("Unknown engine type");
		return;
	}
}

void LifeDriver::next_iteration() {
	if (m_engine && !is_busy.exchange(true)) {
		m_engine->next_iteration();
		emit_signal("update_done");
		is_busy = false;
	}
}

void LifeDriver::_bind_methods() {
	ADD_SIGNAL(MethodInfo("update_cell", PropertyInfo(Variant::INT, "i"), PropertyInfo(Variant::INT, "j"), PropertyInfo(Variant::INT, "state")));
	ADD_SIGNAL(MethodInfo("update_done"));

	ClassDB::bind_method(D_METHOD("setup", "w", "h", "init_board", "engine_type"), &LifeDriver::setup);
	ClassDB::bind_method(D_METHOD("next_iteration"), &LifeDriver::next_iteration);

	BIND_ENUM_CONSTANT(BASIC);
}
