#include "world.hpp"
#include "./../common/game_settings.hpp"
#include "agar.hpp"
#include "cell.hpp"
#include "context.hpp"
#include "grid.hpp"
#include "network_client.hpp"
#include "virus.hpp"
#include <SDL2/SDL.h>
#include <cmath>
#include <memory>
#include <random>
#include <vector>

World::World(std::string player_name, std::unique_ptr<Bot> ai)
    : bot(std::move(ai)) {
  running = true;
  std::random_device rd;
  std::mt19937 eng(rd());
  std::uniform_int_distribution<> distrx(0, PLAYGROUND_WIDTH - AGAR_RADIUS);
  std::uniform_int_distribution<> distry(0, PLAYGROUND_HEIGHT - AGAR_RADIUS);
  std::uniform_int_distribution<> distrc(0, 255);

  uint8_t r = distrc(eng);
  uint8_t g = distrc(eng);
  uint8_t b = distrc(eng);
  agar = std::unique_ptr<Agar>(
      new Agar(player_name, {CellType::Player, distrx(eng), distry(eng), r, g,
                             b, AGAR_RADIUS}));

  interaction_timer.start();
}

void World::update(Context &ctx, NetworkClient &nc) {
  // use ai if requested for mouse update
  if (interaction_timer.get_ticks() >= HUMAN_INPUT_TIMEOUT) {
    if (bot)
      bot->set_mouse_pos(*this, ctx);
  }

  // update player
  (*agar).update(ctx, pellets, viruses);

  // update ejectiles
  for (auto &projectile : ejectiles) {
    projectile.update_pos();
  }

  // move projectile to pellets vec after coming to rest
  std::erase_if(ejectiles, [this](Projectile &projectile) {
    bool pred = projectile.at_rest();
    if (pred) {
      pellets.push_back(std::move(projectile.cell));
    }
    return pred;
  });

  // notify eaten pellets for relocation
  int n_pellets = pellets.size();
  for (int i = 0; i < n_pellets; i++) {
    if (pellets[i].radius == 0 && pellets[i].active) {
      nc.send_atepellet_message(i);
      pellets[i].active = false;
    }
  }
}

void World::render(Context &ctx, float fps) {
  SDL_SetRenderDrawColor(ctx.renderer, 242, 251, 255, 255);
  // draw background
  SDL_RenderClear(ctx.renderer);

  // draw grid
  Grid::draw(ctx);

  CellTexture *texture = ctx.txt;
  VirusTexture *vtexture = ctx.vtxt;

  // render pellets
  for (auto &pellet : pellets) {
    texture->render(pellet, ctx.camera, ctx.renderer);
  }

  // render ejectiles
  for (auto &projectile : ejectiles) {
    texture->render(projectile.cell, ctx.camera, ctx.renderer);
  }

  // shrink the world around
  ctx.camera.scale(ctx.zoom, ctx.renderer);

  // render player
  (*agar).render(ctx);

  // render viruses
  for (auto &virus : viruses) {
    vtexture->render(virus, ctx.camera, ctx.renderer);
  }

  // render fps count
  TextTexture *ttxt = ctx.ttxt;
  ttxt->render("FPS: " + std::to_string(int(fps)), SCREEN_WIDTH - 20,
               SCREEN_HEIGHT - 10, 80, ctx.camera, ctx.renderer);
  SDL_RenderPresent(ctx.renderer);
}

void World::handle_event(SDL_Event &e, Context &ctx) {
  if (e.type == SDL_MOUSEMOTION) {
    SDL_GetMouseState(&ctx.mouse_x, &ctx.mouse_y);
    interaction_timer.start();
  } else if (e.type == SDL_KEYDOWN) {
    switch (e.key.keysym.sym) {
    case SDLK_w: {
      auto ejections = (*agar).eject(ctx.mouse_x, ctx.mouse_y, ctx.camera);
      for (auto &ejection : ejections) {
        ejectiles.push_back(std::move(ejection));
      }
      break;
    }
    case SDLK_SPACE: {
      (*agar).split(ctx.mouse_x, ctx.mouse_y, ctx.camera);
      break;
    }
    }
  }
}

void World::create_pellet(
    std::tuple<int, int, uint8_t, uint8_t, uint8_t> pellet) {
  auto [x, y, r, g, b] = pellet;
  CellSchema cs = {CellType::Pellet, x, y, r, g, b, PELLET_RADIUS};
  pellets.emplace_back(cs);
}

void World::create_virus(std::tuple<int, int> virus) {
  auto [x, y] = virus;
  viruses.emplace_back(x, y, VIRUS_RADIUS);
}
void World::relocate_pellet(int id, int pos_x, int pos_y) {
  pellets[id].x = pos_x;
  pellets[id].y = pos_y;
  pellets[id].radius = PELLET_RADIUS;
  pellets[id].active = true;
}
