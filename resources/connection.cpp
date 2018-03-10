/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "../header/connection.h"
#include "../header/fileserver.h"
#include "../header/ssl.h"

// Destructor, clean up all the mess
Connection::~Connection() {
    std::cout << "#log conn: Connection terminated to client (connection id " << this->connectionId << ")" << std::endl;
    delete this->fo;
    close(this->fd);
    SSL_free(this->ssl);    
    this->directories.clear();
    this->files.clear();
}

/*
 * constructor to handle connection from client.
 * @filedescriptor  fd of socket to get data from client
 * @connid          id of connection
 * @defaulDir       direction working 
 * @hostId          id of host
 * @commandOffset 
 */
Connection::Connection(int filedescriptor,fssl* sslcon, unsigned int connId, 
                                    std::string defaultDir, std::string hostId, bool iSSL, 
                                    unsigned short commandOffset) 
                                    : fd(filedescriptor), connectionId(connId), dir(defaultDir), 
                                    hostAddress(hostId), isSSL(iSSL), commandOffset(commandOffset), 
                                    closureRequested(false), uploadCommand(false), downloadCommand(false),  
                                    receivedPart(0), parameter("") 
{
//    this->files = std::vector<std::string>();
    this->session                   = new Session();
    this->fo                        = new FileHandle(this->dir); // File and directory browser
    this->TLSHandsharkState         = false;
    this->ConfirmedState            = false;
    this->isMainSocket              = false;
    this->isFileSocket              = false;
    this->_isDownloadConnection     = false;
    this->_isUploadConnection       = false;
    this->timeout.tv_sec            = 3;
    this->timeout.tv_usec           = 0;
    this->_dataWriteDone            = false;
    if (iSSL){
        this->ssl           = SSL_new(sslcon->get_ctx());
        SSL_set_fd(this->ssl, this->fd);
    }
    
    std::cout << "#log conn: Connection to client '" << this->hostAddress << "' established" << std::endl;
}

// Check for matching (commands/strings) with compare method
bool 
Connection::commandEquals(std::string a, std::string b) {
    // Convert to lower case for case-insensitive checking
    std::transform(a.begin(), a.end(),a.begin(), tolower);
    int found = a.find(b);
    return (found!=std::string::npos);
}

