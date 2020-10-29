

#include <QTcpSocket>
#include <QtNetwork>

#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <stdio.h>

#include <chrono>
#include <thread>

/***********************************************************************************************************************
 * Definition
 **********************************************************************************************************************/

// You can also send the packets to some other machine. Right now, functionality only exists for sending them to localhost.
#define FLIGHTGEAR_UDP_PORT 5500 // this is also defined in the flightgear startup script. If you change it here, you'll have to change it there to

#define TIME_BETWEEN_PACKETS_MS 40  // Flight gear is set to refresh every 33 ms (configurable in the launch script for fg), anything slower than that should be fine to use. Anything faster, and it will drop packets.

/***********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************/

static int getNumDatapoints(void);
static void fillPackets(char** packets, int numPackets);
static void sendPacketsToFlightGear(char** packets, int numPackets);

/***********************************************************************************************************************
 * Code
 **********************************************************************************************************************/

int main(void)
{

    int numDataPoints = getNumDatapoints();

    char **packets = new char*[numDataPoints];

    if (packets == NULL)
    {
        std::cerr << "System Out of memory, failed to create packet array" << std::endl;
        return -1; 
    }
    
    for(int i = 0; i < numDataPoints; i++)
    {
        packets[i] = new char[100];

        if (packets[i] == NULL)
        {
            std::cerr << "System Out of memory, failed to create packet array" << std::endl;
            return -1; 
        }
    }

    fillPackets(packets, numDataPoints);

    sendPacketsToFlightGear(packets, numDataPoints);

    return 0;
}

static int getNumDatapoints(void)
{
    std::ifstream dataFile;
    std::string line, previousLine;
    std::string returnChar ("\n");
    int i = -1;

    dataFile.open("SimulationBuild/SensorOutputs/roll.txt");

    do
    {
        getline(dataFile, line);
        i++;
    }
    while (!line.empty());

    dataFile.close();

    return i;
}

static void fillPackets(char** packets, int numPackets)
{
    std::ifstream rollFile;
    std::string rollLine;
    std::ifstream pitchFile;
    std::string pitchLine;
    std::ifstream yawFile;
    std::string yawLine;

    std::ifstream latFile;
    std::string latLine;
    std::ifstream longFile;
    std::string longLine;
    std::ifstream altitudeFile;
    std::string altitudeLine;

    rollFile.open("SimulationBuild/SensorOutputs/roll.txt");
    pitchFile.open("SimulationBuild/SensorOutputs/pitch.txt");
    yawFile.open("SimulationBuild/SensorOutputs/yaw.txt");
    latFile.open("SimulationBuild/SensorOutputs/lattitude.txt");
    longFile.open("SimulationBuild/SensorOutputs/longitude.txt");
    altitudeFile.open("SimulationBuild/SensorOutputs/altitude.txt");

    for(int i = 0; i < numPackets; i++)
    {
        getline(rollFile, rollLine);
        getline(pitchFile, pitchLine);
        getline(yawFile, yawLine);
        getline(latFile, latLine);
        getline(longFile, longLine);
        getline(altitudeFile, altitudeLine);


        sprintf(packets[i], "%.2f,%.6f,%.6f,%.2f,%.2f,%.2f,\n", std::stof(altitudeLine), std::stof(latLine), std::stof(longLine), std::stof(rollLine), std::stof(pitchLine), std::stof(yawLine));

    }

    rollFile.close();
    pitchFile.close();
    yawFile.close();
    latFile.close();
    longFile.close();
    altitudeFile.close();
}

static void sendPacketsToFlightGear(char** packets, int numPackets)
{
    QUdpSocket *socket = new QUdpSocket();
    QHostAddress ipaddr = QHostAddress::LocalHost;

    for (int i = 0; i < numPackets; i++)
    {
        socket->writeDatagram(QByteArray((char*)packets[i], strlen(packets[i])), ipaddr, FLIGHTGEAR_UDP_PORT);

        std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }
}
