//============================================================================
// Name        : Server.cpp
// Author      : Hynek Kydlick
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : TCP server - Robots
//============================================================================

#include <cmath> //M_PI, cos ...
#include <sys/socket.h> // socket(), bind(), connect(), listen()
#include <unistd.h> // close(), read(), write()
#include <netinet/in.h> // struct sockaddr_in
#include <arpa/inet.h> // htons(), htonl()
#include <string.h> // bzero()
#include <wait.h> // waitpid()
#include <cfloat>  // DBL_EPSILON
#include <netdb.h> // struct addrinfo
#include <iostream>
#include <sstream>

using namespace std;
const size_t BUFFER_SIZE = 10240, TIMEOUT_DEFAULT = 1, TIMEOUT_RECHARGING = 5, MAX_CONNECTIONS = 20;
const uint16_t SERVER_KEY = 54621, CLIENT_KEY = 45328;

// Create structures for easier manipulation with commands
struct CMD_SERVER{
	// Server commands to robots consist of msg and code
	uint16_t code = 0;
	string msg = "";

	CMD_SERVER() = default;
	CMD_SERVER(uint16_t code, const string & msg){
		this->code = code;
		this->msg = msg;
	}
};

struct CMD_CLIENT{
	// Commands for client need to be identified in place
	// Only maxSize is store in this case
	uint16_t maxSize = 0;
	
	CMD_CLIENT(uint16_t size){
		this->maxSize = size;
	}
};

// Fill it with predefiend commands and constants
struct server_commands{
	CMD_SERVER MOVE, TURN_LEFT, TURN_RIGHT, PICKUP, LOGOUT, OK, LOGIN_FAILED, SYNTAX_ERROR, LOGIC_ERROR;

	server_commands(): MOVE(102, "MOVE"), TURN_LEFT(103, "TURN LEFT"), TURN_RIGHT(104, "TURN RIGHT"),\
		PICKUP(105, "GET MESSAGE"), LOGOUT(106, "LOGOUT"), OK(200, "OK"), LOGIN_FAILED(300, "LOGIN FAILED"),\
		SYNTAX_ERROR(301, "SYNTAX ERROR"), LOGIC_ERROR(302, "LOGIC ERROR"){}
};

struct client_commands{
	CMD_CLIENT USERNAME, CONF, OK, RECHARGE, FULL_POW, MESSAGE;
	client_commands(): USERNAME(12), CONF(7), OK(12), RECHARGE(12), FULL_POW(12), MESSAGE(100){}
};

enum class STATE : int{
	USERNAME, HASH, MOVE, CHARGE, PICKUP
};

enum class ERROR_STATE : int{
	OK, LOGIN, SYNTAX, LOGIC, TIMEOUT
};
const server_commands S;
const client_commands C;


bool compareDBL(double d1, double d2){
	// Compares if two doubles are almost the same
	return abs(d1-d2) <= DBL_EPSILON * abs(max(d1,d2));
}

uint16_t hashK(const string & pass, uint16_t key){
	// Hash pass by summing up its char values * 1000 + key mod key
	uint16_t sum = 0;
	for(const char x : pass)
		sum += (uint16_t)x;
	return sum * (uint16_t)1000 + key;
}


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

struct Cord{
	// Utility function for Coordinates which store position of x and y axis
	int x = 0;
	int y = 0;

	Cord() = default;

	bool operator == (const Cord & b) const{
		return (x == b.x && y == b.y);
	}
};

struct Pos{
	/* Function holds position of robot its coordinates in space and its rotation in radians form 0 to 2 * M_PI
	 * */
	Cord cord;
	double rotation = -1;
	bool valid = false;

	Pos() = default;

	static double modPI(const double radians) {
		// Converts radians to range from 0 to 2 * M_PI
		// Only works for -2*M_PI to 4*M_PI, but is suffice our needs
		if (radians < 0)
			return 2 * M_PI + radians;
		if (radians > 2 * M_PI || compareDBL(2 * M_PI, radians))
			return 2 * M_PI - radians;

		return radians;
	}

