#include <iostream>
#include <string>
#include <sstream>
#include <exception>
#include <algorithm>

template <class T>
class Array {
protected:
    int _size;
    T* _data = nullptr;

    // Устанавливает значение всх ячеек массива значением value
    void setForEach(T toInt) {
        for (int idx = 0; idx < MAX_SIZE + 1; ++idx)
            _data[idx] = toInt;
    }

    // Глубокое копирование
    void copy(const Array& a) {
        if (this != &a) {
            resize(a.getSize());
            for (int idx = 0; idx < getSize(); ++idx)
                _data[idx] = a[idx];
        }
    }

    bool rangeCheck(int toInt, int begin, int end) const {
        return (begin <= toInt && toInt < end);
    }

    // Индексация, допускающая обращение к областям массива, для которых выделена память, 
    // но которые находятся за пределами размера массива _size.
    // Диапазон доступа - [0, MAX_SIZE + 1)
    T& at(int idx) const {
        if (rangeCheck(idx, 0, MAX_SIZE + 1)) {
            return _data[idx];
        }   
        else {
            throw std::out_of_range("array out of range");
        }
    }

public:
    static const int MAX_SIZE = 30;

    Array(int size = 0) : _size(size) {
        if (_data == nullptr) {
            _data = new T[MAX_SIZE + 1];
        }
        setForEach(0);
    }

    Array(const Array& a) : Array() {
        setForEach(0);
        *this = a;
    }

    ~Array() {
        if (_data != nullptr) {
            delete[] _data;
        }
    }

    int getSize() const {
        return _size;
    }

    int isEmpty() {
        return _size == 0;
    }

    // Изменение размера массива на новый. Если новый размер меньше, чем старый, 
    // часть массива отсекается, отсеченной части устанавливаются нулевые значения
    void resize(int new_size) {
        if (new_size < _size) {
            for (int idx = new_size; idx < _size; ++idx) {
                at(idx) = 0;
            }
        }
        _size = new_size;
    }

    Array* operator=(const Array& a) {
        if (this != &a)
            copy(a);
        return this;
    }

    T& operator[](int idx) const {
        if (rangeCheck(idx, 0, getSize())) {
            return _data[idx];
        }
        else {
            throw std::out_of_range("out of range");
        }
            
    }
};

// Bitstring

class Bitstring : public Array<char>
{
private:
    static const char BASE = 2;
    bool _is_negative = false;

    void removeLeadingZeros() {
        while (at(getSize() - 1) == 0 && getSize() > 1)
            _data[--_size] = 0;;
    }

public:
    // Конструкторы, деструкторы, перегрузка оператора присваивания.
    Bitstring() : Array() {
        _size = 1;
    }

    Bitstring(const Bitstring& a) {
        *this = a;
    }

    ~Bitstring() {}


#pragma region ComparisonOperators

    static int compare(const Bitstring& a, const Bitstring& b) {
        for (int idx = std::max(a.getSize(), b.getSize()) - 1; idx >= 0; --idx)
            if (a._data[idx] > b._data[idx])
                return 1;
            else if (a._data[idx] < b._data[idx])
                return -1;
        return 0;
    }

    friend bool operator==(const Bitstring& a, const Bitstring& b) {
        return compare(a, b) == 0;
    }

    friend bool operator!=(const Bitstring& a, const Bitstring& b) {
        return compare(a, b) != 0;
    }

    friend bool operator>=(const Bitstring& a, const Bitstring& b) {
        return compare(a, b) != -1;
    }

    friend bool operator<=(const Bitstring& a, const Bitstring& b) {
        return compare(a, b) != 1;
    }

    friend bool operator >(const Bitstring& a, const Bitstring& b) {
        return compare(a, b) == 1;
    }

    friend bool operator <(const Bitstring& a, const Bitstring& b) {
        return compare(a, b) == -1;
    }

#pragma endregion

#pragma region MathOperators
   
    friend Bitstring operator*(const Bitstring& a, const short number) {
        Bitstring c;
        for (int index = 0; index < a.getSize(); ++index) {
            c.at(index) += (a.at(index) * number);
            c.at(index + 1) += c.at(index) / BASE;
            c.at(index) %= BASE;
        }
        c._size = (c.at(a.getSize()) == 0 ? a.getSize() : a.getSize() + 1);
        c.removeLeadingZeros();
        return c;
    }

    friend Bitstring operator+(const Bitstring& a, const Bitstring& b) {
        Bitstring c;
        int max = std::max(a._size, b._size);
        for (int index = 0; index < max; ++index) {
            c.at(index) += a.at(index) + b.at(index);
            c.at(index + 1) += c.at(index) / BASE;
            c.at(index) %= BASE;
        }
        if (c.at(Bitstring::MAX_SIZE) != 0)
            throw std::overflow_error("owerflow error");
        c._size = (c.at(max) == 0 ? max : max + 1);
        return c;
    }


