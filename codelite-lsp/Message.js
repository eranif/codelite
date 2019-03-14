class Message
{
    constructor()
    {
        this.content = "";
        this.headersMap = new Map()
    }

    isOk() { return this.headers.length && this.content.length; }

    /**
     * read header from the buffer
     */

    getContentLength()
    {
        // TODO: loop over the headers and search for the Content-Length header
        return -1;
    }

    parse(buffer)
    {
        let str = buffer.toString();
        let where = str.indexOf("\r\n\r\n");
        if(where != -1) {
            this.content = str.substr(where + 4);
            let headers = str.substr(0, where).split("\r\n");
            this.headersMap.clear();
            this.headers.forEach(function(v) {
                let pair = v.split(":");
                if(pair.length == 2) { this.headersMap.set(pair[0].trim().toLowerCase(), pair[1].trim().toLowerCase()) }
            }.bind(this));
        }
        return this.getContentLength() == this.content.length;
    }
}