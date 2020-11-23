#include "bnOverworldMap.h"
#include "bnEngine.h"
#include <cmath>

namespace Overworld {
  Map::Map() :
    sf::Drawable(), sf::Transformable() {
  }

  void Map::Load(Map::Tileset tileset, Map::Tile** tiles, unsigned cols, unsigned rows)
  {
    this->tileset = tileset;
    this->tiles = tiles;
    this->cols = cols;
    this->rows = rows;

    // NOTE: hard coded tilesets for now
    tileWidth = 47;
    tileHeight = 24;
  }

  void Map::ToggleLighting(bool state) {
    enableLighting = state;

    if (!enableLighting) {
      for (int i = 0; i < lights.size(); i++) {
        delete lights[i];
      }

      lights.clear();
    }
  }

  const sf::Vector2f Map::ScreenToWorld(sf::Vector2f screen) const
  {
    return OrthoToIsometric(screen);
  }

  Map::~Map() {
    for (int i = 0; i < lights.size(); i++) {
      delete lights[i];
    }

    lights.clear();

    sprites.clear();
  }

  void Map::SetCamera(Camera* _camera) {
    cam = _camera;
  }

  void Map::AddLight(Overworld::Light * _light)
  {
    lights.push_back(_light);
  }

  void Map::AddSprite(const SpriteProxyNode * _sprite, int layer)
  {
    sprites.push_back({ _sprite, layer });
  }

  void Map::RemoveSprite(const SpriteProxyNode * _sprite) {
    auto pos = std::find_if(sprites.begin(), sprites.end(), [_sprite](SpriteLayer in) { return in.node == _sprite; });

    if(pos != sprites.end())
      sprites.erase(pos);
  }

  void Map::Update(double elapsed)
  {
    std::sort(sprites.begin(), sprites.end(), 
        [](SpriteLayer A, SpriteLayer B)
      { return std::tie(A.layer, A.node->getPosition().y) < std::tie(B.layer, B.node->getPosition().y); }
    );

  }

  void Map::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= this->getTransform();

