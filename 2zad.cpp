#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
using namespace std;

struct Product {
    string name;
    double weight;
    double kcal_per_100g;
    double proteins;
    double fats;
    double carbs;

    double calories() const {
        return (weight * kcal_per_100g) / 100.0;
    }
};

vector<Product> filterProducts(const vector<Product>& products, double maxCalories, double maxCarbs) {
    vector<Product> result;
    for (const auto& product : products) {
        if (product.calories() <= maxCalories && product.carbs < maxCarbs) {
            result.push_back(product);
        }
    }
    return result;
}

void filterInThread(const vector<Product>& products, double maxCalories, double maxCarbs, vector<Product>& results, size_t start, size_t end) {
    auto filteredProducts = filterProducts(vector<Product>(products.begin() + start, products.begin() + end), maxCalories, maxCarbs);
    results.insert(results.end(), filteredProducts.begin(), filteredProducts.end());
}

void processWithThreads(const vector<Product>& products, double maxCalories, double maxCarbs) {
    auto start = chrono::high_resolution_clock::now();

    const int numThreads = 5;
    vector<thread> threads;
    vector<Product> results;

    size_t chunkSize = products.size() / numThreads;

    for (int i = 0; i < numThreads; ++i) {
        size_t startIdx = i * chunkSize;
        size_t endIdx = (i == numThreads - 1) ? products.size() : (i + 1) * chunkSize;

        threads.emplace_back(filterInThread, ref(products), maxCalories, maxCarbs, ref(results), startIdx, endIdx);
    }

    for (auto& thread : threads) {
        thread.join(); // Ожидание завершения всех потоков
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    // Вывод результатов
    cout << "\nСписок продуктов, удовлетворяющих условиям (потоки):\n";
    for (const auto& product : results) {
        cout << "Название: " << product.name 
                  << ", Калории: " << product.calories() 
                  << ", Углеводы: " << product.carbs << "\n";
    }

    cout << "Время выполнения с потоками: " << duration.count() << " секунд\n";
}

void processWithoutThreads(const vector<Product>& products, double maxCalories, double maxCarbs) {
    auto start = chrono::high_resolution_clock::now();

    auto results = filterProducts(products, maxCalories, maxCarbs);

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    // Вывод результатов
    cout << "\nСписок продуктов, удовлетворяющих условиям (без потоков):\n";
    for (const auto& product : results) {
        cout << "Название: " << product.name 
                  << ", Калории: " << product.calories() 
                  << ", Углеводы: " << product.carbs << "\n";
    }

    cout << "Время выполнения без потоков: " << duration.count() << " секунд\n";
}

int main() {
    // Пример данных
    vector<Product> products = {
        {"Яблоко", 150, 52, 0.3, 0.2, 14},
        {"Банан", 120, 89, 1.1, 0.3, 23},
        {"Апельсин", 130, 47, 0.9, 0.1, 12},
        {"Клубника", 100, 32, 0.7, 0.3, 7},
        {"Киви", 75, 61, 1.1, 0.5, 15},
        {"Груша", 180, 57, 0.4, 0.1, 15},
        {"Персик", 150, 39, 0.9, 0.3, 10},
        {"Виноград", 100, 69, 0.6, 0.2, 18},
        {"Арбуз", 300, 30, 0.6, 0.2, 8},
        {"Малина", 100, 52, 1.2, 0.7, 12},
        {"Черника", 100, 57, 0.7, 0.3, 14},
        {"Гречка", 100, 343, 13.3, 3.4, 72},
        {"Рис", 100, 130, 2.7, 0.3, 28},
        {"Макароны", 100, 157, 5.8, 0.9, 31},
        {"Картофель", 100, 77, 2.0, 0.1, 17},
        {"Морковь", 100, 41, 0.9, 0.2, 10}
    };

    double maxCalories = 100.0;
    double maxCarbs = 15.0;

    // Время выполнения без потоков
    processWithoutThreads(products, maxCalories, maxCarbs);

    // Время выполнения с потоками
    processWithThreads(products, maxCalories, maxCarbs);

    return 0;
}