// Command switch for the issued client command, only called when this->command is set to 0
std::string 
Connection::commandParser(std::string command) {
    this->parameter;
    std::string res = "";
    this->uploadCommand = false;
    struct stat Status;
    // Commands can have either 0 or 1 parameters, e.g. 'browse' or 'browse ./'
    std::vector<std::string> commandAndParameter = this->extractParameters(command);
    // for (int i = 0; i < parameters.size(); i++) std::cout << "P " << i << ":" << parameters.at(i) << ":" << std::endl;
    std::cout << "#log conn: Connection " << this->connectionId << ": ";

    // #TODO: Test if prone to DOS-attacks (if loads of garbage is submitted)???
    // If command with no argument was issued
    if (commandAndParameter.size() == 1) {
        if (this->commandEquals(commandAndParameter.at(0), "list")) {
            // dir to browse
            std::string curDir = "./";
            std::cout << "Browsing files of the current working dir" << std::endl;
            this->directories.clear();
            this->files.clear();
            this->fo->browse(curDir,directories,files);
            for (unsigned int j = 0; j < directories.size(); j++) {
                res += directories.at(j) + "/\n";
            }
            for (unsigned int i = 0; i < files.size(); i++) {
                res += files.at(i) + "\n";
            }
        } else
        if (this->commandEquals(commandAndParameter.at(0), "pwd")) { // Returns the current working directory on the server
            std::cout << "Working dir requested" << std::endl;
            res = this->fo->getCurrentWorkingDir(false);
        } else
        if (this->commandEquals(commandAndParameter.at(0), "getparentdir")) { // Returns the parent directory of the current working directory on the server
            std::cout << "Parent dir of working dir requested" << std::endl;
            res = this->fo->getParentDir();
        } else
        if (this->commandEquals(commandAndParameter.at(0), "bye") || this->commandEquals(commandAndParameter.at(0), "quit")) {
            std::cout << "Shutdown of connection requested" << std::endl;
            this->closureRequested = true;
            close (this->fd);
        } else {
        // Unknown / no command / enter
            std::cout << "Unknown command encountered!" << std::endl;
            commandAndParameter.clear();
        }
    } else // end of commands with no arguments
    // Command with a parameter received
    if (commandAndParameter.size() > 1) {
        // The parameter
        this->parameter = commandAndParameter.at(1);        
        if (this->commandEquals(commandAndParameter.at(0), "ls")) {
            // read out dir to browse
            std::string curDir = std::string(commandAndParameter.at(1));
            std::cout << "Browsing files of directory '" << curDir << "'" << std::endl;
            this->directories.clear();
            this->files.clear();
            this->fo->browse(curDir,directories,files);
            for (unsigned int j = 0; j < directories.size(); j++) {
                res += directories.at(j) + "/\n";
            }
            for (unsigned int i = 0; i < files.size(); i++) {
                res += files.at(i) + "\n";
            }
        } else
        if (this->commandEquals(commandAndParameter.at(0), "download")) {
            
            this->downloadCommand = true;
            std::cout << "Preparing download of file '" << this->parameter << "'" << std::endl;
            unsigned long lengthInBytes = 0;
            char* fileBlock;
            unsigned long readBytes = 0;
            std::stringstream st;
            if (!this->fo->readFile(this->parameter)) {
                // Read the binary file block-wise
//                do {
                    st.clear();
                    fileBlock = this->fo->readFileBlock(lengthInBytes);
                    st << lengthInBytes;
                    readBytes += lengthInBytes;
//                    this->sendToClient(st.str()); // First, send length in bytes to client
                    this->sendToClient(fileBlock,lengthInBytes); // This sends the binary char-array to the client
//                    filehandle* fn = new filehandle(this->dir);
//                    fn->writeFileAtOnce("./test.mp3",fileBlock);
//                } while (lengthInBytes <= readBytes);
            }
            this->closureRequested = true; // Close connection after transfer
        } else
        if (this->commandEquals(commandAndParameter.at(0), "upload")) {
            this->uploadCommand = true; // upload hit!
            std::cout << "Preparing upload of file '" << this->parameter << "'" << std::endl;
            // all bytes (=parameters[2]) after the upload <file> command belong to the file
            //res = this->fo->beginWriteFile(this->parameter);
            res = (this->fo->beginWriteFile(this->parameter) ? "Preparing for upload failed" : "Preparing for upload successful");
        } else
        if (this->commandEquals(commandAndParameter.at(0), "cd")) { // Changes the current working directory on the server
            std::cout << "Change of working dir to '" << this->parameter << "' requested" << std::endl;
            // Test if dir exists
            if (!this->fo->changeDir(this->parameter)) {
                std::cout << "Directory change to '" << this->parameter << "' successful!" << std::endl;
            }
            res = this->fo->getCurrentWorkingDir(false); // Return current directory on the server to the client (same as pwd)
        } else
        if (this->commandEquals(commandAndParameter.at(0), "rmdir")) {
            std::cout << "Deletion of dir '" << this->parameter << "' requested" << std::endl;
            if (this->fo->dirIsBelowServerRoot(this->parameter)) {
                std::cerr << "Attempt to delete directory beyond server root (prohibited)" << std::endl;
                res = "//"; // Return some garbage as error indication :)
            } else {
                this->directories.clear(); // Reuse directories to spare memory
                this->fo->clearListOfDeletedDirectories();
                this->files.clear(); // Reuse files to spare memory
                this->fo->clearListOfDeletedFiles();
                if (this->fo->deleteDirectory(this->parameter)) {
                    std::cerr << "Error when trying to delete directory '" << this->parameter << "'" << std::endl;
                }
                this->directories = this->fo->getListOfDeletedDirectories();
                this->files = this->fo->getListOfDeletedFiles();
                for (unsigned int j = 0; j < directories.size(); j++) {
                    res += directories.at(j) + "\n";
                }
                for (unsigned int i = 0; i < files.size(); i++) {
                    res += files.at(i) + "\n";
                }
            }
        } else
        if (this->commandEquals(commandAndParameter.at(0), "delete")) {
            std::cout << "Deletion of file '" << this->parameter << "' requested" << std::endl;
            this->fo->clearListOfDeletedFiles();
            if (this->fo->deleteFile(this->parameter)) {
                res = "//";
            } else {
                std::vector<std::string> deletedFile = this->fo->getListOfDeletedFiles();
                if (deletedFile.size() > 0)
                    res = deletedFile.at(0);
            }
        } else
        if (this->commandEquals(commandAndParameter.at(0), "getsize")) {
            std::cout << "Size of file '" << this->parameter << "' requested" << std::endl;
            std::vector<std::string> fileStats = this->fo->getStats(this->parameter, Status);
            res = fileStats.at(4); // file size or content count of directory
        } else
        if (this->commandEquals(commandAndParameter.at(0), "getaccessright")) {
            std::cout << "Access rights of file '" << this->parameter << "' requested" << std::endl;
            std::vector<std::string> fileStats = this->fo->getStats(this->parameter, Status);
            res = fileStats.at(0); // unix file / directory permissions
        } else
        if (this->commandEquals(commandAndParameter.at(0), "getlastmodificationtime")) {
            std::cout << "Last modification time of file '" << this->parameter << "' requested" << std::endl;
            std::vector<std::string> fileStats = this->fo->getStats(this->parameter, Status);
            res = fileStats.at(3); // unix file / directory permissions
        } else
        if (this->commandEquals(commandAndParameter.at(0), "getowner")) {
            std::cout << "Owner of file '" << this->parameter << "' requested" << std::endl;
            std::vector<std::string> fileStats = this->fo->getStats(this->parameter, Status);
            res = fileStats.at(2); // owner
        } else
        if (this->commandEquals(commandAndParameter.at(0), "getgroup")) {
            std::cout << "Group of file '" << this->parameter << "' requested" << std::endl;
            std::vector<std::string> fileStats = this->fo->getStats(this->parameter, Status);
            res = fileStats.at(1); // group
        } else
        if (this->commandEquals(commandAndParameter.at(0), "mkdir")) { // Creates a directory of the specified name in the current server working dir
            std::cout << "Creating of dir '" << this->parameter << "' requested" << std::endl;
            res = (this->fo->createDirectory(this->parameter) ? "//" : this->parameter); // return "//" in case of failure
        } else
        if (this->commandEquals(commandAndParameter.at(0), "touch")) { // Creates an empty file of the specified name in the current server working dir
            std::cout << "Creating of empty file '" << this->parameter << "' requested" << std::endl;
            res = (this->fo->createFile(this->parameter) ? "//" : this->parameter);  // return "//" in case of failure
        } else {
        // Unknown / no command
            std::cout << "Unknown command encountered!" << std::endl;
            commandAndParameter.clear();
            command = "";
            res = "ERROR: Unknown command!";
        }
    } else // end of command with one parameter
    // No command / enter
    if (!commandAndParameter.at(0).empty()) {
        std::cout << "Unknown command encountered!" << std::endl;
        std::cout << std::endl;
        commandAndParameter.clear();
    }
    res += "\n";
    return res;
}

