#pragma once

#include <memory>
#include <variant>

#include "../graphics.h"
#include "../math.h"

using UnitID = uint64_t;

struct Mirage
{
    Sprite Spr = { 0 };
    ivec2 TilePos;

	float StdActionRemaining = 6;
	float MoveActionRemaining = 6;
};

class NullAction : public std::monostate
{
	static constexpr char *_name = "None";

public:
	inline static std::string Name()
	{
		return _name;
	}
};

class MoveAction
{
	std::string _name;

public:
	inline MoveAction(const std::string &name = "Move")
		: _name(name) {}
	const std::string &Name() inline const
	{
		return _name;
	}
};

class AttackAction
{
	std::string _name;

public:
	float Damage;
	float Range;

	inline AttackAction(float damage, float range, const std::string &name = "Attack")
		: Damage(damage), Range(range), _name(name) {}

	const std::string &Name() inline const
	{
		return _name;
	}
};

using Action = std::variant<NullAction, MoveAction, AttackAction>;

const std::vector<Action> DefaultActions{
	MoveAction{},
	AttackAction{ 12, 1.5f, "Melee" },
	AttackAction{ 8, 15, "Ranged" }
};

struct NullOrder : public std::monostate {};

struct MoveOrder
{
	ivec2 TargetPos;
	const MoveAction *Action;
};

struct AttackOrder
{
	UnitID Target;
	const AttackAction *Action;
};

using OrderData = std::variant<NullOrder, MoveOrder, AttackOrder>;

class Unit;
struct Order
{
    Order *Next = nullptr;
	Order *Last = nullptr;

    // Mirage stores a temporary look at the state of the Unit
	// after the order is performed.
	Mirage Mirage = { 0 };

	UnitID Obj;

	struct it
	{
		Order *_o;

		inline it(Order *o) : _o(o) {}

		inline Order &operator*() { return *_o; }
		inline Order *operator->() { return _o; }
		
		inline it &operator++() 
		{ 
			_o = _o->Next; 
			return *this;
		}
		
		inline it operator++(int)
		{
			return { _o->Next };
		}

		inline bool operator==(const it &other)
		{
			return other._o == _o;
		}

		inline bool operator!=(const it &other)
		{
			return !(*this == other);
		}
	};

	OrderData Data{};
};

class Unit : public Mirage
{
	UnitID _id;
public:
	static constexpr UnitID NullID = 0;
	std::vector<Action> PossibleActions{ DefaultActions };

	// Linked list of orders to process
    Order *Orders = nullptr;
	inline Order::it begin()
	{
		return { Orders };
	}
	
	inline Order::it end()
	{
		return { nullptr };
	}

    Unit();
    Unit(Sprite spr, ivec2 pos);
    Unit(const Unit &) = default;
	inline UnitID ID() const
	{
		return _id;
	}

  private:
    static UnitID s_idCount;
};

struct Game;
void DrawUnit(Game *engine, const Unit &unit);
void DrawMirage(Game *engine, const Mirage &mirage);
vec2 GetTileCenter(const ivec2 &v);

template <typename T, typename... Ts>
struct overload : T, overload<Ts...>
{
	using T::operator();
	using overload<Ts...>::operator();
	overload(T t, Ts... ts) : T(t), overload<Ts...>(ts...) {}
};

template <typename T>
struct overload<T> : T
{
	using T::operator();
	overload(T t) : T(t) {}
};

template <typename ...T>
auto make_overload(T... ts)
{
	return overload<T...>(ts...);
}