	bool set(int x, int y){
		if(!valid){
			// Coordinates are valid rotation is not
			valid = true;
			cord = {x, y};
			return false;
		}
		else if(rotation == -1){
			// Try to set rotation to valid state
			if(y != cord.y){
				rotation = modPI(asin(y-cord.y > 0 ? 1 : -1));
			}
			else if(x != cord.x){
				rotation = modPI(acos(x-cord.x > 0 ? 1 : -1));
			}
			else{
				//Fail miserably we haven't moved !
				return false;
			}

		}
		// Set new coordinates
		cord = {x, y};
		return true;
	}

	bool rotateTo(const Cord & newCord,CMD_SERVER & cmd){
		// Returns true if we need to rotate to get to new cord and rotate itself
		double rotation_new = 0;	
		if(newCord.y != this->cord.y){
			rotation_new = modPI(asin(newCord.y-this->cord.y > 0 ? 1 : -1));
		}
		else if(newCord.x != this->cord.x){
			rotation_new = modPI(acos(newCord.x-this->cord.x > 0 ? 1 : -1));
		}
		else{
			return false;
		}
		if(compareDBL(rotation_new, this->rotation))
			return false;

		double left_cost = modPI(rotation_new - this->rotation);
		double right_cost = modPI(this->rotation - rotation_new);
		if(left_cost < right_cost){
			cmd = S.TURN_LEFT;
			rotation = modPI(M_PI/2 + rotation);
		}
		else{
			cmd = S.TURN_RIGHT;
			rotation = modPI(rotation-M_PI/2);
		}
		return true;
	}
};

class RobotSession{

	Pos cur_pos;
	Cord target_dest;
	string username;
	int _sockfd;
	struct timeval timeout;
	string buffer;
	ERROR_STATE error_state;
	STATE last_state;
	char add_str[INET6_ADDRSTRLEN];
	in_port_t port;

	int _send(const string & message) const{
		size_t bytesSent = 0;
		int res = 0;
		while(message.length() > bytesSent){
			res = send(_sockfd, message.data() + bytesSent, message.length()-bytesSent, 0);
			if(res == -1){
				cerr << *this;
				perror(": send");
				return -1;
			}
			bytesSent+= res;
		}
		cout << *this << ": SEND: " << message << endl;
		return 1;
	}
	
	int sendMessage(const string & msg) const{
		string msg_caps = msg + "\a\b"; 
		return _send(msg_caps);

	}

	int sendMessage(const CMD_SERVER & cmd){
		const string msg = to_string(cmd.code) + " " + cmd.msg;
		return sendMessage(msg);
	}

	int getMessage(string & bufOut,const CMD_CLIENT & msg, bool peek=false){
		// 0 -> message is not complete
		// 1 -> message read
		// -1 -> failed to recv or message too long
		//
		size_t maxLen = min((size_t)msg.maxSize, buffer.length());
		size_t x = 0;
		//Message can't be shorter than two bytes
		if(maxLen < 2)
			return 0;

		//Read until \a\b
		for(; x < maxLen-1; x++){
			if(buffer[x] == '\a' && buffer[x+1] == '\b')
				break;
		}
		// If we message is longer than maxSize
		// Or we reached end of buffer
		if(x >= msg.maxSize-1){
			if(!peek){
				error_state = ERROR_STATE::SYNTAX;
				cerr << *this << ": Message too long" << endl;
			}
			return -1;
		}
		else if(x >= buffer.length()-1)
			return 0;

		//Return msg without \a\b and remove if set
		bufOut = buffer.substr(0, x);

		// Remove and print if we must read it
		if(!peek){
			cout << *this << ": RECV: " << bufOut << endl;
			buffer.erase(0,x+2);
		}
		return 1;
	}

	bool compareHash(const uint16_t hash) const{
		return hash == hashK(username, CLIENT_KEY);
	}


public:
	STATE state;

