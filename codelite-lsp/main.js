//===-----------------------------
// Copyright: Eran Ifrah
// This scripts attempts to launch LSP server(s)
// and pass the communication with them over socket
// to CodeLite
//===-----------------------------
let net = require('net');
let Message = require('./Message');
const { spawn } = require('child_process');

function createTCPServer()
{
    let server = net.createServer();
    // Make the server TCP/IP server on port 12898
    server.listen(12898, '127.0.0.1');
    return server;
}

/**
 * start LSP server and associate it with the TCP connection
 */
function processCommand(command, conn)
{
    switch(command.method) {
    case 'execute':
        // The 'execute' is a special command that we handle it ourself
        let lsp = spawn(command.command);
        if(lsp != undefined) {
            conn.lsp_process = lsp;
            lsp.stdout.on('data', (data) => {
                console.log("LSP\n" + data.toString());
                conn.write(data.toString()); // pass the data as-is back to CodeLite
            });
            lsp.stderr.on('stderr', (data) => {
                console.error("LSP\n" + data.toString()); 
            });
            lsp.on('close', (code) => {
               // Close the tcp connection
               process.exit(0);
            });
        }
    break;
    default:
        if(conn.lsp_process != undefined) {
            let asString = JSON.stringify(command);
            let jsonMessage = "Content-Length: " + asString.length + "\r\n\r\n" + asString; 
            conn.lsp_process.stdin.write(jsonMessage);
        }
    break;
    }
}

/**
 * this callback will get called when data arrives on the socket
 * @param data string|buffer
 * @this the connection (socket)
 */
function onDataRead(data)
{
    if(!this.hasOwnProperty('incoming_buffer')) { this['incoming_buffer'] = ""; }
    this.incoming_buffer += typeof data == "string" ? data : data.toString();
    while(true) {
        let message = new Message();
        let command = message.parse(this.incoming_buffer);
        if(command == undefined) { break; }
        this.incoming_buffer = command.remainder;
        processCommand(command.request, this);
    }
}

// Start a server
let server = createTCPServer();
server.on('connection', (conn) => {
    console.log("Client connected!");
    conn.on('data', onDataRead.bind(conn));
    conn.on('close', (code) => {
        // if CodeLite termianted, exit
        process.exit(0);
    });
});
