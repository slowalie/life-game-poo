// game_of_life.cpp
#include <SFML/Graphics.hpp>
#include <vector>
#include <ctime>
#include <cstdlib>

const int cellSize = 10;
const int gridWidth = 80;
const int gridHeight = 80;

std::vector<std::vector<int>> grid(gridWidth, std::vector<int>(gridHeight));
std::vector<std::vector<int>> nextGrid(gridWidth, std::vector<int>(gridHeight));

void initializeGrid() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    for (int x = 0; x < gridWidth; ++x) {
        for (int y = 0; y < gridHeight; ++y) {
            grid[x][y] = std::rand() % 2;  // 0 dead, 1 alive
        }
    }
}

void clearGrid() {
    for (int x = 0; x < gridWidth; ++x)
        for (int y = 0; y < gridHeight; ++y)
            grid[x][y] = 0;
}

// Count neighbors with wrap-around (toroidal). Remove wrapping if you want edges to be dead.
int countNeighbors(int x, int y) {
    int count = 0;
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue;
            int nx = (x + dx + gridWidth) % gridWidth;
            int ny = (y + dy + gridHeight) % gridHeight;
            count += grid[nx][ny];
        }
    }
    return count;
}

void updateGrid() {
    for (int x = 0; x < gridWidth; ++x) {
        for (int y = 0; y < gridHeight; ++y) {
            int neighbors = countNeighbors(x, y);
            if (grid[x][y] == 1) {
                // live cell: survives with 2 or 3 neighbors
                nextGrid[x][y] = (neighbors == 2 || neighbors == 3) ? 1 : 0;
            } else {
                // dead cell: becomes alive with exactly 3 neighbors
                nextGrid[x][y] = (neighbors == 3) ? 1 : 0;
            }
        }
    }
    // swap nextGrid -> grid
    grid.swap(nextGrid);
}

void renderGrid(sf::RenderWindow &window) {
    window.clear(sf::Color::Black);

    sf::RectangleShape cell(sf::Vector2f(static_cast<float>(cellSize - 1), static_cast<float>(cellSize - 1)));
    // draw alive cells
    for (int x = 0; x < gridWidth; ++x) {
        for (int y = 0; y < gridHeight; ++y) {
            if (grid[x][y] == 1) {
                cell.setPosition(static_cast<float>(x * cellSize), static_cast<float>(y * cellSize));
                cell.setFillColor(sf::Color::Green);
                window.draw(cell);
            }
        }
    }

    // optional: draw grid lines (thin)
    // We can draw lighter rectangles or lines if desired — simple outline via rectangles is ok here.
    sf::RectangleShape line;
    line.setFillColor(sf::Color(60,60,60));
    for (int x = 0; x <= gridWidth; ++x) {
        line.setSize(sf::Vector2f(1.0f, static_cast<float>(gridHeight * cellSize)));
        line.setPosition(static_cast<float>(x * cellSize), 0.0f);
        window.draw(line);
    }
    for (int y = 0; y <= gridHeight; ++y) {
        line.setSize(sf::Vector2f(static_cast<float>(gridWidth * cellSize), 1.0f));
        line.setPosition(0.0f, static_cast<float>(y * cellSize));
        window.draw(line);
    }

    window.display();
}

int main() {
    sf::RenderWindow window(sf::VideoMode(gridWidth * cellSize, gridHeight * cellSize), "Game of Life");
    window.setFramerateLimit(60);

    initializeGrid();

    bool paused = false;
    sf::Clock updateClock;
    sf::Time updateInterval = sf::milliseconds(100); // vitesse de mise à jour

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    paused = !paused; // pause / resume
                } else if (event.key.code == sf::Keyboard::R) {
                    initializeGrid(); // randomize
                } else if (event.key.code == sf::Keyboard::C) {
                    clearGrid(); // clear
                } else if (event.key.code == sf::Keyboard::Right) {
                    // step one generation when paused
                    if (paused) updateGrid();
                } else if (event.key.code == sf::Keyboard::Up) {
                    // accelerate
                    if (updateInterval.asMilliseconds() > 10)
                        updateInterval -= sf::milliseconds(10);
                } else if (event.key.code == sf::Keyboard::Down) {
                    // slow down
                    updateInterval += sf::milliseconds(10);
                }
            }

            // Toggle cell with mouse click (only when paused to avoid fast toggles)
            if (event.type == sf::Event::MouseButtonPressed) {
                if (paused && event.mouseButton.button == sf::Mouse::Left) {
                    int mx = event.mouseButton.x;
                    int my = event.mouseButton.y;
                    int gx = mx / cellSize;
                    int gy = my / cellSize;
                    if (gx >= 0 && gx < gridWidth && gy >= 0 && gy < gridHeight) {
                        grid[gx][gy] = 1 - grid[gx][gy]; // toggle
                    }
                }
            }
        }

        // Update automaton at interval if not paused
        if (!paused && updateClock.getElapsedTime() >= updateInterval) {
            updateGrid();
            updateClock.restart();
        }

        renderGrid(window);
        // small sleep to reduce CPU usage if needed (not required because framerate limit)
        sf::sleep(sf::milliseconds(1));
    }

    return 0;
}
