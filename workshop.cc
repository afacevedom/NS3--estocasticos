#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/ipv4-global-routing-helper.h"

#include "ns3/command-line.h"
#include "ns3/config.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/log.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/mobility-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/mobility-model.h"
#include "ns3/internet-stack-helper.h"

#include "ns3/ipv4.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("3 nodos");

CommandLine cmd;
NodeContainer c;
PointToPointHelper pointToPoint;
NetDeviceContainer devices;
MobilityHelper mobility;
Ptr<ListPositionAllocator> positionAlloc;

void setNodeConnections()
{
	NS_LOG_INFO("Set Node Connections.");

	// Cluster Heads
	devices.Add(pointToPoint.Install(c.Get(0), c.Get(1)));
	devices.Add(pointToPoint.Install(c.Get(1), c.Get(2)));
	devices.Add(pointToPoint.Install(c.Get(2), c.Get(3)));
	devices.Add(pointToPoint.Install(c.Get(3), c.Get(4)));
	devices.Add(pointToPoint.Install(c.Get(4), c.Get(5)));

	// T1
	devices.Add(pointToPoint.Install(c.Get(7), c.Get(1)));
	devices.Add(pointToPoint.Install(c.Get(8), c.Get(1)));
	devices.Add(pointToPoint.Install(c.Get(9), c.Get(1)));
	devices.Add(pointToPoint.Install(c.Get(10), c.Get(1)));
	devices.Add(pointToPoint.Install(c.Get(11), c.Get(1)));
	devices.Add(pointToPoint.Install(c.Get(12), c.Get(1)));
	devices.Add(pointToPoint.Install(c.Get(13), c.Get(1)));
	devices.Add(pointToPoint.Install(c.Get(14), c.Get(1)));

	// T2
	devices.Add(pointToPoint.Install(c.Get(15), c.Get(16)));
	devices.Add(pointToPoint.Install(c.Get(15), c.Get(18)));

	// T3
	devices.Add(pointToPoint.Install(c.Get(5), c.Get(20)));
	devices.Add(pointToPoint.Install(c.Get(20), c.Get(6)));
	devices.Add(pointToPoint.Install(c.Get(6), c.Get(21)));
	devices.Add(pointToPoint.Install(c.Get(20), c.Get(21)));
	devices.Add(pointToPoint.Install(c.Get(21), c.Get(22)));
	devices.Add(pointToPoint.Install(c.Get(6), c.Get(23)));
	devices.Add(pointToPoint.Install(c.Get(23), c.Get(24)));
}

void setNodePositions()
{
	NS_LOG_INFO("Set Node Positions.");
	positionAlloc = CreateObject<ListPositionAllocator>();

	// Cluster Heads
	positionAlloc->Add(Vector(6.0, 32.0, 0.0));
	positionAlloc->Add(Vector(17.0, 29.0, 0.0));
	positionAlloc->Add(Vector(29.0, 17.0, 0.0));
	positionAlloc->Add(Vector(36.0, 10.0, 0.0));
	positionAlloc->Add(Vector(52.0, 11.0, 0.0));
	positionAlloc->Add(Vector(58.0, 17.0, 0.0));
	positionAlloc->Add(Vector(56.0, 40.0, 0.0));

	// T1
	positionAlloc->Add(Vector(7.0, 36.0, 0.0));
	positionAlloc->Add(Vector(15.0, 34.0, 0.0));
	positionAlloc->Add(Vector(8.0, 22.0, 0.0));
	positionAlloc->Add(Vector(14.0, 23.0, 0.0));
	positionAlloc->Add(Vector(24.0, 25.0, 0.0));
	positionAlloc->Add(Vector(19.0, 37.0, 0.0));
	positionAlloc->Add(Vector(24.0, 33.0, 0.0));
	positionAlloc->Add(Vector(34.0, 33.0, 0.0));

	// T2
	positionAlloc->Add(Vector(42.0, 5.0, 0.0));
	positionAlloc->Add(Vector(40.0, 8.0, 0.0));
	positionAlloc->Add(Vector(48.0, 5.0, 0.0));
	positionAlloc->Add(Vector(46.0, 8.0, 0.0));
	positionAlloc->Add(Vector(42.0, 15.0, 0.0));

	// T3
	positionAlloc->Add(Vector(53.0, 30.0, 0.0));
	positionAlloc->Add(Vector(71.0, 31.0, 0.0));
	positionAlloc->Add(Vector(67.0, 36.0, 0.0));
	positionAlloc->Add(Vector(61.0, 41.0, 0.0));
	positionAlloc->Add(Vector(60.0, 46.0, 0.0));

	mobility.SetPositionAllocator(positionAlloc);
	mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	mobility.Install(c);
}

void assignIPs()
{
	NS_LOG_INFO("Assign ip addresses.");
	InternetStackHelper ipStack;
	ipStack.Install(c);

	Ipv4AddressHelper ip;
	ip.SetBase("192.168.1.0", "255.255.255.255");
	ip.Assign(devices);
}

void runTestCase(int serverIndex, int clientIndex)
{
	NS_LOG_INFO("Run one test.");

	// Servidor que recibe trafico
	UdpEchoServerHelper echoServer(9);
	ApplicationContainer serverApps = echoServer.Install(c.Get(serverIndex));
	serverApps.Start(Seconds(1.0));
	serverApps.Stop(Seconds(10.0));

	// Se obtiene la IP del Servidor
	Ptr<NetDevice> netDevice = devices.Get(serverIndex);
	Ptr<Node> node = netDevice->GetNode();
	Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();
	Ipv4InterfaceAddress iaddr = ipv4->GetAddress(1, 0);
	Address addr = iaddr.GetLocal();

	// Se asigna la Data del Paquete
	UdpEchoClientHelper echoClient(addr, 9);
	echoClient.SetAttribute("MaxPackets", UintegerValue(1));
	echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
	echoClient.SetAttribute("PacketSize", UintegerValue(1024));

	// Se asigna el Nodo Cliente
	ApplicationContainer clientApps = echoClient.Install(c.Get(clientIndex));

	// Se ejecuta el test
	clientApps.Start(Seconds(1.0));
	clientApps.Stop(Seconds(10.0));
}

int main(int argc, char *argv[]) {
	cmd.Parse(argc, argv);
	Time::SetResolution(Time::NS);
	LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
	LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

	NS_LOG_INFO("Create nodes.");
	c.Create(25);
	// Enlaces
	pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
	pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

	setNodeConnections();

	setNodePositions();

	assignIPs();

	Ipv4GlobalRoutingHelper::PopulateRoutingTables();

	runTestCase(0, 22);

	// Generar archivo pcap
	pointToPoint.EnablePcapAll("tut1");

	Simulator::Run();
	Simulator::Destroy();
	NS_LOG_INFO("Done.");
}
