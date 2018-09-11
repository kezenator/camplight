namespace bbox.net
{
    export interface MessageWebSocketStateHandler
    {
        (open: boolean, error: string): void;
    }

    export class MessageWebSocket
    {
        constructor(url: string, protocol: string, state_handler: MessageWebSocketStateHandler)
        {
            this.url = url;
            this.protocol = protocol;
            this.state_handler = state_handler;
            this.msg_handlers = {};
            this.is_open = false;
            this.first_open_attempt = true;
            this.reconnect_timer = new bbox.ui.Timer(() => { this.reconnect(); })
            this.reconnect_timer.startSingleShot(100);
        }

        public registerHandler(type: bbox.enc.Type, handler: (msg: any) => void)
        {
            this.msg_handlers[type.getName()] = handler;
        }

        public send(msg: bbox.enc.MsgAnyPtr)
        {
            var type = msg.getType();
            var lib = type.getTypeLibrary();

            var toXml = new bbox.enc.ToXml('message');
            toXml.startObject();
            toXml.startNamedValue('type');
            toXml.encodeTypedValue(type.getName(), 'std::string');
            toXml.endNamedValue();
            toXml.startNamedValue('contents');
            toXml.encodeTypedValue(msg, type.getName());
            toXml.endNamedValue();
            toXml.endObject();

            console.assert(!toXml.hasError(), "MessageWebSocket: Send Error: " + toXml.getErrorString());

            var raw_msg = toXml.getXmlString();

            if (this.is_open)
            {
                this.websocket.send(raw_msg);
            }
        }

        private reconnect()
        {
            this.websocket = new WebSocket(this.url, this.protocol);
            this.websocket.onopen = () => { this.updateState(true, ""); };
            this.websocket.onclose = (evt: CloseEvent) => { this.updateState(false, "Cause: " + evt.code.toString()); };
            this.websocket.onmessage = (evt: MessageEvent) => { this.handleMessage(evt); };
        }

        private updateState(open: boolean, error: string)
        {
            if ((open != this.is_open)
                || this.first_open_attempt)
            {
                this.first_open_attempt = false;
                this.is_open = open;
                this.state_handler(open, error);
            }

            if (!open)
            {
                this.reconnect_timer.startSingleShot(10 * 1000);
            }
        }

        private handleMessage(evt: MessageEvent)
        {
            if (typeof(evt.data) !== 'string')
            {
                // We only handle string messages
                this.updateState(false, "Received binary message");
                this.websocket.close();
            }
            else
            {
                var msg: bbox.enc.MsgAnyPtr | null = null;

                var fromXml = new bbox.enc.FromXml(evt.data, 'message');
                fromXml.startObject();
                fromXml.startNamedValue('type');
                var type_name = <string>(fromXml.decodeTypedValue('std::string'));
                fromXml.endNamedValue();
                if (!fromXml.hasError())
                {
                    var type = bbox.enc.TypeLibrary.findType(type_name);
                    if (!type)
                        fromXml.raiseError('Unknown type ' + type_name);
                    else
                    {
                        fromXml.startNamedValue('contents');
                        var msg = <bbox.enc.MsgAnyPtr>(fromXml.decodeTypedValue(type_name));
                        fromXml.endNamedValue();
                        fromXml.endObject();

                        if (!(type_name in this.msg_handlers))
                        {
                            fromXml.raiseError("No handler registered for message type " + type_name);
                        }
                    }
                }
                fromXml.checkComplete();

                console.assert(!fromXml.hasError(), fromXml.getErrorString());

                if (!fromXml.hasError()
                    && (msg != null))
                {
                    this.msg_handlers[type_name](msg);
                }
            }
        }

        private url: string;
        private protocol: string;
        private state_handler: MessageWebSocketStateHandler;
        private msg_handlers: { [type_name: string]: (msg: any) => void };
        private is_open: boolean;
        private first_open_attempt: boolean;
        private reconnect_timer: bbox.ui.Timer;

        private websocket: WebSocket;
    }
}
