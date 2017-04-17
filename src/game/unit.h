#pragma once

#include <memory>
#include <variant>

#include "../graphics.h"
#include "../math.h"

enum class ActionType
{
    None = 0,
    Move,
    Attack
};

struct Unit;
struct Mirage
{
    Sprite Spr = { 0 };
    ivec2 TilePos;

	float StdActionRemaining = 6;
	float MoveActionRemaining = 6;
};

struct Order
{
    ActionType Type = ActionType::None;
    Order *Next = nullptr;
    Mirage Mirage = { 0 };

	Unit *Obj;

    union
    {
        ivec2 TilePos;
        Unit *Other;
    };

    inline Order()
      : TilePos(ivec2{}){};
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
	AttackAction{12, 1.5f, "Melee"},
	AttackAction{8, 15, "Ranged"}
};

struct Unit : public Mirage
{
	std::vector<Action> PossibleActions{ DefaultActions };

	// Linked list of orders to process
    Order *Orders = nullptr;
    int ID;

    Unit();
    Unit(Sprite spr, ivec2 pos);
    Unit(const Unit &) = default;

  private:
    static int _idCount;
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
