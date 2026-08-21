
#include <iostream>
#include <string>

#include "graph.h"
#include "coloring_algotithms.h"

int main(int argc, char* argv[]) {
    if (argc < /*3*/ 2) {
        std::cerr << "Arguments expected..." << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    //int threads = std::stoi(argv[2]);
    /*auto paths = std::filesystem::directory_iterator(argv[1]);
    for (auto file_path: paths) {
        filename = file_path.path().string();
        const Graph graph(filename);  // конструктор открывает файл по этому пути
        graph.Represent();

        std::cout << "For file: " + filename << std::endl;
        int root = 1;
        auto target = algorithms::GreedyBFS(graph, root).numColors;
        std::cout << "target: " << target << " with root: "<< root << std::endl;
        while (root < graph.V()) {
            root*=1;
            auto bfs_coloring = algorithms::GreedyBFS(graph, root);
            std::cout << "BFS with root = " << root << std::endl;
            bfs_coloring.Represent();
            assert(!HasConflicts(graph, bfs_coloring.color));
            assert(target == bfs_coloring.numColors);
            std::cout << std::endl;
        }
    }*/

    const Graph graph(filename);  // конструктор открывает файл по этому пути
    std::cout << "finished graph parsing..." << std::endl;
    //graph.Represent();

    std::cout << "For file: " + filename << std::endl;
    int root = 1;
    // int multiplier = 10;
    std::cout << "Alg with root = " << root << std::endl;
    auto bfs_coloring = algorithms::RLF(graph);
    bfs_coloring.Represent();
    assert(!HasConflicts(graph, bfs_coloring.color));
    //assert(target == bfs_coloring.numColors);
    std::cout << "Bounds:" << std::endl;
    std::cout << "lower_bound_fraction: " << chrom_num_bounds::lower_bound_fraction(graph) << std::endl;
    std::cout << "upper_bound_sqrt: " << chrom_num_bounds::upper_bound_sqrt(graph) << std::endl;
    std::cout << "upper_bound_degree: " << chrom_num_bounds::upper_bound_degree(graph) << std::endl;
    std::cout << "upper_bound_max_min: " << chrom_num_bounds::upper_bound_max_min(graph) << std::endl;

    std::cout << std::endl;
    root+=1;

    /*auto straight_coloring = algorithms::GreedyForward(graph);
    std::cout << "Straight: " << std::endl;
    straight_coloring.Represent();
    std::cout << std::endl;

    auto smart_coloring = algorithms::GreedySortedDESC(graph);
    std::cout << "DESC: " << std::endl;
    smart_coloring.Represent();
    std::cout << std::endl;

    auto dumb_coloring = algorithms::GreedySortedASC(graph);
    std::cout << "ASC: " << std::endl;
    dumb_coloring.Represent();
    std::cout << std::endl;*/
    return 0;
}
