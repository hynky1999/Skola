//============================================================================
// Name        : Client.cpp
// Author      : Hynek Kydlicek
// Version     : 0.1
// Copyright   : Your copyright notice
// Description : UDP Window Client
//============================================================================

#include <unistd.h> //socket
#include <netinet/in.h> // struct sockaddr_in
#include <arpa/inet.h> // htons(), htonl()
#include <strings.h> // bzero()
#include <cstdlib> //rand
#include <string.h> //strlen, memcpy..
#include <netdb.h> // struct addrinfo
#include <iostream>
#include <deque>
#include <fstream>
#include <vector>
using namespace std;


const int SERVER_PORT = 4000, MAX_DATA_SIZE = 255, MAX_PACKET_SIZE = 264, MAX_W_SIZE = 2040, MAX_CONNECTION_ATTEMPTS = 100;
const uint8_t RST = 1, FIN = 2, SYN = 4; 
const char *modeDownload = "\x01", *modeUpload = "\x02", *photoLocation = "photo.jpg";
const suseconds_t TIMEOUT_DEFAULT_USEC = 100*1000;
int DEBUG_ID = 0;

struct Data{
	/* Utility structure for holding Data and their size */

	char data[MAX_DATA_SIZE];
	size_t dataSize;

	Data(): data{0}, dataSize(0){}
	Data(const char* data){
		if(!data){
			dataSize = 0;
			data = nullptr;
			return;
		}
		dataSize = strlen(data);
		memcpy(&this->data, data, dataSize);
	}
};


struct Packet{
	/* Utility structure for holding packet data
	 * Packet structure is 4B id, 2B seq, 2B ack, 1B flag, [0-255]B Data
	 * seq, id and ack are saved in host order and converted to network when with toBytes method */

	uint32_t id;
	uint16_t seq;
	uint16_t ack;
	uint8_t flag;
	Data d;

	Packet(){ id = 0; seq = 0; ack = 0; flag = 0; }

	Packet(uint32_t id, uint16_t seq, uint16_t conf, uint8_t flag = 0, const char* data = nullptr): d(data){
	  this->id = id; this->seq = seq; this->ack = conf; this->flag = flag;
	}

	Packet(uint32_t id, uint16_t seq, uint16_t conf, uint8_t flag, Data data){
	  this->id = id; this->seq = seq; this->ack = conf; this->flag = flag; this->d = data;
	}

	Packet(const char* packetStart, size_t len){
		// Constructor for creating packet from bytes of mem
		// Simply loop throught mem pointed by packetStart and save mem by definition of packet

		if(len < 9 || len > MAX_PACKET_SIZE)
			throw length_error("Packet is too small");

		uint32_t netId = 0;
		uint16_t netSeq = 0;
		uint16_t netConf = 0;
		memcpy(&netId, packetStart, sizeof(id));
		packetStart += sizeof(id);
		memcpy(&netSeq, packetStart, sizeof(seq));
		packetStart += sizeof(seq);
		memcpy(&netConf, packetStart, sizeof(ack));
		packetStart += sizeof(ack);
		memcpy(&flag, packetStart, sizeof(flag));
		packetStart += sizeof(flag);
		d.dataSize =len-9;
		memcpy(d.data, packetStart, d.dataSize);
		id = ntohl(netId);
		seq = ntohs(netSeq);
		ack = ntohs(netConf);
	}

	bool valid() const{
		// Checks if packet is valid 
		if (flag && flag != RST && flag != FIN && flag != SYN){
			cerr << "Flag is not either RST, FIN, SYN" << endl;
			return 0;
		}
		else if (flag == SYN) {
			if (d.dataSize != 1 || (memcmp(modeDownload, d.data, 1) != 0 && memcmp(modeUpload, d.data, 1) != 0)){
				cerr << "Received SYN with wrong data" << endl;
				return 0;
			}
		} else if (flag == FIN) {
			  if (d.dataSize != 0){
					cerr << "Received FIN with data" << endl;
					return 0;
			  }
		}
		return 1;
	}

	bool valid(uint32_t session_id) const{
		// Check if packet id match
		if (flag == 0) {
			if (id != session_id){
				cerr << "Session id missmatch, expected: " << session_id << " got: " << id << endl;
				return 0;
			 }
		}
		return valid();
	}