	RobotSession(int fd,struct sockaddr_storage & st, socklen_t st_len){
		// Convert sockaddr to char[]
		inet_ntop(st.ss_family, decodeAdress((struct sockaddr *)&st), add_str, sizeof(add_str));
		port = ntohs(decodePort((struct sockaddr *) &st));
		cout << "Creating connection with: " << add_str << ":" << port << endl;

		//Default initialization
		_sockfd = fd;
		state = STATE::USERNAME;
		error_state = ERROR_STATE::OK;
		target_dest = {2,2};
		timeout = {TIMEOUT_DEFAULT, 0};
	}


	int getSocket() const{
		return _sockfd;
	}

	struct timeval getTimeout(){
		return timeout;
	}

	int readToBuffer(){
		char buf[BUFFER_SIZE];
		int bytesRead = recv(_sockfd, buf, BUFFER_SIZE-buffer.length(), 0);
		if(bytesRead < 0){
			perror("Error reading from client");
			return -1;
		}
		buffer += string(buf, bytesRead);
		return 1;
	}

	int move(int x, int y){
		if(!cur_pos.set(x, y)){ //Invalid rotation
			cur_pos.set(x,y);
			return sendMessage(S.MOVE);
		}
		CMD_SERVER cmd;
		//Return TURN
		if(cur_pos.rotateTo(target_dest, cmd)){
			return sendMessage(cmd);
		}
		//RETURN PICKUP
		else if(cur_pos.cord == target_dest){
			state = STATE::PICKUP;
			return sendMessage(S.PICKUP);
		}
		//RETURN MOVE
		else{
			return sendMessage(S.MOVE);
		}
	}


	int authorize_username(){
		int res = getMessage(username, C.USERNAME);
		if(res != 1)
			return res;
		// Create hash and send it;
		uint16_t hash = hashK(username, SERVER_KEY);
		cout << *this << ": Received username: " << username << ", sending hash: " << hash << endl;
		if(sendMessage(to_string(hash)) < 0)
			return -1;
		// Set next state
		state = STATE::HASH;
		return 1;
	}

	int authorize_hash(){
		string hash;
		int res = getMessage(hash, C.CONF);
		if(res != 1)
			return res;

		uint16_t hash_uint = 0;
		// Try to convert message to uint_16
		istringstream is(hash);
		is >> hash_uint;
		if(!is.eof()){
			error_state = ERROR_STATE::SYNTAX;
			cerr << *this << ": Unable to convert msg to uint" << endl;
			return -1;
		}
		// Compare hash with received hash
		if(compareHash(hash_uint) != 1){
			error_state = ERROR_STATE::LOGIN;
			cerr << *this << ": Hash mismatch, got: " << hash_uint << " expected: " << hash << endl;
			return -2;
		}
		//Send messages to get to next state
		if(sendMessage(S.OK) < 0 || sendMessage(S.MOVE) < 0)
			return -1;
		
		cout << *this << ": Successfuly authentificated client" << endl;
		state = STATE::MOVE;
		return 1;
	}

	int chargeStart(){
		string msg;
		// Only peek from buffer don't remove
		int res = getMessage(msg, C.RECHARGE, true);
		if(res != 1)
			return res;

		if(!msg.compare("RECHARGING") == 0){
			// Act as nothing has happend
			return 0;
		}
		//Erase from buffer
		getMessage(msg, C.RECHARGE);
		last_state = state;
		state = STATE::CHARGE;
		timeout.tv_sec = TIMEOUT_RECHARGING;
		cout << *this << ": started Charging" << endl;
		return 1;
	}

	int getMove(){
		string msg;
		int res = getMessage(msg, C.OK);
		if(res != 1)
			return res;

		istringstream is = istringstream(msg);
		int x, y; string OK; 
		// Convert message to coordinates
		if(!((is >> OK >> x >> y) && OK.compare("OK") == 0 && is.eof())){
			cerr << *this << ": Couldn't parse message to OK X Y" << endl;
			error_state = ERROR_STATE::SYNTAX;
			return -1;
		}
		// Process coordinates and send message accordingly
		return move(x, y);
		return 1;
	}

