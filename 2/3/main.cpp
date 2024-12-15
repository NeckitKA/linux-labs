#include <iostream>
#include <random>
#include "CLI11.hpp"
using namespace std;

int main(int argc, char * argv[])
{
    CLI::App app{"Мое приложение"};   
    int quantity = 0;
    int min = 0;
    int max = 100;
    std::random_device rd;
    std::mt19937 gen(rd());
    app.add_option("-q,--quantity", quantity, "Количество");  
    app.add_option("-m,--min", min, "Минимум");  
    app.add_option("-M,--max", max, "Максимум");
    CLI11_PARSE(app, argc, argv);
    std::uniform_int_distribution<> distr(min, max);
    for (int i = 0; i < quantity; ++i) {
       int random_number = distr(gen);
       std::cout << random_number << std::endl;        
    }      
    return 0;
}
