namespace bbox.net
{
    export interface AjaxRequestCompleteHandler
    {
        (request: AjaxRequest): void;
    }

    export class AjaxRequest
    {
        constructor(method: string, url: string, handler: AjaxRequestCompleteHandler)
        {
            this.m_sent = false;
            this.m_aborted = false;
            this.m_success = false;
            this.m_handled = false;

            this.m_handler = handler;

            this.m_request = new XMLHttpRequest();
            this.m_request.open(method, url, true);

            this.m_request.onload = (event: Event) =>
            {
                this.handleComplete(true);
            };

            this.m_request.onerror = (event: Event) =>
            {
                this.handleComplete(false);
            };
        }

        isSent(): boolean
        {
            return this.m_sent;
        }

        isComplete(): boolean
        {
            return this.m_request.readyState == XMLHttpRequest.DONE;
        }

        wasSuccessful(): boolean
        {
            return this.isComplete() && this.m_success;
        }

        sendAsync(): void
        {
            console.assert(!this.m_sent, "bbox.net.AjaxRequest: Cannot call sendAsync when already sent");

            this.m_sent = true;

            this.m_request.send();
        }

        sendAsyncWithContent(content: string): void
        {
            console.assert(!this.m_sent, "bbox.net.AjaxRequest: Cannot call sendAsync when already sent");

            this.m_sent = true;

            this.m_request.send(content);
        }

        abortAsync(): void
        {
            console.assert(this.m_sent, "bbox.net.AjaxRequest: Cannot call abortAsync before calling sendAsync");

            if (!this.m_aborted)
            {
                this.m_aborted = true;
                this.m_request.abort();
            }
        }

        getResponseAsText(): string
        {
            console.assert(this.m_handled, "bbox.net.AjaxRequest: Cannot call getResponseAsXmlDocument before request is completed");
            console.assert(this.m_success, "bbox.net.AjaxRequest: Cannot call getResponseAsXmlDocument before on failed request");

            return this.m_request.responseText;
        }

        getResponseAsXmlDocument(): Document
        {
            console.assert(this.m_handled, "bbox.net.AjaxRequest: Cannot call getResponseAsXmlDocument before request is completed");
            console.assert(this.m_success, "bbox.net.AjaxRequest: Cannot call getResponseAsXmlDocument before on failed request");

            return this.m_request.responseXML;
        }

        getResponseAsJson(): any
        {
            console.assert(this.m_handled, "bbox.net.AjaxRequest: Cannot call getResponseAsJson before request is completed");
            console.assert(this.m_success, "bbox.net.AjaxRequest: Cannot call getResponseAsJson before on failed request");

            return JSON.parse(this.m_request.responseText);
        }

        getErrorDescription(): String
        {
            console.assert(this.m_handled, "bbox.net.AjaxRequest: Cannot call getErrorDescription before request is completed");
            console.assert(this.m_success, "bbox.net.AjaxRequest: Cannot call getErrorDescription before on successful request");

            return this.m_request.statusText;
        }

        private handleComplete(success: boolean)
        {
            if (!this.m_aborted
                && !this.m_handled)
            {
                this.m_handled = true;
                this.m_success = success;

                if (this.m_handler)
                {
                    this.m_handler(this);
                }
            }
        }

        private m_handler: AjaxRequestCompleteHandler;
        private m_request: XMLHttpRequest;
        private m_sent: boolean;
        private m_aborted: boolean;
        private m_success: boolean;
        private m_handled: boolean;
    }
}