    friend Bitstring operator-(const Bitstring& a, const Bitstring& b) {
        Bitstring c;
        if (a < b) {
            c = b - a;
            c._is_negative = true;
            return c;
        }
        for (int index = 0; index < a.getSize() + 1; ++index) {
            c.at(index) += a.at(index) - b.at(index);
            if (c._data[index] < 0) {
                c.at(index) += BASE;
                c.at(index + 1) -= 1;
            }
        }
        c._size = a.getSize();
        c.removeLeadingZeros();
        return c;
    }

    friend Bitstring operator*(const Bitstring& a, const Bitstring& b) {
        Bitstring c;
        for (int index = b.getSize() - 1; index >= 0; --index) {
            c.rightShift(1);
            c = c + (a * b.at(index)); // при переполнении здесь сработает исключение для сложения
        }
        return c;
    }

    friend Bitstring operator/(const Bitstring& a, const Bitstring& b) {
        Bitstring result, sub;
        for (int index = 0; a.getSize() - index >= 0; ++index) {
            sub.rightShift(1);
            sub.at(0) = a.at(a.getSize() - index);
            result.rightShift(1);
            int count_div = 0;
            while (Bitstring::compare(sub, b) != -1) {
                sub = sub - b;
                ++count_div;
            }
            result.at(0) = count_div;
        }
        return result;
    }

#pragma endregion

#pragma region Shifts

    // Сдвиг массива вправо (человеко-читаемое число сдвигается влево). 
    // Эквивалентно операции умножения на 10^x
    Bitstring& rightShift(int x) {
        if (*this == Bitstring())
            return *this;
        for (int idx = getSize() - 1; idx >= 0; --idx)
            at(idx + x) = at(idx);
        for (int idx = 0; idx < x; ++idx)
            at(idx) = 0;
        _size += x;
        return *this;
    }

    // Свиг массива влево (человеко-читаемое число сдвигается вправо)
    Bitstring& leftShift(int x) {
        if (*this == Bitstring())
            return *this;
        for (int idx = 0; idx < getSize(); ++idx)
            at(idx) = at(idx + x);
        for (int idx = getSize() - x; idx < getSize(); ++idx)
            at(idx) = 0;
        _size -= x;
        return *this;
    }

    void cyclicRightShift(int offset) {
        int proccessed = 0;
        int* store = new int[_size];

        for (int i = _size - offset; i < _size; ++i) {
            store[proccessed++] = _data[i];
        }

        for (int i = 0; i < _size - offset; ++i) {
            store[proccessed++] = _data[i];
        }

        for (int i = 0; i < _size; ++i) {
            _data[i] = store[i];
        }

        delete[] store;
    }

#pragma endregion

    Bitstring& operator=(const Bitstring& a) {
        if (this != &a) {
            copy(a);
            _is_negative = a._is_negative;
        }
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& out, const Bitstring& a) {
        for (int i = 0; i < a._size; ++i) {
            out << (int)a._data[a._size - 1 - i] << " ";
        }
        out << a.toInt();
        return out;
    }
    void read(int size) {
        resize(size);
        for (int idx = 0; idx < _size; ++idx) {
            int inputAsInt;
            std::cin >> inputAsInt;
            at(idx) = inputAsInt;
        }
    }

    int toInt() const {
        int summa = 0;
        for (int j = _size - 1; j >= 0; --j)
            summa += (int)std::pow(2, j) * _data[j];
        return summa;
    }

    static void display(Bitstring& h1, Bitstring& h2) {
        std::cout << "Bitstring1 = " << h1 << "\n";
        std::cout << "Bitstring2= " << h2 << "\n";
        std::cout << "Bitstring1 + Bitstring2 = " << h1 + h2 << "\n";
        std::cout << "Bitstring1 - Bitstring2 = " << h1 - h2 << "\n";
        std::cout << "Bitstring1 * Bitstring2 = " << h1 * h2 << "\n";
        std::cout << "Bitstring1 / Bitstring2 = " << h1 / h2 << "\n";
        if (h1 == h2) std::cout << "Bitstring1 = Bitstring2\n";
        if (h1 > h2) std::cout << "Bitstring1 > Bitstring2\n";
        if (h1 < h2) std::cout << "Bitstring1 < Bitstring2\n";
    }

};

enum Actions
{
    Exit = 0, MoveFirst = 1, MoveSecond = 2
};

int main()
{
    int  string_size = 0;

    std::cin >> string_size;

    Bitstring first;
    Bitstring second;

    first.read(string_size);
    second.read(string_size);

    Bitstring::display(first, second);


    int action, offset;
    while (true) {
        std::cin >> action;
        switch (action) {
        case Actions::Exit:
            return 0;
        
        case Actions::MoveFirst:
            std::cin >> offset;
            std::cout << "cycle sdvig " << offset << " Bitstring1\n";
            first.cyclicRightShift(offset);
            Bitstring::display(first, second);
            break;
        
        case Actions::MoveSecond:
            std::cin >> offset;
            std::cout << "cycle sdvig " << offset << " Bitstring2\n";
            second.cyclicRightShift(offset);
            Bitstring::display(first, second);
            break;
        
        default: 
            std::cout << "error!\n"; 
            break;
        }
    }
}
