#include <iostream>
#include <cfloat>
#include <cmath>

int main() {
    double price = 25.50;
    int quantity = 5;
    
    std::cout << "DBL_MAX = " << DBL_MAX << std::endl;
    std::cout << "DBL_MAX / quantity = " << (DBL_MAX / quantity) << std::endl;
    std::cout << "price = " << price << std::endl;
    std::cout << "price > DBL_MAX / quantity? " << (price > DBL_MAX / quantity) << std::endl;
    
    // Check if multiplication would overflow
    if (quantity > 0 && price > DBL_MAX / quantity) {
        std::cout << "Would overflow!" << std::endl;
    } else {
        std::cout << "Safe to multiply" << std::endl;
        std::cout << "Result: " << (price * quantity) << std::endl;
    }
    
    return 0;
}
