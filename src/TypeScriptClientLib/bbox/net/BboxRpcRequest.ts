namespace bbox.net
{
    export interface BboxRpcRequestCompleteHandler<InputStruct, OutputStruct>
    {
        (request: BboxRpcRequest<InputStruct, OutputStruct>): void;
    }

    export class BboxRpcRequest<InputStruct, OutputStruct>
    {
        constructor(
            url: string,
            method: string,
            input_type: bbox.enc.TypePrototype<InputStruct>,
            output_type: bbox.enc.TypePrototype<OutputStruct>,
            handler: BboxRpcRequestCompleteHandler<InputStruct, OutputStruct>)
        {
            this.m_url = url;
            this.m_method = method;
            this.m_input_type = input_type.TYPE;
            this.m_output_type = output_type.TYPE;
            this.m_output = null;
            this.m_handler = handler;

            this.m_is_complete = false;
            this.m_was_successful = false;
            this.m_error_str = null;

            var final_url: string;
            final_url = url;
            if (final_url.substring(-1) !== "/")
            {
                final_url = final_url.concat("/");
            }
            final_url = final_url.concat(method);

            this.m_request = new AjaxRequest(
                "POST",
                final_url,
                (request: AjaxRequest) =>
                {
                    this.handleComplete();
                }
            );
        }

        sendAsync(args: InputStruct): void
        {
            var to_xml = new bbox.enc.ToXml("params");
            this.m_input_type.toXml(args, to_xml);

            this.m_request.sendAsyncWithContent(to_xml.getXmlString());
        }

        isComplete(): boolean
        {
            return this.m_is_complete;
        }

        wasSuccessul(): boolean
        {
            console.assert(this.m_is_complete, "bbox.net.BboxRpcRequest: wasSucessful called before complete");

            return this.m_was_successful;
        }

        getErrorDescription(): string
        {
            console.assert(this.m_is_complete, "bbox.net.BboxRpcRequest: getErrorDescription called before complete");
            console.assert(!this.m_was_successful, "bbox.net.BboxRpcRequest: getErrorDescription called on successful request");

            return this.m_error_str;
        }

        getResult(): OutputStruct
        {
            console.assert(this.m_is_complete, "bbox.net.BboxRpcRequest: getResult called before complete");
            console.assert(this.m_was_successful, "bbox.net.BboxRpcRequest: getResult called on failed request");

            return this.m_output;
        }

        private handleComplete()
        {
            this.m_is_complete = true;

            if (!this.m_request.wasSuccessful())
            {
                this.m_was_successful = false;
                this.m_error_str = "Ajax Request Failed: " + this.m_request.getErrorDescription();
            }
            else
            {
                try
                {
                    var from_xml = new bbox.enc.FromXml(this.m_request.getResponseAsText(), "results");

                    this.m_output = this.m_output_type.fromXml(from_xml);

                    if (from_xml.hasError())
                    {
                        this.m_output = null;
                        this.m_was_successful = false;
                        this.m_error_str = "Error decoding result XML: " + from_xml.getErrorString();
                    }
                    else
                    {
                        this.m_was_successful = true;
                    }
                }
                catch (err)
                {
                    this.m_was_successful = false;
                    this.m_error_str = "Could not decode response as JSON";
                }
            }

            this.m_handler(this);
        }

        private m_url: string;
        private m_method: string;
        private m_input_type: enc.Type;
        private m_output_type: enc.Type;
        private m_output: OutputStruct;
        private m_handler: BboxRpcRequestCompleteHandler<InputStruct, OutputStruct>;
        private m_request: AjaxRequest;

        private m_is_complete: boolean;
        private m_was_successful: boolean;
        private m_error_str: string;
    }
}