// Extracts the command and parameter (if existent) from the client call
std::vector<std::string> 
Connection::extractParameters(std::string command) {
    std::vector<std::string> res = std::vector<std::string>();
    std::size_t previouspos = 0;
    std::size_t pos;
    // First get the command by taking the string and walking from beginning to the first blank
    if ((pos = command.find(SEPARATOR, previouspos)) != std::string::npos) { // No empty string
        res.push_back(command.substr(int(previouspos),int(pos-previouspos))); // The command
        std::cout << "#log conn: Command: " << res.back();
    }
    if (command.length() > (pos+1)) {
        //For telnet testing commandOffset = 3 because of the enter control sequence at the end of the telnet command (otherwise = 1)
        res.push_back(command.substr(int(pos+1),int(command.length()-(pos+(this->commandOffset))))); // The parameter (if existent)
//        res.push_back(command.substr(int(pos+1),int(command.length()-(pos+3)))); // The parameter (if existent)
        std::cout << "#log conn: - Parameter: '" << res.back() << "'" << std::endl;
    }
    return res;
}

void 
Connection::TLS_handshark() {
    //handle ssl_handshake non-blocking modle
    struct timeval tv, tvRestore;
    int status  = -1;
    tv.tv_sec   = 2;
    tv.tv_usec  = 0;
    tvRestore   = tv;
       
    fd_set writeFdSet;
    fd_set readFdSet;
             
    do {
        tv = tvRestore;
        FD_ZERO(&writeFdSet);
        FD_ZERO(&readFdSet);
        status = SSL_accept(this->ssl);
        switch (SSL_get_error(this->ssl, status)){
            case SSL_ERROR_NONE:
                status = 0;
                std::cout << "#log conn: SSL_ERROR_NONE" << std::endl;
                break;
            case SSL_ERROR_WANT_WRITE:
                FD_SET(this->fd, &writeFdSet);
                status = 1;
                std::cout << "#log conn: SSL_ERROR_WANT_WRITE" << std::endl;
                break;
            case SSL_ERROR_WANT_READ:
                FD_SET(this->fd, &readFdSet);
                status = 1;
                std::cout << "#log conn: SSL_ERROR_WANT_READ" << std::endl;
                break;
            case SSL_ERROR_ZERO_RETURN:
                std::cerr << "#log conn: SSL_ERROR_ZERO_RETURN" << std::endl;
                break;
            case SSL_ERROR_SYSCALL:     
                std::cout << "#log conn: Peer closed connection during SSL handshake,status: " << status << std::endl;
                status = -1;
                break;
            default:
                std::cout << "#log conn: Unexpected error during SSL handshake,status: " << status << std::endl;
                status = -1;
                break;
        }
        if (status == 1)
            {              
                // Must have at least one handle to wait for at this point.
                status = select(this->fd + 1, &readFdSet, &writeFdSet, NULL, &tv);

                // 0 is timeout, so we're done.
                // -1 is error, so we're done.
                // Could be both handles set (same handle in both masks) so
                // set to 1.
                if (status >= 1)
                    {
                        status = 1;
                    }
                else // Timeout or failure
                    {
                        std::cout << "#log conn: SSL handshake - peer timeout or failure" << std::endl;
                        status = -1;
                    }
            }
                    
        } while (status == 1 && !SSL_is_init_finished(this->ssl));
        std::cout << "#log conn: SSL handshark successed" << std::endl;
        //SSL_set_accept_state(this->ssl);
}

