#include <iostream>
#include <string>

void printFavoriteColorMessage(const std::string& color) {
    std::cout << "Your favorite color is: " << color << std::endl;
}

int main() {
    std::string yourfavoriteColor;


    std::cout << "What is your favorite color? "<< std::endl;
    std::getline(std::cin, yourfavoriteColor); 
    
    printFavoriteColorMessage(yourfavoriteColor);

    return 0;
}

