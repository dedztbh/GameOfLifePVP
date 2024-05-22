#include "life_driver.hpp"

#include <tuple>
#include <vector>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

#include "engine/basic_engine.hpp"
#include "engine/engine_base.hpp"
#include "engine/two_player_engine.hpp"

using namespace godot;

using namespace lifepvp::engine;

// setup_engine_with_ruleset(tuple{bools...}, f) is same as f([]() { return Ruleset{bools...}; })
// Except bools need not be constexpr for Ruleset{bools...} to be constexpr.
template <class Ruleset, size_t I, bool... bools>
auto setup_engine_with_ruleset(auto &tup, auto f)
	requires(I == 0)
{
	return f([]() { return Ruleset{ bools... }; });
}

template <class Ruleset, size_t I, bool... bools>
auto setup_engine_with_ruleset(auto &tup, auto f)
	requires(0 < I && I <= std::tuple_size_v<std::remove_cvref_t<decltype(tup)>>)
{
	if (std::get<I - 1>(tup)) {
		return setup_engine_with_ruleset<Ruleset, I - 1, true, bools...>(tup, f);
	} else {
		return setup_engine_with_ruleset<Ruleset, I - 1, false, bools...>(tup, f);
	}
}

void LifeDriver::setup(size_t w, size_t h, const Variant &init_board, EngineType engine, const Variant &ruleset) {
	auto update_cell_cb = [&](size_t i, size_t j, uint8_t state) {
		m_iteration_updates[Vector2i(i, j)] = state;
	};

	if (engine == BASIC) {
		// workaround for not being able to pass constexpr Ruleset directly
		auto make_engine = [&](auto get_ruleset) {
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
			auto tup = std::make_tuple<bool>(dict.get(String("wrap_around"), false));
			setup_engine_with_ruleset<BasicEngineRuleset, std::tuple_size_v<decltype(tup)>>(tup, make_engine);
		} else {
			ERR_PRINT("Unknown ruleset type, should be either null or Dictionary");
		}
	} else if (engine == TWO_PLAYER) {
		auto make_engine = [&](auto get_ruleset) {
			if (init_board.get_type() == Variant::NIL) {
				m_engine = std::make_unique<TwoPlayerEngine<get_ruleset()>>(std::vector<EngineBase::state_t>(w * h), w, h, update_cell_cb);
			} else if (init_board.get_type() == Variant::PACKED_BYTE_ARRAY) {
				m_engine = std::make_unique<TwoPlayerEngine<get_ruleset(), PackedByteArray>>(init_board, w, h, update_cell_cb);
			} else {
				ERR_PRINT("Unknown init_board type, should be either null or PackedByteArray");
			}
		};
		if (ruleset.get_type() == Variant::NIL) {
			make_engine([]() { return TwoPlayerEngineRuleset{}; });
		} else if (ruleset.get_type() == Variant::DICTIONARY) {
			const Dictionary &dict = ruleset;
			auto tup = std::make_tuple<bool>(
					dict.get(String("wrap_around"), false),
					dict.get(String("cooperative_cells"), false));
			setup_engine_with_ruleset<TwoPlayerEngineRuleset, std::tuple_size_v<decltype(tup)>>(tup, make_engine);
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
		{
			std::lock_guard lock(m_updates_mutex);
			m_updates.merge(m_iteration_updates, true);
			m_iteration_updates.clear();
		}
		is_busy = false;
	}
}

void LifeDriver::start_auto_run() {
	m_timer->start();
}

void LifeDriver::stop_auto_run() {
	m_timer->stop();
}

void LifeDriver::auto_run_set_interval(uint64_t interval_us) {
	m_timer->set_interval(std::chrono::microseconds(interval_us));
}

void LifeDriver::consume_updates(const Callable &cb) {
	std::lock_guard lock(m_updates_mutex);
	cb.call(m_updates);
	m_updates.clear();
}

void LifeDriver::_bind_methods() {
	ClassDB::bind_method(D_METHOD("setup", "w", "h", "init_board", "engine_type", "ruleset"), &LifeDriver::setup);
	ClassDB::bind_method(D_METHOD("next_iteration"), &LifeDriver::next_iteration);
	ClassDB::bind_method(D_METHOD("start_auto_run"), &LifeDriver::start_auto_run);
	ClassDB::bind_method(D_METHOD("stop_auto_run"), &LifeDriver::stop_auto_run);
	ClassDB::bind_method(D_METHOD("auto_run_set_interval", "interval_us"), &LifeDriver::auto_run_set_interval);
	ClassDB::bind_method(D_METHOD("consume_updates"), &LifeDriver::consume_updates);

	BIND_ENUM_CONSTANT(BASIC);
	BIND_ENUM_CONSTANT(TWO_PLAYER);
}