    DrawTiles(target, states);
    DrawSprites(target, states);
  }

  const Map::Tile Map::GetTileAt(const sf::Vector2f& pos) const
  {
    unsigned x = pos.x / (tileWidth * 0.5f);
    unsigned y = pos.y / tileHeight;

    if (x >= 0 && x < cols && y >= 0 && y < rows) {
      return tiles[y][x];
    }

    return Map::Tile{};
  }

  void Map::SetTileAt(const sf::Vector2f pos, const Tile& newTile)
  {
    unsigned x = pos.x / (tileWidth * 0.5f);
    unsigned y = pos.y / tileHeight;

    if (x >= 0 && x < cols && y >= 0 && y < rows) {
      tiles[y][x] = newTile;
    }
  }

  const std::vector<sf::Vector2f> Map::FindToken(const std::string& token)
  {
    std::vector<sf::Vector2f> results;

    for (unsigned i = 0; i < rows; i++) {
      for (unsigned j = 0; j < cols; j++) {
        if (tiles[i][j].token == token) {
          // find tile pos + half of width and height so the point is center tile
          results.push_back(sf::Vector2f((j * tileWidth * 0.5f)+(tileWidth*0.25f), (i * tileHeight)+(tileHeight*0.5f)));
        }
      }
    }

    return results;
  }

  void Map::DrawTiles(sf::RenderTarget& target, sf::RenderStates states) const {
    for (unsigned i = 0; i < rows; i++) {
      for (unsigned j = 0; j < cols; j++) {
        size_t ID = tiles[i][j].ID;

        if (ID == 0) continue; // reserved for empty tile

        sf::Sprite tileSprite = tileset.Graphic(ID);
        sf::Vector2f pos(j * tileWidth * 0.5f, i * tileHeight);
        auto iso = OrthoToIsometric(pos);
        tileSprite.setPosition(iso);

        if (cam) {
          if (true || cam->IsInView(tileSprite)) {
            target.draw(tileSprite, states);
          }
        }
      }
    }
  }

  void Map::DrawSprites(sf::RenderTarget& target, sf::RenderStates states) const {
    for (int i = 0; i < sprites.size(); i++) {
      auto iso = OrthoToIsometric(sprites[i].node->getPosition());
      sf::Sprite tileSprite(*sprites[i].node->getTexture());
      tileSprite.setTextureRect(sprites[i].node->getSprite().getTextureRect());
      tileSprite.setPosition(iso);
      tileSprite.setOrigin(sprites[i].node->getOrigin());

      if (cam) {
        if (true || cam->IsInView(tileSprite)) {
          target.draw(tileSprite, states);
        }
      }
    }
  }
  const sf::Vector2f Map::OrthoToIsometric(const sf::Vector2f& ortho) const {
    sf::Vector2f iso{};
    iso.x = (ortho.x - ortho.y);
    iso.y = (ortho.x + ortho.y) * 0.5f;

    return iso;
  }

  const sf::Vector2f Map::IsoToOrthogonal(const sf::Vector2f& iso) const {
    sf::Vector2f ortho{};
    ortho.x = (2.0f * iso.y + iso.x) * 0.5f;
    ortho.y = (2.0f * iso.y - iso.x) * 0.5f;

    return ortho;
  }

  const sf::Vector2i Map::GetTileSize() const { return sf::Vector2i(tileWidth, tileHeight); }

  const bool Map::LoadFromFile(Map& map, const std::string& path)
  {
    std::ifstream file(path.c_str());

    if (!file.is_open()) {
      file.close();
      return false;
    }

    std::string line;

    // name of map
    std::getline(file, line);
    std::string name = line;

    // rows x cols
    std::getline(file, line);
    size_t space_index = line.find_first_of(" ");

    std::string rows_str = line.substr(0, space_index);
    std::string cols_str = line.substr(space_index);
    unsigned rows = std::atoi(rows_str.c_str());
    unsigned cols = std::atoi(cols_str.c_str());

    Tile** tiles = new Tile * [rows] {};

    for (size_t i = 0; i < rows; i++) {
      tiles[i] = new Tile[cols]{};
    }

    size_t index = 0;

    // load all tiles
    while(std::getline(file, line))
    {
      std::stringstream linestream(line);
      std::string value;

      while (getline(linestream, value, ',') && static_cast<unsigned>(index) < rows*cols)
      {
        size_t row = index / cols;
        size_t col = index % cols;

        Tile& tile = tiles[row][col];

        bool is_integer = !value.empty() && std::find_if(value.begin(),
          value.end(), [](unsigned char c) { return !std::isdigit(c); }) == value.end();

        if (is_integer) {
          unsigned val = std::atoi(value.c_str());
          tile.ID = val;

          if (val != 0) {
            tile.solid = false;
          }
        }
        else {
          tile.ID = 1;
          tile.solid = false;
        }

        tile.token = value;

        index++;
      }
    }

    // default tileset code for now:
    auto texture = TEXTURES.LoadTextureFromFile("resources/ow/basic_tileset.png");
    sf::Sprite blue, green, pink;
    blue.setTexture(*texture);
    blue.setTextureRect(sf::IntRect{0, 0, 47, 30});
    blue.setOrigin(sf::Vector2f(23, 5));
    green.setTexture(*texture);
    green.setTextureRect(sf::IntRect{47, 0, 47, 30});
    green.setOrigin(sf::Vector2f(23, 5));
    pink.setTexture(*texture);
    pink.setTextureRect(sf::IntRect{94, 0, 47, 30});
    pink.setOrigin(sf::Vector2f(23, 5));

    Map::Tileset tileset{};
    tileset.Register(1, blue);
    tileset.Register(2, green);
    tileset.Register(3, pink);

    map.Load(tileset, tiles, cols, rows);
    map.SetName(name);

    return true;
  }
  const std::string& Map::GetName() const
  {
    return name;
  }

  void Map::SetName(const std::string& name)
  {
    this->name = name;
  }
}
