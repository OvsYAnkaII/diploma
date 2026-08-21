#include <random>
#include <fstream>
#include <vector>
#include <set>
#include <iostream>

// --------------------------------------------------------------
// 1. Генератор случайных графов (основной)
// --------------------------------------------------------------
void generateRandomGraph(int V, double density, const std::string& filename, int seed = 42) {
    std::mt19937 gen(seed);
    std::uniform_real_distribution<> dis(0.0, 1.0);
    std::set<std::pair<int,int>> edges;
    
    for (int i = 0; i < V; ++i) {
        for (int j = i+1; j < V; ++j) {
            if (dis(gen) < density) {
                edges.insert({i+1, j+1});
            }
        }
    }
    
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "Cannot create file: " << filename << std::endl;
        return;
    }
    out << "p edge " << V << " " << edges.size() << "\n";
    for (auto [u, v] : edges) {
        out << "e " << u << " " << v << "\n";
    }
    std::cout << "Generated: " << filename << " (V=" << V << ", E=" << edges.size() << ")\n";
}

// --------------------------------------------------------------
// 2. Граф с «вилкой» (путь с дополнительными рёбрами)
// --------------------------------------------------------------
void generateForkGraph(int V, const std::string& filename) {
    // Строим путь 1-2-3-...-V и добавляем рёбра между «дальними» вершинами
    std::set<std::pair<int,int>> edges;
    for (int i = 1; i < V; ++i) {
        edges.insert({i, i+1});
    }
    // Добавляем рёбра, чтобы создать конфликты для BFS
    for (int i = 1; i <= V/3; ++i) {
        edges.insert({i, i + V/2});
        edges.insert({i + V/3, i + 2*V/3});
    }
    
    std::ofstream out(filename);
    if (!out) return;
    out << "p edge " << V << " " << edges.size() << "\n";
    for (auto [u, v] : edges) {
        out << "e " << u << " " << v << "\n";
    }
}

// --------------------------------------------------------------
// 3. Граф с двумя «центрами» (две звезды, соединённые ребром)
// --------------------------------------------------------------
void generateDoubleStar(int V, const std::string& filename) {
    int half = V / 2;
    std::set<std::pair<int,int>> edges;
    // Центры: 1 и half+1
    for (int i = 2; i <= half; ++i) {
        edges.insert({1, i});
    }
    for (int i = half+2; i <= V; ++i) {
        edges.insert({half+1, i});
    }
    // Соединяем центры
    edges.insert({1, half+1});
    
    std::ofstream out(filename);
    if (!out) return;
    out << "p edge " << V << " " << edges.size() << "\n";
    for (auto [u, v] : edges) {
        out << "e " << u << " " << v << "\n";
    }
}

// --------------------------------------------------------------
// Главная функция
// --------------------------------------------------------------
int main() {
    std::string parent = "/Users/ovsyanka/Desktop/back_to_practicum/summer_practicum_kick_in/diploma/data/my_tiny_graphs/counterexamples/";
    // 1. Случайные графы (10 штук)
    for (int n = 10; n <= 50; n += 10) {
        for (double p : {0.2, 0.4, 0.6}) {
            generateRandomGraph(n, p, parent+"random_" + std::to_string(n) + "_" + std::to_string((int)(p*10)) + ".col");
        }
    }
    
    // 2. Графы с вилкой
    for (int n = 20; n <= 50; n += 10) {
        generateForkGraph(n, parent+"fork_" + std::to_string(n) + ".col");
    }
    
    // 3. Двойные звёзды
    for (int n = 20; n <= 50; n += 10) {
        generateDoubleStar(n, parent+"dstar_" + std::to_string(n) + ".col");
    }
    
    std::cout << "All graphs generated!\n";
    return 0;
}