void                         
Connection::handle_uploadRequest(std::vector<TOKEN> _listToken)
{
    std::cout << "#log conn: handle upload request" << std::endl;
    
    char            buffer[BUFFER_SIZE];
    int             bytes = -1;
    struct timeval  time = this->timeout;
    fd_set          fdset;
    int             rc;
    int             cmd;
    Packet*         pk;
    std::string     token, _sender, _receiver, _urlFile, _fileName, _fileSize;
    std::string     res;
    bzero(buffer, sizeof(buffer));
    FD_ZERO(&fdset);
    FD_SET(this->fd, &fdset);

    rc = select(this->fd + 1, &fdset, NULL, NULL, &time);

    if (rc == 0){
        std::cout << "#log conn: timeout request upload" << std::endl;
        this->closureRequested = true;
        return;
    }

    bytes        = SSL_read(this->ssl, buffer, sizeof(buffer));
    pk           = new Packet(std::string(buffer, bytes));
    if (pk->IsAvailableData())
        cmd      = pk->getCMDHeader();
    if (pk->IsAvailableData())
        token    = pk->getContent();
    bool _checkToken = false;
    rep(i,_listToken.size()){
        if (_listToken.at(i).second->compare(token))
            _checkToken = true;
    }
    if (_checkToken){
        std::cout << "#log conn: check token ok: " << _listToken.size() << std::endl;
        if (cmd == CMD_UPLOAD_FILE) {
            if (pk->IsAvailableData())
                _fileName = pk->getContent();
            if (pk->IsAvailableData())
                _fileSize = pk->getContent();
            this->fo->set_File_Size(_fileSize);
            std::cout << "#log conn: token: " << token << "\nfilename: " << _fileName  << "\nfileSize: " << _fileSize << " " << this->fo->get_File_Size() << std::endl;
            this->_isUploadConnection   = true; // upload hit!
            this->receivedPart          = 0;
            this->parameter             = _fileName;
            std::cout << "Preparing upload of file '" << this->parameter << "'" << std::endl;
            res = (this->fo->beginWriteFile(this->parameter) ? "Preparing for upload failed" : "Preparing for upload successful");
            std::cout <<"#log conn: " << res << std::endl; 
            this->response_uploadRequest();
            return;
           
        } if (cmd == CMD_MSG_FILE) {
            if (pk->IsAvailableData())
                _sender     = pk->getContent();
            if (pk->IsAvailableData())
                _receiver   = pk->getContent();
            
        } 
    }else {    
        this->closureRequested = true;
        delete  pk;
        return;
    }
}

