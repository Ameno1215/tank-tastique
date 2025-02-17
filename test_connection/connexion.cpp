#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdio.h>
#include <string>
#include <unordered_map>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(){
    
    sf::RenderWindow window(sf::VideoMode(1900, 1000), "test connexion");
    int sockfd;
    struct sockaddr_in servaddr;

    if(sockfd = socket(AF_INET, SOCK_DGRAM, 0) < 0){
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    }

}