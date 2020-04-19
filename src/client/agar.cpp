#include "agar.hpp"
#include "./../common/game_settings.hpp"
#include "projectile.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <tuple>
#include <vector>

constexpr double PI = 3.14159265358979323846;

void Agar::follow_mouse(int mx, int my, Camera &camera) {
  float mouse_x = mx * camera.current_scale;
  float mouse_y = my * camera.current_scale;

  float camera_x_offset = camera.x_offset();
  float camera_y_offset = camera.y_offset();

  std::sort(cells.begin(), cells.end(), [=](Cell &m, Cell &n) {
    float dist_m = std::sqrt(std::pow(mouse_x - (m.x - camera_x_offset), 2) +
                             std::pow(mouse_y - (m.y - camera_y_offset), 2));
    float dist_n = std::sqrt(std::pow(mouse_x - (n.x - camera_x_offset), 2) +
                             std::pow(mouse_y - (n.y - camera_y_offset), 2));

    return dist_m < dist_n;
  });

  int n_cells = cells.size();

  for (int i = 0; i < n_cells; i++) {
    float v_x = mouse_x - (cells[i].x - camera_x_offset);
    float v_y = mouse_y - (cells[i].y - camera_y_offset);

    float magnitude = std::sqrt(v_x * v_x + v_y * v_y);
    if (magnitude == 0)
      return;

    float speed_factor = std::max(
        MAX_SPEED + (MIN_SPEED - MAX_SPEED) / (MAX_AGAR_RADIUS - AGAR_RADIUS) *
                        (cells[i].radius - AGAR_RADIUS),
        MIN_SPEED);

    float vel_x = speed_factor * v_x / magnitude;
    // move blob
    cells[i].x += vel_x;
    // ensure boundaries
    if (cells[i].x < cells[i].radius)
      cells[i].x = cells[i].radius;
    if ((cells[i].x + cells[i].radius) > PLAYGROUND_WIDTH)
      cells[i].x = PLAYGROUND_WIDTH - cells[i].radius;

    float vel_y = speed_factor * v_y / magnitude;
    cells[i].y += vel_y;
    if (cells[i].y < cells[i].radius)
      cells[i].y = cells[i].radius;
    if ((cells[i].y + cells[i].radius) > PLAYGROUND_HEIGHT)
      cells[i].y = PLAYGROUND_HEIGHT - cells[i].radius;

    // check if it overlaps with other blobs
    for (int j = 0; j < n_cells; j++) {
      if (j != i) {
        if (cells[i].overlaps(cells[j])) {
          if (merge_timer.get_ticks() >= TIME_FOR_MERGE)
            cells[j].consume(cells[i]);
          else
            cells[i].touch_boundary(cells[j]);
          break;
        }
      }
    }
  }
  // remove cells with zero mass
  std::erase_if(cells, [](Cell &cell) { return cell.radius == 0; });
}

float Agar::get_size() {
  float r_2 = 0.0;
  for (auto &cell : cells) {
    r_2 += cell.radius * cell.radius;
  }
  return std::sqrt(r_2);
}

std::vector<Projectile> Agar::eject(int mx, int my, Camera &camera) {
  float mouse_x = mx * camera.current_scale;
  float mouse_y = my * camera.current_scale;

  std::vector<Projectile> ejectiles;
  for (auto &cell : cells) {
    float v_x = mouse_x - (cell.x - camera.x_offset());
    float v_y = mouse_y - (cell.y - camera.y_offset());

    float magnitude = std::sqrt(v_x * v_x + v_y * v_y);
    if (magnitude == 0 || cell.radius < MIN_RADIUS_FOR_SPLIT)
      continue;

    float dx = v_x / magnitude;
    float dy = v_y / magnitude;

    Cell eject_cell({CellType::Ejectile, int(cell.x + cell.radius * dx),
                     int(cell.y + cell.radius * dy), r, g, b, EJECTILE_RADIUS});
    Projectile projectile(std::move(eject_cell), dx, dy, EJECTILE_INIT_VELOCITY,
                          DECELERATION);

    // reduce radius of player
    cell.radius = std::sqrt(cell.radius * cell.radius -
                            EJECTILE_RADIUS * EJECTILE_RADIUS);
    ejectiles.push_back(std::move(projectile));
  }
  return ejectiles;
}