	char* toBytes() const{
		/* Convert Packet into bytes into mem in network order
		   Works pretty much the same as mem to packet constructor but in opposite way */

		char* array = new char[d.dataSize + 9];
		int offset = 0;
		uint32_t netId = htonl(id);
		uint16_t netSeq = htons(seq);
		uint16_t netConf = htons(ack);
		memcpy(array+offset, (char*)&netId, sizeof(id));
		offset += sizeof(id);
		memcpy(array+offset, (char*)&netSeq, sizeof(seq));
		offset += sizeof(seq);
		memcpy(array + offset, (char *)&netConf, sizeof(ack));
		offset += sizeof(ack);
		memcpy(array+offset, (char*)&flag, sizeof(flag));
		offset += sizeof(flag);
		memcpy(array+offset, (char*)&d.data, d.dataSize);
		return array;
	}

	size_t length() const{
		return 9 + d.dataSize;
	}

	friend ostream & operator << (ostream & os, const Packet & p){
		os << p.id;
		os << " seq: " << (int)p.seq;
		os << " ack: " << (int)p.ack;
		os << " flags: " << (int)p.flag;
		os << " data(" << p.d.dataSize << ")";
		return os;
	}
};


class SlidingWindowReceive{
	/* Receiving Sliding window using deque for fast random access and front and back functionality
	 * data deque for storing data, recv deque for indication if data are valid in cell */
	
	deque<char> _window_data;
	deque<bool> _window_recv;
	uint16_t _seq; //first index as seq number

public:
	SlidingWindowReceive(uint16_t start) {
		_window_data.resize(MAX_W_SIZE);
		_window_recv.resize(MAX_W_SIZE, 0);
		_seq = start;
	}

	bool add(Packet & p){
		// Normalize seq to index
		uint16_t difference = p.seq - _seq;
		// Check if out of window
		if(difference > _window_data.size()-p.d.dataSize)
			return 0;

		for(int i = 0; i < p.d.dataSize; ++i){
			  _window_data[difference + i] = p.d.data[i];
			  _window_recv[difference + i] = 1;
			}
		return 1;
	}

	void writeToFile(ofstream & ofs){
	//Write to file as long as cells are valid
	while (_window_recv.front()) {
		ofs.put(_window_data.front());
		_window_data.pop_front();
		_window_recv.pop_front();
		++_seq;
		_window_data.push_back(0);
		_window_recv.push_back(0);
		}
	}

	uint16_t getConf(){
		// Pop valid data (shouldn't happen if implement correctly because those data are lost)
		// adjust _seq by poping data
		while (_window_recv.front()) {
			_window_data.pop_front();
			_window_recv.pop_front();
			_seq++;
            _window_data.push_back(0);
            _window_recv.push_back(0);
		}
		return _seq;
	}

};

class SlidingWindowSend{
	/* Send Sliding window using deque for fast random access and front and back functionality
	 * data deque for storing data */

	deque<char> _window_data;
	uint16_t _seq; // First sequence number of window

public:
	SlidingWindowSend(uint16_t start) {
		_seq = 0;
	}

	bool confirm(Packet & p){
		// Confirm data from p in sliding window
		// Normalize ack to deque index
		uint16_t difference = p.ack - _seq;
		uint16_t difference2 = _seq - p.ack;
		// We are out of window is ack - WindowSize from window or + WindowSize from window
        if (difference > _window_data.size()){
        	if(difference2 <= _window_data.size())
        		return 1;
			return 0;
		}
        for (int i = 0; i < difference; ++i) {
			++_seq;
            _window_data.pop_front();
		}
		// Print new window size
		/* cout << "WINDOW <" << _seq << ", " << _seq + (uint16_t)_window_data.size() << "> "<< endl; */
		return 1;
	}

	size_t size() const{
		return _window_data.size();
	}

	uint16_t end() const{
		return _seq + _window_data.size();
	}

	uint16_t fillData(ifstream & ifs){
		// Push back data from file to back of sliding window until EOF or sufficient size of window
		// Return seq of first byte added
		uint16_t first_seq = (uint16_t)_window_data.size() + _seq;
		while(_window_data.size() < MAX_W_SIZE){
			char c = ifs.get();
			if(ifs.eof())
				break;
			_window_data.push_back(c);
		}
		return first_seq;
	}

	size_t getSeq(){
		return _seq;
	}

	Data operator [] (uint16_t seq) const{
		// Return bytes from seq to seq+MAX_DATA_SIZE if possible as Data
		// If not possible return seq+(data to end of window)
		//
		seq = seq - _seq;// Reset seq to deque base
		Data d;
		d.dataSize = MAX_DATA_SIZE;
		if(seq + d.dataSize > _window_data.size())
			d.dataSize = _window_data.size()-seq;

		for(size_t i = 0; i < d.dataSize; ++i){
			d.data[i] = _window_data[seq + i];
		}
		return d;
	}

};