void                        
Connection::response_uploadRequest()
{
    std::cout << "#log conn: response upload request" << std::endl;
    Packet*         pk;

    pk = new Packet();
    pk->appendData(CMD_UPLOAD_READY);
    pk->appendData(this->parameter);

    SSL_write(this->ssl,  &pk->getData()[0], pk->getData().size());

    delete pk;
}

bool 
Connection::authConnection(const  std::vector<USER> & listUser) {
    
    std::cout << "#log conn: authen connection!!!!" << std::endl;
    
    char            buffer[BUFFER_SIZE];
    int             bytes = -1;
    int             cmd;
    int             Sta = -1;
    std::string     status;
    fd_set          writeFdSet;
    fd_set          readFdSet;

    bzero(buffer, sizeof buffer);
    
    if (isSSL){ 
        bytes = SSL_read(this->ssl, buffer, sizeof(buffer));
        
        if (bytes < 0){
            switch (SSL_get_error(this->ssl, Sta)){
                case SSL_ERROR_NONE:
                    Sta = 0;
                    std::cout << "#log conn: SSL_read SSL_ERROR_NONE" << std::endl;
                    break;
                case SSL_ERROR_WANT_WRITE:
                    FD_SET(this->fd, &writeFdSet);
                    Sta = 1;
                    std::cout << "#log conn: SSL_read SSL_ERROR_WANT_WRITE" << std::endl;
                    break;
                case SSL_ERROR_WANT_READ:
                    FD_SET(this->fd, &readFdSet);
                    Sta = 1;
                    std::cout << "#log conn: SSL_read SSL_ERROR_WANT_READ" << std::endl;
                    break;
                case SSL_ERROR_ZERO_RETURN:
                    std::cout << "#log conn: SSL_ERROR_ZERO_RETURN" << std::endl;
                    Sta = -1;
                    break;
                case SSL_ERROR_SYSCALL:     
                    std::cout << "#log conn: SSL_read Peer closed connection during SSL handshake,status: " << status << std::endl;
                    Sta = -1;
                    break;
                default:
                    std::cout << "#log conn: SSL_read Unexpected error during SSL handshake,status: " << status << std::endl;
                    Sta = -1;
                    break;
            }
        }
           
    } else {
        bytes = recv(this->fd, buffer, sizeof(buffer), 0);
    }
    
    std::cout << "#log conn: size of data ssl read " << bytes << std::endl;
    if (bytes > 0){
        Packet *pk = new Packet(std::string(buffer,bytes));
        
        cmd = pk->getCMDHeader();
        std::string username, password;
        
        if (cmd == CMD_AUTHEN_LOGIN && !this->ConfirmedState){
            username = pk->getContent();
            password = pk->getContent();
        } else {
            this->ConfirmedState = false;
            return false;
        }
        
        std::cout << "#log conn: User request username: " <<username << " password: " << password << std::endl;
        
        delete pk;
        
        rep(i,listUser.size()){
            if ( listUser.at(i).username == username && listUser.at(i).password == password ){
                std::cout << "#log conn: debug status login ok" << std::endl;
                this->_username = username;
                return true;
            } 
        }
        
        std::cout << "#log conn: debug status login fail" << std::endl;
        return false;
    }
    return false;
} 