	int pickup(){
		string msg;
		int res = getMessage(msg, C.MESSAGE);
		if(res != 1)
			return res;

		if(!msg.empty()){
			//We got our message 
			cout << *this << ": Secret message is: " << msg << endl;
			return 2;
		}

		// Set target_destination
		// If we are at even row move to left until border
		// If we are at odd row move to right until border
		int odd = (cur_pos.cord.y+2)%2;
		int dest_num = 0;
		if(odd == 0){
			if(cur_pos.cord.x == -2)
				target_dest = {target_dest.x, target_dest.y -1};
			else
				target_dest = {target_dest.x-1, target_dest.y};
		}
		else{
			if(cur_pos.cord.x == 2)
				target_dest = {target_dest.x, target_dest.y -1};
			else
				target_dest = {target_dest.x+1, target_dest.y};
		}
		// We have searched all possible cells
		if(cur_pos.cord.x == -2 && cur_pos.cord.y == -2){
			cerr << *this << ": Searched whole area and found no message" << endl;
			return -1;
		}
		state = STATE::MOVE;
		return move(cur_pos.cord.x, cur_pos.cord.y);
	}

	int chargeEnd(){
		string msg;
		int res = getMessage(msg, C.FULL_POW);
		if(res != 1)
			return res;

		if(!(msg.compare("FULL POWER") == 0)){
			cerr << *this << ": Robot is reacharging but msg received is not FULL POWER" << endl;
			error_state = ERROR_STATE::LOGIC;
			return -1;
		}
		// Go to last state and reset timeout
		state = last_state;
		timeout.tv_sec = TIMEOUT_DEFAULT;
		return 1;
	}

	friend ostream & operator << (ostream & os, const RobotSession & other){
		os << other.add_str << ":" << other.port;
		return os;
	}

	void timeoutStop(){
		error_state = ERROR_STATE::TIMEOUT;
	}

	~RobotSession(){
		switch(error_state){
			case ERROR_STATE::LOGIN :{
				cerr << *this << ": LOGIN ERROR OCCURED" << endl;
				sendMessage(S.LOGIN_FAILED);
				break;
			}
			case ERROR_STATE::SYNTAX :{
				cerr << *this << ": SYNTAX ERROR OCCURED" << endl;
				sendMessage(S.SYNTAX_ERROR);
				break;
			}
			case ERROR_STATE::LOGIC :{
				cerr << *this << ": LOGIC ERROR OCCURED" << endl;
				sendMessage(S.LOGIC_ERROR);
				break;
			}
			case ERROR_STATE::TIMEOUT :{
			    cerr << *this << ": Connection timeout!" << endl;
				break;
			}
			case ERROR_STATE::OK :{
				sendMessage(S.LOGOUT);
				cout << *this << ": Sucessfully ended conection" << endl;
				break;
			}
		}
		close(_sockfd);
	}

};

