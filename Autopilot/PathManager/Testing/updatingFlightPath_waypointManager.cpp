//
// Created by Dhruv Rawat on 2020-11-21.
//

#include <iostream>
#include "../Inc/waypointManager.hpp"

void append_single_test(WaypointManager * w);
void append_multiple_test(WaypointManager * w);
void insert_first_index_test(WaypointManager * w);
void insert_last_index_test(WaypointManager * w);
void insert_test(WaypointManager * w);
void delete_first_test(WaypointManager * w);
void delete_last_test(WaypointManager * w);
void delete_test(WaypointManager * w);
void update_test(WaypointManager * w);

int id_array[500];
int nextElement = 0;

void printIDArray() {
    for(int i = 0; i < 100; i++) {
        std::cout << i << ":     " << id_array[i] << std::endl;
    }
}

void printFullArray(WaypointManager * w) {
    _PathData ** array = w->get_waypoint_buffer(); // Gets waypoint buffer!
    _WaypointBufferStatus prev_status;
    _WaypointBufferStatus next_status;
    _WaypointBufferStatus current_status;

    for (int i = 0; i < PATH_BUFFER_SIZE; i++) {

        // Prints data if waypoint is defined
        std::cout << "------------------------------------------------------------------------------------" << std::endl;

        //Prints data
        std::cout << i << std::endl;
        std::cout << "Waypoint ID: " << array[i]->waypointId << std::endl;
        std::cout << "WaypointType: " << array[i]->waypointType << std::endl;
        std::cout << "Waypoint Longitude: " << array[i]->longitude << std::endl;
        std::cout << "Waypoint Latitude: " << array[i]->latitude << std::endl;

        current_status = w->get_status_of_index(i);

        if (current_status == FULL) { // If waypoint is defined, then it will also print longitude of next waypoint if applicable
            if (i != 0) {
                prev_status = w->get_status_of_index(i-1);
//                std::cout << "Prev: " << prev_status  << std::endl;
            }
            if (i != PATH_BUFFER_SIZE - 1) {
                next_status = w->get_status_of_index(i+1);
//                std::cout << "Next: " << next_status  << std::endl;
            }

            if (i != 0 && prev_status == FULL) {
                std::cout << "previous long: " << array[i]->previous->longitude << std::endl;
            }
            if (i != PATH_BUFFER_SIZE - 1 && next_status == FULL) {
                std::cout << "Next long: " << array[i]->next->longitude << std::endl;
            }
            std::cout << "------------------------------------------------------------------------------------" << std::endl;
        }
    }
}

void printArray(WaypointManager * w) {
    _PathData ** array = w->get_waypoint_buffer(); // Gets waypoint buffer!
    _WaypointBufferStatus prev_status;
    _WaypointBufferStatus next_status;
    _WaypointBufferStatus current_status;

    for (int i = 0; i < PATH_BUFFER_SIZE; i++) {
        current_status = w->get_status_of_index(i);

        // Prints data if waypoint is defined
        if (current_status == FULL) {
            std::cout << "------------------------------------------------------------------------------------" << std::endl;

            //Prints data
            std::cout << i << std::endl;
            std::cout << "Waypoint ID: " << array[i]->waypointId << std::endl;
            std::cout << "WaypointType: " << array[i]->waypointType << std::endl;
            std::cout << "Waypoint Longitude: " << array[i]->longitude << std::endl;
            std::cout << "Waypoint Latitude: " << array[i]->latitude << std::endl;


            if (i != 0) {
                prev_status = w->get_status_of_index(i-1);
//                std::cout << "Prev: " << prev_status  << std::endl;

            }
            if (i != PATH_BUFFER_SIZE - 1) {
                next_status = w->get_status_of_index(i+1);
//                std::cout << "Next: " << next_status  << std::endl;
            }

            if (i != 0 && prev_status == FULL) {
                std::cout << "previous long: " << array[i]->previous->longitude << std::endl;
            }
            if (i != PATH_BUFFER_SIZE - 1 && next_status == FULL) {
                std::cout << "Next long: " << array[i]->next->longitude << std::endl;
            }
            std::cout << "------------------------------------------------------------------------------------" << std::endl;
        }
    }
}

void printWaypoint(_PathData * p) {
    std::cout << "\n\nPrinting waypoint:" << std::endl;
    std::cout << "Waypoint ID: " << p->waypointId << std::endl;
    std::cout << "WaypointType: " << p->waypointType << std::endl;
    std::cout << "Waypoint Longitude: " << p->longitude << std::endl;
    std::cout << "Waypoint Latitude: " << p->latitude << std::endl;
}