void Agar::split(int mx, int my, Camera &camera) {
  bool split = false;
  int ready_to_split =
      MAX_AGAR_COUNTERPARTS - (cells.size() + projectiles.size());
  float mouse_x = mx * camera.current_scale;
  float mouse_y = my * camera.current_scale;

  for (auto &cell : cells) {
    if (ready_to_split <= 0)
      break;
    float v_x = mouse_x - (cell.x - camera.x_offset());
    float v_y = mouse_y - (cell.y - camera.y_offset());

    float magnitude = std::sqrt(v_x * v_x + v_y * v_y);
    if (magnitude == 0 || cell.radius < MIN_RADIUS_FOR_SPLIT)
      continue;

    float dx = v_x / magnitude;
    float dy = v_y / magnitude;

    Cell split_cell({CellType::Player, int(cell.x + cell.radius * dx),
                     int(cell.y + cell.radius * dy), r, g, b,
                     cell.radius / std::sqrt(2.f)});
    Projectile projectile(std::move(split_cell), dx, dy, SPLIT_INIT_VELOCITY,
                          DECELERATION);

    cell.radius = cell.radius / std::sqrt(2);
    projectiles.push_back(std::move(projectile));
    ready_to_split -= 1;
    split = true;
  }
  if (split)
    merge_timer.start();
}

void Agar::disintegrate_cell(Cell &cell) {
  int n_splits = MAX_AGAR_COUNTERPARTS - (cells.size() + projectiles.size());
  float split_radius = cell.radius / std::sqrt(n_splits);
  for (int i = 1; i < n_splits; i++) {
    float theta = i * 2 * PI / 180 * n_splits;
    float dx = std::sin(theta);
    float dy = std::cos(theta);
    Cell split_cell({CellType::Player, int(cell.x + cell.radius),
                     int(cell.y + cell.radius), r, g, b, split_radius});
    Projectile projectile(std::move(split_cell), dx, dy,
                          DISINTEGRATE_INIT_VELOCITY, DECELERATION);

    cell.radius =
        std::sqrt(std::pow(cell.radius, 2) - std::pow(split_radius, 2));

    projectiles.push_back(std::move(projectile));
  }
  merge_timer.start();
}

void Agar::render(Context &ctx) {
  for (auto &cell : cells) {
    ctx.txt->render(cell, ctx.camera, ctx.renderer);
    ctx.ttxt->render_celltext(player_name, cell, ctx.camera, ctx.renderer);
  }
  for (auto &projectile : projectiles) {
    ctx.txt->render(projectile.cell, ctx.camera, ctx.renderer);
    ctx.ttxt->render_celltext(player_name, projectile.cell, ctx.camera,
                              ctx.renderer);
  }
}

std::tuple<int, int> Agar::get_center() {
  float sum_x = 0;
  float sum_y = 0;
  int n = 0;
  if (projectiles.size() != 0) {
    for (auto &projectile : projectiles) {
      sum_x += projectile.cell.x;
      sum_y += projectile.cell.y;
      n += 1;
    }
  } else {
    sum_x = cells[0].x;
    sum_y = cells[0].y;
    n = 1;
  }
  return std::make_tuple(sum_x / n, sum_y / n);
}

void Agar::adjust_camera(Context &ctx) {
  auto [agar_cx, agar_cy] = get_center();
  ctx.camera.set_center(agar_cx, agar_cy);

  // adjust zoom
  float new_zoom = std::fmin(std::round(get_size()) / AGAR_RADIUS, 2.f);

  ctx.zoom = std::lerp(ctx.zoom, new_zoom, 0.05f);
}

void Agar::update(Context &ctx, std::vector<Cell> &pellets,
                  std::vector<Virus> &viruses) {
  follow_mouse(ctx.mouse_x, ctx.mouse_y, ctx.camera);
  adjust_camera(ctx);

  // update projectiles
  for (auto &projectile : projectiles) {
    projectile.update_pos();
  }

  // move projectile to cells vec after coming to rest
  std::erase_if(projectiles, [this](Projectile &projectile) {
    bool pred = projectile.at_rest();
    if (pred) {
      cells.push_back(std::move(projectile.cell));
    }
    return pred;
  });

  // check if player eats any pellets
  for (auto &pellet : pellets) {
    for (auto &cell : cells) {
      if (cell.can_eat(pellet)) {
        cell.consume(pellet);
      }
    }
    for (auto &projectile : projectiles) {
      if (projectile.cell.can_eat(pellet)) {
        projectile.cell.consume(pellet);
      }
    }
  }

  int n_projectiles = projectiles.size();
  // check if player hovers a virus
  for (auto &virus : viruses) {
    for (auto &cell : cells) {
      if (virus.can_disintegrate(cell)) {
        disintegrate_cell(cell);
        goto end_virus_check;
      }
    }
    for (int i = 0; i < n_projectiles; i++) {
      if (virus.can_disintegrate(projectiles[i].cell)) {
        cells.push_back(std::move(projectiles[i].cell));
        disintegrate_cell(cells.back());
        projectiles.erase(projectiles.begin() + i);
        goto end_virus_check;
      }
    }
  }
end_virus_check:;
}

Agar::Agar(std::string name, CellSchema cs)
    : r(cs.r), g(cs.g), b(cs.b), player_name(name) {
  cells.push_back(Cell(cs));
}