class ServerSession{
	/* Utility function to store information about socket
	 * Unfortunately we can't auto close socket in destructor
	 * because we use multithread so we would close socket for others*/
	int _sockfd = -1;

public:
	ServerSession(char* myPort){
		struct addrinfo hints, *serverInfo, *p;
		char ipstr[INET6_ADDRSTRLEN]; void *addr;

		//Use any address we can find on client
		// Set hints on use any address, IPV4/6, UDP
		memset(&hints,0,sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;
		if(getaddrinfo(NULL, myPort, &hints, &serverInfo) != 0){
			throw invalid_argument("Cannot use any interface on computer");
		}

		//Iterate and find first to bind
		for(p = serverInfo; p != nullptr; p = p->ai_next){
			if((_sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
				perror("socket");
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
			freeaddrinfo(serverInfo);
			throw invalid_argument("Failed to bind any socket");
		}

		if(listen(_sockfd, MAX_CONNECTIONS) == -1){
			perror("listen");
			throw invalid_argument("");
		}
		inet_ntop(p->ai_family, decodeAdress((struct sockaddr *) p->ai_addr), ipstr, sizeof(ipstr));
		cout << "Server address: " << ipstr << ":" << ntohs(decodePort((struct sockaddr *)p->ai_addr)) << endl;
	}

	int getSocket(){
		return _sockfd;
	}

	void closeSock(){
		close(_sockfd);
		_sockfd = -1;
	}

	~ServerSession(){
		if(_sockfd > 0)
			close(_sockfd);
	}
};

void sigchld_handler(int s)
{
	// From beej's to kill ended connections
    int saved_errno = errno;
    while(waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

int startServer(char* port){
	struct sockaddr_storage their_addr;
	struct sigaction sa;
	socklen_t sin_size;
	ServerSession s(port);

	sa.sa_handler = sigchld_handler; // reap all dead processes, by beej's guide
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    // Bind SIGCHLD to call our kill child process
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        return 10;
    }

	while(true){
		// Waiting for incoming connections
		sin_size = sizeof(their_addr);
		cout << "Waiting for incomming connections..." << endl;
		int hostfd = accept(s.getSocket(), (struct sockaddr *) &their_addr, &sin_size);
		if(hostfd < 0){
			perror("Problem with accept()!");
			s.closeSock();
			return -1;
		}

		pid_t pid = fork();
		if(pid == 0){
			// Create a new process in new process we should close the main process socket.
			// We don't want o wait for destructor to do it to not cause any damage.
			// Child process should never use it !
			// No worries it will not change main server socket just ours
			s.closeSock();
			// Prepare values for main loop
			RobotSession Rs = RobotSession(hostfd, their_addr, sin_size);
			struct timeval tmpTimeout;
			// Use masterSet and tmpSet to easier FD_CLEAR
			fd_set tmpSet, masterSet;
			FD_ZERO(&masterSet);
			FD_SET(Rs.getSocket(), &masterSet);
			while(true){
				tmpSet = masterSet;
				tmpTimeout = Rs.getTimeout();
				// We have to use copy of timeval from session because select can something (UNIX)
				// modify tv_usec etc by time it waited
				int retval = select(Rs.getSocket() + 1, &tmpSet, NULL, NULL, &tmpTimeout);
				if(retval < 0){
					perror("Error select()");
					return 4;
				}
				if(!FD_ISSET(Rs.getSocket(), &tmpSet)){
					// We reached timeout, set robot session appropriately to do expected stuff
					// in destructor
					Rs.timeoutStop();
					return 5;
				}
				if(Rs.readToBuffer() < 0){
					return 6;
				}

				while(true){
					// Loop until we can get messages from buffer
					int res = 0;
					// Try to identify message
					// Charing message and end charge can come any time
					if(Rs.chargeStart() == 1)
						continue;

					switch(Rs.state){
						case STATE::USERNAME :{
							res = Rs.authorize_username();
							break;
						}
						case STATE::HASH :{
							res = Rs.authorize_hash();
							break;
						}
						case STATE::MOVE :{
							res = Rs.getMove();
							break;
						}
						case STATE::PICKUP :{
							res = Rs.pickup();
							if(res == 2) //We got the correct message
								return 1;
							break;
						}
						case STATE::CHARGE :{
							res = Rs.chargeEnd();
							break;
						}
					}

					if(res < 0){
						// Error occured
						return 8;
					}
					else if(res == 0){
						// No more messages to read from buffer
						break;
					}
				}
			}			
		}
		close(hostfd);
	}
	return 1;
}

int main(int argc, char **argv) {

	if(argc != 2){
		cerr << "Usage: port" << endl;
		return 2;
	}
	try{	
		return startServer(argv[1]);
	}
	catch (const invalid_argument & e){
		cerr << e.what() << endl;
		return 3;
	}
}