int main(void) {

    int test = 0;

    //Initializes ID array
    for(int i = 0; i < 500; i++) {
        id_array[i] = 0;
    }

    std::cout << "Creating WaypointManager object" << std::endl;

    WaypointManager * w = new WaypointManager(); // Creates object

    std::cout << "WaypointManager object created\n\nInitializing PathData objects" << std::endl;

    // Creates the initial flight path and home base

    const int numPaths = 50;

    _PathData ** initialPaths = new _PathData*[numPaths];
    _PathData * homeBase = w->initialize_waypoint(1000,500, 5, 0);

    std::cout << "Hold paths:" << std::endl;

    long double longitude = 1;
    long double latitude = 10;
    int altitude = 100;
    int waypointType = 0;

    for(int i = 0; i < numPaths/2; i++) {
        initialPaths[i] = w->initialize_waypoint(longitude, latitude, altitude, waypointType);
        id_array[i] = initialPaths[i]->waypointId;
        nextElement = i+1;
        longitude++;
        latitude++;
        altitude++;
    }


    std::cout << "Circle Paths:" << std::endl;

    waypointType = 2;
    float turnRadius = 10;

    for(int i = numPaths/2; i < numPaths; i++) {
        initialPaths[i] = w->initialize_waypoint(longitude, latitude, altitude, waypointType, turnRadius);
//        std::cout << i << " " << initialPaths[i].waypointId << std::endl;
        id_array[i] = initialPaths[i]->waypointId;
        nextElement = i+1;
        longitude++;
        latitude++;
        altitude++;
        turnRadius++;
    }

    printIDArray();

    std::cout << "PathData objects created\n\nInitializing waypointManager" << std::endl;

    // Initializes waypint maanger with the flight path
    w->initialize_waypoint_manager(initialPaths, numPaths, homeBase);


    std::cout << "-\n-\n-\n-\nOriginal Array:\n-\n-\n-\n-" << std::endl;
    printArray(w);


    std::cout << "waypointManager initialized\n\nConducting update operations" << std::endl;


    /*** COMMENTED OUT TESTS DESIGNED TO FAIL ***/
    append_single_test(w);
    append_multiple_test(w);
//    insert_first_index_test(w);
//    insert_last_index_test(w);
    insert_test(w);
    delete_first_test(w);
    delete_last_test(w);
    delete_test(w);
    update_test(w);

    std::cout << "Freeing Heap" << std::endl;
    w->clear_path_nodes();

    delete[] initialPaths;

    return 0;
}

void append_single_test(WaypointManager * w) {
    std::cout << "Single Appending Test:" << std::endl;

    _PathData * appendSingle = w->initialize_waypoint(1, 2, 3, 1);
    id_array[nextElement] = appendSingle->waypointId;
//    printWaypoint(appendSingle);

    _WaypointStatus e = w->update_path_nodes(appendSingle, APPEND_WAYPONT, 0, 0, 0);

    if (e == UNDEFINED_FAILURE) {
        std::cout << "Did not work!" << std::endl;
        exit(0);
    }
    std::cout << "-\n-\n-\n-\nSingle Appending Array:\n-\n-\n-\n-" << std::endl;
    printArray(w);
//    printIDArray();
    nextElement++;
}

void append_multiple_test(WaypointManager * w) {
    std::cout << "Multiple Appending Test:" << std::endl;

    const int appending_num = 20;

    _PathData * append_waypoint;

    long double longitude = 10.393;
    long double latitude = 100.34;
    int altitude = 3000;
    int waypointType = 1;

    for(int i = 0; i < appending_num; i++) {
        append_waypoint = w->initialize_waypoint(longitude, latitude, altitude, waypointType);
        id_array[nextElement] = append_waypoint->waypointId;
        _WaypointStatus e = w->update_path_nodes(append_waypoint, APPEND_WAYPONT, 0, 0, 0);

        if (e == UNDEFINED_FAILURE) {
            std::cout << "Did not work!" << std::endl;
            exit(0);
        }

        longitude++;
        latitude++;
        altitude++;

        nextElement++;
    }

    std::cout << "-\n-\n-\n-\nMultiple Appending Array:\n-\n-\n-\n-" << std::endl;
    printArray(w);
//    printIDArray();

}

void insert_first_index_test(WaypointManager * w) {
    std::cout << "Inserting first index test" << std::endl;

    int nextId = id_array[0];
    _PathData * insertFirst = w->initialize_waypoint(1, 2, 3, 1);

    _WaypointStatus e = w->update_path_nodes(insertFirst, INSERT_WAYPOINT, 0,  0, nextId);

    if (e == UNDEFINED_FAILURE) {
        std::cout << "Did not work!" << std::endl;
        exit(0);
    } else {
        for (int i = 500-2 ; i >= 0; i--) {
            id_array[i+1] = id_array[i];
        }

        id_array[0] = insertFirst->waypointId;
        nextElement++;
    }

    std::cout << "-\n-\n-\n-\nInsert First Element Array:\n-\n-\n-\n-" << std::endl;
    printArray(w);
}