void 
Connection::respondAuthen(){
    if (this->ConfirmedState && !this->closureRequested){
        this->session->buildSession(this->connectionId, this->hostAddress);
        std::string ses = this->session->getSession();
        Packet *pk = new Packet();
        pk->appendData(CMD_AUTHEN_SUCCESS);
        pk->appendData(ses);
        std::cout << "#log conn: " << ses << std::endl;
        SSL_write(this->ssl, &pk->getData()[0], pk->getData().size() );
        delete pk;
    } else {
        Packet *pk = new Packet();
        pk->appendData(CMD_AUTHEN_FAIL);
        SSL_write(this->ssl, &pk->getData()[0], pk->getData().size() );
        delete pk;
    } 
}

void 
Connection::respondClassifyConnectionDone(bool state){
    Packet *pk = new Packet();
    
    if (state){
        pk->appendData(CMD_CLASSIFY_DONE);
        SSL_write(this->ssl, &pk->getData()[0], pk->getData().size());
    } else {
        pk->appendData(CMD_CLASSIFY_FAIL);
        SSL_write(this->ssl, &pk->getData()[0], pk->getData().size());
    }
    delete pk;
}

void 
Connection::classify_connection(){
    std::cout << "#log conn: Classify connection." << std::endl;
    char        buffer[BUFFER_SIZE];
    int         _bytes = -1;
    Packet*     _pk;
    bzero(buffer, sizeof(buffer));
    
    _bytes = SSL_read(this->ssl, buffer, sizeof(buffer));
    
    if (_bytes > 0){
        _pk = new Packet(std::string(buffer,_bytes));
        
        int _cmd = _pk->getCMDHeader();
        
        std::cout << "#log conn: recieved data " << _cmd << std::endl;
        
        if (_cmd == CMD_IS_MAIN_CONNECTION) {    
            std::cout << "#log conn: This is main connection." << std::endl;
            this->isMainSocket  = true;
            this->_isClassified = true;
            this->respondClassifyConnectionDone(true);
            return;
        }
        
        if (_cmd == CMD_IS_FILE_CONNECTION) {    
            std::cout << "#log conn: This is file connection." << std::endl;
            this->isFileSocket  = true;
            this->_isClassified = true;
            this->respondClassifyConnectionDone(true);
            return;
        }
        delete _pk;
    } 
    std::cout << "#log conn: " << _bytes << std::endl;
    if (!this->_isClassified)
        this->closureRequested  = true;
    this->respondClassifyConnectionDone(false);
    return;
}

