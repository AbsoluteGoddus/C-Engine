//
// Created by absol on 10/3/2024.
//

#ifndef C_ENGINE_BIN_HPP
#define C_ENGINE_BIN_HPP

#include <stack>
#include <fstream>

using byte = unsigned char;

template <typename T>
class unstack_t {
public:
    void push_back(T val) {
        data_.push_back(val);
    }

    T pop_back() {
        if (data_.empty()) return -1;
        int val = data_.back();
        data_.pop_back();
        return val;
    }

    T top() {
        if (data_.empty()) return -1;
        return data_.back();
    }

    void push_front(T val) {
        data_.push_front(val);
    }

    T pop_front() {
        if (data_.empty()) return -1;
        int val = data_.front();
        data_.pop_front();
        return val;
    }

    T bottom() {
        if (data_.empty()) return -1;
        return data_.front();
    }

    size_t size() {
        return data_.size();
    }

private:
    std::deque<T> data_;
};

/**
 * Class to convert fixed size types (int, float ect.) to binary, & to save that binary into a file.
 * can also read files & cast them back into their original types.
 * can also cast some dynamic size types into binary (currently supported are: const char* & std::vector<T>())
 * if you are pushing a vector, the elements need to be fixed size!!!
 */
class BinBuffer {
private:
    unstack_t<byte> s;
public:
    void push(byte b);
    byte pop();

    void push_d(const char* str);

    template<typename T>
    void push_d(std::vector<T> v);

    template<typename T>
    std::vector<T> pop_d();

    template<typename T>
    void push(T val);

    // pops without removing the top of the stack
    byte pop_s();

    size_t size();

    BinBuffer operator<<(BinBuffer o);
    void operator+=(BinBuffer o);

    void write(std::string path);

    void read(std::string path);

    template<typename T>
    T cast_back();

    const char* castC_str();
};

size_t BinBuffer::size() {
    return s.size();
}

void BinBuffer::push(byte b) {
    s.push_back(b);
}

byte BinBuffer::pop_s() {
    return s.bottom();
}

byte BinBuffer::pop() {
    return s.pop_front();
}

BinBuffer BinBuffer::operator<<(BinBuffer o) {
    BinBuffer tmp;
    for (int i = 0; i < this->size(); i++) {
        tmp.push(this->pop());
    }
    for (int i = 0; i < o.size(); i++) {
        tmp.push(o.pop());
    }
    return tmp;
}

void BinBuffer::operator+=(BinBuffer o) {
    for (int i = 0; i < o.size(); ++i) {
        this->push(o.pop());
    }
}

void BinBuffer::write(std::string path) {
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) { // Check if the file is open before trying to write
        throw std::runtime_error("Unable to create file! bin.hpp:70");
    }

    file.clear();

    while (this->size() > 0) {
        file << s.pop_front();
    }

    file.close();
}

void BinBuffer::read(std::string path) {
    std::ifstream file(path, std::ios::binary);
    /*if (!file.is_open()) {
        throw std::runtime_error("Unable to open file! bin.hpp:85");
    }*/

    file.seekg(0, std::ios::beg); // Move the file pointer to the beginning of the file

    byte b;
    while (file.read(reinterpret_cast<char*>(&b), sizeof(b))) {
        this->push(b);
    }
    file.close();
}

void BinBuffer::push_d(const char *str) {
    size_t idx = 0;
    while (str[idx] != '\0') {
        this->push(str[idx]);
        idx++;
    }
    this->push(str[idx]);
}

const char* BinBuffer::castC_str() {
    std::string tmp;
    byte curr;
    curr = s.pop_front();
    while (curr != '\0') {
        tmp += curr;
        curr = s.pop_front();
    }
    return tmp.c_str();
}

template<typename T>
std::vector<T> BinBuffer::pop_d() {
    std::vector<T> tmp;
    byte b;
    b = this->pop_s();
    while (b != '\0') {
        tmp.push_back(this->cast_back<T>());
        b = this->pop_s();
    }
    this->pop();
    return tmp;
}

template<typename T>
void BinBuffer::push_d(std::vector<T> v) {
    for (int i = 0; i < v.size(); ++i) {
        this->push<T>(v[i]);
    }
    this->push('\0');
}



template<typename T>
T BinBuffer::cast_back() {
    size_t required = sizeof(T);
    T result;

    for (int i = 0; i < required; ++i) {
        byte b = s.pop_front();
        reinterpret_cast<byte*>(&result)[i] = b;
    }

    return result;
}

template<typename T>
void BinBuffer::push(T val) {
    size_t required = sizeof(T);
    for (int i = 0; i < required; ++i) {
        s.push_back((byte)(((byte*)(&val))[i]));
    }
}

#endif //C_ENGINE_BIN_HPP
