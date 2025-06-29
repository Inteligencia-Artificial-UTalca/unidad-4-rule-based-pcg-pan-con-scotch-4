#include <iostream>
#include <vector>
#include <random>   // For random number generation
#include <chrono>   // For seeding the random number generator

// Define Map as a vector of vectors of integers.
// You can change 'int' to whatever type best represents your cells (e.g., char, bool).
using Map = std::vector<std::vector<int>>;

/**
 * @brief Prints the map (matrix) to the console.
 * @param map The map to print.
 */
void printMap(const Map& map) {
    std::cout << "--- Current Map ---" << std::endl;
    for (const auto& row : map) {
        for (int cell : row) {
            std::cout << (cell == 1 ? "#" : " ") << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "-------------------" << std::endl;
}
/**
 * @brief Function to implement the Cellular Automata logic.
 * It should take a map and return the updated map after one iteration.
 * @param currentMap The map in its current state.
 * @param W Width of the map.
 * @param H Height of the map.
 * @param R Radius of the neighbor window (e.g., 1 for 3x3, 2 for 5x5).
 * @param U Threshold to decide if the current cell becomes 1 or 0.
 * @return The map after applying the cellular automata rules.
 */
Map cellularAutomata(const Map& currentMap, int W, int H, int R, double U) {
    Map newMap = currentMap; // Copia del mapa actual
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 gen(seed);
    std::uniform_real_distribution<> distProb(0.0, 1.0);

    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            // Contar vecinos con valor 1 en la ventana de radio R
            int countOnes = 0;
            for (int di = -R; di <= R; ++di) {
                for (int dj = -R; dj <= R; ++dj) {
                    int ni = i + di;
                    int nj = j + dj;
                    if (ni >= 0 && ni < H && nj >= 0 && nj < W) {
                        countOnes += currentMap[ni][nj];
                    }
                }
            }
            // Aplicar regla
            double neighborRatio = static_cast<double>(countOnes) / ((2 * R + 1) * (2 * R + 1));
            newMap[i][j] = (neighborRatio > U) ? 1 : 0;
        }
    }

    return newMap;
}

/**
 * @brief Function to implement the Drunk Agent logic.
 * It should take a map and parameters controlling the agent's behavior,
 * then return the updated map after the agent performs its actions.
 *
 * @param currentMap The map in its current state.
 * @param W Width of the map.
 * @param H Height of the map.
 * @param J The number of times the agent "walks" (initiates a path).
 * @param I The number of steps the agent takes per "walk".
 * @param roomSizeX Max width of rooms the agent can generate.
 * @param roomSizeY Max height of rooms the agent can generate.
 * @param probGenerateRoom Probability (0.0 to 1.0) of generating a room at each step.
 * @param probIncreaseRoom If no room is generated, this value increases probGenerateRoom.
 * @param probChangeDirection Probability (0.0 to 1.0) of changing direction at each step.
 * @param probIncreaseChange If direction is not changed, this value increases probChangeDirection.
 * @param agentX Current X position of the agent (updated by reference).
 * @param agentY Current Y position of the agent (updated by reference).
 * @return The map after the agent's movements and actions.
 */
Map drunkAgent(Map& currentMap, int W, int H, int J, int I, int roomSizeX, int roomSizeY,
               double probGenerateRoom, double probIncreaseRoom,
               double probChangeDirection, double probIncreaseChange,
               int& agentX, int& agentY) {
    Map newMap = currentMap;
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 gen(seed);
    std::uniform_int_distribution<> distDirection(0, 3);
    std::uniform_real_distribution<> distProb(0.0, 1.0);

    std::vector<std::pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    double currentProbRoom = probGenerateRoom;
    double currentProbChange = probChangeDirection; // Probabilidad actual para cambiar dirección

    for (int walk = 0; walk < J; ++walk) {
        int currentDirection = distDirection(gen);
        for (int step = 0; step < I; ++step) {
            // Marcar la posición actual como pasillo (1)
            newMap[agentX][agentY] = 1;

            // Generar habitación
            if (distProb(gen) < currentProbRoom) {
                int halfX = roomSizeX / 2;
                int halfY = roomSizeY / 2;
                int startX = std::max(0, agentX - halfX);
                int endX = std::min(H - 1, agentX + halfX);
                int startY = std::max(0, agentY - halfY);
                int endY = std::min(W - 1, agentY + halfY);
                for (int x = startX; x <= endX; ++x) {
                    for (int y = startY; y <= endY; ++y) {
                        newMap[x][y] = 1;
                    }
                }
                currentProbRoom = probGenerateRoom;
            } else {
                currentProbRoom += probIncreaseRoom;
            }

            // Decidir si cambiar de dirección
            if (distProb(gen) < currentProbChange) {
                currentDirection = distDirection(gen);
                currentProbChange = probChangeDirection; // Reiniciar probabilidad
            } else {
                currentProbChange += probIncreaseChange; // Aumentar probabilidad
            }

            // Calcular el siguiente movimiento
            int dx = directions[currentDirection].first;
            int dy = directions[currentDirection].second;
            int nextX = agentX + dx;
            int nextY = agentY + dy;

            // Verificar límites
            if (nextX >= 0 && nextX < H && nextY >= 0 && nextY < W) {
                agentX = nextX;
                agentY = nextY;
            } else {
                currentDirection = distDirection(gen);
                currentProbChange = probChangeDirection; // Reiniciar probabilidad al chocar con el borde
            }
        }
    }

    return newMap;
}

