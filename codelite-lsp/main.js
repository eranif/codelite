//===-----------------------------
// Copyright: Eran Ifrah
// This scripts attempts to launch LSP server(s)
// and pass the communication with them over socket
// to CodeLite
//===-----------------------------
let net = require('net');

function createTCPServer()
{
    let server = net.createServer();
    // Make the server TCP/IP server on port 12898
    server.listen(12898, '127.0.0.1');
    return server;
}

/**
 * parse buffer and attempt to consume a complete message out of it
 */
function parseMessage(buffer) {
    let tmpbuffer = buffer.toString();
    let where = tmpbuffer.indexOf('\n');
    let messageLen = -1;
    if(where != -1) {
        let header = tmpbuffer.substr(0, where + 1); // including the "\n"
        tmpbuffer = tmpbuffer.substr(where+1); // consume the header
        if(tmpbuffer.startsWith("Content-Length: ")) {
            tmpbuffer = tmpbuffer.substr("Content-Length: ".length);
        }
        tmpbuffer = tmpbuffer.substr("Content-Length: ".length);
        
    }
}

/**
 * this callback will get called when data arrives on the socket
 * @param data string|buffer
 * @this the connection (socket)
 */
function onDataRead(data)
{
    if(!this.hasOwnProperty('incoming_buffer')) { this['incoming_buffer'] = {}; }
    let message = parseMessage(this.incoming_buffer);
    if(message != undefined) {}
}

// Start a server
let server = createTCPServer();
server.on('connection', (c) => {
    console.log("Client connected!");
    c.on('data', onDataRead.bind(c));
});