void* decodeAdress(struct sockaddr * sa){
	if(sa->sa_family == AF_INET)
		return &(((struct sockaddr_in*)sa)->sin_addr);
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

in_port_t decodePort(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return (((struct sockaddr_in*)sa)->sin_port);

    return (((struct sockaddr_in6*)sa)->sin6_port);
}


class Session{
	uint32_t _id;
	struct addrinfo* _destaddr; // address of server
	bool _initialized; // Connection initialized ?
	int _sockfd; 
	size_t  _repSeq; // Number of repetions of the same number
	uint16_t _lastSeq; // Last seq number
	bool endRST;

public:

	Session(){ _id = 0; _destaddr = nullptr; _initialized = 0; _sockfd = -1; _lastSeq = MAX_W_SIZE + 1; _repSeq = 1; endRST = 1; }

	Session(int myPort, int outPort, const char* address) {
		_id = 0; _initialized = 0; endRST = 1; _lastSeq = MAX_W_SIZE + 1; _repSeq = 1;
		struct addrinfo hints, *clientInfo, *p;
		char ipstr[INET6_ADDRSTRLEN]; void *addr;

		//Use any address we can find on client
		// Set hints on use any address, IPV4/6, UDP
		memset(&hints,0,sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_flags = AI_PASSIVE;
		string strPort = to_string(myPort);
		if(getaddrinfo(NULL, strPort.c_str(), &hints, &clientInfo) != 0){
			throw invalid_argument("Cannot use any interface on computer");
		}
		//Iterate and find first to bind
		for(p = clientInfo; p != nullptr; p = p->ai_next){
			if((_sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
				perror("socket REEEE");
				continue;
			}
			if(bind(_sockfd, p->ai_addr, p->ai_addrlen) != 0){
				close(_sockfd);
				perror("bind");
				continue;
			}
			break;
		}
		if(p == nullptr){
			freeaddrinfo(clientInfo);
			throw invalid_argument("Failed to bind any socket");
		}
        inet_ntop(p->ai_family,decodeAdress((struct sockaddr*) p->ai_addr), ipstr, sizeof(ipstr));
		cout << "Client address: " << ipstr << ":" << ntohs(decodePort((struct sockaddr *)p->ai_addr)) << endl;
		
		// Search for address
		memset(&hints,0,sizeof(hints));
		hints.ai_family = p->ai_family;
		hints.ai_socktype = p->ai_socktype;
		// Free client addressinfo we dont need it anymore
		freeaddrinfo(clientInfo);
		strPort = to_string(outPort);
		// Load info about address
		if(getaddrinfo(address, strPort.c_str(), &hints, &_destaddr) != 0){
			close(_sockfd);
			throw invalid_argument("Cannot transform Server address to IPV4");
		}
        if (_destaddr->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)_destaddr->ai_addr;
            addr = &(ipv4->sin_addr);
        } else { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)_destaddr->ai_addr;
            addr = &(ipv6->sin6_addr);
        }
        inet_ntop(_destaddr->ai_family,decodeAdress((struct sockaddr*) _destaddr->ai_addr), ipstr, sizeof(ipstr));
		cout << "Translated address to: " << ipstr << ":" << ntohs(decodePort((struct sockaddr *)_destaddr->ai_addr)) << endl;
	}

	int getSocket() const{
		return _sockfd;
	}
	uint32_t getId() const{
		return _id;
	}

	uint16_t initialized() const{
		return _initialized;
	}

	int initialize(uint32_t id){
		_initialized = 1;
		_id = id;
	}

	void success(){
		endRST = 0;
	}

	bool send(Packet & p){
		cout << "SEND: " << DEBUG_ID << " " << p << endl;
		++DEBUG_ID;
		// Adjust _repSeq
		if(p.seq == _lastSeq)
			_repSeq +=1;
		else
			_repSeq = 1;

		_lastSeq = p.seq;

		if(_repSeq == 20){
			// We have sent the same seq 20x times
			cerr << "Sent the same seq number 20x" << endl;
			return 0;
		}

		char* dataSend = p.toBytes();
		// RST flag send no need to send another at destructor
		if(p.flag == RST)
			endRST = 0;
		if(sendto(_sockfd, dataSend, p.length(), 0, _destaddr->ai_addr, _destaddr->ai_addrlen) != p.length()){
			perror("send: ");
			delete [] dataSend;
			return 0;
		}
		delete [] dataSend;
		return 1;
	}

	bool recv(Packet & p){
			
		char packet[MAX_PACKET_SIZE];
		struct sockaddr from;
		socklen_t fromLen = sizeof(from);
		int bytesRead;
		// Receive data
		if((bytesRead = recvfrom(_sockfd, packet, MAX_PACKET_SIZE, 0, (struct sockaddr *)&from, &fromLen)) == -1){
			perror("receive: ");
			return 0;
		}
		try{
			p = Packet(packet, bytesRead);
		}
		catch(const length_error e){
			cerr << "Invalid packet size" << endl;
			return 0;
		}
		cout << "RECV: " << DEBUG_ID << " " << p << endl;
		++DEBUG_ID;

		// If we received RST send
		if(p.flag == RST){
			endRST = 0; // Dont send RST back
			cerr << "Received RST from Sender" << endl;
			return 0;
		}
		//Validity check
		if((_initialized && !p.valid(_id)) || (!_initialized && !p.valid())){
			return 0;
		}
		return 1;
	}

	~Session(){
		cerr << "Ending connection...." << endl;
		if(_sockfd >= 0){
			// Send RST if we closed unexpectedly
			if(endRST){
				cerr << "Sending RST packet...." << endl;
				Packet p(0,0,0,RST);
				send(p);
			}
			// Close socket
			close(_sockfd);
		}
		// Free allocated address
		freeaddrinfo(_destaddr);
	}

};

bool downloadPhoto(ofstream & ofs, Session & s){
	cout << "DOWNLOADING!" << endl;
	fd_set masterSet, tmpSet;
	struct timeval timeout = {0,0}; //Set first timeout to zero
	FD_ZERO(&masterSet); //Master set so we dont have to clr always
	FD_SET(s.getSocket(), &masterSet);
	SlidingWindowReceive win(0);
	while(true){
		tmpSet = masterSet;
		int retval = select(s.getSocket() + 1, &tmpSet, NULL, NULL, &timeout);
		if(retval < 0){
			perror("Select() error");
			return 0;
		}
		if(FD_ISSET(s.getSocket(), &tmpSet)){
			// We got data to receive
			Packet pInc;
			if(!s.recv(pInc))
				return 0;

			if(!s.initialized()){
				// Accept SYN packet and init connection or RST and end. Ignore everything else
				if(pInc.seq == 0 && pInc.d.dataSize == 1 && memcmp(modeDownload, pInc.d.data, 1) == 0){
					s.initialize(pInc.id);
				}
				if(pInc.flag == RST){
					cerr << "RST received" << endl;
					return 0;
				}
			}
			else{
				switch(pInc.flag){
					case RST:{
						cerr << "RST received" << endl;
						return 0;
					}
					case SYN:{
						cerr << "SYN packet unexpected" << endl;
						return 0;
					}
					case FIN:{
						if(pInc.seq != win.getConf()){
							cerr << "FIN packet unexpected" << endl;
							return 0;
						}
						Packet pOut(s.getId(), 0, win.getConf(), FIN);
						s.send(pOut);
						s.success();
						cout << "Successffuly downloaded image!" << endl;
						return 1;
					}
					case 0:{
						//DATA packet add it to sliding window
						win.add(pInc);
						break;
					}
					default:{
						cerr << "Unidentified packet!" << endl;
						return 0;
						break;
					}
				}
			}
		}
		if(!s.initialized()){
			// Send SYN packet if we were timed out
			if(!FD_ISSET(s.getSocket(), &tmpSet)){
				cout << "Establishing new connection...." << endl;
				Packet pOut(0, 0, 0, SYN, modeDownload);
				if(!s.send(pOut))
					return 0;
			}
		}
		else{
			// Read window to file
			win.writeToFile(ofs);
			Packet pOut(s.getId(), 0, win.getConf());
			s.send(pOut);
		}
		// Reset timeout
		timeout.tv_usec = TIMEOUT_DEFAULT_USEC;
	}
}


bool uploadFirmware(ifstream & ifs, Session & s){
	cout << "UPLOAD!" << endl;
	struct timeval timeout = {0,0};
	size_t lastAck = MAX_W_SIZE + 1, ackRep = 1; //Set lastSeq to anything bigger than window size so it doesn't match first run
	bool awaitFin = 0;
	fd_set tmpSet, masterSet;
	FD_ZERO(&masterSet);
	FD_SET(s.getSocket(), &masterSet);
	SlidingWindowSend win(0);
	while(true){
		tmpSet = masterSet; // We don't have to use FDCLEAR just swap with setted one
		int retval = select(s.getSocket() + 1, &tmpSet, NULL, NULL, &timeout);
		if(retval < 0){
			perror("Select() error");
			return 0;
		}
		if(FD_ISSET(s.getSocket(), &tmpSet)){
			// We received something, identify it
			Packet pInc;
			if(!s.recv(pInc))
				return 0;

			if(!s.initialized()){
				// Initiaze connection if correct packet
				if(pInc.flag == SYN && pInc.seq == 0 && pInc.d.dataSize == 1 && memcmp(modeUpload, pInc.d.data, 1) == 0){
					s.initialize(pInc.id);
				}
				// RST if received
				if(pInc.flag == RST){
					cerr << "RST received" << endl;
					return 0;
				}
			}
			else{
				switch(pInc.flag){
					case RST:{
						cerr << "RST received" << endl;
						return 0;
					}
					case SYN:{
						cerr << "SYN packet unexpected" << endl;
						return 0;
					}
					case FIN:{
						// Received FIN check ack and if expected
						if (pInc.ack != win.getSeq() || !awaitFin) {
							cerr << "FIN packet unexpected" << endl;
							return 0;
						}
						cout << "Successfully uploaded firmware!" << endl;
						s.success();
						return 1;
					}
					case 0:{
						// Check we received the same ack 3x
						if (pInc.ack == lastAck)
							++ackRep;
						else
							ackRep = 1;

						if (ackRep >= 3) {
							cerr << "Resending Packet after 3x same ack" << endl;
							Packet pOut(s.getId(), pInc.ack, 0, 0, win[pInc.ack]);
							if (!s.send(pOut))
								return 0;
						}
						lastAck = ackRep;
						if(!win.confirm(pInc)){
							cerr << "Receive data out of window" << endl;
							return 0;
						}
						break;
					}
					default:{
						cerr << "Unidentified packet" << endl;
						return 0;
						break;
					}
				}
			}
		}
		// Send data	
		// Get current sequence of window
		uint16_t seq = win.getSeq();
		if(!s.initialized()){
			// Send SYN if we timed out
			if(!FD_ISSET(s.getSocket(), &tmpSet)){
				cout << "Establishing new connection...." << endl;
				Packet pOut(0, seq, 0, SYN, modeUpload);
				if(!s.send(pOut))
					return 0;
			}
		}
		else if(awaitFin && !FD_ISSET(s.getSocket(), &tmpSet)){
			// Resend FIN packet if timeout
			Packet pOut(s.getId(), win.getSeq(), 0, FIN);
			if(!s.send(pOut))
				return 0;
		}
		else if(win.size() == 0 && ifs.eof()){
			//We have no more data to send
			awaitFin = 1;
			Packet pOut(s.getId(), win.getSeq(), 0, FIN);
			if(!s.send(pOut))
				return 0;
		}
		else{
			seq = win.fillData(ifs); //fillData return index of first index filled;
			if(!FD_ISSET(s.getSocket(), &tmpSet)){
				// Resend whole Window if timeout
				/* cerr << "RESENDING WHOLE WINDOW" << endl; */
				seq = win.getSeq();
			}
			//Send until we reach end of window
			while(seq != win.end()){
				Data d = win[seq];
				Packet pOut(s.getId(), seq, 0, 0, d);
				if(!s.send(pOut))
					return 0;
				seq += d.dataSize;
			}
		}
		timeout.tv_usec = TIMEOUT_DEFAULT_USEC;
	}
	return 1;
}

int serverUploadFirmware(const char* address, const char*firmwareFileName){
	try{
		int port = 2000 + rand()%10000; // Set random port
		Session session(port, SERVER_PORT, address);
		ifstream fs(firmwareFileName, ios::binary);
		if(!uploadFirmware(fs, session))
			return 2;
	}
	catch (const invalid_argument& e){
		cerr << e.what() << endl;
		return 3;
	}
	return 1;
}

int serverRecvPhoto(const char* address){
	try{
		int port = 2000 + rand()%10000; // Set random port
		cout << rand() << endl;
		Session session(port, SERVER_PORT, address);
		ofstream fs(photoLocation, ios::binary);
		if(!downloadPhoto(fs, session))
			return 2;
	}
	catch (const invalid_argument& e){
		cerr << e.what() << endl;
		return 3;
	}
	return 1;
}

int main(int argc, char **argv) {
	if(argc < 2 || argc > 3){
		cerr << "Usage: address <firmware.bin>" << endl;
		return 1;
	}
	srand(time(0));
	if(argc == 2)
		return serverRecvPhoto(argv[1]);

	else
		return serverUploadFirmware(argv[1], argv[2]);
}