int main() {
    std::cout << "--- CELLULAR AUTOMATA AND DRUNK AGENT SIMULATION ---" << std::endl;

    // Configurar generador de números aleatorios
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 gen(seed);
    std::uniform_int_distribution<> dist01(0, 1);
    std::uniform_int_distribution<> distJ(3, 7); // Rango para J
    std::uniform_int_distribution<> distI(5, 15); // Rango para I
    std::uniform_int_distribution<> distRoomX(3, 7); // Rango para roomSizeX
    std::uniform_int_distribution<> distRoomY(2, 5); // Rango para roomSizeY
    std::uniform_real_distribution<> distProb(0.05, 0.3); // Rango para probabilidades
    std::uniform_real_distribution<> distProbInc(0.01, 0.1); // Rango para incrementos

    // --- Initial Map Configuration ---
    int mapRows = 10;
    int mapCols = 20;
    Map myMap(mapRows, std::vector<int>(mapCols, 0));

    // Inicializar mapa con valores aleatorios
    for (int i = 0; i < mapRows; ++i) {
        for (int j = 0; j < mapCols; ++j) {
            myMap[i][j] = dist01(gen);
        }
    }

    // Drunk Agent's initial position
    int drunkAgentX = mapRows / 2;
    int drunkAgentY = mapCols / 2;

    std::cout << "\nInitial map state:" << std::endl;
    printMap(myMap);

    // --- Simulation Parameters ---
    int numIterations = 5;

    // Cellular Automata Parameters
    int ca_W = mapCols;
    int ca_H = mapRows;
    int ca_R = 1;
    double ca_U = 0.5;

    // --- Main Simulation Loop ---
    for (int iteration = 0; iteration < numIterations; ++iteration) {
        std::cout << "\n--- Iteration " << iteration + 1 << " ---" << std::endl;

        // Generar parámetros aleatorios para Drunk Agent
        int da_J = distJ(gen);
        int da_I = distI(gen);
        int da_roomSizeX = distRoomX(gen);
        int da_roomSizeY = distRoomY(gen);
        double da_probGenerateRoom = distProb(gen);
        double da_probIncreaseRoom = distProbInc(gen);
        double da_probChangeDirection = distProb(gen);
        double da_probIncreaseChange = distProbInc(gen);

        std::cout << "Parámetros Drunk Agent: J=" << da_J << ", I=" << da_I
                  << ", roomSizeX=" << da_roomSizeX << ", roomSizeY=" << da_roomSizeY
                  << ", probRoom=" << da_probGenerateRoom << ", probIncRoom=" << da_probIncreaseRoom
                  << ", probChange=" << da_probChangeDirection << ", probIncChange=" << da_probIncreaseChange
                  << std::endl;

        // Ejecutar simulaciones
        myMap = cellularAutomata(myMap, ca_W, ca_H, ca_R, ca_U);
        myMap = drunkAgent(myMap, ca_W, ca_H, da_J, da_I, da_roomSizeX, da_roomSizeY,
                           da_probGenerateRoom, da_probIncreaseRoom,
                           da_probChangeDirection, da_probIncreaseChange,
                           drunkAgentX, drunkAgentY);

        printMap(myMap);
    }

    std::cout << "\n--- Simulation Finished ---" << std::endl;
    return 0;
}