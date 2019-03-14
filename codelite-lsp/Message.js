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
        if(this.headersMap.has("content-length")) {
           return parseInt(this.headersMap.get("content-length"));
        }
        return -1;
    }

    parse(buffer)
    {
        let str = (typeof buffer == "string") ? buffer : buffer.toString();
        let where = str.indexOf("\r\n\r\n");
        if(where != -1) {
            this.content = str.substr(where + 4);
            let headers = str.substr(0, where).split("\r\n");
            this.headersMap.clear();
            headers.forEach(function(v) {
                let pair = v.split(":");
                if(pair.length == 2) { 
                    let key = pair[0].trim().toLowerCase();
                    let val = pair[1].trim().toLowerCase();
                    this.headersMap.set(key, val);
                }
            }.bind(this));
        }
        let contentLen = this.getContentLength();
        if((contentLen == -1) || (contentLen > this.content.length)) { return undefined; }
        let messageBuffer = this.content.substr(0, contentLen);
        let remainder = this.content.substr(contentLen);
        
        try {
            let requestObject = JSON.parse(messageBuffer);
            return { request: requestObject, remainder: remainder };
        } catch (e) {
            return undefined;
        }
    }
}

module.exports = Message;