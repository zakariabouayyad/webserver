#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include "../server.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

class server;

class Config {
        Config();
        std::string         path_name;

        bool isWhitespace(const std::string& str);
        std::string throwmessage(int number, const std::string& str);
        void    parse();
        void    parse_server(std::ifstream &ConfFile);
        bool    check_ext();
        void    defaultCheck();
        void    dublesrvcheck();
        void    smSoServersInit();
    public:
        std::vector<server> Servers;
        Config(int ac, char **av);
        ~Config();
		void    display();
};

#endif // CONFIG_HPP
