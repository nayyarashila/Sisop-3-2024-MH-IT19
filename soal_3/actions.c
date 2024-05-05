#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* gap(float distance) {
    if (distance < 0) return "Invalid distance";
    else if (distance < 3.5) return "Gogogo";
    else if (distance >= 3.5 && distance <= 10) return "Push";
    else return "Stay out of trouble";
}

const char* fuel(char* fuel_input) {
    float fuel_percentage;

    // Cek jika input adalah string persentase
    if (strchr(fuel_input, '%') != NULL) {
        fuel_percentage = atof(fuel_input);
    } 
    // Cek jika input adalah string atau integer
    else if (strchr(fuel_input, '.') == NULL) {
        fuel_percentage = atoi(fuel_input);
    } 
    // Input adalah float
    else {
        fuel_percentage = atof(fuel_input);
    }

    if (fuel_percentage > 80) return "Push Push Push";
    else if (fuel_percentage >= 50 && fuel_percentage <= 80) return "You can go";
    else return "Conserve Fuel";
}

const char* tire(int tire_usage) {
    if (tire_usage < 0) return "Invalid Tire Usage";
    if (tire_usage > 80) return "Go Push Go Push";
    else if (tire_usage >= 50 && tire_usage <= 80) return "Good Tire Wear";
    else if (tire_usage >= 30 && tire_usage < 50) return "Conserve Your Tire";
    else return "Box Box Box";
}

const char* tire_change(char* tire_type) {
    if (strcmp(tire_type, "Soft") == 0) return "Mediums Ready";
    else if (strcmp(tire_type, "Medium") == 0) return "Box for Softs";
    else return "Invalid Tire Type";
}
