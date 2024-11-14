#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype> // for isdigit
using namespace std;

class User {
public:
    string fullName, userId, password;

    void registerUser() {
        cout << "Enter Full Name: ";
        cin.ignore(); // Clear input buffer
        getline(cin, fullName);

        // Check if the full name contains any numbers
        if (any_of(fullName.begin(), fullName.end(), ::isdigit)) {
            cout << "Error: Full Name should not contain numbers. Registration failed.\n";
            return; // Exit the function if validation fails
        }

        cout << "Enter User ID: ";
        cin >> userId;
        cout << "Enter Password: ";
        cin >> password;
    }
};

class Room {
public:
    int roomNumber;
    string roomType;
    bool isAvailable;
    string bookedBy;

    Room(int num, string type) : roomNumber(num), roomType(type), isAvailable(true), bookedBy("") {}
};

class HostelManagementSystem {
private:
    vector<User> users;
    vector<Room> rooms;

    void loadUsersFromFile() {
        ifstream infile("users.txt");
        User user;
        while (getline(infile, user.fullName) && infile >> user.userId >> user.password) {
            users.push_back(user);
            infile.ignore(); // Clear newline
        }
        infile.close();
    }

    void saveUserToFile(const User& user) {
        ofstream outfile("users.txt", ios::app);
        outfile << user.fullName << "\n" << user.userId << "\n" << user.password << "\n";
        outfile.close();
    }

    void loadRoomsFromFile() {
        ifstream infile("rooms.txt");
        int roomNumber;
        string roomType;
        bool isAvailable;
        string bookedBy;
        while (infile >> roomNumber >> roomType >> isAvailable) {
            Room room(roomNumber, roomType);
            room.isAvailable = isAvailable;
            if (!room.isAvailable) {
                infile >> bookedBy;
                room.bookedBy = bookedBy;
            }
            rooms.push_back(room);
        }
        infile.close();
    }
    void saveRoomsToFile() {
     ofstream outfile("rooms.txt", ios::trunc); // Open in truncate mode to overwrite the file
    for (const Room& room : rooms) {
        outfile << room.roomNumber << " " << room.roomType << " "
                << room.isAvailable << " ";
        if (!room.isAvailable) {
            outfile << room.bookedBy;
        }
        outfile << endl;
    }
    outfile.close();
}
    void adminAddRoom() {
        int roomNumber;
        string roomType;
        cout << "Enter Room Number: ";
        cin >> roomNumber;
        cout << "Enter Room Type: ";
        cin >> roomType;
        rooms.push_back(Room(roomNumber, roomType));
        saveRoomsToFile();
        cout << "Room added successfully!\n";
    }

    void adminRemoveRoom() {
        int roomNumber;
        cout << "Enter Room Number to remove: ";
        cin >> roomNumber;
        rooms.erase(remove_if(rooms.begin(), rooms.end(),
            [roomNumber](const Room& room) { return room.roomNumber == roomNumber; }),
            rooms.end());
        saveRoomsToFile();
        cout << "Room removed successfully!\n";
    }

    void releaseRoom() {
        string userId, password;
        cout << "Enter your User ID: ";
        cin >> userId;
        cout << "Enter your Password: ";
        cin >> password;

        for (Room& room : rooms) {
            if (room.bookedBy == userId) {
                room.isAvailable = true;
                room.bookedBy = "";
                saveRoomsToFile();
                cout << "Room " << room.roomNumber << " released successfully!\n";
                return;
            }
        }
        cout << "No room found booked under your User ID.\n";
    }

public:
    HostelManagementSystem() {
        loadUsersFromFile();
        loadRoomsFromFile();
    }

    void registerUser() {
        User newUser;
        newUser.registerUser();
        
        // Only save the user if full name validation passed
        if (!any_of(newUser.fullName.begin(), newUser.fullName.end(), ::isdigit)) {
            users.push_back(newUser);
            saveUserToFile(newUser);
            cout << "User registered successfully!\n";
        }
    }

    void bookRoom() {
        cout << "Available Rooms:\n";
        for (const Room& room : rooms) {
            if (room.isAvailable) {
                cout << "Room Number: " << room.roomNumber << ", Type: " << room.roomType << endl;
            }
        }

        int roomNumber;
        cout << "Enter Room Number to book (or enter 0 to release a room): ";
        cin >> roomNumber;

        if (roomNumber == 0) { // Release a room
            releaseRoom();
            return;
        }

        for (Room& room : rooms) {
            if (room.roomNumber == roomNumber && room.isAvailable) {
                string userId, password;
                cout << "Enter your User ID: ";
                cin >> userId;
                cout << "Enter your Password: ";
                cin >> password;

                // Verify user credentials
                for (const User& user : users) {
                    if (user.userId == userId && user.password == password) {
                        room.isAvailable = false;
                        room.bookedBy = userId;
                        saveRoomsToFile();
                        cout << "Room " << roomNumber << " booked successfully!\n";
                        return;
                    }
                }
                cout << "Invalid User ID or Password.\n";
                return;
            }
        }
        cout << "Room not available or invalid room number.\n";
    }

    void adminMenu() {
        string password;
        cout << "Enter Admin Password: ";
        cin >> password;
        if (password != "1234") {
            cout << "Invalid password. Access denied.\n";
            return;
        }

        char choice;
        do {
            cout << "Admin Menu:\n1. Add Room\n2. Remove Room\n3. Exit\n";
            cout << "Enter your choice: ";
            cin >> choice;
            switch (choice) {
                case '1': adminAddRoom(); break;
                case '2': adminRemoveRoom(); break;
                case '3': break;
                default: cout << "Invalid choice. Try again.\n";
            }
        } while (choice != '3');
    }

    void accessAdmin() {
        adminMenu();
    }
};
int main() {
    HostelManagementSystem system;
    string choice;

    do {
        cout << "1. Register\n2. Book/Release a Room\n3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == "1") {
            system.registerUser();
        } else if (choice == "2") {
            system.bookRoom();
        } else if (choice == "3") {
            cout << "Exiting...\n";
        } else if (choice == "A" || choice == "a") {
            system.accessAdmin();
        } else {
            cout << "Invalid choice. Try again.\n";
        }
    } while (choice != "3");

    return 0;
}
