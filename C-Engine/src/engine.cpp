#include <string>
#include <fstream>
#include <CFG.hpp>
#include <task.hpp>
#include <iostream>
#include <cxxabi.h>
#include <stdexcept>
#include <filesystem>

#include <utility>

using _av = std::vector<std::any>;

std::vector<std::any> av(std::any v) {
    return std::any_cast<std::vector<std::any>>(v);
}

class TPH {
private:
    std::string _p;
    task& _t;
public:
    TPH(std::string process, task& t) : _p(std::move(process)), _t(t) {
        t.setToutProcess(t.getToutProcess() + _p);
    }

    ~TPH() {
        _t.setToutProcess(_t.getToutProcess().substr(0, _t.getToutProcess().size() - _p.size()));
    }
};

std::string getFile(const std::ifstream& file) {
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

std::string demangle(const char* name) {
    int status = -1;
    std::unique_ptr<char[], void(*)(void*)> res {
            abi::__cxa_demangle(name, NULL, NULL, &status),
            std::free
    };
    return (status == 0) ? res.get() : name;
}

void createData(task& t, std::any args) {
    t.tout("Creating Data.cfg...");
    std::ofstream file("C:/C-Engine/Data.cfg");
    if (!file.is_open()) {
        t.tout("Failed to create Data.cfg");
        throw std::runtime_error("Failed to create Data.cfg");
    }
    t.tout("Data.cfg created successfully");
}

void getData(task& t, std::any args) {
#ifndef NDEBUG
    {
        TPH tph("[DEBUG] ", t);
        t.tout("Supplied args.type: " + demangle(args.type().name()));
        t.tout("Required args.type: std::pair<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, Node&> / std::pair<std::string, Node&>");
    }
#endif
    if (demangle(args.type().name()) != "std::pair<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, Node&>") {
        t.tout("Invalid arguments passed to getData");
        throw std::runtime_error("Invalid arguments passed to getData");
    }

    t.tout("Getting Data.cfg...");
    std::ifstream file("C:/C-Engine/Data.cfg");
    if (!file.is_open()) {
        t.tout("Failed to open Data.cfg");
        t.tout("Creating Data.cfg...");
        task CreateData;
        CreateData.setID(0x0000); // Set ID to 0x0000 for ERROR:FIX
        CreateData.setName("Create Data");
        CreateData.setFunction(createData);
        CreateData.tRun();
        file.open("C:/C-Engine/Data.cfg");
    }

    Node n;
    engine::cfg_parse(getFile(file), n);

    std::any_cast<std::pair<std::string, Node&>>(args).second = n;
}

void getUsr(task& t, std::any args) {
#ifndef NDEBUG
    {
        TPH tph("[DEBUG] ", t);
    }
#endif
    auto it = t.getChildArgs().find("Get Data");
    if (it == t.getChildArgs().end()) {
        t.tout("Required task Get Data not found");
        throw std::runtime_error("Required task Get Data not found");
    }

    auto cd = std::any_cast<std::pair<std::string, Node&>>(t.getChildArgs()["Get Data"].second);
    Node data = cd.second;
    t.tout("Getting User (Local)...");
    if (!data["Users"].contains("Number") || std::any_cast<int>(av(data["Users"]("Number"))[0]) == 0) {
        t.tout("No users found");
        t.tout("Register New User? (y / n)");
        std::cout << "> ";
        std::string res;
        std::getline(std::cin, res);
        if (res == "y") {
            t.tout("Registering New User...");
            Node newUser;
            t.tout("Please enter your local name:");
            std::cout << "> ";
            std::getline(std::cin, res);
            newUser.setName(res);
            t.tout("Please enter your name");
            std::cout << "> ";
            std::getline(std::cin, res);
            newUser.pushValue("Name", _av{res});
            t.tout("Generating User ID...");
            newUser.pushValue("ID", _av{0});
            t.tout("User ID: " + std::to_string(data["Users"].size() - 1));
            t.tout("Pushing User to Data.cfg...");
            data["Users"] << newUser;
            t.tout("Updating User Count...");
            if (!data["Users"].contains("Number")) {
                data["Users"].pushValue("Number", _av{1});
            } else {
                data["Users"]("Number") = _av{1};
            }
            t.tout("Successfully Updated User Count");
            t.tout("Opening Data.cfg...");

            std::ofstream file("C:/C-Engine/Data.cfg", std::ios::out | std::ios::trunc);
            if (!file.is_open()) {
                t.tout("Failed to open Data.cfg");
                throw std::runtime_error("Failed to open Data.cfg");
            }

#ifndef NDEBUG
            {
                TPH tph("[DEBUG] ", t);
                t.tout("Data: " + engine::cfg_serialize_(data, 0));
            }
#endif
            file << engine::cfg_serialize_(data, 0);
            file.flush(); // Ensure data is written to the file
            file.close(); // Close the file after writing
            t.tout("User Registered Successfully");
            t.tout("Please restart engine.exe");
            exit(0);
        } else {
            t.tout("Exiting...");
            exit(0);
        }
    } else if (std::any_cast<int>(av(data["Users"]("Number"))[0]) == 1) {
        t.tout("User found");
        for (auto usr : data["Users"].getSubNodes()) {
            t.tout("User ID: " + std::to_string(std::any_cast<int>(av(usr.second("ID"))[0])));
            t.tout("User Name: " + std::any_cast<std::string>(av(usr.second("Name"))[0]));
        }
    } else {
        t.tout("Multiple users found");
        t.tout("Please select a user:");
        for (size_t i = 0; i < std::any_cast<int>(av(data["Users"]("Number"))[0]); ++i) {
            t.tout("[" +  + "] " + std::any_cast<std::string>(av(data["Users"]("Name"))[i]));
        }
        t.tout("Please enter the local user Name:");
        std::cout << "> ";
        std::string res;
        std::getline(std::cin, res);
        t.tout("Setting Current User to: " + res + "...");
        args = data["Users"][res];
    }

}

int main() {
    Node data;
    task GetData;
    GetData.setID(0x0001);
    GetData.setName("Get Data");
    GetData.setFunction(getData);
    GetData.setArgs(std::pair<std::string, Node&>("C:/C-Engine/Data.cfg", data));

    task GetUsrLocal;
    GetUsrLocal.setID(0x0002);
    GetUsrLocal.setName("Get User (Local)");
    GetUsrLocal.setFunction(getUsr);
    GetUsrLocal.setRequiredTasks({&GetData});

    try {
        GetUsrLocal.tRun();
    } catch (std::exception& e) {
        std::cerr << e.what() << "\n";
    }

    system("pause");

    return 0;
}