// Receives the incoming data and issues the apropraite commands and responds
void 
Connection::respondToQuery() {
    char buffer[BUFFER_SIZE];
    int bytes;

    if (!isSSL){
        bytes = recv(this->fd, buffer, sizeof(buffer), 0);
        std::cout << "#log conn: read query TCP " << std::endl;
    } else {
        bytes = SSL_read(this->ssl, buffer, sizeof(buffer));
        std::cout << "#log conn: read query SSL " << std::endl;
    }

    // In non-blocking mode, bytes <= 0 does not mean a connection closure!
    if (bytes > 0) {  
        std::string clientCommand = std::string(buffer, bytes);
        std::cout << "#log conn: ++client command: " << std::endl;
        
        if (this->uploadCommand) { // (Previous) upload command
            std::cout << "#log conn: Write block" << std::endl;
            // Previous (upload) command continuation, store incoming data to the file
            std::cout << "#log conn: Part" << ++(this->receivedPart) << ": ";
            this->fo->writeFileBlock(clientCommand);
        } else {
            // If not upload command issued, parse the incoming data for command and parameters
            std::cout << "#log conn: ++client command: " << clientCommand << std::endl;
            std::string res = this->commandParser(clientCommand);
            
            if (!this->uploadCommand){
                this->closureRequested = true;
            }
            
            //if (!this->downloadCommand) {
            //    this->sendToClient(res); // Send response to client if no binary file
            //    this->downloadCommand = false;
            //}
        }
    } else { // no bytes incoming over this connection
        if (this->uploadCommand) { // If upload command was issued previously and no data is left to receive, close the file and connection
            //this->fo->closeWriteFile();
            //this->uploadCommand = false;
            //this->downloadCommand = false;
            //this->closureRequested = true;
            //this->receivedPart = 0;
        }
    }
}

bool
Connection::get_Data_Write_Done(){
    return this->_dataWriteDone;
}

void                        
Connection::wirte_Data(){
    char            buffer[BUFFER_SIZE];
    int             bytes;
    std::string     data;
    long long       _totalData      = this->fo->get_File_Size();
    long long       _recievedData   = this->fo->get_Data_Received();
    
    if (_totalData == _recievedData){
        _dataWriteDone = true;
        return;
    }
    else {
        if (_recievedData + sizeof(buffer) <= _totalData)
        {
            bytes   = SSL_read(this->ssl, buffer, sizeof(buffer));
            if (bytes > 0){
                data = std::string(buffer, bytes);
                std::cout << "#log conn: Write block" << std::endl;
                // Previous (upload) command continuation, store incoming data to the file
                std::cout << "#log conn: Part" << ++(this->receivedPart) << ": " << bytes << std::endl;
                this->fo->writeFileBlock(data);
            } else {
                //this->closureRequested = true;
                std::cerr << "#log conn: 1 read zero data" << std::endl;
            }
            return;
        } else {
            if ((_totalData - _recievedData < sizeof(buffer)) && (_totalData > _recievedData))  
            {
                bytes   = SSL_read(this->ssl, buffer, (_totalData - _recievedData));
                if (bytes > 0){
                    data = std::string(buffer, bytes);
                    std::cout << "#log conn: Write block" << std::endl;
                    // Previous (upload) command continuation, store incoming data to the file
                    std::cout << "#log conn: Part" << ++(this->receivedPart) << ": " << bytes << std::endl;
                    this->fo->writeFileBlock(data);
                } else {
                    //this->closureRequested = true;
                    std::cerr << "#log conn: 2 read zero data" << std::endl;
                }
            }
            return;
        }
    }
}

