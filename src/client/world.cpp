#include "world.hpp"
#include "./../shared/game_settings.hpp"
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
      new Agar(player_name, {CellType::Player, float(distrx(eng)),
                             float(distry(eng)), r, g, b, AGAR_RADIUS}));

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

  // shrink the world around
  ctx.camera.scale(ctx.zoom, ctx.renderer);

  // render player
  (*agar).render(ctx);

  // render other players
  render_online_players(ctx);

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
      (*agar).eject(ctx.mouse_x, ctx.mouse_y, ctx.camera);
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
  auto x = std::get<0>(pellet);
  auto y = std::get<1>(pellet);
  auto r = std::get<2>(pellet);
  auto g = std::get<3>(pellet);
  auto b = std::get<4>(pellet);
  CellSchema cs = {CellType::Pellet, float(x), float(y), r, g, b,
                   PELLET_RADIUS};
  pellets.emplace_back(cs);
}

void World::create_virus(std::tuple<int, int> virus) {
  auto x = std::get<0>(virus);
  auto y = std::get<1>(virus);
  viruses.emplace_back(x, y, VIRUS_RADIUS);
}
void World::relocate_pellet(int id, int pos_x, int pos_y) {
  if (id < (int)pellets.size()) {
    pellets[id].x = pos_x;
    pellets[id].y = pos_y;
    pellets[id].radius = PELLET_RADIUS;
    pellets[id].active = true;
  }
}

void World::add_player(
    std::tuple<std::string, uint8_t, uint8_t, uint8_t> player) {
  auto name = std::get<0>(player);
  auto r = std::get<1>(player);
  auto g = std::get<2>(player);
  auto b = std::get<3>(player);
  players_info.emplace_back(true, name, r, g, b);
}

void World::render_online_players(Context &ctx) {
  int n_players = players_stats.size();
  for (int i = 0; i < n_players; i++) {
    auto active = std::get<0>(players_info[i]);
    auto name = std::get<1>(players_info[i]);
    auto r = std::get<2>(players_info[i]);
    auto g = std::get<3>(players_info[i]);
    auto b = std::get<4>(players_info[i]);
    if (active && i != my_index) {
      render_cells(ctx, players_stats[i].cells, name, r, g, b);
      render_ejectiles(ctx, players_stats[i].ejectiles, r, g, b);
    }
  }
}

void World::render_cells(Context &ctx,
                         std::vector<std::tuple<float, float, float>> &cells,
                         std::string name, uint8_t r, uint8_t g, uint8_t b) {
  CellTexture *texture = ctx.txt;
  TextTexture *ttexture = ctx.ttxt;
  for (auto &cell : cells) {
    texture->render(cell, r, g, b, ctx.camera, ctx.renderer);
    ttexture->render_celltext(name, cell, ctx.camera, ctx.renderer);
  }
}

void World::render_ejectiles(Context &ctx,
                             std::vector<std::tuple<float, float>> &ejectiles,
                             uint8_t r, uint8_t g, uint8_t b) {
  CellTexture *texture = ctx.txt;
  for (auto &ejectile : ejectiles) {
    auto x = std::get<0>(ejectile);
    auto y = std::get<1>(ejectile);
    auto cell = std::make_tuple(x, y, float(EJECTILE_RADIUS));
    texture->render(cell, r, g, b, ctx.camera, ctx.renderer);
  }
}
