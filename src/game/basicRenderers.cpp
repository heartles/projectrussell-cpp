#include "basicRenderers.h"

#include "../mathstrings.h"

const int UNIT_LAYER = -10;
const int TILE_LAYER = -100;

// // Returns true if there are still orders remaining
// // False otherwise
// bool
// ProcessOrder(Game &Engine, Order *o, Unit *u)
// {
//     std::visit(make_overload(
//         [](const NullOrder &) {},
//         [&](const MoveOrder &move){
//             Log("\t\tExecuting Move from " + std::to_string(u->TilePos) +
//                     " to " + std::to_string(move.TargetPos));
//             u->TilePos = move.TargetPos;
//         }, 
//         [&](const AttackOrder &att){
//             const auto otherUnitID = Engine.Level.GetUnit(att.Target)->ID();
//             Log("\t\tAttack made against unit " +
//                 std::to_string(otherUnitID));
//         }), o->Data);

//     auto nextO = o->Next;
//     Engine.Level.Orders.Free(o);
//     u->Orders = nextO;
//     if (nextO) {
//         nextO->Last = nullptr;
//         return false;
//     }

//     return true;
// }

// void
// UnitRenderer::Draw(Game &Engine)
// {
// 	if (!_unitRendering) {
// 		for (auto &u : Engine.Level.Units) {
// 			DrawUnit(&Engine, u);
// 		}
// 		return;
// 	}

// 	Unit &unit = *Engine.Level.GetUnit(_unitRendering);

// 	Order &currentOrder = *unit.Orders;
// 	auto handler = make_overload(
// 		[](const NullOrder &) { return true; },
// 		[&currentOrder, this, &Engine, &unit](const MoveOrder &o)
// 		{
// 			const auto lastPos = unit.TilePos;
// 			auto totalDist = AbsComponentSum(o.TargetPos - lastPos); 
// 			//TODO: allow movement along diagonal

// 			auto totalTime = abs(totalDist * 0.5f);

// 			//TODO: pathfinding
// 			auto pos = Lerp(
// 				vec2(GetTileCenter(lastPos)),
// 				vec2(GetTileCenter(o.TargetPos)),
// 				_animProgress / totalTime);
			
// 			Engine.View.DrawSprite(
// 				unit.Spr, pos, 0, { 0.5f, 0.5f }, Colors::White);

// 			return _animProgress >= totalTime;
// 		},
// 		[](const AttackOrder &o)
// 		{
// 			Log("Attacking");
// 			return true;
// 		});

// 	bool finished = std::visit(handler, currentOrder.Data);

//     _animProgress += Engine.DT;
// 	if (finished) {
//         _animProgress = 0;
//         if (ProcessOrder(Engine, &currentOrder, &unit)) {
//             Unit *u = &unit;
//             while (_unitRendering && !u->Orders) {
//                 u = Engine.Level.GetUnit(_unitRendering);
//                 _unitRendering = Engine.Level.GetNext(u->ID());
//             }
//         }
// 	}

// }

// void 
// UnitRenderer::BeginRenderAnimations(Game &Engine) 
// {
// 	for (auto &u : Engine.Level.Units) {
// 		if (u.Orders) {
// 			_unitRendering = u.ID();
// 			_animProgress = 0.0f;
// 			return;
// 		}
// 	}
// }

// RenderOrder
// UnitRenderer::RequestedDrawOrder()
// {
//     return RenderOrder(UNIT_LAYER);
// }

void
TilemapRenderer::Draw(Game &Engine)
{
    auto shader = Engine.Content.LoadShader("/content/textured.gl.vert",
                                            "/content/textured.gl.frag");

    shader->Apply();

    mat3 viewMat = Engine.View.Matrix();

    for (auto &t : Engine.Level.Tilesets) {
        glBindVertexArray(t.VertexArrayID);

        glBindTexture(GL_TEXTURE_2D, t.Image->TextureID);

        SetUniform("projection", viewMat);
        SetUniform("color", Colors::White);

        glEnableVertexAttribArray(0);

        glEnableVertexAttribArray(1);

        glDrawArrays(GL_TRIANGLES, 0, t.Positions.size());
    }
}