void insert_last_index_test(WaypointManager * w) {
    std::cout << "Inserting Last index test" << std::endl;

    int prevId = id_array[nextElement-2];
    _PathData * insertLast = w->initialize_waypoint(1, 2, 3, 1);

    _WaypointStatus e = w->update_path_nodes(insertLast, INSERT_WAYPOINT, 0, prevId, 0);

    if (e == UNDEFINED_FAILURE) {
        std::cout << "Did not work!" << std::endl;
        exit(0);
    } else {
        for (int i = 500-2 ; i >= 0; i--) {
            id_array[i+1] = id_array[i];
        }

        id_array[0] = insertLast->waypointId;
        nextElement++;
    }

    std::cout << "-\n-\n-\n-\nInsert Last Element Array:\n-\n-\n-\n-" << std::endl;
    printArray(w);
}

void insert_test(WaypointManager * w) {
    std::cout << "Inserting index test" << std::endl;

    int nextIndex = 11;
    int prevIndex = 10;
    _PathData * insert = w->initialize_waypoint(1, 2, 10, 1);

    _WaypointStatus e = w->update_path_nodes(insert, INSERT_WAYPOINT, 0, id_array[prevIndex], id_array[nextIndex]);

    if (e == UNDEFINED_FAILURE) {
        std::cout << "Did not work!" << std::endl;
        exit(0);
    } else {

        for (int i = 500-2 ; i >= 0; i--) {
            id_array[i+1] = id_array[i];
        }

        id_array[nextIndex] = insert->waypointId;


        nextElement++;
    }

    std::cout << "-\n-\n-\n-\nInsert Element Array:\n-\n-\n-\n-" << std::endl;
    printArray(w);
//    printIDArray();
}

void delete_first_test(WaypointManager * w) {

    std::cout << "Deleting First  test" << std::endl;


    int remove_index = 0;
    int id_remove = id_array[remove_index];
    _PathData * temp;
    _WaypointStatus e = w->update_path_nodes(temp, DELETE_WAYPOINT, id_remove, 0, 0);

    if (e == UNDEFINED_FAILURE) {
        std::cout << "Did not work!" << std::endl;
        exit(0);
    } else {

        for (int i = remove_index; i < 500 - 1; i++) {
            id_array[i] = id_array[i+1];
        }

        nextElement--;
    }

    std::cout << "-\n-\n-\n-\nDeleting First Array:\n-\n-\n-\n-" << std::endl;
//    printIDArray();
    printArray(w);
}

void delete_last_test(WaypointManager * w) {
    std::cout << "Deleting test" << std::endl;

    int remove_index = 18;
    int id_remove = id_array[remove_index];
    _PathData * temp;
    _WaypointStatus e = w->update_path_nodes(temp, DELETE_WAYPOINT, id_remove, 0, 0);

    if (e == UNDEFINED_FAILURE) {
        std::cout << "Did not work!" << std::endl;
        exit(0);
    } else {

        for (int i = remove_index; i < 500 - 1; i++) {
            id_array[i] = id_array[i+1];
        }

        nextElement--;
    }

    std::cout << "-\n-\n-\n-\nDeleting Last Array:\n-\n-\n-\n-" << std::endl;
    printArray(w);
//    printIDArray();
}

void delete_test(WaypointManager * w) {
    std::cout << "Deleting Last  test" << std::endl;

    int remove_index = 15;
    int id_remove = id_array[remove_index];
    _PathData * temp;
    _WaypointStatus e = w->update_path_nodes(temp, DELETE_WAYPOINT, id_remove, 0, 0);

    if (e == UNDEFINED_FAILURE) {
        std::cout << "Did not work!" << std::endl;
        exit(0);
    } else {

        for (int i = remove_index; i < 500 - 1; i++) {
            id_array[i] = id_array[i+1];
        }

        nextElement--;
    }

    std::cout << "-\n-\n-\n-\nDeleting Array:\n-\n-\n-\n-" << std::endl;
    printArray(w);
}

void update_test(WaypointManager * w) {

    std::cout << "Updating Test:" << std::endl;

    int update_index = 16;
    int update_id = id_array[update_index];

    std::cout << update_id << std::endl;

    long double longitude = -20.393;
    long double latitude = -10.34;
    int altitude = -400;
    int waypointType = 1;

    _PathData * update_waypoint = w->initialize_waypoint(longitude, latitude, altitude, waypointType);

   _WaypointStatus e = w->update_path_nodes(update_waypoint, UPDATE_WAYPOINT, update_id, 0 ,0);

    if (e == UNDEFINED_FAILURE) {
        std::cout << "Did not work!" << std::endl;
        exit(0);
    } else {
        id_array[update_index] = update_waypoint->waypointId;
    }

    std::cout << "-\n-\n-\n-\nUpdating Array:\n-\n-\n-\n-" << std::endl;
    printArray(w);
//    printIDArray();
}


