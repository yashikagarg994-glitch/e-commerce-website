#include<iostream>
using namespace std;

class Student {
public:
    int age;

    Student(int a) {
        age = a;
    }
};

int main() {
    Student s1(18);
}