FILE_TRANSACTION*           
Connection::handle_CMD_MSG_FILE(){
    char                _buffer[BUFFER_SIZE];
    int                 _bytes,_cmd;
    Packet*             _pk;
    std::string         _sender, _receiver, _urlFile,_filesize;
    FILE_TRANSACTION*   _ft;
    _bytes   = SSL_read(this->ssl, _buffer, sizeof(_buffer));
    
    if (_bytes > 0){
        _pk = new Packet(std::string(_buffer,_bytes));
        if (_pk->IsAvailableData())
            _cmd        = _pk->getCMDHeader();
        if (_pk->IsAvailableData())
            _sender     = _pk->getContent();
        if (_pk->IsAvailableData())
            _receiver   = _pk->getContent();
        if (_pk->IsAvailableData())
            _urlFile    = _pk->getContent();
        if (_pk->IsAvailableData())
            _filesize   = _pk->getContent();
        std::cout <<"#log conn: msg\ncmd: " << _cmd << "\nsender: " << _sender << "\nreceiver: " << _receiver << "\nurlfile: " << _urlFile <<"\nfile size: " << _filesize << std::endl; 
        _ft = new FILE_TRANSACTION;
        _ft->_sender    = _sender;
        _ft->_receiver  = _receiver;
        _ft->_url       = _urlFile;
        _ft->_filesize  = this->fo->get_File_Size();
        delete _pk;
        return _ft;
    } 
    this->closureRequested = true;
    return NULL;
}

/*
 * @response  data response to client
 * @length size of data
 * @return void
 */
void 
Connection::sendToClient(char* response, unsigned long length) {
    // Now we're sending the response
    unsigned int bytesSend = 0;
    while (bytesSend < length) {
        int ret;
        if (!this->isSSL)
            ret = send(this->fd, response+bytesSend, length-bytesSend, 0);
        else
            ret = SSL_write(this->ssl, response + bytesSend, length-bytesSend);
        if (ret <= 0) {
            return;
        }
        bytesSend += ret;
    }
}

// Sends the given string to the client using the current connection
void 
Connection::sendToClient(std::string response) {
    // Now we're sending the response
    unsigned int bytesSend = 0;
    while (bytesSend < response.length()) {
        int ret;
        if (!this->isSSL)
            ret = send(this->fd, response.c_str()+bytesSend, response.length()-bytesSend, 0);
        else
            ret = SSL_write(this->ssl, response.c_str()+bytesSend, response.length()-bytesSend);
        if (ret <= 0) {
            return;
        }
        bytesSend += ret;
    }
}

// Returns the file descriptor of the current connection
int 
Connection::getFD() {
    return this->fd;
}

// Returns whether the connection was requested to be closed (by client)
bool 
Connection::get_Close_Request_Status() {
    return this->closureRequested;
}

void 
Connection::set_Close_Request_Status(bool status){
    this->closureRequested = status;
}

unsigned int 
Connection::getConnectionId() {
    return this->connectionId;
}

bool 
Connection::get_TLShandshark_state(){
    return this->TLSHandsharkState;
}

void 
Connection::set_TLShandshark_state(bool state){
    this->TLSHandsharkState = state;
}

bool 
Connection::get_authen_state(){
    return this->ConfirmedState;
}

void 
Connection::set_authen_state(bool state){
    this->ConfirmedState = state;
}

void 
Connection::set_isMainConnection(bool state){
    this->isMainSocket = state;
}

bool 
Connection::get_isMainConnection(){
    return this->isMainSocket;
}
    
void 
Connection::set_isFileConnection(bool state){
    this->isFileSocket = state;
}

bool 
Connection::get_isFileConnection(){
    return this->isFileSocket;
}


void 
Connection::getAllData(){
    char buf[BUFFSIZE];
    SSL_read(this->ssl, buf, sizeof(buf));
    std::cout <<"#log conn: " << buf << std::endl;
    return;
}

bool
Connection::get_isUploadConnection(){
    return this->_isUploadConnection;
}
    
bool
Connection::get_isDownloadConnection(){
    return this->_isDownloadConnection;
}

std::string                 
Connection::get_Username_Of_Connection(){
    return this->_username;
}
    
int
Connection::get_Usser_Id_Of_Connection(){
    return this->_userID;
}

unsigned int                         
Connection::get_Connection_Id(){
    return this->connectionId;
}

Session*
Connection::get_Session(){
    return this->session;
}       
   
bool                        
Connection::get_Is_Classified()
{
    return this->_isClassified;
};
    
void                        
Connection::set_Is_Classified_State(bool _state)
{
    this->_isClassified = _state;
} 