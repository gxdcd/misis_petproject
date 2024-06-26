#include <vector>

int sum_integers(const std::vector<int> integers)
{
    auto sum = 0;
    for (auto i: integers) {
        sum += i;
    }
    return sum;
}

int main(int argc, char** argv)
{
    auto integers = {1, 2, 3, 4, 5};

    if (sum_integers(integers) == 15) {
        return 0;
    } else {
        return 1;
    }
}