#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <string>
using namespace std;

//shared variable to see if time has expired yet
atomic<bool> timeExpired(false);

void timerFunction(int durationSeconds) {
    this_thread::sleep_for(chrono::seconds(durationSeconds));
    timeExpired = true;
    cout << "\nTiempo agotado!\n";
}

int main() {
    string respuesta;
    int tiempoLimite = 5;

    cout << R"(     
▗▄▄▄▖ ▗▖ ▗▖▗▄▄▄▖▗▄▄▄▖▗▖  ▗▖    ▗▄▄▄▖ ▗▖ ▗▖▗▄▄▄▖▗▄▄▄▖▗▄▄▖ ▗▄▄▄▖     ▗▄▄▖▗▄▄▄▖▗▄▄▖     ▗▖  ▗▖▗▄▄▄▖▗▖   ▗▖    ▗▄▖ ▗▖  ▗▖ ▗▄▖ ▗▄▄▖ ▗▄▄▄▖ ▗▄▖ 
▐▌ ▐▌ ▐▌ ▐▌  █  ▐▌   ▐▛▚▖▐▌    ▐▌ ▐▌ ▐▌ ▐▌  █  ▐▌   ▐▌ ▐▌▐▌       ▐▌   ▐▌   ▐▌ ▐▌    ▐▛▚▞▜▌  █  ▐▌   ▐▌   ▐▌ ▐▌▐▛▚▖▐▌▐▌ ▐▌▐▌ ▐▌  █  ▐▌ ▐▌
▐▌ ▐▌ ▐▌ ▐▌  █  ▐▛▀▀▘▐▌ ▝▜▌    ▐▌ ▐▌ ▐▌ ▐▌  █  ▐▛▀▀▘▐▛▀▚▖▐▛▀▀▘     ▝▀▚▖▐▛▀▀▘▐▛▀▚▖    ▐▌  ▐▌  █  ▐▌   ▐▌   ▐▌ ▐▌▐▌ ▝▜▌▐▛▀▜▌▐▛▀▚▖  █  ▐▌ ▐▌
▐▙▄▟▙▖▝▚▄▞▘▗▄█▄▖▐▙▄▄▖▐▌  ▐▌    ▐▙▄▟▙▖▝▚▄▞▘▗▄█▄▖▐▙▄▄▖▐▌ ▐▌▐▙▄▄▖    ▗▄▄▞▘▐▙▄▄▖▐▌ ▐▌    ▐▌  ▐▌▗▄█▄▖▐▙▄▄▖▐▙▄▄▖▝▚▄▞▘▐▌  ▐▌▐▌ ▐▌▐▌ ▐▌▗▄█▄▖▝▚▄▞▘
    )" << endl;

    //initialize timer
    thread timerThread(timerFunction, tiempoLimite);

    cout << "Tienes " << tiempoLimite << " segundos para responder:\n";
    cout << "Ingresa tu respuesta: ";

    //while time hasn't expired, read wahtever user puts
    while (!timeExpired) {
        if (cin.peek() != EOF) { //verify if there is an entrance
            getline(cin, respuesta); //read
            break;
        }
    }

    //verify user entry
    if (timeExpired) {
        respuesta = "No ingresaste nada a tiempo.";
    } else {
        cout << "Respuesta recibida: " << respuesta << "\n";
    }

    //wait for thread to finish
    if (timerThread.joinable()) {
        timerThread.join();
    }

    return 0;
}
